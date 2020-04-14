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

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
