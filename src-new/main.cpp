#include <iostream>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

int main(int argc, char** argv)
{
  Polynomial f(3, 4); // 3 states, 4 variables 0,1,2,  3,4,5,6
  int n1 = f.createPlusNode(3,4);
  int n2 = f.createPlusNode(5,n1);
  int n3 = f.createTimesNode(n1,n2);
  int n4 = f.createTimesNode(n3, 1);
  f.debug_display(std::cout);
  int pt[4] {0,1,1,2};
  std::cout << "f(0,1,1,2) = " << f.evaluate(pt) << std::endl;
  std::cout << f.evaluateSymbolic() << std::endl;

  Polynomial g(3, 4);
  n1 = g.createPlusNode(1,1);
  n2 = g.createPowerNode(n1,2);
  g.createPlusNode(5,n2);
  g.debug_display(std::cout);

  Polynomial h(3, 4);
  h.createPlusNode(1,1);
  h.debug_display(std::cout);

#if 0
  if (argc < 2)
    {
      std::cout << "simFDS <FDS file> -o <output dot file>" << std::endl;
      return 1;
    }
  #endif
  return 0;
}

