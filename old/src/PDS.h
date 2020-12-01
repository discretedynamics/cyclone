#ifndef CYCLONE_PDS
#define CYCLONE_PDS

#include <string>
#include <vector>

#include "constants.h"

using namespace std;

// Paul Vines
// 6-23-2011
// polynomial class, respresents a single polynomial equation

class PDS{
 private:

  int numStates;

  // PRE: all inputs are defined
  // POST: the mathematical result is returned
  int resolveTerm(uchar * state, uchar coef, vector<uchar> vars, vector<uchar> pows);

  
  // PRE: state and pow are defined
  // POST: returns the specified state multiplied pow times
  int resolveVar(uchar state, uchar pow);

  bool hasCoef(string input);

  // PRE: input contains a string of either: 1 or 1x or 1x^2
  // POST: The input has been added to either the coefs or vars and pows
  // vectors
  void parseVar(string input, int termNum);
  
    
  // PRE: input contains a term such as 1*x1^2*x2, termNum indicates
  // which number term this is in the overall input
  // POST: the term has been further parsed as vars, pows, and coefs and
  // placed in the arrays in the correct positions
  void parseTerm(string input, int termNum);
  
 public:
  vector<vector<uchar> > vars;
  vector<vector<uchar> > pows;
  vector<uchar> coefs;
  
  // PRE: This object is undefined, input is a valid polynomial string as defined by the README
  // POST: This object is defined with variables matching the input string polynomial
  // f1=x1*x4*x3*x2^2+2*x1^2*x4^2*x3*x2
  PDS(string input, int states);
 
  // PRE: this PDS is defined and state is defined
  // POST: the resulting level for this variable is returned
  int evaluate(uchar * state);

};

#endif // CYCLONE_PDS