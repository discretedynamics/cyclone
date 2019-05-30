#include "constants.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// PRE: n is the decimal to convert, v is a uchar array for
//the conversion, j is the base to convert to
// POST: v contains the base-j form of n, in ascending order of power
// ASSUMPTIONS: v is long enough to fit n in base-j
void decimalToTernary(unsigned long n, uchar * v, unlong bases[], int num_vars)
{
  unsigned long b=n;
  for(int i = num_vars-1; i >= 0; i--)
    {
      v[i] = b%bases[i];
      b /=bases[i];
    }
}

// PRE: v is an array of numbers for a number in base-j, j is the base
// of the number
// POST: the decimal value of this is returned
unlong ternaryToDecimal(int num_vars, uchar v[], unlong bases[]){
  unsigned long result = 0;
  for (int i = 0; i < num_vars; i++){
    result = (result * bases[i]) + (unsigned long)(v[i]);
  }

  return result;
}


// PRE: v is an array of numbers for a number in base-j, j is the base
// of the number
// POST: the decimal value of this is returned
/*unsigned long ternaryToDecimal(int num_vars, uchar v[], int j){
  unsigned long result = 0;
  for (int i = 0; i < num_vars; i++){
    result += (unsigned long)(v[i] * pow(j, i));
  }
  return result;
}
*/
long primPow(long base, long power){
  long temp = base;
  if (power == 0 && base != 0){
    temp = 1;
  }
  else {
    for (long i = 1; i < power; i++){
      temp *= base;
    }
  }

  return temp;
}

void waitfor (pid_t aPID) {
  int status;
  bool done = false;
  while (!done) {
    waitpid(aPID, &status, 0);
    done = WIFEXITED(status);
  }
  cout << "Process " << aPID << " exited." << endl;
}

/*unsigned long IndirectMergeSort(unsigned long cycles, unsigned long * values){
  
  unsigned long * sort = indirectMergeSor(cycles, values, 0, cycles);

  return sort;

}

unsigned long * IndirectMergeSort(unsigned long cycles, unsigned long low, unsigned long high, unsigned long * values){
  
  if (cycles > 1){
    sort = 
  }

  unsigned long * sort = new unsigned long();

  return sort;

}
*/