#include <string>
#include <vector>

#include "State.hpp"
#include "Polynomial.hpp"

/// Class of a FDS which is given by polynomials
class PolynomialFDS
{
private:
  int mNumStates; // per variable, all the same.
  int mTotalNumStates; // mNumStates^#vars
  std::vector<std::string> mVariableNames;
  std::vector<Polynomial*> mPolynomials;
public:
  State evaluate(State u);
};

PDS readPDS(std::string filename); // TODO: to be written

std::vector<long> computeStateSpace(const PolynomialFDS& FDS)
{
  std::vector<long> stateSpace;
  stateSpace.reserve(FDS.numStates()); // set length to FDS.numStates()

  State u = new int[FDS.numVars()];
  State v = new int[FDS.numVars()];
  for (long i=0; i<FDS.numStates(); ++i)
    {
      stateFromIndex(i, u); // needs more info to work
      FDS.evaluate(u, v);
      int nextstate = indexFromState(v); // need same info
      stateSpace[i] = nextstate;
    }
  delete[] u;
  delete[] v;
  return stateSpace;
}
  
