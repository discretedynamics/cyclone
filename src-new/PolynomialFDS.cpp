#include "PolynomialFDS.hpp"

void PolynomialFDS::evaluate(const int u[], int output[])
{
  for (int i=0; i < numVariables(); i++)
    {
      output[i] = mPolynomials[i].evaluate(u);
    }
}

std::vector<long> computeStateSpace(PolynomialFDS& FDS)
{
  std::vector<long> stateSpace;
  stateSpace.reserve(FDS.numStates()); // TODO: set length to FDS.numStates()

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

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
