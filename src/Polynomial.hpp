#ifndef __polynomial_hpp_
#define __polynomial_hpp_

#include <string>
#include <vector>
#include <iostream>

// To run (on linux or mac):
// mkdir build
// cd build
// cmake ..
// make
// ./simFDS

// TODO for now (to include NOT, OR, AND, MAX, MIN)
//   1. create all of these nodes
//   2. evaluator needs to be able to evaluate these kinds of nodes
//   3. parser: needs to recognize and translate them
//   constructor working DONE
//   add in the create nodes functions DONE
//   evaluation at a point. DONE
//   print function for the polynomial DONE
//   parse function: string --> creates a polynomial, or gives an intelligible error.

const char MAX_OPERATOR = '>';
const char MIN_OPERATOR = '<';

class Polynomial
{
  // (x0+1)*(x1+1) + x2^2
  // ((x1+1))
  // 0*(...)
  // (1+1)
  //  3 [1 2 3] 4 [0 0 1] == 3 x0*x1^2*x2^3 + 4*x2
  // (((3*x0)*(x1^2))*(x3^2))+(4*x2)
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

  // eval at a point (1,1,0)
  //  start with
  // [0 1 1 1 0]

  enum class operandType {PLUS, TIMES, POWER, OR, NOT, MAX, MIN};
  
  struct operand
  {
    operandType op;
    //enum { PLUS, TIMES, POWER, OR, NOT, MAX, MIN} op;
    // make OR ==> MAX
    // make AND ==> MIN
    // make NOT ==> a |--> p-1-a
    std::vector<int> args;
  };
  
  int mNumStates; // possible values of each variable are 0..mNumState-1, mNumStates must be prime.
  int mNumVariables;
  int mFirstOperation; // index into mEvaluationValues.
  int mResultLocation; // index into mEvaluationValues where result is.

  std::vector<int> mEvaluationValues; // array 0..9 of values in range 0..numStates-1.
      // numStates + numVars + length of mOperands array.

  std::vector<operand> mOperands; // array of length 5.  Constructed during parsing.

private:  
  bool isConstantLocation(int loc)
  {
    return loc < mNumStates;
  }
public:
  Polynomial(int numstates, int numvars);

  Polynomial(const Polynomial& f) = default;

  int numStates() const { return mNumStates; }
  int numVariables() const { return mNumVariables; }
  
  std::ostream& debug_display(std::ostream& o) const;

  int constantNode(int a);
  int variableNode(int var);
  int createPlusNode(int first_loc, int second_loc);
  int createTimesNode(int first_loc, int second_loc);

  int createOrNode(int first_loc, int second_loc);
  int createNotNode(int first_loc);
  int createMaxMinNode(int left, int right, char op);
  
  int exp(int base, int exponent);
  int createPowerNode(int first_loc, int exponent);
  int evaluate(const int pt[]);
  std::string evaluateSymbolic(const std::vector<std::string>& varnames);
  
  //  static Polynomial parse(const std::string& input_string, const std::vector<std::string>& var_names);
};

Polynomial parsePolynomial(const std::vector<std::string>& varnames, int numstates, const std::string& str);

int parseNumber(const std::string &str, int begin, int end);

#endif

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
