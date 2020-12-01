#include <stdlib.h>
#include <sstream>
#include "PDS.h"
#include "Table.h"
#include "cyclone.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "utilities.h"
#include <string>

using namespace std;

// PRE: filename contains the name of a file with tables
// POST: the file has been read and its tables constructed into Table
// objects in the tables array
string readInput(string filename)
{
  ifstream inputStream(filename.c_str());
  string input = "";

  // Try to open the file
  if (inputStream.is_open())
    {

      char * temp = new char[LINESIZE];

      // Read the entire file contents into one input string "input"
      // adding one character "\n" at the end of each line read
      while (inputStream)
        {
          int oldLength = input.length();
          inputStream.getline(temp, LINESIZE);
          input += temp;
          while (input.length() - oldLength == LINESIZE)
            {
              oldLength = input.length();
              inputStream.getline(temp, LINESIZE);
              input += temp;
            }
          input += "\n";
        }
      input += '\0';
    }
  else
    {
      cout << "Input File: *" << filename << "* Not Found" << endl;
    }
  std::cout << "read in file with contents: " << std::endl;
  std::cout << "########################" << std::endl;
  std::cout << input << std::endl;
  std::cout << "########################" << std::endl;
  return input; // this input has the entire string data in the input file
}

// PRE: tables abd pds are defined as representing the user's choice
// (or lack thereof if they are both false) of input type. numStates
// is the number of states the user has entered
// POST: either tables or pds is true, and num_statesis >= MIN_STATES
void promptUser(bool & tables, bool & pds)
{
  // If table/PDS is unspecified:
  while (!tables && !pds)
    {
      cout << "Is input in table (t) or PDS (p) form?" << endl;
      string tempTables = "";
      cin >> tempTables;
      tables = (tempTables[0] == 't');
      pds = (tempTables[0] == 'p');
    }
}

// Main function, takes in arguments for running cyclone. If the
// minimum arguments are not provided then the user is prompted for them.
int main(int argc, char * argcv[])
{

  string filename;
  string trajFile;
  string outputFile;
  int cores = 0;
  int numTraj = 0;
  int numRandomSteps = 0;
  int numRandomRuns = 0;
  bool tables = true;
  bool pds = false;
  bool writeFile = false;
  bool edges = false;
  bool verbose = false;
  bool traj = false;
  bool multitraj = false;
  bool randomUpdate = false;
  bool includeTables = false;
  char ** multitrajFiles;
  char ** multitrajOutfiles;

  // No arguments
  if (argc == 1)
    {
      // cout << "Enter input filename: " << endl;
      // cin >> filename;

      // promptUser(tables, pds);

      std::cout << "cyclone <file> args..." << std::endl;
      std::cout << "  actions" << std::endl;
      std::cout << "    -edges                                   compute edges (meaning what?)" << std::endl;
      std::cout << "    -traj <file>                              compute trajectory or random trajectory" << std::endl;
      std::cout << "    -multitraj <#trajectories:int> <file1> .. <filen>" << std::endl;
      std::cout << "                                             multi-XX trajectory" << std::endl;
      std::cout << "    (none specified)                         do the main thing (which is...?)" << std::endl;
      std::cout << "  arguments:" << std::endl;
      std::cout << "    -v                    verbose output (meaning what?)" << std::endl;
      std::cout << "    -random <int> <int>   random update" << std::endl;
      std::cout << "    -r <int> <int>        same" << std::endl;
      std::cout << "    -c <int>              number of cores to use" << std::endl;
      std::cout << "    -f <filename1> ... <filenamen>  output file(s).  Must come after action" << std::endl;
      std::cout << "    -it                   include the input tables in the output" << std::endl;
      std::cout << "    -PDS                  input is in polynomial format" << std::endl;
      std::cout << "  help" << std::endl;
      std::cout << "    -help format          the format of the input file (not yet written)" << std::endl;
      std::cout << "\
  The main activity of cyclone is to iterate over the state space of a graph input\n\
  into it as either a system of polynomial equations or a series of\n\
  tables. This iteration then produces all limit cycles in the state\n\
  space and outputs these, along with the percentage of the state\n\
  space which is a part of each component of the graph." << std::endl;
      
      return 0;
    }
  // One or more arguments in addition to filename
  else
    {
      string temp = argcv[1];

      filename = temp;
 
      for (int i = START_OF_ARGS; i < argc; i++)
        {
          temp = argcv[i];

          // Check for -v argument
          if (temp.compare("-v") == 0)
            {
              verbose = true;
            }
          else if (temp.compare("-PDS") == 0) {
              pds = true;
              tables = false;
          }
          // Check for -edges argument
          else if (temp.compare("-edges") == 0) {
              edges = true;
          }
          // Check for -traj argument
          else if (temp.compare("-traj") == 0)
            {
              traj = true;
              trajFile = argcv[i + 1];
              i++;
            }
          // Check for -random argument
          else if (temp.compare("-random") == 0 || temp.compare("-r") == 0)
            {
              randomUpdate = true;
              numRandomRuns = atoi(argcv[i + 1]);
              numRandomSteps = atoi(argcv[i + 2]);
              i += 2;
            }
          else if (temp.compare("-multitraj") == 0)
            {
              multitraj = true;
              numTraj = atoi(argcv[i + 1]);
              i++;
              multitrajFiles = new char*[numTraj];
              for (int k = 0; k < numTraj; k++)
                {
                  multitrajFiles[k] = (argcv[i + 1]);
                  i++;
                }
            }
          // Check for -c argument
          else if (temp.compare("-c") == 0)
            {
              cores = atoi(argcv[i + 1]);
              i++;
            }
          // Check for a file write arguments
          else if (temp.compare("-f") == 0)
            {
              writeFile = true;
              if (multitraj)
                {
                  multitrajOutfiles = new char*[numTraj];
                  for (int k = 0; k < numTraj; k++)
                    {
                      multitrajOutfiles[k] = argcv[i + 1];
                      i++;
                    }
                }
              else
                {
                  outputFile = argcv[i + 1];
                  i++;
                }
            }
          // Check for write-table with results argument
          else if (temp.compare("-it") == 0)
            {
              includeTables = true;
            }
        }
      promptUser(tables, pds);
    }

  // Run cyclone
  string input = readInput(filename);
  std::cout << "filename      = " << filename << std::endl;
  std::cout << "pds           = " << pds << std::endl;
  std::cout << "tables        = " << tables << std::endl;
  std::cout << "edges         = " << edges << std::endl;
  std::cout << "writeFile     = " << writeFile << std::endl;
  std::cout << "verbose       = " << verbose << std::endl;
  std::cout << "includeTables = " << includeTables << std::endl;
  if (input.length() > 0)
    {

      Cyclone * cyc = new Cyclone(filename, tables);
      if (edges)
        {
          std::cout << "Computing state space" << std::endl;
          cyc->generateEdges(writeFile, outputFile, verbose, includeTables);
          std::cout << "  Done computing state space" << std::endl;
        }
      else if (traj)
        {
          if (randomUpdate)
            {
              cyc->generateRandomTrajectory(writeFile, outputFile, verbose,
                                            trajFile, filename, numRandomRuns,
                                            numRandomSteps, includeTables);
            }
          else
            {
              cyc->generateTrajectory(writeFile, outputFile, verbose, trajFile,
                                      filename, includeTables);
            }
        }
      else if (multitraj)
        {
          for (int i = 0; i < numTraj; i++)
            {
              if (writeFile)
                {
                  if (randomUpdate)
                    {
                      cyc->generateRandomTrajectory(writeFile,
                                                    multitrajOutfiles[i],
                                                    verbose, multitrajFiles[i],
                                                    filename, numRandomRuns,
                                                    numRandomSteps,
                                                    includeTables);
                    }
                  else
                    {
                      cyc->generateTrajectory(writeFile, outputFile, verbose,
                                              trajFile, filename,
                                              includeTables);
                    }
                }
              else
                {
                  if (randomUpdate)
                    {
                      cyc->generateRandomTrajectory(writeFile, outputFile,
                                                    verbose, multitrajFiles[i],
                                                    filename, numRandomRuns,
                                                    numRandomSteps,
                                                    includeTables);
                    }
                  else
                    {
                      cyc->generateTrajectory(writeFile, outputFile, verbose,
                                              trajFile, filename,
                                              includeTables);
                    }
                }
            }
        }
      else
        {
          if (cores > 0)
            {
              cyc->run(writeFile, outputFile, cores, verbose, includeTables);
            }
          else
            {
              std::cout << "Computing limit points and cycles" << std::endl;
              cyc->run(writeFile, outputFile, 1, verbose, includeTables);
              std::cout << "  done computing limit points and cycles" << std::endl;
            }
        }

      delete cyc;
    }
  return 0;
}
