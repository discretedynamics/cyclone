#include <sys/times.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <streambuf>
#include "constants.h"
#include <iostream>
#include <vector>
#include "PDS.h"
#include <math.h>
#include <sstream>
#include "Table.h"
#include <stdlib.h>
#include "cyclone.h"
#include "utilities.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
// #include <boost/shared_ptr.hpp>

using namespace std;

// Paul Vines
// 2012-4-25
// The cyclone class iterates over the state space of a graph input
// into it as either a system of polynomial equations or a series of
// tables. This iteration then produces all limit cycles in the state
// space and outputs these, along with the percentage of the state
// space which is a part of each component of the graph.

// Sook Ha
// Modified for new table input format
// 2014-May

Cyclone::Cyclone():
  mSHMKey(0),
  mSHMId(-1),
  mSHMptr(NULL)
{
  numStates = new unlong[1];
  numStates[0] = 3;
  usingTables = true;
}

// Basic constructor: takes the number of states, the number of bits
// to use for marking different limit cycles, the input text, and
// whether the input is in table or PDS form
Cyclone::Cyclone(string input, bool useTables):
  mSHMKey(0),
  mSHMId(-1),
  mSHMptr(NULL)
{
  usingTables = useTables;
  if (usingTables)
    {
      readTableInput(input);
    }
  else
    {
      readInput(input);
    }

  total_states = 1;
  for (int i = 0; i < num_vars; i++)
    {
      total_states *= numStates[i];
    }
  associations = new unlong[total_states + 1];
  checkedArray = new unlong[total_states];
}

// Deep copy constructor
Cyclone::Cyclone(const Cyclone &orig):
  mSHMKey(0),
  mSHMId(-1),
  mSHMptr(NULL)
{
  usingTables = orig.usingTables;
  num_vars = orig.num_vars;
  numStates = new unlong[num_vars];
  for (int i = 0; i < num_vars; i++)
    {
      numStates[i] = orig.numStates[i];
    }
  total_states = orig.total_states;
  state = orig.state;

  pds = new vector<PDS>();
  tables = new vector<Table>();
  for (int i = 0; i < orig.pds->size(); i++)
    {
      pds->push_back(orig.pds->at(i));
    }
  for (int i = 0; i < orig.tables->size(); i++)
    {
      tables->push_back(orig.tables->at(i));
    }

}

// PRE: Input is a valid string which may contain spaces or tabs
// POST: Input is returned with all spaces and tabs removed
string Cyclone::removeSpaces(string input)
{
  string output = input;
  for (int i = 0; i < output.length(); i++)
    {
      if (output[i] == ' ' || output[i] == '\t')
        {
          output.erase(i, 1);
          i--;
        }
    }
  return output;
}

// PRE: filename contains the name of a file with tables
// POST: the file has been read and its tables constructed into Table
// objects in the tables array
void Cyclone::readTableInput(string filename)
{
  ifstream inputStream(filename.c_str());
  string input = "";
  if (inputStream.is_open())
    {

      char * temp = new char[LINESIZE];

      // Read each line into the input string variable one line at a time
      while (inputStream)
        {
          inputStream.getline(temp, LINESIZE);
          input += "\n";
          input += temp;
        }
      input[input.length() - 1] = '\0';
      parseTableInput(input);

      delete[] temp;
    }
  else
    {
      cout << "Input File Not Found" << endl;
    }

}

// PRE: input is defined as containing a series of n tables, headed by
// the number n at the beginning
// POST: Table objects have been constructed and added to the tables
// array by parsing input
void Cyclone::parseTableInput(string& input)
{
  // Read the model name, the simulation name, number of 
  // variables, and the number of states of each variable (node)
  input = parseHeader(input); 
  vector<int> breakPoints;
  tables = new vector<Table>();
  tables->reserve(num_vars);
  // ASSERT: num_vars is now set and header is removed

  for (int i = 1; i < input.length()-7; i++) {
      if (input.substr(i, 5).compare("STATE") == 0 && input.at(i - 1) == '\n') {
          int k = i;
          while (input.at(k) != ':') { k++; }
          if (input.at(k) == ':') {
              breakPoints.push_back(k+1);
          }
      }
  }
  
  initializeUpdateSpeeds(varSpeeds);
  // why??  breakPoints.push_back(input.length());

  // Read the states transition table data for each variable (node) and construct 
  // a Table for the variable and push the Table to the array of tables "tables"
  for (int i = 0; i < num_vars; i++) {
      tables->push_back(
          Table(input.substr(breakPoints[i], breakPoints[i + 1]-breakPoints[i]), 
                numStates, num_vars, varNamesVector));
  }
  breakPoints.clear();

}

// PRE: num_vars and unsortedSpeeds are both defined, 
// |unsortedSpeeds| = num_vars
// POST: varsByUpdateSpeed, sizeVarsByUpdateSpeed, and numUpdateSpeeds
// have all been initialized by parsing unsortedSpeeds
void Cyclone::initializeUpdateSpeeds(const int * unsortedSpeeds)
{
  // Find the max speed
  numUpdateSpeeds = 0;
  for (int i = 0; i < num_vars; i++)
    {
      if (unsortedSpeeds[i] > numUpdateSpeeds)
        {
          numUpdateSpeeds = unsortedSpeeds[i];
        }
    }
  numUpdateSpeeds++;

  // Initialize variables
  sizeVarsByUpdateSpeed = new int[numUpdateSpeeds];
  varsByUpdateSpeed = new int*[numUpdateSpeeds];
  for (int i = 0; i < numUpdateSpeeds; i++)
    {
      varsByUpdateSpeed[i] = new int[num_vars];
      sizeVarsByUpdateSpeed[i] = 0;
      for (int k = 0; k < num_vars; k++)
        {
          varsByUpdateSpeed[i][k] = -1;
        }
    }

  for (int i = 0; i < num_vars; i++)
    {
      //std::cout << "i " << i << " unsortedSpeeds " << unsortedSpeeds[i] << " sizeVarsByUpdateSpeed " << sizeVarsByUpdateSpeed[unsortedSpeeds[i]] << std::endl;
      varsByUpdateSpeed[unsortedSpeeds[i]][sizeVarsByUpdateSpeed[unsortedSpeeds[i]]] = i;
      sizeVarsByUpdateSpeed[unsortedSpeeds[i]]++;
    }

}
// This function returns the number of characters in the string starting from 
// the beginning of the string and the ending finished with a newline character  
int Cyclone::findNewLine(string& str)
{
  // Because find_first_of fails...
  int newline = 0;
  for (int i = 0; i < str.length() && newline == 0; i++)
    {
      if (str.at(i) == '\n')
        {
          newline = i;
        }
    }
  return newline;
}

// PRE: input contains a header with the number of variables and the number of 
// states of each var
// POST: the num_vars has been extracted and the input has had the
// header removed
string Cyclone::parseHeader(string& input)
{
    // newline is the length of the newline found by fineNewLine() function
    int newline = 0;
    newline = findNewLine(input);
    
    // read the model name
    if (input.substr(1, 11).compare("MODEL NAME:") == 0 ) {
        modelname = input.substr(13, newline-12);    
    }
    // jump to the next new line in the input string
    input = input.substr(newline, input.length() - newline);
    
    // read the experiment name
    newline = findNewLine(input);
    if (input.substr(1, 16).compare("SIMULATION NAME:") == 0 ) {
        runname = input.substr(18, newline-17);    
    }
    input = input.substr(newline, input.length() - newline);
     
    // read the total number of variables(nodes)
    newline = findNewLine(input);
    if (input.substr(1, 20).compare("NUMBER OF VARIABLES:") == 0) {
        stringstream stream (input.substr(22, newline-21)); 
        stream >> num_vars;
    }
    input = input.substr(newline, input.length() - newline);
  
    
    // read variable name each matching to the corresponding node name
    newline = findNewLine(input);
    if (input.substr(1, 15).compare("VARIABLE NAMES:") == 0) {
        varNamesVector = new vector<string>; 
        stringstream stream (input.substr(17, newline-16));
        string name;
        while (stream >> name) {
            varNamesVector->push_back(name);
        }
    }      
    input = input.substr(newline, input.length() - newline);
    
    // read the number of states for each variable (node)
    newline = findNewLine(input);
    numStates = new unlong[num_vars];
    if (input.substr(1, 17).compare("NUMBER OF STATES:") == 0) {
        stringstream stream (input.substr(19, newline-18));
        int num;
        int i = 0;
        while (stream >> num) {
            numStates[i++] = num;
        }
    }
    input = input.substr(newline, input.length() - newline);
     
    // read the speed for each variable (node) 
    newline = findNewLine(input);
    varSpeeds = new int[num_vars];
    if (input.substr(1, 19).compare("SPEED OF VARIABLES:") == 0) {
        usingSpeeds= true;
        stringstream stream (input.substr(21, newline-20));
        int speed;
        int i = 0;
        while (stream >> speed){
            varSpeeds[i++] = speed; 
        } 
    } else {usingSpeeds = false; }
    input = input.substr(newline, input.length() - newline);
    return input;
}

// PRE: filename contains the name of a valid input file
// POST: the number of variables in the input is returned, the input
// file has been extracted to a string and passed to parseInput and
// the PDSs constructed
void Cyclone::readInput(string filename)
{
  ifstream inputStream(filename.c_str());
  string input = "";

  if (inputStream.is_open())
    {
      char * temp = new char[LINESIZE];

      // Read each line into the input string variable one line at a time
      while (inputStream)
        {
          inputStream.getline(temp, LINESIZE);
          input += "\n";
          input += temp;
        }
      input[input.length() - 1] = '\0';
      parseInput(input);

      delete[] temp;
    }
  else
    {
      cout << "Input File Not Found" << endl;
    }

  num_vars = pds->size();
}

// PRE: input contains all the file contents
// POST: pds's have been constructed for each polynomial in the input
// string
void Cyclone::parseInput(string input)
{
  input = parseHeader(input);

  pds = new vector<PDS>();
  vector<int> breakPoints;
  PDS * temp;


  // Generate break points for each 'f'
  for (int i = 0; i < input.length(); i++)
    {
      if (input[i] == 'f' || input[i] == '\0')
        {
          breakPoints.push_back(i);
        }
    }

  // Split input at breakpoints and construct PDSs
  for (int i = 0; i < breakPoints.size() - 1; i++)
    {
      string function = removeSpaces(
          input.substr(breakPoints[i], breakPoints[i + 1] - breakPoints[i]));
      temp = new PDS(function, numStates[i]);
      pds->push_back(*temp);
    }
  delete temp;
}

// PRE: curState is a valid state in state space
// POST: curState is output in its array form
void Cyclone::printState(uchar * state)
{
  cerr << "[";
  for (int i = 0; i < num_vars; i++)
    {
      cerr << " " << (int) state[i];
    }
  cerr << " ]" << endl;
}

// PRE: curState is a valid state in state space
// POST: curState is output in its array form
void Cyclone::printState(unlong curState)
{

  uchar * temp = new uchar[num_vars]();
  for (int i = 0; i < num_vars; i++)
    {
      temp[i] = 0;
    }

  //cout << "CUR: " << curState << endl;
  decimalToTernary(curState, temp, numStates, num_vars);

  //cout << "[";
  for (int i = 0; i < num_vars; i++)
    {
      //cout << " " << (int) temp[i];
    }
  //cout << " ]" << endl;
  delete[] temp;
}

// PRE: PDSs are defined
// POST: The PDSs are output in the same format as input for
// comparison purposes
void Cyclone::printPDS()
{

  stringstream output;

  for (int a = 0; a < pds->size(); a++)
    {
      output << "f" << (int) (a + 1) << "=";

      for (int b = 0; b < pds->at(a).coefs.size(); b++)
        {

          if (pds->at(a).coefs[b] != -1 || pds->at(a).pows[b][0] == 0)
            {
              output << (int) pds->at(a).coefs[b];
            }
          if (pds->at(a).pows[b][0] != 0)
            {
              for (int c = 0; c < pds->at(a).vars[b].size(); c++)
                {
                  if (pds->at(a).coefs[b] == -1 && c == 0)
                    {
                      output << "x" << (int) pds->at(a).vars[b][c] + 1;
                    }
                  else
                    {
                      output << "*x" << (int) pds->at(a).vars[b][c] + 1;
                    }
                  if (pds->at(a).pows[b][c] != 1)
                    {
                      output << "^" << (int) pds->at(a).pows[b][c];
                    }
                }
            }
          if (b < pds->at(a).coefs.size() - 1)
            {
              output << '+';
            }
        }
      output << "\n";
    }
  //cout << output.str() << endl;
}

// PRE: key is defined as valid key for an SHM segment. size is
// defined as less than the max shared memory size / BYTES_PER_INT
// POST: a shared memory segment of size size * BYTES_PER_INT has been
// created with the key parameter. Its ID has been assigned to
// ID. The start address of the segment is returned.
void Cyclone::initializeSHM(unlong size)
{
  mSHMKey = getpid();

  // Create segment
  std::cout << "initializeSHM: allocating " << size * BYTES_PER_INT << " bytes" << std::endl;
  if ((mSHMId = shmget(mSHMKey, size * BYTES_PER_INT, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    {
      std::cout << "shmget errno = " << strerror(errno) << std::endl;
      if ((mSHMId = shmget(mSHMKey, size * BYTES_PER_INT, IPC_CREAT | 0666)) < 0)
        {
          std::cout << "shmget errno2 = " << strerror(errno) << std::endl;
          cerr << "shmget error: " << mSHMKey << endl;
          exit(1);
        }

      // This must be a leftover id we need to release it.
      mSHMptr = (int *) shmat(mSHMId, 0, 0);
      removeSHM();

      if ((mSHMId = shmget(mSHMKey, size * BYTES_PER_INT, IPC_CREAT | 0666)) < 0)
        {
          std::cout << "shmget errno3 = " << strerror(errno) << std::endl;
          cerr << "shmget error: " << mSHMKey << endl;
          exit(1);
        }
    }

  // Get the pointer to the start of the block
  if ((mSHMptr = (int *) shmat(mSHMId, 0, 0)) == (int *) -1)
    {
      std::cout << "shmget errno4 = " << strerror(errno) << std::endl;
      cerr << "shmat";
      exit(1);
    }

  int * fill = mSHMptr;

  // Set all indices to 0
  for (unlong i = 0; i < size; i++, fill++)
    {
      *fill = 0;
    }

  return;
}

// PRE: SHM segments are allocated
// POST: all SHM segments have been detached and removed
void Cyclone::removeSHM()
{
  if (mSHMptr != NULL)
    {
      shmdt(mSHMptr);
      mSHMptr = NULL;
    }

  if (shmctl(mSHMId, IPC_RMID, NULL) < 0)
    {
      cerr << "REMOVE ERROR5: " << errno << endl;
      exit(1);
    }

  mSHMId = 0;
}

// PRE: This instance of cyclone is defined using a valid
// constructor, filenameOut is a valid output filename, fileWrite
// is a boolean determining whether the output file is written to
// filenameOut. filenameOut is a valid output name for a file. Cores
// is the number of subprocesses to make, optimally the number of
// cores available. Verbose is a bool to determine whether the run
// outputs to the command line.
// POST: All limit cycles with component sizes are output to the
// command line (if verbose) and/or the output file (if fileWrite).
void Cyclone::run(bool fileWrite, string filenameOut, int cores, bool pverbose,
                  bool pincludeTables)
{

  writeToFile = fileWrite;
  verbose = pverbose;
  initializeSHM(total_states);

  run(cores);

  writeOutput(filenameOut);

  if (writeToFile)
    {
      delete fileOut;
    }
  removeSHM();
}

void Cyclone::fillEdgeArray(int cores)
{
  pid_t * children = new pid_t[cores];
  unlong cycleCountBlocks = total_states / cores;

  // Split into multiple processes
  for (int i = 0; i < cores; i++)
    {
      if (cores > 1)
        {
          children[i] = fork();
        }
      else
        {
          children[i] = 0;
        }

      if (children[i] == 0)
        {
          if (!usingSpeeds)
            {
              subprocessRun(i, cores, cycleCountBlocks, total_states);
            }
          else
            {
              subprocessRunWithSpeeds(i, cores, cycleCountBlocks, total_states);
            }

          if (cores > 1) exit(0);
        }
    }
  // Wait for all processes to finish before continuing
  if (cores > 1) // added MS+BS: waitfor seems to not work with 1 process...
    for (int i = 0; i < cores; i++)
        {
            waitfor(children[i]);
        }
}
// PRE: cores is defined as the number of subprocesses to make. PDSs
// or Tables are defined and ready to be used. SHM is initialized to
// the proper size for use.
// POST: All limit cycles and component sizes have been found and
// stored in the output buffers for output to the command line or
// file.
void Cyclone::run(int cores)
{
  fillEdgeArray(cores);

  for (int i = 0; i < total_states; i++)
    {
      associations[i] = 0;
      checkedArray[i] = 0;
    }
  associations[total_states] = 0;

  makeTrajectories(total_states);
  delete checkedArray;
  delete associations;
}

// PRE: all SHM segments have been allocated. childNum is defined as
// the child number of this subprocess. NumProccesses is the total
// number of processes created. cycleCountBlocks is the size of the
// block to compute the edges for in each subprocess. maxState is the
// size of the statespace.  
// POST: all edges between childNum * cycleCountBlocks and childNum +
// 1 * cycleCountBlocks have been computed and stored in the
// mShmID.
void Cyclone::subprocessRun(int childNum, int numProcesses,
                            unlong cycleCountBlocks, unlong maxState)
{

  // initialize variables for the run
  uchar * ternCurState = new uchar[num_vars];
  uchar * ternNextState = new uchar[num_vars];

  unlong startState = cycleCountBlocks * childNum;
  unlong endState = cycleCountBlocks * (childNum + 1);
  if (numProcesses - 1 == childNum)
    {
      endState = maxState;
    }

  for (unlong iterState = startState; (iterState < endState); iterState++)
    {
      // Convert to trinary
      unlong b = iterState;
      for (int i = num_vars - 1; i >= 0; i--)
        {
          ternCurState[i] = b % numStates[i];
          b /= numStates[i];
        }

      // Get next state from tables
      for (uchar i = 0; i < num_vars; i++)
        {
          if (usingTables)
            {
              ternNextState[i] = tables->at(i).getEntry(ternCurState);
            }
          else
            {
              ternNextState[i] = pds->at(i).evaluate(ternCurState);
            }
        }
      // convert back to decimal
      unsigned long result = 0;
      for (int i = 0; i < num_vars; i++)
        {
          result = (result * numStates[i])
              + (unsigned long) (ternNextState[i]);
        }
      *((int *) (mSHMptr + iterState)) = result;
    }

  delete[] ternCurState;
  delete[] ternNextState;

}

// PRE: all SHM segments have been allocated. childNum is defined as
// the child number of this subprocess. NumProccesses is the total
// number of processes created. cycleCountBlocks is the size of the
// block to compute the edges for in each subprocess. maxState is the
// size of the statespace. varsByUpdateSpeed, sizeVarsByUpdateSpeed
// and numUpdateSpeeds have all been defined
// POST: all edges between childNum * cycleCountBlocks and childNum +
// 1 * cycleCountBlocks have been computed and stored in the
// edgeArray using variable update speeds
void Cyclone::subprocessRunWithSpeeds(int childNum, int numProcesses,
                                      unlong cycleCountBlocks, unlong maxState)
{

  // initialize variables for the run
  uchar ** ternState = new uchar*[numUpdateSpeeds];
  uchar * ternNextState = new uchar[num_vars];

  for (int i = 0; i < numUpdateSpeeds; i++)
    {
      ternState[i] = new uchar[num_vars];
      for (int k = 0; k < num_vars; k++)
        {
          ternState[i][k] = 0;
        }
    }

  unlong startState = cycleCountBlocks * childNum;
  unlong endState = cycleCountBlocks * (childNum + 1);
  if (numProcesses - 1 == childNum)
    {
      endState = maxState;
    }

  for (unlong iterState = startState; (iterState < endState); iterState++)
    {
      // Convert to trinary and put in speed catagory 1 curState (the
      // slowest update speeed)
      unlong b = iterState;
      for (int i = num_vars - 1; i >= 0; i--)
        {
          ternState[1][i] = b % numStates[i];
          b /= numStates[i];
        }

      // Fill all current states with the speed 1 state
      for (int i = 0; i < numUpdateSpeeds; i++)
        {
          if (sizeVarsByUpdateSpeed[i] > 0)
            {
              for (int k = 0; k < num_vars; k++)
                {
                  ternState[i][k] = ternState[1][k];
                }
            }
        }

      // Compute multiple changes for faster-speed variables
      for (int i = numUpdateSpeeds - 1; i > 0; i--)
        { // No 0 Speed!
          for (int a = 0; a < num_vars; a++)
            {
              ternNextState[a] = ternState[i][a];
            }

          for (int k = numUpdateSpeeds - 1; k >= i; k--)
            {

              // Get next state from tables
              for (uchar m = 0; m < sizeVarsByUpdateSpeed[k]; m++)
                {
                  ternNextState[varsByUpdateSpeed[k][m]] = tables->at(
                      varsByUpdateSpeed[k][m]).getEntry(ternState[k]);
                }
              for (int a = 0; a < num_vars; a++)
                {
                  ternState[k][a] = ternNextState[a];
                }
            }

          for (int a = i; a < numUpdateSpeeds; a++)
            {
              for (int b = 0; b < num_vars; b++)
                {
                  ternState[a][b] = ternState[i][b];
                }
            }
        }

      // convert final result of all changes
      // convert back to decimal
      // ternCurState actually contains the next state, due to the swap
      // at the end of the loop above, but we're going to just avoid
      // reassigning the pointer again, because why bother?
      unsigned long result = 0;
      for (int i = 0; i < num_vars; i++)
        {
          result = (result * numStates[i])
              + (unsigned long) (ternNextState[i]);
        }
      *((int *) (mSHMptr + iterState)) = result;
    }

  delete[] ternState;
  delete[] ternNextState;

}

// PRE: maxState is defined as the size of the statespace. All SHM
// segments have been initialized. The edgeArray has been initialized
// with the edges for all states.
// POST: The checkedArray has been marked for each state with the
// number of the trajectory which visited it. 
// The associationsArray has been set for the associations between
// each trajectory. trajCount has been set to the number of
// trajectories produced.
unlong Cyclone::makeTrajectories(unlong maxState)
{
  unlong curState = 0;
  unlong prevState = 0;
  unlong cycleCount = 0;
  unlong trajCount = 1;
  vector<unlong> * path = new vector<unlong>();
  // Main run loop, iterates through all possible starting states and
  // records cycles

  for (unlong iterState = 0; (iterState < maxState); iterState++)
    {
      path->clear();
      prevState = curState;
      curState = iterState;
      if (checkedArray[curState] == 0)
        {
          // Continue traversal until hitting a checked state
          do
            {
              path->push_back(curState);
              checkedArray[curState] = trajCount;

              prevState = curState;
              curState = *((int *) (mSHMptr + curState));
            } while (checkedArray[curState] == 0);
        }

      // If this trajectory exists
      if (path->size() > 0)
        {
          // Create an association with the first state reached that has
          // already been reached

          if (trajCount == checkedArray[curState])
            {
              associations[trajCount] = trajCount;
              // vector<unlong> cycle;
              int indexOfCycle = getCycle(path, curState);
              // for (int i = indexOfCycle; i < path->size(); i++)
              //   {
              //     cycle.push_back(path->at(i));
              //   }
              cycleCount++;

              outputCycle(*path, indexOfCycle);
            }
          else
            {
              associations[trajCount] = associations[checkedArray[curState]];
            }
          trajCount++;
        }
    }

  getComponentSizes(cycleCount, trajCount);

  delete path;
  return cycleCount;
}

////// NOTE \\\\\
// This method is not used in the subprocessRun to increase efficiency
// PRE: curState is an integer <= numStates^num_vars - 1
// POST: the next state, according to the PDSs or Tables, is returned
unlong Cyclone::nextState(unlong curState, uchar temp1[], uchar temp2[])
{
  // Convert to trinary
  unsigned long b = curState;
  for (int i = num_vars - 1; i >= 0; i--)
    {
      temp1[i] = b % numStates[i];
      b /= numStates[i];
    }

  // Get the next value for each index based on the tables
  if (!usingSpeeds)
    {
      for (uchar i = 0; i < num_vars; i++)
        {
          if (usingTables)
            {
              temp2[i] = tables->at(i).getEntry(temp1);
            }
          else
            {
              temp2[i] = pds->at(i).evaluate(temp1);
            }
        }
    }
  else
    {
      uchar * tempSwapHolder;
      // Compute multiple changes for faster-speed variables
      for (int i = numUpdateSpeeds - 1; i > 0; i--)
        { // No 0 Speed!
          for (int k = numUpdateSpeeds - 1; k >= i; k--)
            {
              // Get next state from tables
              for (uchar m = 0; m < sizeVarsByUpdateSpeed[k]; m++)
                {
                  if (usingTables)
                    {
                      temp2[varsByUpdateSpeed[k][m]] = tables->at(
                          varsByUpdateSpeed[k][m]).getEntry(temp1);
                    }
                  else
                    {
                      temp2[m] = pds->at(m).evaluate(temp2);
                    }
                }
            }
          // Swap the pointers to avoid rewriting each entry in temp2
          // to temp1. Then use temp1's memory to write the
          // next state in the next iteration
          tempSwapHolder = &temp1[0];
          temp1 = &temp2[0];
          temp2 = tempSwapHolder;
        }
    }

  // Convert back to decimal
  unsigned long result = 0;
  for (int i = 0; i < num_vars; i++)
    {
      result = (result * numStates[i]) + (unsigned long) (temp2[i]);
    }

  return result;
}

// This method is not used in the subprocessRun to increase efficiency
// PRE: curState is an integer <= numStates^num_vars - 1, temp and
// temp2 are blank uchar arrays to store the current and next states
// in for converting back and forth. Knockouts is a bool array with
// true indicating a node in the network that should be treated as
// always = 0
// POST: the next state, according to the PDSs or Tables, is returned
unlong Cyclone::trajNextState(unlong curState, uchar temp1[], uchar temp2[],
                              bool knockouts[])
{
  for (int i = 0; i < num_vars; i++)
    {
      temp1[i] = 0;
      temp2[i] = 0;
    }
  // Convert to trinary
  unsigned long b = curState;
  for (int i = num_vars - 1; i >= 0; i--)
    {
      temp1[i] = b % numStates[i];
      b /= numStates[i];
    }

  for (uchar i = 0; i < num_vars; i++)
    {
      if (!knockouts[i])
        {
          if (usingTables)
            {
              temp2[i] = tables->at(i).getEntry(temp1);
            }
          else
            {
              temp2[i] = pds->at(i).evaluate(temp1);
            }
        }
      // If this node has been knocked out for this trajectory
      else
        {
          temp2[i] = 0;
        }
    }

  // Convert back to decimal
  unsigned long result = 0;
  for (int i = 0; i < num_vars; i++)
    {
      result = (result * numStates[i]) + (unsigned long) (temp2[i]);
    }

  return result;
}

// This method is not used in the subprocessRun to increase efficiency
// PRE: curState is an integer <= numStates^num_vars - 1, temp and
// temp2 are blank uchar arrays to store the current and next states
// in for converting back and forth. Knockouts is a bool array with
// true indicating a node in the network that should be treated as
// always = 0
// POST: the next state, according to the PDSs or Tables, is returned
unlong Cyclone::randomUpdateNextState(const unlong curState, uchar stateArray[],
                                      const bool knockouts[],
                                      const int varToChange)
{
  unlong result = curState;

  if (!knockouts[varToChange])
    {
      int valToSubtract = 0;
      int valueOfPosition = 1;
      int valToAdd = 0;

      unsigned long b = curState;
      for (int i = num_vars - 1; i >= 0; i--)
        {
          stateArray[i] = b % numStates[i];
          b /= numStates[i];
          if (i > varToChange)
            {
              valueOfPosition *= numStates[i];
            }
        }
      valToSubtract = stateArray[varToChange] * valueOfPosition;

      result -= valToSubtract;
      if (usingTables)
        {
          valToAdd = tables->at(varToChange).getEntry(stateArray)
              * valueOfPosition;
        }
      else
        {
          valToAdd = pds->at(varToChange).evaluate(stateArray)
              * valueOfPosition;
        }

      result += valToAdd;
    }
  // If this node has been knocked out for this trajectory

  return result;
}

// This method is not used in the subprocessRun to increase efficiency
// PRE: curState is an integer <= numStates^num_vars - 1, ternState
// is an array of blank state vectors to store the next state
// computated for each iteration of table lookups, ternNextState is a
// blank state vector to store the next state in. Knockouts is a bool
// array with true indicating a node in the network that should always
// be treated as 0.
// POST: the next state, according to the PDSs or Tables, is returned
void Cyclone::trajNextStateWithSpeeds(const unlong curState,
                                      uchar * ternCurState[],
                                      uchar * ternNextState[],
                                      unlong nextStates[],
                                      const bool knockouts[])
{

  // Convert to trinary and put in speed catagory 1 curState (the
  // slowest update speeed)
  unlong b = curState;
  for (int i = num_vars - 1; i >= 0; i--)
    {
      ternCurState[1][i] = b % numStates[i];
      b /= numStates[i];
    }

  // Fill all current states with the speed 1 state
  for (int i = 0; i <= numUpdateSpeeds; i++)
    {
      if (sizeVarsByUpdateSpeed[i] > 0)
        {
          for (int k = 0; k < num_vars; k++)
            {
              ternCurState[i][k] = ternCurState[1][k];
            }
        }
      printState(ternCurState[i]);
    }

  // <<<< ALGORITHM OUTLINE >>>> //
  // N updates will be run, where N = numUpdateSpeeds. In each update, there
  // will be a minimum speed, all variables equal or above this
  // minimum speed get updated by looking at the proper row of
  // curState (curState[this variable's speed]). Each new state is
  // copied into a row of NextState, which starts off =
  // curState[numUpdateSpeeds], which is the most up-to-date row. Therefore,
  // nextState[i] = curState[numUpdateSpeeds] for all variables too slow to
  // be updated, and is up to date for all variables updated in this
  // iteration.
  // This nextState is stored for returning as part of the
  // path. Additionally, it is deep-copied into each row of curState
  // with a high enough speed. So that all rows of curState with
  // speeds >= minimum speed now equal nextState. This is repeated
  // until done.
  // The minimum speed of an iteration is checked by the condition:
  // minSpeed % i == 0, so it will give a pattern (for speeds
  // 1-2-4-8-16) of:
  // 16 8 16 4 16 8 16 2 16 8 16 4 16 1

  // i is the update #, so on update 1 the fastest variable is
  // updated, on update 2 the two fastest, update 3 just the fastest,
  // 4 the three fastest, and so on

  int minSpeed = 0;
  for (int i = 1; i < numUpdateSpeeds; i++)
    {
      // fill the nextState entry about to be worked on with the most
      // recently updated curState, which is always the fastest row
      for (int a = 0; a < num_vars; a++)
        {
          ternNextState[i][a] = ternCurState[numUpdateSpeeds - 1][a];
        }

      for (int reciprocalMinSpeed = 1; i % reciprocalMinSpeed == 0;
          reciprocalMinSpeed *= 2)
        {
          minSpeed = (numUpdateSpeeds - 1) / reciprocalMinSpeed;
          for (int k = 0; k < sizeVarsByUpdateSpeed[minSpeed]; k++)
            {
              if (!knockouts[k])
                {
                  ternNextState[i][varsByUpdateSpeed[minSpeed][k]] = tables->at(
                      varsByUpdateSpeed[minSpeed][k]).getEntry(
                      ternCurState[minSpeed]);
                }
            }
        }

      // fill all rows in curState above minSpeed with the new NextState
      for (int m = minSpeed; m < numUpdateSpeeds; m *= 2)
        {
          for (int n = 0; n < num_vars; n++)
            {
              ternCurState[m][n] = ternNextState[i][n];
            }
        }

    }

  // convert all of nextStates back to decimal and put in nextStates
  for (int a = 0; a < numUpdateSpeeds; a++)
    {
      for (int b = 0; b < num_vars; b++)
        {
          nextStates[a] = (nextStates[a] * numStates[b])
              + (unsigned long) (ternNextState[a + 1][b]);
        }
    }
}

// PRE: array and size are defined and sort is declared
// POST: sort is of length = size and contains the index numbers of
// the array sorted
void Cyclone::sortComponents(unlong array[], int size)
{
  // Sort the size from largest to smallest

  sort = new int[size];

  for (int i = 0; i < size; i++)
    {
      sort[i] = i;
    }

  for (int i = 1; i < size; i++)
    {
      int temp = sort[i];
      for (int k = i - 1; k >= -1; k--)
        {
          if (k >= 0 && array[temp] > array[sort[k]])
            {
              sort[k + 1] = sort[k];
            }
          else
            {
              sort[k + 1] = temp;
              k = -2;
            }
        }
    }
}

// PRE: cycles is defined as the number of cycles in the graph,
// outBuffer also contains that many strings in its
// vector. Trajectories is the number of trajectories created during
// the makeTrajectories stage
// POST: gets the number of states in each cycle and concatonates that
// output into the outBuffer entry for the cycle
void Cyclone::getComponentSizes(unlong cycles, unlong trajectories)
{
  //// NOTE: I do not know why, but odd-numbered trajectories cause
  //// memory corruption so this simply adds 1 to it if it is
  //// odd. This should not affect the functionality except for
  //// wasting one address in memory and a loop iteration here or
  //// there.
  if (trajectories % 2 == 1)
    {
      trajectories += 1;
    }

  // Initialize temporary counting array
  unlong * stateCountByTraj = new unlong[trajectories];
  for (int i = 0; i <= trajectories; i++)
    {
      stateCountByTraj[i] = 0;
    }

  // Get the size of each component
  for (unlong i = 0; i < total_states; i++)
    {
      stateCountByTraj[associations[checkedArray[i]]]++;
    }

  // Condenses the count, which is located in |cycles| number of
  // indices in a |trajectories| length array, own to a |cycles| length
  // array
  unlong * condensedCount = new unlong[cycles];
  int k = 0;
  for (unlong i = 0; i < trajectories; i++)
    {
      if (stateCountByTraj[i] > 0)
        {
          condensedCount[k] = stateCountByTraj[i];
          k++;
        }
    }

  // Initialize the Sort array to contain the sorted order of indices
  // in condensed count
  sortComponents(condensedCount, cycles);

  // Concatonate output
  for (int i = 0; i < cycles; i++)
    {
      stringstream temp;
      double percentage;
      percentage = ((long double) (100 * condensedCount[sort[i]]))
          / total_states;
      temp << condensedCount[sort[i]];
      stringstream conv;
      conv << showpoint << percentage;
      stringstream ival;
      ival << i+1;
      //outBuffer.at(sort[i]) = "\nLIMIT CYCLE:\nComponent Size: " + temp.str()
      //    + "(" + conv.str() + " %)" + "\n" + outBuffer.at(sort[i]);
      outBuffer.at(sort[i]) = "\nCOMPONENT " + ival.str() + ":\n"
              + "Size = " + temp.str() + "(" + conv.str() + " %)\n" 
              + "Limit Cycle:\n" + outBuffer.at(sort[i]);
    }
  //  delete condensedCount;
  //  delete stateCountByTraj;
}

// PRE: cycle is a vector consisting of all the states in the cycle
// POST: each state in the cycle is added to the output buffer
void Cyclone::outputCycle(vector<unlong> & cycle)
{

  stringstream output;
  output << "Length = " << cycle.size() << endl;

  uchar * temp = new uchar[num_vars];

  for (unlong i = 0; i < cycle.size(); i++)
    {
      // Reset temp array
      for (int k = 0; k < num_vars; k++)
        {
          temp[k] = 0;
        }

      // get the state in trinary form
      decimalToTernary(cycle.at(i), temp, numStates, num_vars);

      // output the state
      output << "States = [ ";
      for (int k = 0; k < num_vars; k++)
        {
          output << (short) temp[k];
          output << " ";
        }
      output << "]\n";
    }

  outBuffer.push_back(output.str());
  delete[] temp;
}

// PRE: cycle is a vector consisting of all the states in the cycle
// POST: each state in the cycle is added to the output buffer
void Cyclone::outputCycle(vector<unlong> & path, int indexOfPath)
{

  stringstream output;
  output << "Length = " << path.size() - indexOfPath << endl;

  uchar * temp = new uchar[num_vars];

  for (unlong i = indexOfPath; i < path.size(); i++)
    {
      // Reset temp array
      for (int k = 0; k < num_vars; k++)
        {
          temp[k] = 0;
        }

      // get the state in trinary form
      decimalToTernary(path.at(i), temp, numStates, num_vars);

      // output the state
      output << "States = [ ";
      for (int k = 0; k < num_vars; k++)
        {
          output << (short) temp[k];
          output << " ";
        }
      output << "]\n";
    }

  outBuffer.push_back(output.str());
  delete[] temp;
}

// PRE: outBuffers are filled with the limit cycles and component
// sizes. filename is the name of the output file. writeToFile and
// verbose are defined to determine whether to output to a file and/or
// the command line.
// POST: the outBuffers have been output to command line or an output
// file
void Cyclone::writeOutput(string filename)
{
  if (writeToFile)
    {
      fileOut = new ofstream(filename.c_str());
    }
  if (writeToFile)
    {
      *fileOut << "MODEL NAME: " << modelname << endl;
      *fileOut << "SIMULATION NAME: " << runname << endl;
    }
  if (verbose)
    {
      cout << "MODEL NAME: " << modelname << endl;
      cout << "SIMULATION NAME: " << runname << endl;
    }
  if (writeToFile)
    {
      *fileOut << "SIZE OF STATE SPACE: " << total_states << endl << endl;
      *fileOut << "NUMBER OF CYCLES: " << outBuffer.size() << endl;
    }
  if (verbose)
    {
      cout << "SIZE OF STATE SPACE: " << total_states << endl << endl;
      cout << "NUMBER OF CYCLES: " << outBuffer.size() << endl;
    }
  for (int i = 0; i < outBuffer.size(); i++)
    {
      if (writeToFile)
        {
          *fileOut << outBuffer.at(sort[i]);
        }
      if (verbose)
        {
          cout << outBuffer.at(sort[i]);
        }
    }
  if (writeToFile)
    {
      *fileOut << "\nDONE" << endl;
      *fileOut << "<NOTES>\n</NOTES>\n" << endl;
    }
  if (verbose)
    {
      cout << "\nDONE" << endl;
      cout << "<NOTES>\n</NOTES>\n" << endl;
    }

  if (includeTables)
    {
      outputTables();
    }

  if (writeToFile)
    {
      fileOut->close();
    }
}

// PRE: This cyclone object is defined
// POST: this cyclone object has been deleted along with all its
// tables or PDSs
Cyclone::~Cyclone()
{
  if (usingTables)
    {
      tables->clear();
      delete tables;
    }
  else
    {
      pds->clear();
      delete pds;
    }
}

// PRE: writeFile and filename are defined. The tables or PDSs have
// been initialized.
// POST: the state transitioned to by each state has been computed and
// stored in the edges array.
// This array has then been output in two column format with the first
// column being the origin state number and the second column the
// state transitioned to.
void Cyclone::generateEdges(bool writeFile, string filename, bool pverbose,
                            bool pincludeTables)
{
  initializeSHM(total_states);

  fillEdgeArray(1);

  verbose = pverbose;
  writeToFile = writeFile;
  includeTables = pincludeTables;

  ofstream fileOut(filename.c_str());

  unlong curState = 0;
  unlong nextState = 0;

  uchar * CurrentState = new uchar[num_vars];

  unlong maxState = numStates[0];
  for (unlong i = 1; i < num_vars; i++)
    {
      maxState *= numStates[i];
    }

  // Main run loop, iterates through all possible starting states and
  // records cycles

  if (writeToFile)
    {
      fileOut << "digraph test {" << std::endl;
    }

  if (verbose)
    {
      cout << "digraph test {" << std::endl;
    }

  for (curState = 0; curState < maxState; curState++)
    {
      decimalToTernary(curState, CurrentState, numStates, num_vars);

      if (writeToFile)
        {
          fileOut << "node" << curState << " [label=\"";

          for (int k = 0; k < num_vars; k++)
            {
              fileOut << " " << (short) CurrentState[k];
            }

          fileOut << "\"];" << endl;
        }

      if (verbose)
        {
          cout << "node" << curState << " [label=\"";

          for (int k = 0; k < num_vars; k++)
            {
              cout << " " << (short) CurrentState[k];
            }

          cout << "\"];" << endl;
        }
    }

  for (curState = 0; curState < maxState; curState++)
    {
      nextState = *((int *) (mSHMptr + curState));

      // output the state
      if (writeToFile)
        {
          fileOut << "node" << curState << " -> node" << nextState << endl;
        }

      if (verbose)
        {
          cout << "node" << curState << " -> node" << nextState << endl;
        }
    }

  if (writeToFile)
    {
      fileOut << "}" << std::endl;
      fileOut.close();
    }

  if (verbose)
    {
      cout << "}" << std::endl;
    }

 delete[] CurrentState;
}

// PRE: writeFile, maxState, edges, and filename are all defined.
// POST: All the edges are output to the command line or the provided
// filename
void Cyclone::outputEdges(unlong maxState, unsigned int * edges,
                          string filename)
{

  if (writeToFile)
    {
      fileOut = new ofstream(filename.c_str());
    }

  for (unlong i = 0; i < maxState; i++)
    {
      if (writeToFile)
        {
          *fileOut << i << " " << edges[i] << endl;
        }
      if (verbose)
        {
          cout << i << " " << edges[i] << endl;
        }
    }

  if (writeToFile)
    {
      fileOut->close();
    }
  //delete fileOut;
}

// PRE: inFile is defined as the input file  containing a
// trajectory correctly formatted. Trajectory is an array of uchars
// |num_vars|
// POST: trajectory has been filled with the starting trajectory
// contained in the file of inFilename
void Cyclone::readTrajectory(ifstream& inFile, uchar trajectory[])
{
  char * inputLine = new char[LINESIZE];

  inFile.getline(inputLine, LINESIZE);
  // Skip the modelname

  inFile.getline(inputLine, LINESIZE);
  runname = inputLine;
  while (inputLine[0] != '[')
    {
      inFile.getline(inputLine, LINESIZE);
    }

  // fill trajectory, the input should be formatted as:
  // [ 0 1 2 3 ]     the input
  // 0123456789      indices in inputLine
  int trailingIndex = 1;
  int leadingIndex = 0;
  char* numString = new char[LINESIZE];
  while (inputLine[trailingIndex] == '\t' || inputLine[trailingIndex] == ' ')
    {
      trailingIndex++;
    }
  leadingIndex = trailingIndex;
  while (inputLine[leadingIndex] != '\t' && inputLine[leadingIndex] != ' '
      && inputLine[leadingIndex] != ']')
    {
      leadingIndex++;
    }
  for (int i = 0; inputLine[leadingIndex] != ']'; i++)
    {
      int k = 0;
      for (; k < leadingIndex - trailingIndex; k++)
        {
          numString[k] = inputLine[trailingIndex + k];
        }
      numString[k] = '\0';
      trajectory[i] = atoi(numString);
      trailingIndex = leadingIndex;
      while (inputLine[trailingIndex] == '\t' || inputLine[trailingIndex] == ' ')
        {
          trailingIndex++;
        }
      leadingIndex = trailingIndex;
      while (inputLine[leadingIndex] != '\t' && inputLine[leadingIndex] != ' '
          && inputLine[leadingIndex] != ']')
        {
          leadingIndex++;
        }
    }
  delete[] inputLine;
}

// PRE: path is defined and empty, trajStart is defined and filled
// with the start state for the trajectory run
// POST: path is filled with the decimal states of the trajectory
// run. RV = the index of path where the limit cycle begins
int Cyclone::runTrajectory(vector<unlong> * path, uchar trajStart[])
{

  unlong startState = 0;
  uchar * startWithKOsRemoved = new uchar[num_vars];
  bool * knockouts = new bool[num_vars];

  for (int i = 0; i < num_vars; i++)
    {
      if (trajStart[i] < numStates[i])
        {
          startWithKOsRemoved[i] = trajStart[i];
          knockouts[i] = false;
        }
      else
        {
          startWithKOsRemoved[i] = 0;
          knockouts[i] = true;
        }
    }
  startState = ternaryToDecimal(num_vars, startWithKOsRemoved, numStates);
  unlong curState = startState;
  int cycleStart = -1;

  // run until a cycle is found
  if (!usingSpeeds)
    {
      uchar * ternCurState = new uchar[num_vars];
      uchar * ternNextState = new uchar[num_vars];
      while (cycleStart == -1)
        {
          path->push_back(curState);
          curState = trajNextState(curState, ternCurState, ternNextState,
                                   knockouts);
          cycleStart = getCycle(path, curState);
        }
      delete[] ternCurState;
      delete[] ternNextState;
    }

  // using speeds
  else
    {
      cycleStart = runTrajectoryWithSpeeds(path, curState, knockouts);
    }

  return cycleStart;
}

int Cyclone::runTrajectoryWithSpeeds(vector<unlong> * path, unlong startState,
                                     bool knockouts[])
{
  uchar ** ternCurStates = new uchar*[numUpdateSpeeds + 1];
  uchar ** ternNextStates = new uchar*[numUpdateSpeeds + 1];
  unlong * nextStates = new unlong[numUpdateSpeeds + 1];
  int cycleStart = -1;
  unlong curState = startState;

  for (int i = 0; i <= numUpdateSpeeds; i++)
    {
      ternCurStates[i] = new uchar[num_vars];
      ternNextStates[i] = new uchar[num_vars];
      nextStates[i] = 0;
      for (int k = 0; k < num_vars; k++)
        {
          ternCurStates[i][k] = 0;
          ternNextStates[i][k] = 0;
        }
    }

  path->push_back(curState);
  while (cycleStart == -1)
    {
      trajNextStateWithSpeeds(curState, ternCurStates, ternNextStates,
                              nextStates, knockouts);
      curState = nextStates[numUpdateSpeeds - 2];
      cycleStart = getCycle(path, curState);

      for (int i = 0; i < numUpdateSpeeds - 1; i++)
        {
          path->push_back(nextStates[i]);
          nextStates[i] = 0;
        }
    }
  path->pop_back();

  delete[] ternCurStates;
  delete[] ternNextStates;
  delete[] nextStates;

  return cycleStart;
}

// PRE: path is defined and empty, trajStart is defined and filled
// with the start state for the trajectory run. StepsToRun is defined
// POST: path is filled with the decimal states of the trajectory up
// to StepsToRun number of steps.
void Cyclone::runRandomUpdateTrajectory(vector<unlong> * path,
                                        const uchar trajStart[],
                                        const int stepsToRun)
{

  unlong startState = 0;
  uchar * startWithKOsRemoved = new uchar[num_vars];
  bool * knockouts = new bool[num_vars];

  for (int i = 0; i < num_vars; i++)
    {
      if (trajStart[i] < numStates[i])
        {
          startWithKOsRemoved[i] = trajStart[i];
          knockouts[i] = false;
        }
      else
        {
          startWithKOsRemoved[i] = 0;
          knockouts[i] = true;
        }
    }
  startState = ternaryToDecimal(num_vars, startWithKOsRemoved, numStates);
  unlong curState = startState;

  // run until a cycle is found
  uchar * ternCurState = new uchar[num_vars];

  for (int i = 0; i < stepsToRun; i++)
    {
      int varToChange = rand() % num_vars;
      path->push_back(curState);
      curState = randomUpdateNextState(curState, ternCurState, knockouts,
                                       varToChange);
    }
  path->push_back(curState);

  delete[] ternCurState;
}

// PRE: path and state are defined.
// POST: RV = -1 if state is not in path. Otherwise, RV = the index of
// state in path
int Cyclone::getCycle(vector<unlong> * path, unlong state)
{
  int result = -1;
  for (int i = 0; i < path->size() && result == -1; i++)
    {
      if (state == path->at(i))
        {
          result = i;
        }
    }
  return result;
}

// PRE: outFile has been successfully opened, verbose and writeToFile
// have been defined
// POST: the tables has been output to command line (if verbose) and
// outFile (if writeToFile)
void Cyclone::outputTables()
{
  if (verbose) {
      cout << "MODEL NAME: " << modelname << endl ;
      cout << "SIMULATION NAME: " << runname << endl ;
      cout << "NUMBER OF VARIABLES: " << num_vars << endl << endl;
      cout << "VARIABLE NAMES: ";
      for (int i=0; i < num_vars; i++) {
          cout << varNamesVector->at(i) << " ";
      } cout << endl << endl;

      cout << "NUMBER OF STATES: ";
      for (int i = 0; i < tables->size(); i++) {
          cout << numStates[i] << " ";
      } cout << endl << endl;

      cout << "SPEED OF VARIABLES: "; 
      for (int i = 0; i < tables->size(); i++) {
          if (usingSpeeds) {
              cout << varSpeeds[i] << " ";
          }
      } cout << endl << endl;
      
      for (int i = 0; i < tables->size(); i++) {
          tables->at(i).printTable(cout, varNamesVector);
          cout << endl << endl;
      }
  }
    if (writeToFile) {
      *fileOut << "MODEL NAME: " << modelname << endl;
      *fileOut << "SIMULATION NAME: " << runname << endl;
      *fileOut << "NUMBER OF VARIABLES: " << num_vars << endl << endl;
      *fileOut << "VARIABLE NAMES: ";
      for (int i=0; i < num_vars; i++) {
          *fileOut << varNamesVector->at(i) << " ";
      } *fileOut << endl << endl;
      
      *fileOut << "NUMBER OF STATES: ";
      for (int i = 0; i < tables->size(); i++) {
          *fileOut << numStates[i] << " ";
      } *fileOut << endl << endl;

      *fileOut << "SPEED OF VARIABLES: "; 
      for (int i = 0; i < tables->size(); i++) {
          if (usingSpeeds) {
              *fileOut << varSpeeds[i] << " ";
          }
      } *fileOut << endl << endl;
      
      for (int i = 0; i < tables->size(); i++) {
          *fileOut << "STATE TRANSITION TABLE for " << varNamesVector->at(i) << ":\n";
          tables->at(i).printTable(*fileOut, varNamesVector);
          *fileOut << endl << endl;
      }
    }
  
}

// PRE: path is defined as the completed trajectory. CycleIndex is the
// index at which the limit cycle portion of the trajectory
// begins. writeFile and verbose are booleans specifying whether or
// not to write the output to the commandline and/or an ouput file,
// outFilename specifies the name of the output file. tableFilename
// specifies the name of the tables file used as the state space for
// the trajectory
// POST: path is output to the commandline and/or an output file
// broken into a nonlooping and limit cycle section
void Cyclone::outputTrajectory(vector<unlong> * path, int cycleIndex,
                               string outFilename, string tableFilename)
{
  uchar * ternState = new uchar[num_vars];
  for (int i = 0; i < num_vars; i++)
    {
      ternState[i] = 0;
    }

  if (writeToFile)
    {
      fileOut = new ofstream(outFilename.c_str());
    }

  if (verbose)
    {
      cout << modelname << endl;
      cout << runname << endl;
      cout << "TRAJECTORY" << endl;
      cout << "NONLOOPING: " << endl;
      cout << "Length: " << cycleIndex << endl;
    }
  if (writeToFile)
    {
      *fileOut << modelname << endl;
      *fileOut << runname << endl;
      *fileOut << "TRAJECTORY" << endl;
      *fileOut << "NONLOOPING: " << endl;
      *fileOut << "Length: " << cycleIndex << endl;
    }
  for (int i = 0; i < cycleIndex; i++)
    {
      decimalToTernary(path->at(i), ternState, numStates, num_vars);

      if (verbose)
        {
          cout << "[ ";
        }
      if (writeToFile)
        {
          *fileOut << "[ ";
        }
      for (int j = 0; j < num_vars; j++)
        {
          if (verbose)
            {
              cout << (int) ternState[j] << ' ';
            }
          if (writeToFile)
            {
              *fileOut << (int) ternState[j] << ' ';
            }
        }
      if (verbose)
        {
          cout << "]" << endl;
        }
      if (writeToFile)
        {
          *fileOut << "]" << endl;
        }
    }
  if (verbose)
    {
      cout << "LIMIT CYCLE: " << endl;
      cout << "Length: " << path->size() - cycleIndex << endl;
    }
  if (writeToFile)
    {
      *fileOut << "LIMIT CYCLE: " << endl;
      *fileOut << "Length: " << path->size() - cycleIndex << endl;
    }
  for (int i = cycleIndex; i < path->size(); i++)
    {
      decimalToTernary(path->at(i), ternState, numStates, num_vars);

      if (verbose)
        {
          cout << "[ ";
        }
      if (writeToFile)
        {
          *fileOut << "[ ";
        }
      for (int j = 0; j < num_vars; j++)
        {
          if (verbose)
            {
              cout << (int) ternState[j] << ' ';
            }
          if (writeToFile)
            {
              *fileOut << (int) ternState[j] << ' ';
            }
        }
      if (verbose)
        {
          cout << "]" << endl;
        }
      if (writeToFile)
        {
          *fileOut << "]" << endl;
        }
    }
  if (verbose)
    {
      cout << "TABLE SOURCE: " << tableFilename << endl;
      cout << "<NOTES>\n</NOTES>" << endl;
    }
  if (writeToFile)
    {
      *fileOut << "TABLE SOURCE: " << tableFilename << endl;
      *fileOut << "<NOTES>\n</NOTES>" << endl;
    }

  if (includeTables)
    {
      outputTables();
    }

  if (writeToFile)
    {
      fileOut->close();
    }
}

// PRE: path is an array of numRuns number of vectors, each vector
// represents a path resulting from a random update trajectory run of
// numSteps length. writeFile, *fileOut, verbose, and tableFilename are
// all defined
// POST: path is output as a random trajectory to command line or
// output file depending on parameters verbose and writefile.
void Cyclone::outputRandomTrajectory(vector<unlong> * path[],
                                     string fileOutname, string tableFilename,
                                     int numSteps, int numRuns)
{
  uchar * ternState = new uchar[num_vars];
  for (int i = 0; i < num_vars; i++)
    {
      ternState[i] = 0;
    }

  if (writeToFile)
    {
      fileOut = new ofstream(fileOutname.c_str());
    }

  if (verbose)
    {
      cout << modelname << endl;
      cout << runname << endl;
      cout << "RANDOM TRAJECTORY" << endl;
      cout << "Runs: " << numRuns << endl;
      cout << "Steps: " << numSteps << endl;
    }
  if (writeToFile)
    {
      *fileOut << modelname << endl;
      *fileOut << runname << endl;
      *fileOut << "RANDOM TRAJECTORY" << endl;
      *fileOut << "Runs: " << numRuns << endl;
      *fileOut << "Steps: " << numSteps << endl;
    }
  for (int g = 0; g < numRuns; g++)
    {
      if (verbose)
        {
          cout << "RUN: " << g << endl;
        }
      if (writeToFile)
        {
          *fileOut << "RUN: " << g << endl;
        }
      for (int i = 0; i < path[g]->size(); i++)
        {
          decimalToTernary(path[g]->at(i), ternState, numStates, num_vars);

          if (verbose)
            {
              cout << "[ ";
            }
          if (writeToFile)
            {
              *fileOut << "[ ";
            }
          for (int j = 0; j < num_vars; j++)
            {
              if (verbose)
                {
                  cout << (int) ternState[j] << ' ';
                }
              if (writeToFile)
                {
                  *fileOut << (int) ternState[j] << ' ';
                }
            }
          if (verbose)
            {
              cout << "]" << endl;
            }
          if (writeToFile)
            {
              *fileOut << "]" << endl;
            }
        }
    }
  if (verbose)
    {
      cout << "TABLE SOURCE: " << tableFilename << endl;
    }
  if (writeToFile)
    {
      *fileOut << "TABLE SOURCE: " << tableFilename << endl;
    }

  if (includeTables)
    {
      outputTables();
    }
  if (writeToFile)
    {
      fileOut->close();
    }
}

// PRE: writeFile, outFilename, verbose, inFilename are all
// defined. inFilename identifies a file with a correctly formatted
// trajectory start entry in it. This Cyclone object is
// defined. tableFilename is the filename for the file used to create
// the tables
// POST: the path of the trajectory start to and through its limit
// cycle are output either to command line, an output file, or both
void Cyclone::generateTrajectory(bool writeFile, string outFilename,
                                 bool pverbose, string inFilename,
                                 string tableFilename, bool pincludeTables)
{
  ifstream inFile(inFilename.c_str());
  verbose = pverbose;
  writeToFile = writeFile;
  includeTables = pincludeTables;

  if (inFile.is_open())
    {
      uchar * trajStart = new uchar[num_vars];
      readTrajectory(inFile, trajStart);
      vector<unlong> * path = new vector<unlong>();
      int cycleIndex = 0;
      cycleIndex = runTrajectory(path, trajStart);
      outputTrajectory(path, cycleIndex, outFilename, tableFilename);

      delete path;
    }
  else
    {
      cerr << "Input Trajectory File not found" << endl;
    }
}

// PRE: writeFile, outFilename, verbose, inFilename are all
// defined. inFilename identifies a file with a correctly formatted
// trajectory start entry in it. This Cyclone object is
// defined. tableFilename is the filename for the file used to create
// the tables. numRuns is defined and determines the number of runs of
// this trajectory to perform, numSteps is defined and determines how
// many steps are in each run.
// POST: the path of the trajectory through numSteps number of steps
// where each step is the update of a single randomly chosen variable
// has been created numRuns times and output according to the vrbose
// and writeFile parameters
void Cyclone::generateRandomTrajectory(bool writeFile, string outFilename,
                                       bool pverbose, string inFilename,
                                       string tableFilename, int numRuns,
                                       int numSteps, bool pincludeTables)
{
  ifstream inFile(inFilename.c_str());
  verbose = pverbose;
  writeToFile = writeFile;
  includeTables = pincludeTables;

  if (inFile.is_open())
    {
      uchar * trajStart = new uchar[num_vars];
      readTrajectory(inFile, trajStart);
      vector<unlong> ** path = new vector<unlong> *[numRuns];

      for (int i = 0; i < numRuns; i++)
        {
          path[i] = new vector<unlong>();
          runRandomUpdateTrajectory(path[i], trajStart, numSteps);
        }
      outputRandomTrajectory(path, outFilename, tableFilename, numSteps,
                             numRuns);

      delete[] path;
    }
  else
    {
      cerr << "Input Trajectory File not found" << endl;
    }
}
