// create image file from a doot file as like:
//   dot -Tpng -o foo.png foo-statespace.dot
// TODO for June 2020
//   1. DONE: write a function readFDS().  DONE.
//   2. DONE: incorporate that into `main`, also printing to a file. WORKING ON THIS. Write limit cycle to a file. Use states, not indices.
//   3. DONE: Write a dot file.
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
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

PolynomialFDS examplePDS1()
{
// MODEL NAME: eg2 (unused)
// SIMULATION NAME: eg2 (unused)
// NUMBER OF VARIABLES: 3
// VARIABLE NAMES: x1 x2 x3 (unused, sort of)
// NUMBER OF STATES: 2 2 2 (must all be the same, a prime number)
// SPEED OF VARIABLES: 1 1 1  (unused)

// f1 = 1+x2
// f2 = x1+x3
// f3 = x1*x2

  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<Polynomial> polys;
  // loc0 = 0
  // loc1 = 1
  // loc2 = x1
  // loc3 = x2
  // loc4 = x3
  // Create polynomial f1
  int nstates = 101;
  Polynomial f1(nstates, 3); // 2 states, 3 vars
  f1.createPlusNode(1,nstates+2-1);
  polys.push_back(f1);
  // Create polynomial f2
  Polynomial f2(nstates, 3);
  f2.createPlusNode(nstates+1-1,nstates+3-1);
  polys.push_back(f2);
  // Create polynomial f3
  Polynomial f3(nstates, 3);
  f3.createTimesNode(nstates+1-1,nstates+2-1);
  polys.push_back(f3);

  return PolynomialFDS(polys, varnames);
}

Polynomial examplePoly1()
{
  Polynomial f(3, 4); // 3 states, 4 variables 0,1,2,  3,4,5,6
  int n1 = f.createPlusNode(3,4);
  int n2 = f.createPlusNode(5,n1);
  int n3 = f.createTimesNode(n1,n2);
  f.createTimesNode(n3, 1);
  f.debug_display(std::cout);
  return f;
}

Polynomial examplePoly2()
{
  Polynomial g(3, 4);
  int n1 = g.createPlusNode(1,1);
  int n2 = g.createPowerNode(n1,2);
  g.createPlusNode(5,n2);
  g.debug_display(std::cout);
  return g;
}

Polynomial examplePoly3()
{
  Polynomial h(3, 4);
  h.createPlusNode(1,1);
  h.debug_display(std::cout);
  return h;
}

void someTests()
{
  std::vector<std::string> varnames = { "x3", "x", "P53" };
  Polynomial f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
  f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 5*  AB_3");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
  f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 4*  AB_3");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  f = parsePolynomial(varnames, 3, "x3 + x^3 + (2+1)*P53 + AB_3^6");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  f = parsePolynomial(varnames, 5, "x3 + x^3 + (2+1)*P53 + AB_3^6");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  f = parsePolynomial(varnames, 5, "1+(2*5)^2+3");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;
}

PolynomialFDS examplePDS2()
{
  int nstates = 2;
  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<std::string> polystrs
    {
     "x1+x2",
     "x2*x3+1",
     "x1+x1*x3"
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

#if 0  
  f = examplePoly1();
  int pt[4] {0,1,1,2};
  std::cout << "f(0,1,1,2) = " << f.evaluate(pt) << std::endl;
  std::cout << f.evaluateSymbolic({"x1","x3","x5","x7"}) << std::endl;

  Polynomial g = examplePoly2();
  Polynomial h = examplePoly3();

  PolynomialFDS pds = examplePDS1();
  std::cout << pds << std::endl;
  std::vector<long> stateSpace = computeStateSpace(pds);
  //  for (auto a : stateSpace) {
  //    std::cout << a << " ";
  //  }
  
  if (argc < 2)
    {
      std::cout << "simFDS <FDS file> -o <output dot file>" << std::endl;
      return 1;
    }
  return 0;
}
#endif

PolynomialFDS cyclonePDS1()
{
  int nstates = 2;
  std::vector<std::string> varnames { "x1", "x2" };
  std::vector<std::string> polystrs
    {
     "1+x2",
     "0+x1" // TODO: BUG: replacing this with x1 causes the poly to become 0!
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

PolynomialFDS cyclonePDS2()
{
  int nstates = 2;
  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<std::string> polystrs
    {
     "1+x2",
     "x1+x3",
     "x1*x2"
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

PolynomialFDS cyclonePDS3()
{
  int nstates = 2;
  std::vector<std::string> varnames { "M", "B", "Ob", "Tb", "A", "Da", "L", "Hl" };
  std::vector<std::string> polystrs
    {
     "A",
     "M + (B*(Tb+1)) + M*B*(Tb+1)",
     "(M+1)*B",
     "(M+1)*Ob",
     "(B*L + Hl + B*L*Hl) + A*Da*(B+1) + (B*L + Hl + B*L*Hl)*A*Da*(B+1)",
     "((B+1) + (L+1) + (B+1)*(L+1))*(Hl+1)*A",
     "L",
     "Hl"
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

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

void runTrajectoryComputation(std::string projectName, const std::vector<int>& startPoint, bool write_dot_file)
{
  std::cout << "running trajectory computation" << std::endl;

  PolynomialFDS* pds = readPDS(projectName + ".pds");

#if 0

  // TODO: get this code working.
  std::vector<long> trajectory;
  std::unordered_map<long, long> hash;

  State u(pds.numStates(), fds.numVariables());
  State v(pds.numStates(), fds.numVariables());

  // TODO: u = startPoint
  hash.insert(u.getIndex(), 0);
  long count = 1;
  while (true)
    {
      trajectory.push_back(u);
      pds.evaluate(u.getState(), v.getState());
      long vindex = v.getIndex();
      auto result = hash.insert(vindex,count);
      if (not result.second)
        final_index = result.first->second; // ??
      // either break out, or set u and continue
      u.setFromIndex(vindex);
      count++;
    }
  // TODO: display.
#endif
  // TODO: check: startPoint has length nvars, all values are between 0 and numStates-1.
  // TODO: have a loop that runs through the trajectory:
  //   std::vector<int> trajectory.
  //   put u onto trajectory
  //   count = 0;
  //   put u=>count into a hash table
  //   loop
  //      v = next state from u
  //      count++
  //      if v is in the hash table: break out of loop, and write the files.
  //         (also will add v=>count to hash table).
  //      trajetory.push_back(v)
  //   end loop
  //   write the trajectory to a dot file
  //   write the summary text.
  // 
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
  
  std::cout << "trajectory index: " << trajectoryIndex << std::endl;
  std::cout << "summary index: " << summaryIndex << std::endl;
  std::cout << "trajectory: ";
  for (auto a : trajectoryStart) std::cout << a << " ";
  std::cout << std::endl;
  
  bool write_dot_file = (summaryIndex == -1);

  if (trajectoryIndex == -1)
    runStateSpaceComputation(projectName, write_dot_file);
  else
    runTrajectoryComputation(projectName, trajectoryStart, write_dot_file);

  return 0;
}
