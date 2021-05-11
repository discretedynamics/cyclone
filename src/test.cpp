#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

TEST_CASE( "Addition", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y" };
  Polynomial f = parsePolynomial(varnames, 3, "x + y");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames).compare("(x+y)") == 0 );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "z", "w" };
    REQUIRE( f.evaluateSymbolic(varnames2).compare("(z+w)") == 0 );
  }

  SECTION( "Numerical evaluation" ) {
    for( int x = 0; x < 3; x++) {
      for( int y = 0; y < 3; y++) {
        int params [2] = {x, y};
        REQUIRE( f.evaluate(params) == (x+y) % 3 );
      }
    }
  }
}


TEST_CASE( "Multiplication", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y" };
  Polynomial f = parsePolynomial(varnames, 3, "x * y");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames).compare("(x*y)") == 0 );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "z", "w" };
    REQUIRE( f.evaluateSymbolic(varnames2).compare("(z*w)") == 0 );
  }

  SECTION( "Numerical evaluation" ) {
    for( int x = 0; x < 3; x++) {
      for( int y = 0; y < 3; y++) {
        int params [2] = {x, y};
        REQUIRE( f.evaluate(params) == (x*y) % 3 );
      }
    }
  }
}



TEST_CASE( "Max", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y", "z" };
  Polynomial f = parsePolynomial(varnames, 3, ">(x,y,z)");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames).compare("max(max(x,y),z)") == 0 );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "r", "s", "t" };
    REQUIRE( f.evaluateSymbolic(varnames2).compare("max(max(r,s),t)") == 0 );
  }

  SECTION( "Numerical evaluation" ) {
    for( int r = 0; r < 3; r++) {
      for( int s = 0; s < 3; s++) {
        for( int t = 0; t < 3; t++) {
          int params [3] = {r, s, t};
          REQUIRE( f.evaluate(params) == std::max(std::max(r,s),t) );
        }
      }
    }
  }
}



TEST_CASE( "Min", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y", "z" };
  Polynomial f = parsePolynomial(varnames, 3, "<(x,y,z)");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames).compare("min(min(x,y),z)") == 0 );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "r", "s", "t" };
    REQUIRE( f.evaluateSymbolic(varnames2).compare("min(min(r,s),t)") == 0 );
  }

  SECTION( "Numerical evaluation" ) {
    for( int r = 0; r < 3; r++) {
      for( int s = 0; s < 3; s++) {
        for( int t = 0; t < 3; t++) {
          int params [3] = {r, s, t};
          REQUIRE( f.evaluate(params) == std::min(std::min(r,s),t) );
        }
      }
    }
  }
}



TEST_CASE( "Misc", "[poly]") {
  std::vector<std::string> varnames = { "x3", "x", "P53" };
  Polynomial f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)");
  
  SECTION( "variable names with numbers, order of operations" ) {
    REQUIRE( f.evaluateSymbolic(varnames).compare("(x3+(P53*(1+x)))") == 0 );
  }
    
  SECTION( "simplify exponentials" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 5*  AB_3");
    REQUIRE( f.evaluateSymbolic(varnames).compare("(x3+((P53*(1+x))*AB_3))") == 0 );
  }

  SECTION( "simplify exponentials2" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 4*  AB_3");
    REQUIRE( f.evaluateSymbolic(varnames).compare("(x3+((P53*((1+x)^2))*AB_3))") == 0 );
  }

  SECTION( "bases" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 3, "x3 + x^3 + (2+1)*P53 + AB_3^6");
    REQUIRE( f.evaluateSymbolic(varnames).compare("((x3+x)+(AB_3^2))") == 0 );
    
    f = parsePolynomial(varnames, 5, "x3 + x^3 + (2+1)*P53 + AB_3^6");
    REQUIRE( f.evaluateSymbolic(varnames).compare("(((x3+(x^3))+(3*P53))+(AB_3^2))") == 0 );
  }
  
  SECTION( "numeric constants simplify" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 5, "1+(2*5)^2+3");
    REQUIRE( f.evaluateSymbolic(varnames).compare("4") == 0 );
  }

}
