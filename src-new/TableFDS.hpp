#include <string>
#include <vector>
#include "State.hpp"

/// Class of a FDS which is given by transition tables
class Table
{
};

class TableFDS
{
private:
  std::vector<std::string> mVariableNames;
  std::vector<Table> mTables;
public:
  void evaluate(State u, State result);
};

Table readTable(std::string filename);
