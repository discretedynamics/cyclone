# compile via:
#  cd build
#  cmake -DCMAKE_BUILD_TYPE=Release ..
#  make

#set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_BUILD_DEBUG on)
#set(SUPPRESS_COMPILER_WARNINGS on)

#cmake_minimum_required(VERSION 3.14)
cmake_minimum_required(VERSION 3.5)

## specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

## set the project name, version, and languages
project(simFDS)
# project(simFDS
#  VERSION 0.1
#  DESCRIPTION "Simulation of finite dynamical systems"
#  HOMEPAGE_URL https://github.com/discretedynamics/cyclone.git
#  LANGUAGES CXX
#  )

set(SOURCES
  State.hpp
  Polynomial.hpp
  Polynomial.cpp
  PolynomialFDS.hpp
  PolynomialFDS.cpp
  examples.hpp
  examples.cpp
  main.cpp
  )

add_executable(simFDS ${SOURCES})


if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using Clang or AppleClang
  target_compile_options(simFDS PRIVATE
    -Wall -Wuninitialized
    -Wextra -Wno-unused-parameter
    -pedantic-errors -Wno-extra-semi
    -Wno-sign-conversion
    -Wno-sign-compare
    #  -Wno-error=pragmas # not needed for appleclang
    -Wno-unknown-pragmas
    #  -Wno-unused-local-typedefs # one warning, in mathic.h from x-aring.cpp
    #  -Wno-cast-qual # not needed for appleclang
    )
endif()
