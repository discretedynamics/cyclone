#include <string>
#include <vector>
#include "constants.h"
#include "Table.h"
#include <math.h>

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "utilities.h"

using namespace std;

// Paul Vines
// 11-23-2011

// PRE: this table is undefined. input is defined as a valid table
// input, vars is the number of variables in the whole system, states
// is the number of states possible (0, 1, 2)
// POST: this table is defined according to the input table
Table::Table(string input, unlong states[], int states_size, vector<string> * varNamesVector){

  string temp = input + "\0";

  temp = parseVars(temp, varNamesVector);
  // ASSERT: varOrder is initialized and temp contains only the output
  // column
  num_states_size = states_size;
  num_states = states;
  /* num_states = new unlong(states_size);
  for (int i = 0; i < states_size; i++){
    num_states[i] = states[i];
    }*/

  // compute size of the values table
  table_size = 1;
  for (int i = 0; i < num_vars; i++){
    table_size *= states[varOrder->at(i)];
  }

  // compute size of the referencing table
  unlong result = 1;
  reftable_size = 0;
  for (int i = 0; i < num_vars - 1; i++){
    result *= num_states[varOrder->at(i)];
    reftable_size += result;
  }

  // allocate memory for the table and reftable

  reftable = new unlong[reftable_size];
  table = new unlong[table_size];

  // INCREDIBLY HACKISH, ONLY WORKS FOR num_states < 11
  makeTable(temp);

}

// PRE: orig is defined
// POST: this table is defined as a deep copy of orig
Table::Table(const Table &orig){
  num_vars = orig.num_vars;
  num_states_size = orig.num_states_size;
  /* num_states = new unlong[orig.num_states_size];
  for (int i = 0; i < num_states_size; i++){
    num_states[i] = orig.num_states[i];
    }*/
  num_states = orig.num_states;

  varOrder = new vector<uchar>();
  for (int i = 0; i < orig.varOrder->size(); i++){
    varOrder->push_back(orig.varOrder->at(i));
  }
  
  table_size = orig.table_size;
  reftable_size = orig.reftable_size;
  reftable = new unlong[reftable_size];
  table = new unlong[table_size];
  copyValues(orig);
}

// PRE: orig is defined as a valid Table
// POST: this table has had its value table initialized to a copy of
// the values of orig, and this table's reftable has been initialized
// to properly evaluate input.
void Table::copyValues(const Table &orig){
  if (reftable_size > 0){
    int ref_to_ref_size = reftable_size - (table_size / num_states[varOrder->at(num_vars -1)]);
  
    // Initialize linking indices from index to index of the referencing
    // table
    int result = num_states[varOrder->at(0)];
    int position1 = 0;
    int position2 = result;
    for (int i = 0; i < num_vars - 2; i++){
      for (int k = 0; k < result; k++){
	//      for (int k = 0; k < num_states[varOrder->at(i)]; k++){
	//	cerr << "REF: " << k + position1 << " -> " << position2 + (k * num_states[varOrder->at(i + 1)]) << endl;
	reftable[k + position1] = (unlong)(&reftable[position2 + (k * num_states[varOrder->at(i + 1)])]);
      }
      result *= num_states[varOrder->at(i + 1)];
      position1 = position2;
      position2 += result;
    }

    // Initialize linking indices from the referencing table to the
    // value table
    for (int i = ref_to_ref_size; i < reftable_size; i++){
      //      cerr << "VALREF: " << i << " -> " << (i-ref_to_ref_size) * num_states[varOrder->at(num_vars -1)] << endl;
      reftable[i] = (unlong)(&table[(i-ref_to_ref_size) * num_states[varOrder->at(num_vars -1)]]);
    }  
  }


  // Initialize value indices
  for (int i = 0; i < table_size; i++){
    table[i] = orig.table[i];
  }
}

// PRE: values is a string with a value for each entry in the (T+1)
// column of an input table. These values must be less than 10 because
// this function simply subtracts the char '0' from the input char to
// determine its integer value.
// POST: The values table has been initialized to the value in the
// input string. Reftable has been initialized to properly navigate
// input states to the correct output value for the table
void Table::makeTable(string values){

  if (reftable_size > 0){
    int ref_to_ref_size = reftable_size - (table_size / num_states[varOrder->at(num_vars -1)]);
  
    // Initialize linking indices from index to index of the referencing
    // table
    int result = num_states[varOrder->at(0)];
    int position1 = 0;
    int position2 = result;
    for (int i = 0; i < num_vars; i++){
      //      cerr << "i: " << i << "varOrder->at(i): " << (int)varOrder->at(i) << "num_states[varOrder->at(i): " << num_states[varOrder->at(i)] << endl;
    }
    for (int i = 0; i < num_vars - 2; i++){
      //	cerr << "Pos1: " << position1 << " Pos2: " << position2 << endl;
      for (int k = 0; k < result; k++){
	//	cerr << "REF: " << k + position1 << " -> " << position2 + (k * num_states[varOrder->at(i + 1)]) << endl;
	reftable[k + position1] = (unlong)(&reftable[position2 + (k * num_states[varOrder->at(i + 1)])]);
      }
      result *= num_states[varOrder->at(i + 1)];
      position1 = position2;
      position2 += result;
    }

    // Initialize linking indices from the referencing table to the
    // value table
    for (int i = ref_to_ref_size; i < reftable_size; i++){
      //      cerr << "VALREF: " << i << " -> " << (i-ref_to_ref_size) * num_states[varOrder->at(num_vars -1)] << endl;
      reftable[i] = (unlong)(&table[(i-ref_to_ref_size) * num_states[varOrder->at(num_vars -1)]]);
    }  
  }

  // Initialize value indices
  int i = 0;
  for (int k = 0; i < table_size && k < values.length(); k++){
    if (values[k] >= '0' && values[k] <= '9'){
      table[i] = (unlong)(values[k] - '0');
      i++;
    }
  }
}

// PRE: input contains a list of x variables (e.g. x1 x2 x3 ... xN)
// POST: varOrder is initialized to contain integers corresponding to
// the x values in the order they are read in input, indexed to zero.
// Thus: x1 x3 x5 creates a varOrder vector of [0, 2, 4]
// input is modified to strip these x values from it and is passed
// back by reference as well as returned
string Table::parseVars(string & input, vector<string> * varNamesVector){
  //std::cout << "\n*** input 1 = " << input << std::endl;
  // Erase leading \n's
  while ((!isalpha(input.at(0))) || input.at(0) == '\n'){  
    input = input.substr(1, input.length());
  }
  
  // add each variable name position
  stringstream stream(input);
  string name;
  varOrder = new vector<uchar>();
  while (stream >> name) {
      if (name == "\n") break;
      for (int i=0; i < varNamesVector->size(); i++) {
          if (name == varNamesVector->at(i)) {
              varOrder->push_back(i);
              break;
          }
      }
  }
  num_vars = varOrder->size();
  int len = 0;
  for (int i=0; i < num_vars; i++) {
      len += (varNamesVector->at(varOrder->at(i)).length()+1);
  }

  input = input.substr(len, input.length() - len);
  
  varOrder->pop_back(); // remove the last variable name (duplicated one at the last)
  num_vars = varOrder->size();
  //std::cout << "\n*** input 2 = \n" << input << std::endl;
  return input;
}

// PRE: entry is defined as an array of uchar of length = num_states
// POST: the table output corresponding to the input state is
// returned.
unlong Table::getEntry(uchar* entry){
  unlong result;
  // if the table has no external inputs (is of length num_states)
  // then reftable is not used at all
  if (table_size > num_states[varOrder->at(0)]){
    // initialize the reftable navigation pointer based on the value of
    // the state at the index of the first element of the varOrder index
    unlong temp = reftable[entry[varOrder->at(0)]];

    // loop through all but the last remaining input states to consider
    // according to varOrder
    for (int i = 1; i < num_vars -1; i++){
      temp = *((unlong*)(temp + (entry[varOrder->at(i)] * BYTES_PER_ADDR)));    
    }
    // get the final value based on the navigation pointer and the value
    // of the last state to consider for the table, getting the value
    // from that index of the value table
    result = *((unlong *)(temp + (entry[varOrder->at(num_vars - 1)] * BYTES_PER_ADDR)));
  }
  else{
    result = table[entry[varOrder->at(0)]];
  }
  return result;
}

void Table::printTable(ostream &output, vector<string> * varNamesVector){

  string name = "";
  for (int i = 0; i < varOrder->size(); i++){
    //output << "x" << (int)varOrder->at(i) + 1<< " ";
    name = varNamesVector->at(varOrder->at(i));
    output << name << " ";
  }
  output << name << endl;

  for (int i = 0; i < table_size; i++){
    output << (int)table[i] << "\n";
  } output << endl;

}

// PRE: This table is defined.
// POST: This table is deleted and all memory freed.
Table::~Table(){
  delete [] table;
  delete[] reftable;
  // delete [] num_states;
  varOrder->clear();
  delete varOrder;
}