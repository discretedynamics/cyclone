#include <string>
#include <iostream>
#include <fstream>
#include <vector>
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
  f.createTimesNode(n3, 1);
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

void someTests()
{
  std::vector<std::string> varnames = { "x3", "x", "P53" };
  Polynomial f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
  f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 5*  AB_3");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
  f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 4*  AB_3");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  f = parsePolynomial(varnames, 3, "x3 + x^3 + (2+1)*P53 + AB_3^6");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  f = parsePolynomial(varnames, 5, "x3 + x^3 + (2+1)*P53 + AB_3^6");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;

  f = parsePolynomial(varnames, 5, "1+(2*5)^2+3");
  std::cout << f.evaluateSymbolic(varnames) << std::endl;
}

PolynomialFDS examplePDS2()
{
  int nstates = 2;
  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<std::string> polystrs
    {
     "x1+x2",
     "x2*x3+1",
     "x1+x1*x3"
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

#if 0  
  f = examplePoly1();
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
  
  if (argc < 2)
    {
      std::cout << "simFDS <FDS file> -o <output dot file>" << std::endl;
      return 1;
    }
  return 0;
}
#endif

PolynomialFDS cyclonePDS1()
{
  int nstates = 2;
  std::vector<std::string> varnames { "x1", "x2" };
  std::vector<std::string> polystrs
    {
     "1+x2",
     "0+x1" // TODO: BUG: replacing this with x1 causes the poly to become 0!
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

PolynomialFDS cyclonePDS2()
{
  int nstates = 2;
  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<std::string> polystrs
    {
     "1+x2",
     "x1+x3",
     "x1*x2"
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}

PolynomialFDS cyclonePDS3()
{
  int nstates = 2;
  std::vector<std::string> varnames { "M", "B", "Ob", "Tb", "A", "Da", "L", "Hl" };
  std::vector<std::string> polystrs
    {
     "A",
     "M + (B*(Tb+1)) + M*B*(Tb+1)",
     "(M+1)*B",
     "(M+1)*Ob",
     "(B*L + Hl + B*L*Hl) + A*Da*(B+1) + (B*L + Hl + B*L*Hl)*A*Da*(B+1)",
     "((B+1) + (L+1) + (B+1)*(L+1))*(Hl+1)*A",
     "L",
     "Hl"
    };

  std::vector<Polynomial> polys;
  for (const auto& s : polystrs)
    polys.push_back(parsePolynomial(varnames, nstates, s));
  return PolynomialFDS(polys, varnames);
}
