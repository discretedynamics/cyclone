###############################################################################
#
# Description       : CMake build script for COPASI
# Original author(s): Ralph Gauges <ralph.gauges@bioquant.uni-heidelberg.de>
#                     Frank Bergmann <fbergman@caltech.edu>
#
# This file is part of COPASI.  Please visit http://COPASI.org for more
# information about COPASI, and the latest version of COPASI.
#
# Copyright (C) 2011 - 2010 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc., University of Heidelberg, and The University
# of Manchester.
# All rights reserved.
#
# Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc., EML Research, gGmbH, University of Heidelberg,
# and The University of Manchester.
# All rights reserved.
#
# Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc. and EML Research, gGmbH.
# All rights reserved.
#
###############################################################################

# we need at least cmake 2.8 for the FindLAPACK functions
cmake_minimum_required (VERSION 2.8)
project (cyclone)

# preferably link against the static versions 
# as compared to the dynamic ones
if (NOT WIN32)
 SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
endif()


include (CMakeTestCCompiler)
include (CheckCSourceCompiles)
include (CheckCXXSourceCompiles)
include (CheckStructHasMember)
include (CheckLibraryExists)
include (CheckFunctionExists)
include (CheckCCompilerFlag)
include (CheckCSourceRuns)
include (CheckSymbolExists)
include (CheckTypeSize)

CHECK_TYPE_SIZE("void*"  CMAKE_SIZEOF_VOID_P)

set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Full path to the library output directory")
mark_as_advanced(CMAKE_INSTALL_LIBDIR)

set(CYCLONE_BUILD_TYPE "native")
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(CYCLONE_BUILD_TYPE "32bit")
else()
  set(CYCLONE_BUILD_TYPE "64bit")
endif()
if (APPLE AND ENABLE_UNIVERSAL)
  set(CYCLONE_BUILD_TYPE "universal")
endif()

set(CMAKE_MODULE_PATH "${CYCLONE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

IF(DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING 
      "Choose the type of build, options are: Debug or Release."
      FORCE)
ELSE()
  SET(CMAKE_BUILD_TYPE Release CACHE STRING 
      "Choose the type of build, options are: Debug or Release."
      FORCE)
ENDIF()

#platform specific defines

if(UNIX)
  if(APPLE)
    add_definitions(-DDarwin)
    if(CMAKE_GENERATOR AND XCODE_VERSION)
      option (CYCLONE_OVERWRITE_XCODE_GENERATOR "If true, will default to llvmgcc42 rather than clang." OFF)
      if(CYCLONE_OVERWRITE_XCODE_GENERATOR AND CMAKE_GENERATOR MATCHES "Xcode" AND (${XCODE_VERSION} VERSION_EQUAL 4 OR ${XCODE_VERSION} VERSION_GREATER 4))
        # Xcode 4 defaults to the Apple LLVM Compiler.
        set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
      endif()
    endif()
  else()
    add_definitions(-DLinux)
    if(NOT CYGWIN)
      # on cygwin all code is position independent so -fPIC is not needed
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -fPIC")
      set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing -fPIC")
    endif()
  endif()
else()
  if (CYGWIN)
    add_definitions(-DCygwin)
  else()
    add_definitions(-DWin32)
  endif()
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  add_definitions(-DFreeBSD)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif()

if (WIN32 AND NOT CYGWIN)
  add_definitions(-DLIBSBML_EXPORTS -DLIBLAX_EXPORTS)
  add_definitions(-DRAPTOR_STATIC)
endif()

set(FILE_SEP)
set(PATH_SEP)
if(UNIX OR CYGWIN)
    set(PATH_SEP "/")
    set(FILE_SEP ":")
else()
    set(PATH_SEP "\\")
    set(FILE_SEP ";")
endif()

# are we using the intel compiler
set(USING_INTEL)
if ((WIN32 OR (UNIX AND NOT APPLE))
    AND CMAKE_C_COMPILER 
    AND ${CMAKE_C_COMPILER} MATCHES ".*ic[cl].*$")
  message(STATUS "Detected Intel Compiler")
  set(USING_INTEL TRUE)
endif ()

# if using msvc or intel windows compiler allow for the use of the static runtime
if(MSVC OR USING_INTEL)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
    set(BUILD_DEFINITIONS "${BUILD_DEFINITIONS} -D_CRT_SECURE_NO_WARNINGS")
    option(WITH_STATIC_RUNTIME "Compile using the static MSVC Runtime." OFF)
    if(WITH_STATIC_RUNTIME)
        foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)

            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    add_definitions( -D_MT)
    endif(WITH_STATIC_RUNTIME)
endif(MSVC OR USING_INTEL)

set(APPS "" CACHE INTERNAL "")
set(DIRS "" CACHE INTERNAL "")

add_subdirectory(src)

get_directory_property(DirDefs COMPILE_DEFINITIONS)

message(STATUS "-----------------------------------------------------------
 Building cyclone (${CMAKE_BUILD_TYPE})

   Source Dir          = ${CMAKE_SOURCE_DIR}
   Binary Dir          = ${CMAKE_BINARY_DIR}
   Install Prefix      = ${CMAKE_INSTALL_PREFIX}

   Additional Defines  = ${DirDefs}

-----------------------------------------------------------
")