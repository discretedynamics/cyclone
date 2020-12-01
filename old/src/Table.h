#ifndef CYCLONE_TABLE
#define CYCLONE_TABLE

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "constants.h"
#include "Table.h"

using namespace std;

// Paul Vines
// 7-13-2011

class Table{

 private:

  // PRE: input contains a list of x variables (e.g. x1 x2 x3 ... xN)
  // POST: varOrder is initialized to contain integers corresponding to
  // the x values in the order they are read in input, indexed to zero.
  // Thus: x1 x3 x5 creates a varOrder vector of [0, 2, 4]
  // input is modified to strip these x values from it and is passed
  // back by reference as well as returned
  string parseVars(string & input, vector<string> * varNamesVector);

  // PRE: values is a string with a value for each entry in the (T+1)
  // column of an input table. These values must be less than 10 because
  // this function simply subtracts the char '0' from the input char to
  // determine its integer value.
  // POST: The values table has been initialized to the value in the
  // input string. Reftable has been initialized to properly navigate
  // input states to the correct output value for the table
  void makeTable(string values);

  // PRE: orig is defined as a valid Table
  // POST: this table has had its value table initialized to a copy of
  // the values of orig, and this table's reftable has been initialized
  // to properly evaluate input.
  void copyValues(const Table &orig);

 public:
  vector<uchar> * varOrder; // the order to read an input state in
  unlong * table; // the table of values for the output
  unlong * reftable; // a navigational table to speed getEntry
			   // resolution
  unlong num_vars; // the number of vars in the table
  unlong * num_states; // the number of possible values for each
			    // state
  unlong num_states_size; // the number of variables in the total model
  unlong table_size; // the size of the values table:
			    // (num_states^num_vars)
unlong reftable_size;  // size of the reference table:
  // (((num_states^num_vars) -1) / (num_states - 1)) - 1
  // if num_states[i] == numstates[j] for all i, j
  

void printTable(ostream & output, vector<string> * varNamesVector);

  // PRE: this table is undefined. input is defined as a valid table
  // input, vars is the number of variables in the whole system, states
  // is the number of states possible (0, 1, 2)
  // POST: this table is defined according to the input table
 Table(string input, unlong * states, int states_size, vector<string> * varNamesVector);
  
  // PRE: orig is defined
  // POST: this table is defined as a deep copy of orig
  Table(const Table &orig);

  // PRE: This table is defined.
  // POST: This table is deleted and all memory freed.
  ~Table();
  
  // PRE: entry is defined as an array of uchar of length = num_states
  // POST: the table output corresponding to the input state is
  // returned.
  unlong getEntry(uchar* state);

};

#endif // CYCLONE_TABLE