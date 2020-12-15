# Cyclone: Simulation and Analysis of Finite Dynamical Systems
Cyclone simulates the complete state space for an input finite dynamical system and finds all attractors (steady states and limit cycles),
together with statistics about the size of components. 


### Authors 
#### Version 2.0.0 (2020)
* Developers: Elena Dimitrova, Adam Knapp, Brandilyn Stigler, Michael Stillman
#### Version 1.0.0 (2016)
* PI: Reinhard Laubenbacher reinhard@vbi.vt.edu
* coPIs: Brett Tyler, John McDowell, Stefan Hoops 
* Developers: Bonny Guang, Madison Brandon, Rustin McNeill, Paul Vines, Franziska Hinkelmann, Seda Arat

Cyclone is available at https://github.com/discretedynamics/cyclone/. 
See [NSF Award 1146819](http://www.nsf.gov/awardsearch/showAward.do?AwardNumber=1146819&WT.z_pims_id=5444) for details on the project. 


## Running Cyclone
### Input
A plain text file containing an _n_-dimensional finite dynamical system _f_ = (_f_<sub>1</sub>, ... ,_f<sub>n</sub>_), 
where each _f<sub>i</sub>_ is a polynomial in _F<sub>p</sub>_[_x_<sub>1</sub>, ..., _x<sub>n</sub>_] and _F<sub>p</sub>_ is a finite field with cardinality _p_.
The number of variables _n_ can be any positive integer.
The number of states _p_ can be any prime integer.
Variable names can be any string without spaces.

### Example file
```
#example FDS 
NUMBER OF VARIABLES: 4
NUMBER OF STATES: 3
x1 = x2
x2 = x1+x3
x3 = x1*x2 + 1
x4 = (x1+x4)*x3
```
The first line is a commented line that users can describe their file.  The code ignores this line.

The next two lines are read by the code: the numbers at the ends of the lines can be changed by the user.

The subsequent lines contain the equations for the finite dynamical system.


### Outputs
Cyclone returns the following files by default.
* state space
* summary
* visualization
### Running Cyclone from the command line
```simFDS <project-name>```

## Files
### In src/
* main.cpp
* Polynomial.cpp
* Polynomial.hpp
* PolynomialFDS.cpp
* PolynomialFDS.hpp
* State.hpp
### In examples/
