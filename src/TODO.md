TODO: created 27 April 2021, modified 4 May 2021    

 * disallow and, or if characteristic is not 2.
 * get this translation to work for parsing any string.
    * translate max, min to max /min characters (<, >), Translators from not to ~, etc.
      preprocess to remove AND, OR, NOT, XOR, give error if used, but not char 2 (at least AND, OR, XOR)
 * think about: adding |, & in char 2.
 * Check that runTrajectoryComputation is working, and displaying what it should be. (perhaps a stale todo?)
 * testing
    * create tests for the parser, evaluator, perhaps in examples.cpp
    * add catch2.hpp for testing framework?
        for running tests, consider using catch2.cpp (unit testing framework), will require changed to CMakeLists.txt
    * clean up the tests, add a few more tests.
    * test-runner
    * create a number of tests (at least 20 examples, with at least the summary answers, some with dot files)
    * boolean format from Claus' examples: need to translate.
 * error checking and error display (i.e. useful error checking for input data, to send back to the user)
 * walk through the code and clean/document it
    
TODO for algorun:
  * add doc tab to algorun
  * bugs in algorun: find them, fix them
  * change parameters to allow user to select only summary
      (want: on, off, auto: auto chooses 'on' if size is reasonable...)
  * don't run simFDS in debug mode!
  * algorun: would like multiple sample input files (e.g. dropdown menu)
  * update algorun_info/manifest.json
    * get description from ibrahim about parameters (algo_input, algo_output)
    * update input_example, output_example (mostly done, bugfix, maybe change to example with cycle)

