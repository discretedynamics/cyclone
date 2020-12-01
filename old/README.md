ADAM - Analysis of Dynamic Algebraic Models
====

### Investigators 
* Reinhard Laubenbacher reinhard@vbi.vt.edu (Principal Investigator)
* Brett Tyler (Co-Principal Investigator)
* John McDowell (Co-Principal Investigator)
* Stefan Hoops (Co-Principal Investigator)

* See [NSF Award 1146819](http://www.nsf.gov/awardsearch/showAward.do?AwardNumber=1146819&WT.z_pims_id=5444) for details on the project. 

## Current status
ADAM is available at http://adam.plantsimlab.org/adam.html

ADAM uses a combination of simulation and mathematical algorithms to analyze the dynamics of discrete biological systems. It can analyze bounded Petri-nets (generated with Snoopy), Logical Models (in GINSim format), Polynomial Dynamical Systems (PDS), open Polynomial Dynamical Systems (oPDS), Probabilistic Boolean (or multistate) Networks, Stochastic Discrete Dynamical Systems (SDDS), and open Stochastic Discrete Dynamical Systems (oSDDS). For small enough networks (deterministic or probabilistic), ADAM simulates the complete state space of the model and finds all attractors (steady states and limit cycles) together with statistics about the size of components. For larger networks, ADAM computes fixed points for both deterministic and probabilistic networks, and limit cycle of the length specified by the user for deterministic networks. You can follow our step-by-step tutorial or read the user guide. It is important that you follow the format specified in the guide. Make your selections and provide inputs (if any) in the form below and click Analyze to run the software. To generate a model from experimental time course data, you can use Polynome.
 

Development
----
ADAM is currently still under development; if you spot any bugs or have any questions/comments, please e-mail us.
Developers: Bonny Guang, Madison Brandon, Rustin McNeill, Paul Vines, Franziska Hinkelmann, Seda Arat

For mathematical calculations ADAM uses Macaulay2.

BMC Bioinformatics ADAM: Analysis of Discrete Models of Biological Systems Using Computer Algebra by
Franziska Hinkelmann, Madison Brandon, Bonny Guang, Rustin McNeill, Paul Vines, Grigoriy Blekherman, Alan Veliz-Cuba, and Reinhard Laubenbacher

Structure
----
adam.pl is the starting page

adam.pl is calling perl scripts, ruby, or Macaulay2 scripts to analyze a given model. Output is vizualized with Graphviz dot tool. 

### Perl scripts
* translator.pl - convert Boolean model to polynomials
* dvd_stochastic_runner.pl - generate state space of deterministic or probabilistic polynomial dynamical system, uses DVDCore.pm

###Ruby scripts
* adam_largeNetwork.rb - find steady states by solving system of equations, uses M2 code in solveGB.m2
* ginSim_converter.rb - convert logical model to PDS, uses M2 code in convertToPDS.m2
* petri-converter.rb - convert Petri net in Snoopy format to PDS, calls petri_parser.rb
* petri_parser.rb - uses code in Snoopy.rb
* snoopy.rb - uses M2 code in indicatorFunc.rb
* transitionTables.rb - convert one transition table to a polynomials, uses indicatorFunc.m2
* transitionTablesContinuous.rb convert one transition table to a continuous polynomials, uses indicatorFunc.m2
