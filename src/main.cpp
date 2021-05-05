#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

void testComma()
{
  {
    std::vector<std::string> varnames = { "x3", "a", "P53" };
    Polynomial f = parsePolynomial(varnames, 3, translateOperatorNames("max(x3,a+P53)"));
    std::cout << f.evaluateSymbolic(varnames) << std::endl;
  }
  
  {
    std::vector<std::string> varnames = { "x3", "a", "P53" };
    Polynomial f = parsePolynomial(varnames, 3, translateOperatorNames("min(x3,a+P53,~x3)"));
    std::cout << f.evaluateSymbolic(varnames) << std::endl;
  }
}

bool hasExtension(const std::string &name, const std::string &extension) {
  return name.size() >= extension.size() &&
    name.compare(name.size() - extension.size(),
                 extension.size(),
                 extension) == 0;
}

void runStateSpaceComputation(std::string projectName, bool write_dot_file)
{
 
  std::string fileName = (hasExtension(projectName, ".pds") ?
                          projectName : (projectName + ".pds"));
  PolynomialFDS* pds = readPDS(fileName);

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

  std::string fileName = (hasExtension(projectName, ".pds") ?
                          projectName : (projectName + ".pds"));
  PolynomialFDS* pds = readPDS(fileName);

  if (startPoint.size() != pds->numVariables())
    {
      std::cerr << "Expected trajectory start state of length " << pds->numVariables() << std::endl;
      exit(1);
    }
  // TODO:check that all entries of startPoint are in range 0..pds.numState()-1

  std::vector<long> trajectory;
  std::unordered_map<long, long> hash; //encoded state => position in the trajectory.
  
  State u(pds->numStates(), startPoint);
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
  // std::cout << translateOperatorNames("NOT (x NOTX  AND NOT(b)) xor sdda and  not c OR d") << std::endl;
  // std::cout << translateOperatorNames("a OR OR b") << std::endl;
  // std::cout << translateOperatorNames("max(a,maxmax(b,c))") << std::endl;
  // testComma();
  // return 0;

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
