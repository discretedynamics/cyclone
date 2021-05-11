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
  src/State.hpp
  src/Polynomial.hpp
  src/Polynomial.cpp
  src/PolynomialFDS.hpp
  src/PolynomialFDS.cpp
  src/examples.hpp
  src/examples.cpp
  src/main.cpp
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

# tests
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)

FetchContent_MakeAvailable(Catch2)

add_executable(tests
  src/test.cpp
  src/State.hpp
  src/Polynomial.hpp
  src/Polynomial.cpp
  src/PolynomialFDS.hpp
  src/PolynomialFDS.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2)

