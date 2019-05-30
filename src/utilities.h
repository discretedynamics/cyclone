#ifndef CYCLONE_UTILITIES
#define CYCLONE_UTILITIES

#include <vector>
#include <cmath>

#include "constants.h"

using namespace std;

// PRE: n is the decimal to convert, v is a unsigned char array for
//the conversion, j is the base to convert to
// POST: v contains the base-j form of n, in ascending order of power
// ASSUMPTIONS: v is long enough to fit n in base-j
void decimalToTernary(unsigned long n, uchar * v, unlong bases[], int numvars);

// PRE: v is an array of numbers for a number in base-j, j is the base
// of the number
// POST: the decimal value of this is returned
unsigned long ternaryToDecimal(int num_vars, unsigned char v[], unlong bases[]);

long primPow(long base, long power);

void waitfor (pid_t aPID);

/*
vector<unsigned long> IndirectQuickSort(unsigned long cycles, unsigned long * values);
*/

#endif // CYCLONE_UTILITIES