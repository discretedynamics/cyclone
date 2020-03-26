#include <string>
#include <vector>

class State
{
  long indexOfState();
};

class Polynomial
{
  // (x0+1)*(x1+1) + x2^2
  // ((x1+1))
  // 0*(...)
  // (1+1)
  //  3 [1 2 3] 4 [0 0 1] == 3 x0*x1^2*x2^3 + 4*x2
  // Straight line program.
  // array of nodes 0..final value-1
  // 0: constant 0
  // 1: constant 1
  // 2: x0  (var 0)
  // 3: x1  (var 1)
  // 4: x2  (var 2)
  // 5: + 2 1
  // 6: + 3 1
  // 7: * 5 6
  // 8: ^ 4 2
  // 9: + 7 8 (RESULT)

  int mNumConstants; // constants 0, 1, ..., numConstants-1
  int mNumVariables;
  int mFirstOperation; // index into mEvaluationValues.

  std::vector<int> mEvaluationValues; // array 0..9 of values in range 0..numStates-1.
      // numStates + numVars + length of ops array.

  struct operand
  {
    enum { PLUS, TIMES, POWER } op;
    int first_arg;
    int second_arg;
  };
  
  std::vector<operands> mOperands; // array of length 5.  Constructed during parsing.

  bool isConstantLocation(int loc)
  {
    return loc < mNumConstants;
  }
public:
  Polynomial(int numstates, int numvars)
    : mNumConstants(numstates),
      mNumVariables(numvars),
      mFirstOperation(numstates + numvars)
  {
    for (int i=0; i < mNumConstants; ++i)
      mEvaluationValues.push_back(i);
    for (int i=0; i < mNumVariables; ++i)
      mEvaluationValues.push_back(0);
  }

  int createPlusNode(int first_loc, int second_loc)
  {
    if (first_loc < mNumConstants and second_loc < mNumConstants)
      {
        
        // evaluate now, because they are constants
      }
    else
      {
        mOperands.push_back({operand::PLUS, first_loc, second_loc});
        mEvaluationValues.push_back(0); // make space in evaluation array for the output.
        return mEvaluationValues.size()-1;
      }
  }

  int evaluate(State u) {
    // evaluate at a point
    // 1. fill in the n variables values in the array 'values'.
    for (int i = mFirstOperation; i < mEvalutationValues.size(); i++)
      {
        operand& a = mOperands[i-mFirstOperation];
        switch (a.operation) {
        case PLUS:
          mEvaluationValues[i] = (mEvaluationValues[a.first_arg]
                                   + mEvaluationValues[a.second_arg]) % mNumConstants;
      }
    return mEvaluationValues[mEvaluationValues.size()-1];
  }
  
  static Polynomial parse(const std::string& input_string, const std::vector<std::string>& var_names);
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
  
