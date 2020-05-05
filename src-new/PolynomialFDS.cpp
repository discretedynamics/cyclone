#include "PolynomialFDS.hpp"

void PolynomialFDS::display(std::ostream& o)
{
  o << "PolynomialFDS numstates=" << numStates()
    << " numvars=" << numVariables()
    << std::endl;
  for (int i=0; i < mPolynomials.size(); i++)
    o << "  " << mPolynomials[i].evaluateSymbolic(mVariableNames) << std::endl;
}

void PolynomialFDS::evaluate(const int u[], int output[])
{
  for (int i=0; i < numVariables(); i++)
    {
      output[i] = mPolynomials[i].evaluate(u);
    }
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

void computeComponentsAndCycles(const std::vector<long>& stateSpace)
{
  std::vector<long> cycle { stateSpace.size(), -1};

  // add in nlimitcycles, limitcycles, cyclelength, trajectory (scratch)
  // populate all of these figure out what to return.
}

#if 0
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
