#include <string>
#include <vector>

class State
{
  long indexOfState();
};

class Polynomial
{
public:
  Polynomial(); // create one, or parse one.

  int evaluate(State); // evaluate at a point
};

class Table
{
};

/// Class of a FDS which is given by polynomials
class PolynomialFDS
{
private:
  std::vector<std::string> mVariableNames;
  std::vector<Polynomial> mPolynomials;
public:
  State evaluate(State u);
};

/// Class of a FDS which is given by transition tables
class TableFDS
{
private:
  std::vector<std::string> mVariableNames;
  std::vector<Table> mTables;
public:
  void evaluate(State u, State result);
};

Table readTable(std::string filename);
PDS readPDS(std::string filename);

std::vector<long> computeStateSpace(const TableFDS& FDS)
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
  
