#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

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
