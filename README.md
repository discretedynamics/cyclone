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
where each _f<sub>i</sub>_ is a polynomial in _Z<sub>p</sub>_[_x_<sub>1</sub>, ..., _x<sub>n</sub>_] where _Z<sub>p</sub>_ is the ring _Z/pZ_.
The number of variables _n_ and number of states _p_ can be any positive integer.


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
The first line is a commented line that users can describe their file.  The code ignores this line and any others which begin with the comment character `#`. 

The next two lines are read by the code: the numbers at the ends of the lines can be changed by the user.

The subsequent lines contain the functions determining the finite dynamical system.  The function for a variable is represented as ```variable_name = function_expression```. 
* Variable names must begin with a letter but may otherwise be any string without spaces and must not include the built-in arithmetic operator symbols.
* Function expressions can include 
    * ```+``` (addition), ```*``` (multiplication), ```^``` (exponentiation), ```MAX``` (maximum of a parenthesized list of variables), ```MIN``` (mimimum of a parenthesized list of variables), ```NOT``` (_x_+ NOT _x_ = _p_-1).
    * Boolean only (_p_=2): ```AND```, ```OR``` or ```|```, ```XOR```
    * All caps or lowercase is accepted for MAX, MIN, NOT, AND, OR, XOR. 

__Note:__ When the number of states, _p_, is not prime, not all possible update functions are expressible by poynomials due to the presence of zero divisors. 

### Running Cyclone from the command line 
```simFDS <project-name>.pds```
See ** note below. 

### Outputs
Cyclone returns the following files by default.
* ```<project-name>-statespace.dot```: a plain-text file containing the state space of the finite dynamical system _f_. The state space of _f_ is a directed graph with vertex the _n_-tuples with entries in _F<sub>p</sub>_ and edges a --> b iff _f_(a)=b.  The file is formatted as an input to the ```dot``` layout engine in the open source graph visualization software Graphviz.  
*  ```<project-name>-limitcycles.txt```: a plain-text file containing the number of components (basin of attraction + attractor) in the state space graph; the size of each component; and the states that form the limit cycle (attractor).  

The following options are useful for large networks.
* ```--summary``` returns only the limit cycle file.  
* ```–-trajectory a1 a2 ... an``` returns only produces the portion of the state space beginning at the given initial state ```a1 a2 ... an``` and ending at the associated limit cycle.

To create an image file from the dot file:
    ```dot -Tpng -o <project-name>.png <project-name>-statespace.dot```

## Building the Executable
To build the Cyclone executable ```simFDS``` (on linux or mac):
```  
mkdir -p build
cd build
cmake ..
make
./simFDS
```  
** If the directory continaing `simFDS` (in the above instructions, the build directory) has been added to the `PATH` environment variable, then ```simFDS``` can be used anywhere. ([Documentation](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/path) on this environment variable can be easily googled.) Otherwise, running `simFDS` from the command line will require using the path where `simFDS` resides. e.g. When in the build directory, use ```./simFDS```.
