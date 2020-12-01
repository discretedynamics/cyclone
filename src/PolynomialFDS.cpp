#include <fstream>
#include <regex>
#include "PolynomialFDS.hpp"

void PolynomialFDS::display(std::ostream& o)
{
  o << "PolynomialFDS numstates=" << numStates()
    << " numvars=" << numVariables()
    << std::endl;
  for (int i=0; i < mPolynomials.size(); i++)
    o << "  " << mVariableNames[i] << " = " << mPolynomials[i].evaluateSymbolic(mVariableNames) << std::endl;
}

void PolynomialFDS::evaluate(const int u[], int output[])
{
  for (int i=0; i < numVariables(); i++)
    {
      output[i] = mPolynomials[i].evaluate(u);
    }
}

PolynomialFDS* readPDS(std::string filename)
{
  // Step1: read in the file as a list of lines.
  std::ifstream ifil;
  ifil.open(filename);
  if (not ifil.is_open())
    throw std::runtime_error("failed to open file: " + filename);
  std::vector<std::string> lines;
  std::string thisline;
  // todo: add in some error checking. throw an error if a problem.
  while (getline(ifil, thisline))
    lines.push_back(thisline);
  ifil.close();

  // Loop through the lines one by one:
  int numstates = -1;
  int numvars = -1;
  std::vector<std::string> varnames;
  std::vector<std::string> polystrs;

  for (auto& line : lines)
    {
      // removes all white space on the line
      line.erase(std::remove_if(line.begin(),
                                line.end(),
                                [](unsigned char x) { return std::isspace(x); }
                                ),
                 line.end());

      if (line.size() == 0) continue;
      if (line[0] == '#') continue;

      // is this a : line or = line, where is the location of that?
      auto colon = line.find_first_of(":"); // returns a giant number if not found.

      if (colon < line.size())
        {
          std::string firstpart = line.substr(0, colon);
          std::string secondpart = line.substr(colon+1, line.size());
          if (0 == firstpart.compare("NUMBEROFVARIABLES"))
            {
              if (numvars >= 0)
                throw std::runtime_error("cannot specify number of variables more than once");
              numvars = parseNumber(line, colon+1, line.size());
              continue;
            }

          if (0 == firstpart.compare("NUMBEROFSTATES"))
            {
              if (numstates >= 0)
                throw std::runtime_error("cannot specify number of states more than once");
              numstates = parseNumber(line, colon+1, line.size());
              // if (not isprime(numstates))
              //   throw std::runtime_error("expected the number of states to be a prime number");
              continue;
            }
          throw std::runtime_error("unexpected colon on line");
        }

      // at this point, we had better have a string of the form "var=poly"      
      auto equals = line.find_first_of("=");
      if (equals < line.size())
        {
          // create two strings, place them into varnames, polystrs.
          varnames.push_back(line.substr(0,equals));
          polystrs.push_back(line.substr(equals+1, line.size()));
          continue;
        }
      throw std::runtime_error("unexpected line format on line");
    }
  
  if (numvars >= 0 and varnames.size() != numvars)
    throw std::runtime_error("number of variables declared differs from the number of equations");

  if (numstates <= 1)
    throw std::runtime_error("expected a prime number of states, not " + std::to_string(numstates));
  
  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, numstates, s));

  return new PolynomialFDS(polys, varnames);
}

std::vector<long> computeStateSpace(PolynomialFDS& FDS)
{
  std::vector<long> stateSpace(FDS.totalNumStates(), -1); // set length, initialize each to a bad value

  State u(FDS.numStates(), FDS.numVariables());
  State v(FDS.numStates(), FDS.numVariables());
  for (long i=0; i < FDS.totalNumStates(); ++i)
    {
      u.setFromIndex(i);
      FDS.evaluate(u.getState(), v.getState());
      int nextstate = v.getIndex();
      stateSpace[i] = nextstate;
    }
  return stateSpace;
}

std::vector<ComponentData> computeComponentsAndCycles(const std::vector<long>& stateSpace)
{
  // cycle[0..N-1] starts out with all -1's, and then each -1 is eventually replaced with the
  //   index of the limit cycle that state goes to (N = stateSpace.size()
  std::vector<long> cycle(stateSpace.size(), -1);

  // A growing array, the length is the number of limit cycles found so far
  std::vector<long> limitCycles; // limitCycles[i] is a state in the i-th limit cycle.

  long nextIndex = 0;
  for (long i = 0; i < stateSpace.size(); ++i)
    {
      if (cycle[i] >= 0) continue;
      long pt = i;
      while (cycle[pt] == -1)
        {
          cycle[pt] = nextIndex;
          pt = stateSpace[pt];
        }
      if (cycle[pt] == nextIndex)
        {
          limitCycles.push_back(pt);
          nextIndex++; // note nextIndex is always limitCycles.size() (as a long).
        }
      else
        {
          long thisLimtCycle = cycle[pt];
          long j = i;
          while (j != pt)
            {
              cycle[j] = thisLimtCycle;
              j = stateSpace[j];
            }
        }
    }

  // This contains the component info, and each limit cycle
  std::vector<ComponentData> result(limitCycles.size());

  // fill in the component sizes of each component
  for (long i = 0; i < cycle.size(); ++i)
    {
      result[cycle[i]].componentSize++;
    }

  for (long j = 0; j < limitCycles.size(); j++)
    {
      long pt = limitCycles[j];
      result[j].limitCycle.push_back(pt);
      long q = stateSpace[pt];
      while (q != pt)
        {
          result[j].limitCycle.push_back(q);
          q = stateSpace[q];
        }
    }
  return result;
};

void writeDotFile(std::ostream& o,
                  PolynomialFDS& FDS,
                  const std::vector<long>& stateSpace)
{
  o << "digraph statespace {" << std::endl;

  // display label for each node in the statespace

  // display the edges
  State u(FDS.numStates(), FDS.numVariables());
  State v(FDS.numStates(), FDS.numVariables());
  for (long i=0; i < stateSpace.size(); ++i)
    {
      u.setFromIndex(i);
      v.setFromIndex(stateSpace[i]);
      o << "  "
        << "\"" << u << "\""
        << " -> "
        << "\"" << v << "\""
        << std::endl;
    }

  o << "}" << std::endl;
}

void displayLimitCycleInfo(std::ostream& o,
                           const PolynomialFDS& pds,
                           const std::vector<ComponentData>& cycleinfo)
{
  o << "Number of cycles (components): " << cycleinfo.size() << std::endl << std::endl;
  State u(pds.numStates(), pds.numVariables());  
  for (long i=0; i<cycleinfo.size(); i++)
    {
      long cyclelen = cycleinfo[i].limitCycle.size();
      o << "COMPONENT #" << i+1 << ":" << std::endl;
      o << "  component size: " << cycleinfo[i].componentSize << std::endl;
      if (cyclelen == 1)
        {
          u.setFromIndex(cycleinfo[i].limitCycle[0]);
          o << "  fixed point: [" << u << "]" << std::endl << std::endl;
        }
      else {
        o << "  cycle of length " << cyclelen << ": " << std::endl;
        for (int j=0; j<cyclelen; j++)
          {
            u.setFromIndex(cycleinfo[i].limitCycle[j]);
            o << "    [" << u << "] -> " << std::endl;
          }
        u.setFromIndex(cycleinfo[i].limitCycle[0]);
        o << "    [" << u << "]" << std::endl << std::endl;
      }
    }
}

#if 0
pseudo-code vars:
  p = #states per variable
  n = #vars
  N = p^n
  stateSpace[0..N-1]
  #limitcycles
  std::vector limitCycles: length #limitcycles. At i-th index, this is a point in the i-th limit cycle.
  std::vector limitcyclelengths
  cycle[0..N-1]: vector of ints (limit cycle index)
pseudo-code
  set cycle[i] = -1 for all i=0..N-1
  nextIndex = 0
  for i from 0 to N-1 do (
    if cycle[i] >= 0 then continue
    pt = i

    while cycle[pt] == -1 do (
        cycle[pt] = nextIndex
        pt = stateSpace[pt]
        )

    if cycle[pt] == nextIndex
    then (
        -- new limit cycle
        limitCycles.push_back(pt)
        nextIndex = nextIndex + 1
        )
    else (
        j = i
        while j != pt do (
            cycle[j] = cycle[pt]
            j = stateSpace[j]
            )
        
        )
    )
        
    

// Example for computeComponentsAndCycles.
// We decided: probably union-find is not needed here.
// But we thought of a modified version which should work well here.

cycle [-1 -1 -1 -1 -1 -1 -1 -1]
      [ 0 -1  0 -1 -1 -1  0  0]
      [ 0  0  0  1  1  1  0  0]

nlimitcycles = 0, be incremented.
limitcycle[0..nlimitcycles-1]
  limitcycle[0] = 7
  limitcycle[1] = 4
  cyclelength[0] = 3
  cyclelength[1] = 1

stateSpace[0..N-1]
nlimitcycles (#comps), int
cycle[0..N-1], initialized to -1's.
limitcycle[0..nlimitcycles-1]
cyclelength[0..nlimitcycles-1]

trajectory array: keep the trajectory.

0  7 
1  7
2  7
3  5
4  4  (fixed point)
5  4
6  2
7  6

7 --> 6 --> 2 --> 7 limit cycle of length 3.
3 --> 5 --> 4.
0, 1 --> 7 (first limit cycle)

0 --> 7 --> 6 --> 2 --> 7 (detect limit cycle 762).
[0, 7, 6, 2]

[0, 0, 0, 3, 3, 3, 0, 0]  i: name of a component.

new example:
  0 1 2 3 4 5 6 7
  1 4 7 7 4 7 2 6

cycle array at various times
   0  1  2  3  4  5  6  7
  -1 -1 -1 -1 -1 -1 -1 -1 (after this line: nextCycle=0)
   0  0  .  .  0  .  .  . (after this line: nextCycle=1)
   .  .  1  .  .  .  1  1 (after: nextCycle=2)
            1        
                  1



                      union-find
parent: [0 1 2 3 4 5 6 7]
        [0 1 2 3 4 5 6 0]
        [1 1 2 3 4 5 6 0]  -- after 17
        [1 1 2 3 4 5 6 1]  -- after root(7) call (compression)
        [1 2 2 3 4 5 6 1]  -- after 27
        [1 2 2 3 4 3 6 1]  -- after 35
        [1 2 2 3 3 3 6 1]  -- after 54
        [1 2 6 3 3 3 6 1]  -- after 62
        [1 6 6 3 3 3 6 6]  -- after 62
root of each:
        [6 6 6 3 3 3 6 6]
        
1 -> 7 
-- 07
root(0) = 0
root(7) = 7
parent(root(7)) = root(0) = 0

-- 17
-- root(1) = 1
-- root(7) = 0
-- parent(root(7)) = root(1):  parent(0) = 1

-- 27
-- root(2) = 2
-- root(7) = 1
-- parent(root(7)) = root(2),  parent(1) = 2

-- 35
-- root(3) = 3
-- root(5) = 5
-- parent(root(5)) = root(3): parent(5) = 3

-- 44
-- nothing

-- 54
-- root(5) = 3 (no compression is done)
-- root(4) = 4 (no compression)
-- parent(root(4)) = root(5): parent(4) = 3

-- 62
-- root(6) = 6
-- root(2) = 2
-- parent(root(2)) = root(6): parent(2) = 6

-- 76
-- root(7) = 6
-- root(6) = 6

#endif
                      
// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
