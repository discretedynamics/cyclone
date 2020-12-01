// create image file from a dot file as like:
//   dot -Tpng -o foo.png foo-statespace.dot
// TODO (created November 2020)
//   1. source code
//      a. tests
//   2. web interface (in progress)
//      a. update algorun_info/manifest.json <-
//      b. update input_example, output_example
//      c. update run.py must: no-translation, update calls to cyclone (replace with shell script, remove python from dockerfile)
//   3. dockerize
//      a. fix Dockerfile (modern cmake, gcc) DONE, "modern" achieved.
//   4. contact Ibrahim for update to Ubuntu 20.04 (tagged version)
//   5. node installation via apt repository.
//   6. file extension resolve project vs. filename
// TODO for June 2020
//   1. DONE: write a function readFDS().  DONE.
//   2. DONE: incorporate that into `main`, also printing to a file. WORKING ON THIS. Write limit cycle to a file. Use states, not indices.
//   3. DONE: Write a dot file. DONE
//   4. test this on some real examples, that we can test "by hand", or "by M2", or by cyclone.
//   5. create a number of tests (at least 20 examples, with at least the summary answers, some with dot files)
//   6. try to incorporate this into algorun and plantsimlab.
//   7. clean up the tests, add a few more tests.
//   8. error checking and error display
//   9. walk through the code and clean/document it
//   

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

void runStateSpaceComputation(std::string projectName, bool write_dot_file)
{
  PolynomialFDS* pds = readPDS(projectName + ".pds");

  std::vector<long> stateSpace = computeStateSpace(*pds);
  auto limitCycleInfo = computeComponentsAndCycles(stateSpace);

  std::ofstream summaryFile;
  summaryFile.open(projectName + "-limitcycles.txt");
  displayLimitCycleInfo(summaryFile, *pds, limitCycleInfo);
  summaryFile.close();

  if (write_dot_file)
    {
      std::ofstream ofil;
      ofil.open(projectName + "-statespace.dot");
      writeDotFile(ofil, *pds, stateSpace);
      ofil.close();
    }
}

  // TODO: display.
  //  length of the trajectory until it hits the limit cycle (summary)
  //  length of the limit cycle (summary)
  //  the limit cycle itself (dot file/summary)
  //  entire graph of the whole trajectory (dot file).

void writeTrajectoryGraph(std::ostream& o,
                            int numStates,
                            int numVariables,
                            const std::vector<long>& trajectory,
                            long finalIndex)
{
  o << "digraph trajectory {" << std::endl;

  // display the edges
  State u(numStates, numVariables);
  State v(numStates, numVariables);
  for (long i=0; i < trajectory.size(); ++i)
    {
      u.setFromIndex(trajectory[i]);
      if (i < trajectory.size() - 1)
        v.setFromIndex(trajectory[i+1]);
      else
        v.setFromIndex(trajectory[finalIndex]);
      o << "  "
        << "\"" << u << "\""
        << " -> "
        << "\"" << v << "\""
        << std::endl;
    }

  o << "}" << std::endl;
}

void writeTrajectorySummary(std::ostream& o,
                            int numStates,
                            int numVariables,
                            const std::vector<long>& trajectory,
                            long finalIndex // index into 'trajectory'
                            )
{
  long cycleLength = trajectory.size() - finalIndex;
  if (cycleLength == 1)
    {
      o << "#steps to the fixed point: " << finalIndex << std::endl;
    }
  else
    {
      o << "#steps to enter the limit cycle: " << finalIndex << std::endl;
      o << "length of the limit cycle: " << cycleLength << std::endl;
    }
}

void runTrajectoryComputation(std::string projectName, const std::vector<int>& startPoint, bool write_dot_file)
{
  // TODO: get this code working.

  std::cout << "running trajectory computation" << std::endl;

  PolynomialFDS* pds = readPDS(projectName + ".pds");

  if (startPoint.size() != pds->numVariables())
    {
      std::cerr << "Expected trajectory start state of length " << pds->numVariables() << std::endl;
      exit(1);
    }
  // TODO:check that all entries of startPoint are in range 0..pds.numState()-1

  std::vector<long> trajectory;
  std::unordered_map<long, long> hash; //encoded state => position in the trajectory.
  
  State u(pds->numStates(), startPoint); // TODO: write this constructor.
  State v(pds->numStates(), pds->numVariables());

  hash.insert(std::make_pair(u.getIndex(), 0));
  long count = 1;
  long final_index = -1;
  while (true)
    {
      trajectory.push_back(u.getIndex());
      pds->evaluate(u.getState(), v.getState());
      long vindex = v.getIndex();
      auto result = hash.insert(std::make_pair(vindex,count));
      if (not result.second) // if the state v already has been seen on this trajectory
        {
          final_index = hash[vindex];
          break;
        }
      u.setFromState(v);
      count++;
    }

  writeTrajectorySummary(std::cout, pds->numStates(), pds->numVariables(), trajectory, final_index);
  if (write_dot_file)
    {
      std::ofstream ofil;
      ofil.open(projectName + "-trajectory.dot");
      writeTrajectoryGraph(ofil, pds->numStates(), pds->numVariables(), trajectory, final_index);
      ofil.close();
    }
}

int main(int argc, char* argv[])
{
  if (argc < 2)
    {
      std::cout << "Usage:" << std::endl;
      std::cout << "  simFDS <FDS project name>" << std::endl;
      std::cout << "    -- read in polynomial dynamical system (e.g. foo.pds if project name is 'foo')" << std::endl;
      std::cout << "    -- create statespace and summary (limit cycle info), e.g. in files foo-statespace.dot, foo-limitcycles.txt" << std::endl;
      std::cout << "  simFDS <FDS project name> --summary" << std::endl;
      std::cout << "    -- create summary only, no state space, in file e.g. foo-limitcycles.txt" << std::endl; 
      std::cout << "  simFDS foo --trajectory 1 1 0 1 1 2" << std::endl;
      std::cout << "    -- run the trajectory with the given start point" << std::endl;
      std::cout << "    -- create files foo-110112.dot, foo-110112.txt." << std::endl;
      std::cout << "    -- the first file is a dot file with the whole trajectory graph" << std::endl;
      std::cout << "    -- the second file is a text file describing the length of the path, and the resulting limit cycle" << std::endl;
      std::cout << "  simFDS foo --summary --trajectory 1 1 0 1 1 2" << std::endl;
      std::cout << "    -- same as --trajectory, except the .dot file is not created" << std::endl;
      
      return 1;
    }

  std::vector<std::string> args;
  args.assign(argv+1, argv+argc);
  
  std::string projectName = argv[1];
  std::vector<int> trajectoryStart; // length 0 if --trajectory is not set, i.e. if trajectoryIndex == -1.

  // summaryIndex: will be set to the index of the argument --summary.  (e.g. in simFDS foo --summary, the value will be 2).
  auto summaryFound = std::find(args.begin(), args.end(), "--summary");
  int summaryIndex = (summaryFound == args.end() ? -1 : summaryFound - args.begin());

  // trajectoryIndex: will be set to the index of the argument --trajectory.  (e.g. in simFDS foo --trajectory, the value will be 2).
  auto trajectoryFound = std::find(args.begin(), args.end(), "--trajectory");
  int trajectoryIndex = (trajectoryFound == args.end() ? -1 : trajectoryFound - args.begin());

  // Fill in the trajectory, if it exists.
  if (trajectoryIndex != -1)
    {
      int top = (summaryIndex == -1 or summaryIndex < trajectoryIndex ? args.size() : summaryIndex);
      for (int i = trajectoryIndex + 1; i < top; i++)
        {
          size_t lastloc;
          int val = std::stoi(args[i], &lastloc, 10);
          if (lastloc != args[i].size())
            {
              std::cerr << "expected nonnegative integers as arguments after --trajectory" << std::endl;
              return 1;
            }
          trajectoryStart.push_back(val);
        }
    }
  
  bool write_dot_file = (summaryIndex == -1);

  if (trajectoryIndex == -1)
    runStateSpaceComputation(projectName, write_dot_file);
  else
    runTrajectoryComputation(projectName, trajectoryStart, write_dot_file);

  return 0;
}
