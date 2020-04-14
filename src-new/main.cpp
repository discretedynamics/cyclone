#include <iostream>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

PolynomialFDS examplePDS1()
{
// MODEL NAME: eg2 (unused)
// SIMULATION NAME: eg2 (unused)
// NUMBER OF VARIABLES: 3
// VARIABLE NAMES: x1 x2 x3 (unused, sort of)
// NUMBER OF STATES: 2 2 2 (must all be the same, a prime number)
// SPEED OF VARIABLES: 1 1 1  (unused)

// f1 = 1+x2
// f2 = x1+x3
// f3 = x1*x2

  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<Polynomial> polys;
  // loc0 = 0
  // loc1 = 1
  // loc2 = x1
  // loc3 = x2
  // loc4 = x3
  // Create polynomial f1
  int nstates = 101;
  Polynomial f1(nstates, 3); // 2 states, 3 vars
  f1.createPlusNode(1,nstates+2-1);
  polys.push_back(f1);
  // Create polynomial f2
  Polynomial f2(nstates, 3);
  f2.createPlusNode(nstates+1-1,nstates+3-1);
  polys.push_back(f2);
  // Create polynomial f3
  Polynomial f3(nstates, 3);
  f3.createTimesNode(nstates+1-1,nstates+2-1);
  polys.push_back(f3);

  return PolynomialFDS(polys, varnames);
}

Polynomial examplePoly1()
{
  Polynomial f(3, 4); // 3 states, 4 variables 0,1,2,  3,4,5,6
  int n1 = f.createPlusNode(3,4);
  int n2 = f.createPlusNode(5,n1);
  int n3 = f.createTimesNode(n1,n2);
  int n4 = f.createTimesNode(n3, 1);
  f.debug_display(std::cout);
  return f;
}

Polynomial examplePoly2()
{
  Polynomial g(3, 4);
  int n1 = g.createPlusNode(1,1);
  int n2 = g.createPowerNode(n1,2);
  g.createPlusNode(5,n2);
  g.debug_display(std::cout);
  return g;
}

Polynomial examplePoly3()
{
  Polynomial h(3, 4);
  h.createPlusNode(1,1);
  h.debug_display(std::cout);
  return h;
}

int main(int argc, char** argv)
{
  Polynomial f = examplePoly1();
  int pt[4] {0,1,1,2};
  std::cout << "f(0,1,1,2) = " << f.evaluate(pt) << std::endl;
  std::cout << f.evaluateSymbolic({"x1","x3","x5","x7"}) << std::endl;

  Polynomial g = examplePoly2();
  Polynomial h = examplePoly3();

  PolynomialFDS pds = examplePDS1();
  std::cout << pds << std::endl;
  std::vector<long> stateSpace = computeStateSpace(pds);
  //  for (auto a : stateSpace) {
  //    std::cout << a << " ";
  //  }
  
#if 0
  if (argc < 2)
    {
      std::cout << "simFDS <FDS file> -o <output dot file>" << std::endl;
      return 1;
    }
#endif
  return 0;
}

