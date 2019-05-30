#include <string>
#include <vector>
#include "constants.h"
#include "PDS.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <ctype.h>

using namespace std;

// Paul Vines
// 6-23-2011
// polynomial class, respresents a single polynomial equation

// PRE: This object is undefined, input is a valid polynomial string as defined by the README. States is the number of values available for the states [0,1,2]
// POST: This object is defined with variables matching the input string polynomial
// f1=x1*x4*x3*x2^2+2*x1^2*x4^2*x3*x2
PDS::PDS(string input, int states){
  //cout << input << endl;
  // Trim to remove f#=

  numStates = states;

  int equalBreak = 0;
  for (int i = 0; i < input.length() && input[i] != '='; i++){
    equalBreak = i + 2;
  }
  input = input.substr(equalBreak, input.size() - equalBreak);
  

  int termIndex = 0;

  /*  // If there is a leading 1+
  if (input[1] == '+'){
    vector<uchar> * tempVar = new vector<uchar>();
    tempVar->push_back(0);
    vector<uchar> * tempPows = new vector<uchar>();    
    tempPows->push_back(0);
    vars.push_back(*tempVar);
    pows.push_back(*tempPows);
    coefs.push_back((uchar)(input[0] - '0'));
    input = input.substr(2, input.size() - 2);
    termIndex = 1;
    }*/

  // Input is now in the form of 1*x1... or x1...
  vector<int> breakPoints;
  breakPoints.push_back(0);
  for (int i = 0; i < input.length(); i++){
    if (input[i] == '+'){
      breakPoints.push_back(i + 1);
    }
  }

  for (int i = 0; i < breakPoints.size(); i++){
    vars.push_back(*(new vector<uchar>()));
    pows.push_back(*(new vector<uchar>()));
  }
  // Break apart at pluses and send to submethod
  for (int i = 0; i < breakPoints.size(); i++){
    if (i < breakPoints.size() - 1){
      parseTerm(input.substr(breakPoints[i], breakPoints[i+1] - breakPoints[i] - 1) + "\0", i + termIndex);
      //      cout << " TERM: " << input.substr(breakPoints[i], breakPoints[i+1] - breakPoints[i] - 1) << endl;
}
    else{
      parseTerm (input.substr(breakPoints[i], input.length() - breakPoints[i]) + "\0", i + termIndex);
      //       cout << " TERM: " << input.substr(breakPoints[i], input.length() - breakPoints[i]) << endl;    
    }
  }

  //cout << "END" << endl;
}

bool PDS::hasCoef(string input){
  int xIndex = -1;
  for (int i = 0; i < input.length(); i++){
    if (isalpha(input[i])){
    //if (input[i] == 'x'){
      xIndex = i;
    }
  }
  return xIndex == -1;
}

// PRE: input contains a term such as 1*x1^2*x2, termNum indicates
// which number term this is in the overall input
// POST: the term has been further parsed as vars, pows, and coefs and
// placed in the arrays in the correct positions
void PDS::parseTerm(string input, int termNum){
  vector<int> breakPoints;
  breakPoints.push_back(0);
  for (int i = 0; i < input.length(); i++){
    if (input[i] == '*'){
      breakPoints.push_back(i + 1);
    }
}

  // Input is NOT "+1"
  if (input.length() > 1){

    // cout << "COEF: " << input.substr(0, breakPoints[1]) << endl;
    //if (isalpha(input[0])|| (breakPoints.size() > 1 && !hasCoef(input.substr(0, breakPoints[1])))){
    if (input[0] == 'x' || (breakPoints.size() > 1 && !hasCoef(input.substr(0, breakPoints[1])))){
      coefs.push_back(1);
    }

    for (int i = 0; i < breakPoints.size(); i++){
      if (i < breakPoints.size() - 1){
	parseVar(input.substr(breakPoints[i], breakPoints[i + 1] - breakPoints[i]- 1) + "\0", termNum);
	//    cout << "VAR: " << input.substr(breakPoints[i], breakPoints[i + 1] - breakPoints[i] - 1) << endl;
      }
      else{
	parseVar(input.substr(breakPoints[i], input.length() - breakPoints[i]) + "\0", termNum);
	//  cout << "VAR: " << input.substr(breakPoints[i], input.length() - breakPoints[i]) << endl;
      }
    }
  }
  else{
    vector<uchar> * tempVar = new vector<uchar>();
    tempVar->push_back(0);
    vector<uchar> * tempPows = new vector<uchar>();    
    tempPows->push_back(0);
    vars.push_back(*tempVar);
    pows.push_back(*tempPows);
    coefs.push_back((uchar)(input[0] - '0'));
  }
}

// Pre: input contains a string of either: 1 or 1x or 1x^2
// POST: The input has been added to either the coefs or vars and pows
// vectors
void PDS::parseVar(string input, int termNum){
  // if Coef
  if (input.size() == 1){
    coefs.push_back(input[0] - '0');
    //  cout << "ADDING COEF " << endl;
  }
  // if Var
  else {
    int expIndex = 0;
    // find '^' if it exists
    for (int i = 0; i < input.length() && input[i] != '^'; i++){
    	expIndex = i;
    }

    // If no '^' was present 
    if (expIndex == input.length() -1){
      pows[termNum].push_back(1);
    }
    else{
      pows[termNum].push_back((uchar)input[expIndex + 2] - '0');
    }

    //  cout << "VAR VALUE: " << (int)((uchar)atoi(input.substr(1, expIndex).c_str()) -1) << endl;
    vars[termNum].push_back((uchar)atoi(input.substr(1, expIndex).c_str()) - 1);
  }
}

 // PRE: this PDS is defined and state is defined
 // POST: the resulting level for this variable is returned
 int PDS::evaluate(uchar * state){
   int result = 0;
   
   //cout << "SPLIT STATE: " <<  (int)state[0] << " , " << (int)state[1]  << " , " << (int)state[2] << endl;

   for (int i = 0; i < coefs.size(); i++){
     result  += resolveTerm(state, coefs[i], vars[i], pows[i]);
     /*if (result > 429496720){
        cout << "BIG: " << result << endl;
	}*/
     if (result < 0){
       cout << "OVERFLOW" << endl;
     }
     //cout << "Term: " << i << " = " << temp << endl;
     //  result += temp;
  }
   // cout << "E: " << result << endl;
       result = result % numStates;
       //   cout << "POST MOD: " << result << endl;
   return result;
}


// PRE: all inputs are defined
// POST: the mathematical result is returned
int PDS::resolveTerm(uchar * state, uchar coef, vector<uchar> theseVars, vector<uchar> thesePows){
  int result = coef;

  for (int i = 0; i < theseVars.size(); i++){
    result *= resolveVar(state[theseVars[i]], thesePows[i]);
  }

  result = result % numStates;

  return result;
}


// PRE: state and pow are defined
// POST: returns the specified state multiplied pow times
int PDS::resolveVar(uchar state, uchar pow){
  int result = 1;
    for (int i = 0; i < pow; i++){
      result *= state;
    }

    return result;
}