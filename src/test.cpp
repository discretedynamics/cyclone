#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "Polynomial.hpp"
#include "PolynomialFDS.hpp"

TEST_CASE( "Addition", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y" };
  Polynomial f = parsePolynomial(varnames, 3, "x + y");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames) == "(x+y)" );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "z", "w" };
    REQUIRE( f.evaluateSymbolic(varnames2) == "(z+w)" );
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
    REQUIRE( f.evaluateSymbolic(varnames) == "(x*y)" );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "z", "w" };
    REQUIRE( f.evaluateSymbolic(varnames2) == "(z*w)" );
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
    REQUIRE( f.evaluateSymbolic(varnames) == "max(max(x,y),z)" );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "r", "s", "t" };
    REQUIRE( f.evaluateSymbolic(varnames2) == "max(max(r,s),t)" );
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
    REQUIRE( f.evaluateSymbolic(varnames) == "min(min(x,y),z)" );
  }

  SECTION( "Variable substitution" ) {
    std::vector<std::string> varnames2 = { "r", "s", "t" };
    REQUIRE( f.evaluateSymbolic(varnames2) == "min(min(r,s),t)" );
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



TEST_CASE( "And", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y", "z" };
  Polynomial f = parsePolynomial(varnames, 2, "x AND y AND z");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames) == "((x*y)*z)" );
  }

  SECTION( "Numerical evaluation" ) {
    for( int r = 0; r < 2; r++) {
      for( int s = 0; s < 2; s++) {
        for( int t = 0; t < 2; t++) {
          int params [3] = {r, s, t};
          REQUIRE( f.evaluate(params) == (r*s*t)%2 );
        }
      }
    }
  }
}


TEST_CASE( "XOR", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y", "z" };
  Polynomial f = parsePolynomial(varnames, 2, "x XOR y XOR z");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames) == "((x+y)+z)" );
  }

  SECTION( "Numerical evaluation" ) {
    for( int r = 0; r < 2; r++) {
      for( int s = 0; s < 2; s++) {
        for( int t = 0; t < 2; t++) {
          int params [3] = {r, s, t};
          REQUIRE( f.evaluate(params) == (r+s+t)%2 );
        }
      }
    }
  }
}


TEST_CASE( "Or", "[poly]" ) {
  
  std::vector<std::string> varnames = { "x", "y", "z" };
  Polynomial f = parsePolynomial(varnames, 2, "x OR y OR z");
  
  SECTION( "Symbolic evaluation" ) {
    REQUIRE( f.evaluateSymbolic(varnames) == "max(max(x,y),z)" );
  }

  SECTION( "Numerical evaluation" ) {
    for( int r = 0; r < 2; r++) {
      for( int s = 0; s < 2; s++) {
        for( int t = 0; t < 2; t++) {
          int params [3] = {r, s, t};
          REQUIRE( f.evaluate(params) == (r|s|t) );
        }
      }
    }
  }
}



TEST_CASE( "Misc", "[poly]") {
  std::vector<std::string> varnames = { "x3", "x", "P53" };
  Polynomial f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)");
  
  SECTION( "variable names with numbers, order of operations" ) {
    REQUIRE( f.evaluateSymbolic(varnames) == "(x3+(P53*(1+x)))" );
  }
    
  SECTION( "simplify exponentials" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 5*  AB_3");
    REQUIRE( f.evaluateSymbolic(varnames) == "(x3+((P53*(1+x))*AB_3))" );
  }

  SECTION( "simplify exponentials2" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 3, "x3 + P53*( 1 + x)^ 4*  AB_3");
    REQUIRE( f.evaluateSymbolic(varnames) == "(x3+((P53*((1+x)^2))*AB_3))" );
  }

  SECTION( "bases" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 3, "x3 + x^3 + (2+1)*P53 + AB_3^6");
    REQUIRE( f.evaluateSymbolic(varnames) == "((x3+x)+(AB_3^2))" );
    
    f = parsePolynomial(varnames, 5, "x3 + x^3 + (2+1)*P53 + AB_3^6");
    REQUIRE( f.evaluateSymbolic(varnames) == "(((x3+(x^3))+(3*P53))+(AB_3^2))" );
  }
  
  SECTION( "numeric constants simplify" ) {
    varnames = { "x3", "x", "P53", "AB_3", "AB_1", "AB4" };
    f = parsePolynomial(varnames, 5, "1+(2*5)^2+3");
    REQUIRE( f.evaluateSymbolic(varnames) == "4" );
  }

}


TEST_CASE( "Comma" ) {

  SECTION( "Single comma" ) {
    std::vector<std::string> varnames = { "x3", "a", "P53" };
    Polynomial f = parsePolynomial(varnames, 3, "max(x3,a+P53)");
    REQUIRE( f.evaluateSymbolic(varnames) == "max(x3,(a+P53))" );
  }

  SECTION( "Pair of commas" ) {
    std::vector<std::string> varnames = { "x3", "a", "P53" };
    Polynomial f = parsePolynomial(varnames, 3, "min(x3,a+P53,~x3)");
    REQUIRE( f.evaluateSymbolic(varnames) == "min(min(x3,(a+P53)),(~x3))" );
  }
  
}

TEST_CASE( "examplePDS1", "[poly]" ) {

  // # example eg2
  // NUMBER OF VARIABLES: 3
  // NUMBER OF STATES: 2

  // x1 = 1+x2
  // x2 = x1+x3
  // x3 = x1*x2    
  std::vector<std::string> varnames { "x1", "x2", "x3" };
  std::vector<Polynomial> polys;
  // loc0 = 0
  // loc1 = 1
  // loc2 = x1
  // loc3 = x2
  // loc4 = x3
  // Create polynomial f1
  int nstates = 2;
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

  PolynomialFDS pds = PolynomialFDS(polys, varnames);
  
  SECTION( "pds debug output" ) {

    std::ostringstream o;
    o << pds;
    
    REQUIRE( o.str() == "PolynomialFDS numstates=2 numvars=3\n  x1 = (1+x2)\n  x2 = (x1+x3)\n  x3 = (x1*x2)\n" );

  }

  SECTION( "compute state space" ) {

    std::vector<long> stateSpace = computeStateSpace(pds);
    REQUIRE( stateSpace == std::vector<long>{4, 6, 0, 2, 6, 4, 3, 1});

  }
  
}


TEST_CASE( "examplePoly", "[poly]" ) {

  Polynomial f(3, 4); // 3 states, 4 variables 0,1,2,  3,4,5,6
  int n1 = f.createPlusNode(3,4);
  REQUIRE( n1 == 7 ); // variables are slots, not values
  int n2 = f.createPlusNode(5,n1);
  REQUIRE( n2 == 8 );
  int n3 = f.createTimesNode(n1,n2);
  REQUIRE( n3 == 9 );
  int n4 = f.createPowerNode(n3,2);
  REQUIRE( n4 == 10 );
  
  SECTION( "do not create new node for mult by 1" ) {
    REQUIRE( f.createTimesNode(n4, 1) == n4 ); 
  }
  
  std::vector<std::string> varnames = { "x1", "x3", "x5", "x7" };
  REQUIRE( f.evaluateSymbolic(varnames) == "(((x1+x3)*(x5+(x1+x3)))^2)" );

  int pt[4] {0,1,1,2};
  REQUIRE( f.evaluate(pt) == 1 );

}


TEST_CASE( "exp", "[exp]") {

  Polynomial f(3, 1); // 3 states, 1 variable
  REQUIRE( f.exp(1,7) == 1 );
  REQUIRE( f.exp(1,8) == 1 );
  REQUIRE( f.exp(1,9) == 1 );

  REQUIRE( f.exp(2,7) == 2 );
  REQUIRE( f.exp(2,8) == 1 );
  REQUIRE( f.exp(2,9) == 2 );

}

  // std::cout << translateOperatorNames("NOT (x NOTX  AND NOT(b)) xor sdda and  not c OR d", 2) << std::endl;
  // std::cout << translateOperatorNames("a OR OR b", 2) << std::endl;
  // std::cout << translateOperatorNames("max(a,maxmax(b,c))", 3) << std::endl;
