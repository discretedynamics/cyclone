#ifndef CYCLONE_CYCLONE
#define CYCLONE_CYCLONE

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <stdlib.h>

#include "constants.h"
#include "Table.h"
#include "PDS.h"

using namespace std;

// Paul Vines
// 2012-3-4
// The cyclone class iterates over the state space of a graph input
// into it as either a system of polynomial equations or a series of
// tables. This iteration then produces all limit cycles in the state
// space and outputs these, along with the percentage of the state
// space which is a part of each component of the graph.

class Cyclone{

 private:

  /**
   *
   * INPUT FUNCTIONS
   *
   **/
  // PRE: Input is a valid string which may contain spaces or tabs
  // POST: Input is returned with all spaces and tabs removed
  string removeSpaces(string input);
    
  // PRE: input contains a header with the number of variables
  // POST: the num_vars has been extracted and the input has had the
  // header removed
  string parseHeader(string& input);

  int findNewLine(string& str);

  // PRE: input is defined as containing a series of n tables, headed by
  // the number n at the beginning
  // POST: Table objects have been constructed and added to the tables
  // array by parsing input
  void parseTableInput(string& input);

  // PRE: filename contains the name of a file with tables
  // POST: the file has been read and its tables constructed into Table
  // objects in the tables array
  void readTableInput(string filename);
	

  // PRE: input contains all the file contents
  // POST: pds's have been constructed for each polynomial in the input
  // string
  void parseInput(string input);

  // PRE: filename contains the name of a valid input file
  // POST: the number of variables in the input is returned, the input
  // file has been extracted to a string and passed to parseInput and
  // the PDSs constructed
  void readInput(string filename);

  // PRE: inFile is defined as the input file  containing a
  // trajectory correctly formatted. Trajectory is an array of uchars
  // |num_vars|
  // POST: trajectory has been filled with the starting trajectory
  // contained in the file of inFilename
  void readTrajectory(ifstream& inFile, uchar trajectory[]);

  /**
   *
   * DEBUGGING FUNCTIONS
   *
   **/
  // PRE: PDSs are defined
  // POST: The PDSs are output in the same format as input for
  // comparison purposes
  void printPDS();


  // PRE: curState is a valid state in state space
  // POST: curState is output in its array form
  void printState(unlong curState);

  // PRE: curState is a valid state in state space
  // POST: curState is output in its array form
  void printState(uchar * state);

  /**
   *
   * MULTIPROCESS FULL SIMULATION FUNCTIONS
   *
   **/ 
  // PRE: cores is defined as the number of subprocesses to make. PDSs
  // or Tables are defined and ready to be used. SHM is initialized to
  // the proper size for use.
  // POST: All limit cycles and component sizes have been found and
  // stored in the output buffers for output to the command line or
  // file.
  void run(int cores);

  // PRE: no shared memory segments have been created
  // POST: all shared memory segments have been created and their
  // addresses assigned to the appropriate variables
  void initializeSHM(unlong total_states);

  // PRE: SHM segments are allocated
  // POST: all SHM segments have been detached and removed
  void removeSHM();

  // PRE: num_vars and unsortedSpeeds are both defined, 
  // |unsortedSpeeds| = num_vars
  // POST: varsByUpdateSpeed, sizeVarsByUpdateSpeed, and numUpdateSpeeds
  // have all been initialized by parsing unsortedSpeeds
  void initializeUpdateSpeeds(const int * unsortedSpeeds);

  // PRE: all SHM segments have been allocated. childNum is defined as
  // the child number of this subprocess. NumProccesses is the total
  // number of processes created. cycleCountBlocks is the size of the
  // block to compute the edges for in each subprocess. maxState is the
  // size of the statespace.  
  // POST: all edges between childNum * cycleCountBlocks and childNum +
  // 1 * cycleCountBlocks have been computed and stored in the
  // edgeArray.
  void subprocessRun(int childNum, int numProcesses, unlong cycleCountBlocks, unlong maxState);


  // PRE: all SHM segments have been allocated. childNum is defined as
  // the child number of this subprocess. NumProccesses is the total
  // number of processes created. cycleCountBlocks is the size of the
  // block to compute the edges for in each subprocess. maxState is the
  // size of the statespace. varsByUpdateSpeed, sizeVarsByUpdateSpeed
  // and numUpdateSpeeds have all been defined
  // POST: all edges between childNum * cycleCountBlocks and childNum +
  // 1 * cycleCountBlocks have been computed and stored in the
  // edgeArray using variable update speeds
  void subprocessRunWithSpeeds(int childNum, int numProcesses, unlong cycleCountBlocks, unlong maxState);


  // PRE: maxState is defined as the size of the statespace. All SHM
  // segments have been initialized. The edgeArray has been initialized
  // with the edges for all states.
  // POST: The checkedArray has been marked for each state with the
  // number of the trajectory which visited it. 
  // The associationsArray has been set for the associations between
  // each trajectory. trajCount has been set to the number of
  // trajectories produced.
  unlong makeTrajectories(unlong maxState);

  ////// NOTE \\\\\
  // This method is not used in the subprocessRun to increase efficiency
  // PRE: curState is an integer <= num_states^num_vars - 1
  // POST: the next state, according to the PDSs or Tables, is returned
  unlong nextState(unlong curState, uchar temp[], uchar temp2[]);


  // This method is not used in the subprocessRun to increase efficiency
  // PRE: curState is an integer <= num_states^num_vars - 1, temp and
  // temp2 are blank uchar arrays to store the current and next states
  // in for converting back and forth. Knockouts is a bool array with
  // true indicating a node in the network that should be treated as
  // always = 0
  // POST: the next state, according to the PDSs or Tables, is returned
  unlong trajNextState(unlong curState, uchar temp1[], uchar temp2[], bool knockouts[]);


  // PRE: curState is defined, stateArray is of length = num_vars,
  // knockouts is defined and is of length = num_vars, varToChange is
  // defined and is < num_vars
  // POST: the next state has been produced by updating varToChange
  unlong randomUpdateNextState( const unlong curState, uchar stateArray[], const bool knockouts[], const int varToChange);


// This method is not used in the subprocessRun to increase efficiency
// PRE: curState is an integer <= num_states^num_vars - 1, ternState
// is an array of blank state vectors to store the next state
// computated for each iteration of table lookups, ternNextState is a
// blank state vector to store the next state in. Knockouts is a bool
// array with true indicating a node in the network that should always
// be treated as 0.
// POST: the next state, according to the PDSs or Tables, is returned
void  trajNextStateWithSpeeds(const unlong curState, uchar * ternState[], uchar * ternNextState[], unlong nextStates[] ,const bool knockouts[]);


  // PRE: array and size are defined and sort is declared
  // POST: sort is of length = size and contains the index numbers of
  // the array sorted
  void sortComponents(unlong array[], int size);

  // PRE: cycles is defined as the number of cycles in the graph,
  // outBuffer also contains that many strings in its vector. Passes is
  // the number of passes performed in the resolveTrajectory stage
  // POST: gets the number of states in each cycle and concatonates that
  // output into the outBuffer entry for the cycle
  void getComponentSizes(unlong cycles, unlong passes);

  // PRE: sharedTrajCount has been created with SHM and initialized to a
  // positive integer.
  // POST: the sharedTrajCount is returned and the shared version incremented
  // by one
  int getTrajCount();

  // PRE: path is defined and empty, trajStart is defined and filled
  // with the start state for the trajectory run
  // POST: path is filled with the decimal states of the trajectory
  // run. RV = the index of path where the limit cycle begins
  int runTrajectory(vector<unlong> * path, uchar trajStart[]);


  int runTrajectoryWithSpeeds(vector<unlong> * path, unlong startState, bool knockouts[]);


  // PRE: path is defined and empty, trajStart is defined and filled
  // with the start state for the trajectory run. StepsToRun is defined
  // POST: path is filled with the decimal states of the trajectory up
  // to StepsToRun number of steps.
  void runRandomUpdateTrajectory(vector<unlong> * path, const uchar trajStart[], const int stepsToRun);

  // PRE: path and state are defined.
  // POST: RV = -1 if state is not in path. Otherwise, RV = the index of
  // state in path
  int getCycle(vector<unlong> * path, unlong state);

  /**
   *
   * OUTPUT FUNCTIONS
   *
   **/// PRE: path is defined as the completed trajectory. CycleIndex is the
  // index at which the limit cycle portion of the trajectory
  // begins. writeFile and verbose are booleans specifying whether or
  // not to write the output to the commandline and/or an ouput file,
  // outFilename specifies the name of the output file. tableFilename
  // specifies the name of the tables file used as the state space for
  // the trajectory
  // POST: path is output to the commandline and/or an output file
  // broken into a nonlooping and limit cycle section
  void outputTrajectory(vector<unlong> * path, int cycleIndex, string outFilename, string tableFilename);

  // PRE: path is an array of numRuns number of vectors, each vector
  // represents a path resulting from a random update trajectory run of
  // numSteps length. writeFile, outFile, verbose, and tableFilename are
  // all defined
  // POST: path is output as a random trajectory to command line or
  // output file depending on parameters verbose and writefile.
  void outputRandomTrajectory(vector<unlong> * path[], string outFilename, string tableFilename, int numSteps, int numRuns);

  // PRE: cycle is a vector consisting of all the states in the cycle
  // POST: each state in the cycle is added to the output buffer
  void outputCycle(vector<unlong> & cycle);
  void outputCycle(vector<unlong> & path, int indexOfPath);

  // PRE: outBuffers are filled with the limit cycles and component
  // sizes. filename is the name of the output file. writeToFile and
  // verbose are defined to determine whether to output to a file and/or
  // the command line.
  // POST: the outBuffers have been output to command line or an output
  // file
  void writeOutput(string filename);

  // PRE: writeFile, maxState, edges, and filename are all defined.
  // POST: All the edges are output to the command line or the provided
  // filename
  void outputEdges(unlong maxState, unsigned int * edges, string filename);

  // PRE: outFile has been successfully opened, verbose and writeToFile
  // have been defined
  // POST: the tables has been output to command line (if verbose) and
  // outFile (if writeToFile)
  void outputTables();

  /**
   *
   * CLASS VARIABLES
   *
   **/

  key_t mSHMKey;
  int mSHMId;
  int * mSHMptr;

  unlong * associations; // an array of length total_states+1.

  vector<PDS> * pds; // vector of polynomial equations for each variable

  vector<Table> * tables; // vector of tables
 
  unlong state; // the current state

  unlong total_states; // total state space size

  unlong * numStates; // number of possibilities for states [0,1,2],
  // with an entry for each table/PDS

  unlong num_processes; // number of subprocesses being made
  
  unlong my_rank; // mpi rank within the cluster execution

  unsigned int num_vars; // number of variables in the total system

  ofstream * fileOut; // output file pointer

  vector<string> outBuffer; // Vector of bufffers for storing the
  // output cycles until the run finishes
 
  string modelname, runname;
  
  vector<string> * varNamesVector;

  int * varSpeeds; // speed-by-variable

  bool usingTables; // Whether this cyclone will be using tables or
  // polynomials

  bool writeToFile; // Whether the output is to be written to a file

  bool verbose; // whether the output is to be made to the command line or not

  bool includeTables; // Whether to output the tables used as well

  int * sort; // the sorted indices of the component counts

  unlong * checkedArray; 

  int ** varsByUpdateSpeed; // 2D array of variables in each update
			    // speed: [0][var1 var2 var3] = variables
			    // of speed class 0
  int * sizeVarsByUpdateSpeed; // array of the actual filled size of
  // each subarray in varsByUpdateSpeed
  int numUpdateSpeeds; // number of different update speeds in this
  // model
  bool usingSpeeds;

 public:
  /**
   *
   * CONSTRUCTORS/DESTRUCTORS
   *
   **/
  // Basic constructor: the input text, and
  // whether the input is in table or PDS form
  Cyclone(string input, bool useTables);
 
  // Deep copy constructor
  Cyclone(const Cyclone &orig);
 
  Cyclone();

  // Destructor
  ~Cyclone();
 
  /**
   *
   * PUBLIC RUN CALLS
   *
   **/

  // PRE: This instance of cyclone is defined using a valid
  // constructor, filenameOut is a valid output filename, fileWrite
  // is a boolean determining whether the output file is written to
  // filenameOut. filenameOut is a valid output name for a file. Cores
  // is the number of subprocesses to make, optimally the number of
  // cores available. pVerbose is a bool to determine whether the run
  // outputs to the command line.
  // POST: All limit cycles with component sizes are output to the
  // command line (if verbose) and/or the output file (if fileWrite).
  void run(bool fileWrite, string filenameOut, int cores, bool pverbose, bool includeTables);

  // PRE: writeFile, verbose, and filename are defined. The tables or PDSs have
  // been initialized.
  // POST: the state transitioned to by each state has been computed and
  // stored in the edges array.
  // This array has then been output in two column format with the first
  // column being the origin state number and the second column the
  // state transitioned to.
  void generateEdges(bool writeFile, string filename, bool verbose, bool includeTables);

  void fillEdgeArray(int cores);

  // PRE: writeFile, outFilename, verbose, inFilename are all
  // defined. inFilename identifies a file with a correctly formatted
  // trajectory start entry in it. This Cyclone object is
  // defined. tableFilename is the filename for the file used to create
  // the tables
  // POST: the path of the trajectory start to and through its limit
  // cycle are output either to command line, an output file, or both
  void generateTrajectory(bool writeFile, string outFilename, bool verbose, string inFilename, string tableFilename, bool includeTables);

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
  void generateRandomTrajectory(bool writeFile, string outFilename, bool verbose, string inFilename, string tableFilename, int numRuns, int numSteps, bool includeTables);

};

#endif // CYCLONE_CYCLONE

