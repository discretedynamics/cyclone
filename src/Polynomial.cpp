#include "Polynomial.hpp"

#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <regex>

std::string translateAndOrNotXor(const std::string& s)
{
  // TODO (created 19 Jan 2021: this doesn't handle A or(A and B) (paren after or, and, xor).
  // fix this.  Can this code be simplified?
  // possibly change to a translate(string, word, replacement word) (perhaps no regex here?)
  std::regex e1 ("(\\bNOT )");
  std::regex e2 ("(\\bnot )");
  std::regex e3 ("(\\bNOT\\()");
  std::regex e4 ("(\\bnot\\()");
  std::regex e5 ("(\\bAND) +");
  std::regex e6 ("(\\band) +");
  std::regex e7 ("(\\bOR +)");
  std::regex e8 ("(\\bor +)");
  std::regex e9 ("(\\bXOR +)");
  std::regex e10 ("(\\bxor +)");
  std::vector<std::regex> reps = {e1, e2, e3, e4, e5, e6, e7, e8, e9, e10};
  std::vector<std::string> strs = {"~", "~", "~(", "~(", "*", "*", "|", "|", "+", "+"};

  auto result = s;
  for (int i = 0; i < reps.size(); ++i)
    {
      result = std::regex_replace(result, reps[i], strs[i]);
      std::cout << result << std::endl;
    }
  return result;
}

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
      for (auto a : mOperands[i].args)
        o << " " << a;
      o << std::endl;
      // o << " " << mOperands[i].first_arg << " " << mOperands[i].second_arg << std::endl;
    }
  return o;
}

int Polynomial::constantNode(int a)
{
  int c = a % mNumStates;
  mResultLocation = c;
  return c;
}

int Polynomial::variableNode(int var)
{
  // TODO: check that var is in range?
  int v = mNumStates + var;
  mResultLocation = v;
  return v;
}

int Polynomial::createPlusNode(int first_loc, int second_loc)
{
  mResultLocation = mEvaluationValues.size();
  if (isConstantLocation(first_loc) and isConstantLocation(second_loc))
    {
      // evaluate now, because they are constants
      int val = first_loc + second_loc;
      if (val >= mNumStates) val -= mNumStates;
      mResultLocation = val;
    }
  else if (first_loc == 0)
    mResultLocation = second_loc;
  else if (second_loc == 0)
    mResultLocation = first_loc;
  else
    {
      std::vector<int> args { first_loc, second_loc};
      mOperands.push_back({operand::PLUS, args});
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
      std::vector<int> args { first_loc, second_loc};
      mOperands.push_back({operand::TIMES, args});
      mEvaluationValues.push_back(0); // make space in evaluation array for the output.
    }
  return mResultLocation;
}

// a | b == a + b + a*b
// mod 3
// a | a == a
// 0 | a == a
// 2 | a == 2
// a | b == b | a
// 1 | 1 == 1
// a + a + a^2 == a^2 - a = (0,1,2) 0, 0, 2
int Polynomial::createOrNode(int first_loc, int second_loc)
{
  std::vector<int> args { first_loc, second_loc };
  return createMaxNode(args);
}

int Polynomial::createNotNode(int first_loc)
{
  // TODO XXX: 9 Feb 2021.  Ask Adam, what should it be?
  mResultLocation = mEvaluationValues.size();

  // TODO XXX: get this part functional too.
  // if (first_loc == 0) // NOT 0
  //   mResultLocation = p-1 - a;
  // else if (first_loc == 1) // NOT 1
  //   mResultLocation = 1;
  // else if (isConstantLocation(first_loc)) // NOT const
  //   {
  //     // evaluate now, because they are constants
  //     mResultLocation = exp(first_loc, exponent);
  //   }

  std::vector<int> args { first_loc };
  mOperands.push_back({operand::NOT, args});
  mEvaluationValues.push_back(0); // make space in evaluation array for the output.
  return mResultLocation;
}

int Polynomial::createMaxNode(std::vector<int> locs)
{
  // TODO XXX: 9 Feb 2021
}

int Polynomial::createMinNode(std::vector<int> locs)
{
  // TODO XXX: 9 Feb 2021
  // Also in this same todo:
  // Change 2 evaluators
  // Change display
  // Change parser to handle not(...), min(expr1, expr2, ...), max.
  // Translators from not to ~, etc. (we have this function, might need to
  //   clean it up.
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
  // x^mNumStates == x (mod mNumStates)
  // if mNumStates == 3, say
  //   x^2 == 1 (unless x == 0)
  if (exponent != 0)
    {
      exponent = exponent % (mNumStates-1); // Check this!
      if (exponent == 0) exponent += (mNumStates-1);
    }
  mResultLocation = mEvaluationValues.size();
  if (first_loc == 0) // 0^anything is 0
    mResultLocation = 0;
  else if (first_loc == 1) // 1^anything is 1
    mResultLocation = 1;
  else if (isConstantLocation(first_loc))
    {
      // evaluate now, because they are constants
      mResultLocation = exp(first_loc, exponent);
    }
  else if (exponent == 0)
    mResultLocation = 1;
  else if (exponent == 1)
    mResultLocation = first_loc;
  else
    {
      std::vector<int> args { first_loc, exponent };
      mOperands.push_back({operand::POWER, args});
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
        mEvaluationValues[i] = (mEvaluationValues[a.args[0]]
                                + mEvaluationValues[a.args[1]]) % mNumStates;
        break;
      case operand::TIMES:
        mEvaluationValues[i] = (mEvaluationValues[a.args[0]]
                                * mEvaluationValues[a.args[1]]) % mNumStates;
        break;
      case operand::POWER:
        mEvaluationValues[i] = exp(mEvaluationValues[a.args[0]], a.args[1]);
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
        strs.push_back("(" + strs[a.args[0]] + "+" + strs[a.args[1]] + ")");
        break;
      case operand::TIMES:
        strs.push_back("(" + strs[a.args[0]] + "*" + strs[a.args[1]] + ")");
        break;
      case operand::POWER:
        strs.push_back("(" + strs[a.args[0]] + "^" + std::to_string(a.args[1]) + ")");
        break;
      }
    }
  return strs[mResultLocation];
}

// helper function for parsePoly
int findLocation(const std::string& str, int begin, int end, char op)
{
  int level = 0;
  for (int i=end-1; i>=begin; --i) {
    char c = str[i];
    if (c == ')') {
      ++level;
      continue;
    }
    if (c == '(') {
      --level;
      if (level < 0)
        throw std::runtime_error("parentheses mismatch");
      continue;
    }
    if (level > 0) continue;
    if (c == op)
      return i;
  }
  if (level > 0) throw std::runtime_error("parentheses mismatch");
  return end;
}

int parseNumber(const std::string &str, int begin, int end)
{  
  if (isdigit(str[begin])) {
    //    std::cout << "found number at: " << begin << " and " << end << std::endl;
    size_t lastloc;
    std::string s { str.substr(begin, end-begin) };
    int a = std::stoi(s, &lastloc, 10); // Not correct yet:  "23(a+b)" return val will be 2
    if (lastloc != end-begin)
      throw std::runtime_error("expected the entire string " + s + " to be a number");
    return a;
  }
  return 0;
}

class PolynomialParser
{
private:
  Polynomial mResult;
  std::string mString;
  const std::vector<std::string>& mVarNames;
private:
  // TODO (19 Jan 2021): make changes to parsing and evaluation necessary to handle boolean functions, max and min calls.
  //   add in new nodes for SLP.
  //   add in operators: |, ~, min, max
  //   preprocess to remove AND, OR, NOT, XOR, give error if used, but not char 2 (at least AND, OR, XOR)
  //   min(a,b): needs to be handled.
  //   parsing changes...
#if 0
  // Just some of our meanderings related to allowing boolean functions
  ~ (& *) | +
  a*b == a & b
    ~a == 1 + a, char=5, ~0 == 4, ~1 == 3,  ~2 == 2  (char-1)*(1+a)
  a|b == a*b + a + b
  a^b == a+b 

  [ AND ] ==> *
  [ NOT ] ==> ~
  [ OR ] ==> |
  [ XOR ] ==> +
  NOT
  OR
  AND == *
  XOR == +
    
    ~ ^ & |
  ~ & ^ |
  |, &^, ~
  &, |, ~, ^
  a & b & c
  (a & b ^ c) | (~d) == ((a & b) ^ c) | (~d)
  a ^ (b & c)
  a ^ b ^ c == (a ^ b) ^ c == a ^ (b ^ c)
#endif
    
  // parse str[begin]..str[end-1].
  int parsePoly(int begin, int end)
  {
    // if the top level has a '+', break it at the plus:
    int i = findLocation(mString, begin, end, '+');
    if (i < end) {
      //      std::cout << "found + at: " << i << std::endl;
      if (i == end-1)
        throw std::runtime_error("+ needs two expressions");
      if (i == begin) return parsePoly(begin+1, end);
      int left = parsePoly(begin,i);
      int right = parsePoly(i+1,end);
      return mResult.createPlusNode(left, right);
    }

    i = findLocation(mString, begin, end, '*');
    if (i < end) {
      //      std::cout << "found * at: " << i << std::endl;
      if (i == begin or i == end-1)
        throw std::runtime_error("* needs two expressions");
      int left = parsePoly(begin,i);
      int right = parsePoly(i+1,end);
      return mResult.createTimesNode(left, right);
    }

    i = findLocation(mString, begin, end, '^');
    if (i < end) {
      //      std::cout << "found ^ at: " << i << std::endl;
      if (i == begin or i == end-1)
        throw std::runtime_error("^ needs two expressions");
      int left = parsePoly(begin,i);
      int exp = parseNumber(mString, i+1, end);
      return mResult.createPowerNode(left, exp);
    }

    if (mString[begin] == '(') {
      if (mString[end-1] != ')')
        throw std::runtime_error("mismatched parentheses");
      //      std::cout << "found parens at: " << begin << " and " << end << std::endl;      
      return parsePoly(begin+1, end-1);
    }

    // check for the string being a number
    if (isdigit(mString[begin])) {
      int a = parseNumber(mString, begin, end);
      return mResult.constantNode(a);      
    }

    // check for the string being a variable name
    //    std::cout << "looking for identifier: in " << mString.substr(begin, end-begin) << " at: [" << begin << ", " << end << ")" << std::endl;
    auto pos = std::find(mVarNames.begin(), mVarNames.end(), mString.substr(begin, end-begin));
    if (pos == mVarNames.end())
      throw std::runtime_error("expected a variable name starting at position " + std::to_string(begin));
    //    std::cout << "found identifier: " << pos-mVarNames.begin() << " at: " << begin << " and " << end << std::endl;
    return mResult.variableNode(pos-mVarNames.begin());
  }
public:
  PolynomialParser(const std::vector<std::string>& varnames,
                   int numstates,
                   const std::string& str)
    : mResult(numstates, varnames.size()),
      mString(str),
      mVarNames(varnames)
  {
    // Step 1: remove spaces.
    //    std::cout << "string before removing spaces: ." << mString << "." << std::endl;
    mString.erase(std::remove_if(mString.begin(),
                                 mString.end(),
                                 [](unsigned char x) { return std::isspace(x); }
                                 ),
                  mString.end());
  }

  Polynomial value()
  {
    try {
      parsePoly(0, mString.size());
    }
    catch (const std::string& excep) {
      std::cout << "exception: " << excep << std::endl;
    }
      
    return mResult;
  }
};

Polynomial parsePolynomial(const std::vector<std::string>& varnames, int numstates, const std::string& str)
{
  PolynomialParser P(varnames, numstates, str);
  return P.value();
}
// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
