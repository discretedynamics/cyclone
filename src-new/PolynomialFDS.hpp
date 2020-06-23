#ifndef __polynomialFDS_hpp_
#define __polynomialFDS_hpp_

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>

#include "State.hpp"
#include "Polynomial.hpp"

struct ComponentData {
  long componentSize;
  std::vector<long> limitCycle;

  ComponentData() : componentSize(0) {}
};


/// Class of a FDS which is given by polynomials
class PolynomialFDS
{
private:
  int mNumStates; // per variable, all the same.
  int mTotalNumStates; // mNumStates^#vars
  std::vector<std::string> mVariableNames;
  std::vector<Polynomial> mPolynomials;
public:
  PolynomialFDS(const std::vector<Polynomial>& polys,
                const std::vector<std::string>& varnames)
    : mVariableNames(varnames),
      mPolynomials(polys)
  {
    assert(polys.size() > 0);
    for (int i=0; i<polys.size(); i++)
      {
        assert(polys.size() == polys[i].numVariables());
        assert(polys[i].numStates() == polys[0].numStates());
      }
    assert(varnames.size() == polys.size());

    mNumStates = polys[0].numStates();
    mTotalNumStates = 1;
    for (int i=0; i < polys.size(); i++)
      mTotalNumStates *= mNumStates;
  }

  int numVariables() const { return mPolynomials.size(); }

  int numStates() const { return mNumStates; }

  int totalNumStates() const { return mTotalNumStates; }

  void evaluate(const int u[], int output[]);

  void display(std::ostream& o);

};

inline std::ostream& operator<<(std::ostream& o, PolynomialFDS f)
{
  f.display(o);
  return o;
}
  
PolynomialFDS* readPDS(std::string filename); // TODO: being written

std::vector<long> computeStateSpace(PolynomialFDS& FDS);

void writeDotFile(std::ostream& o,
                  PolynomialFDS& FDS,
                  const std::vector<long>& stateSpace);

std::vector<ComponentData>  computeComponentsAndCycles(const std::vector<long>& stateSpace);

void displayLimitCycleInfo(std::ostream& o, const PolynomialFDS& pds, const std::vector<ComponentData>& cycleinfo);

#endif

// Local Variables:
// compile-command: "make "
// indent-tabs-mode: nil
// End:
