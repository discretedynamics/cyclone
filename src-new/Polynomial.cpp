#include "Polynomial.hpp"

Polynomial::Polynomial(int numstates, int numvars)
  : mNumStates(numstates),
    mNumVariables(numvars),
    mFirstOperation(numstates + numvars),
    mResultLocation(0)
{
  for (int i=0; i < mNumStates; ++i)
    mEvaluationValues.push_back(i);
  for (int i=0; i < mNumVariables; ++i)
    mEvaluationValues.push_back(0);
}

std::ostream& Polynomial::debug_display(std::ostream& o) const
{
  o << "Polynomial: numstates=" << mNumStates
    << " numvars=" << mNumVariables
    << " evalValuesLength=" << mEvaluationValues.size()
    << " resultLoc=" << mResultLocation
    << " firstOp=" << mFirstOperation << std::endl;
  o << " program: " << std::endl;
  for (int i=0; i<mNumStates; i++)
    o << "    " << i << " = " << i << " (constant)" << std::endl;
  for (int i=0; i<mNumVariables; i++)
    o << "    " << i+mNumStates << " = x" << i << std::endl;
  for (int i=0; i < mOperands.size(); i++)
    {
      o << "    " << mFirstOperation+i << " = ";
      switch (mOperands[i].op)
        {
        case operand::PLUS:
          o << "PLUS";
          break;
        case operand::TIMES:
          o << "TIMES";
          break;
        case operand::POWER:
          o << "POWER";
          break;
        }
      o << " " << mOperands[i].first_arg << " " << mOperands[i].second_arg << std::endl;
    }
  return o;
}
  
int Polynomial::createPlusNode(int first_loc, int second_loc)
{
  mResultLocation = mEvaluationValues.size();
  if (isConstantLocation(first_loc) and isConstantLocation(second_loc))
    {
      // evaluate now, because they are constants
      int val = first_loc + second_loc;
      if (val > mNumStates) val -= mNumStates;
      mResultLocation = val;
    }
  else if (first_loc == 0)
    mResultLocation = second_loc;
  else if (second_loc == 0)
    mResultLocation = first_loc;
  else
    {
      mOperands.push_back({operand::PLUS, first_loc, second_loc});
      mEvaluationValues.push_back(0); // make space in evaluation array for the output.
    }
  return mResultLocation;
}

int Polynomial::createTimesNode(int first_loc, int second_loc)
{
  mResultLocation = mEvaluationValues.size();
  if (isConstantLocation(first_loc) and isConstantLocation(second_loc))
    {
      // evaluate now, because they are constants
      int val = (first_loc * second_loc) % mNumStates;
      mResultLocation = val;
    }
  else if (first_loc == 0)
    mResultLocation = 0;
  else if (second_loc == 0)
    mResultLocation = 0;
  else if (first_loc == 1)
    mResultLocation = second_loc;
  else if (second_loc == 1)
    mResultLocation = first_loc;
  else
    {
      mOperands.push_back({operand::TIMES, first_loc, second_loc});
      mEvaluationValues.push_back(0); // make space in evaluation array for the output.
    }
  return mResultLocation;
}

int Polynomial::exp(int base, int exponent)
{
  // Only handles exponents >= 1
  // does no optimization.
  int val = 1;
  for (int i=0; i<exponent; i++) val *= base;
  val = val % mNumStates;
  return val;
}
  
int Polynomial::createPowerNode(int first_loc, int exponent)
{
  // NOTE: 0^0 = 0 by convention
  mResultLocation = mEvaluationValues.size();
  if (isConstantLocation(first_loc))
    {
      // evaluate now, because they are constants
      mResultLocation = exp(first_loc, exponent);
    }
  else if (first_loc == 0)
    mResultLocation = 0;
  else if (exponent == 0)
    mResultLocation = 1;
  else if (first_loc == 1)
    mResultLocation = 1;
  else if (exponent == 1)
    mResultLocation = first_loc;
  else
    {
      mOperands.push_back({operand::POWER, first_loc, exponent});
      mEvaluationValues.push_back(0); // make space in evaluation array for the output.
    }
  return mResultLocation;
}
  
int Polynomial::evaluate(const int pt[]) {
  // ASSUME: pt is a pointer to an array 0..numvars-1 of values in the range 0..numConstants-1
  // evaluate at a point
  // 1. fill in the n variables values in the array 'mEvaluationValues'.
  for (int i=0; i < mNumStates; ++i)
    mEvaluationValues[i] = i;
  for (int i=0; i<mNumVariables; i++)
    mEvaluationValues[i + mNumStates] = pt[i];
  for (int i = mFirstOperation; i <= mResultLocation; i++)
    {
      operand& a = mOperands[i-mFirstOperation];
      switch (a.op) {
      case operand::PLUS:
        mEvaluationValues[i] = (mEvaluationValues[a.first_arg]
                                + mEvaluationValues[a.second_arg]) % mNumStates;
        break;
      case operand::TIMES:
        mEvaluationValues[i] = (mEvaluationValues[a.first_arg]
                                * mEvaluationValues[a.second_arg]) % mNumStates;
        break;
      case operand::POWER:
        mEvaluationValues[i] = exp(mEvaluationValues[a.first_arg], a.second_arg);
        break;
      }
    }
  return mEvaluationValues[mResultLocation];
}

std::string Polynomial::evaluateSymbolic(const std::vector<std::string>& varnames) {
  // ASSUME: pt is a pointer to an array 0..numvars-1 of values in the range 0..numConstants-1
  // evaluate at a point
  // 1. fill in the n variables values in the array 'mEvaluationValues'.
  std::vector<std::string> strs; // one for each element in mEvaluationValues, up to the result
  for (int i=0; i < mNumStates; ++i)
    strs.push_back(std::to_string(i));
  for (int i=0; i<mNumVariables; i++)
    strs.push_back(varnames[i]); // previously: "x" + std::to_string(i));
  for (int i = mFirstOperation; i <= mResultLocation; i++)
    {
      operand& a = mOperands[i-mFirstOperation];
      switch (a.op) {
      case operand::PLUS:
        strs.push_back("(" + strs[a.first_arg] + "+" + strs[a.second_arg] + ")");
        break;
      case operand::TIMES:
        strs.push_back("(" + strs[a.first_arg] + "*" + strs[a.second_arg] + ")");
        break;
      case operand::POWER:
        strs.push_back("(" + strs[a.first_arg] + "^" + std::to_string(a.second_arg) + ")");
        break;
      }
    }
  return strs[mResultLocation];
}

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
