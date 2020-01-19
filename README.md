rarray - multidimensional array class template
==============================================

rarray is a C++ library for multidimensional arrays.  It is a
header-only implementation that uses templates, which allows most
compilers to generate fast code.

The current version is 2.1.

rarray is open-source, and is released under the MIT license. This
library is distributed WITHOUT ANY WARRANTY. For details, see the file
named 'LICENSE', and license statements in the source files.

For details on the installation procedure see the file named 'INSTALL'.

Documentation
=============

The rarray library comes with documentation in rararaydoc.pdf.

The LaTeX source of the documentation is rarraydoc.tex. To re-create
the pdf documentation from this source, you need to have pdflatex
installed, and type

    make doc

Release History and Changes
===========================

 * Version 0.x, December 2013

   First implementation of the header-only library rarray for runtime
   (dynamic) multidimensional arrays. Added references counting around
   that time.

 * Version 1.0, January 2014

   Code put on github.

 * Version 1.1, March 2015

   Changed the text output format used to use newlines.
   Reference counting removed. 

 * Version 1.2, February 2017

   Added c++11 aliases rvector, rmatrix and rtensor. Eliminated the
   need for rarraymacros.h and rarraydelmacros.h in the installed
   version of rarray. Reference counting reenabled.

 * Version 2.0, October 2019

   To fix a persistent memory leak, rarray was rewritten from the
   ground up, leaving most of the API unchanged, except for the
   following:
   
     - From this version on, rarray only works with C++11 (or higher).

     - To use the member function rarray::reshape() with a new shape
       that is smaller than the old one, an additional function
       parameter "ra::RESIZE::ALLOWED" must be supplied.
     
     - To get array bounds checking of indices, you must use the new
       at() member function of rarray instead of square parenthesis.

     - Rarray now has its own unit testing library, 'rut'.

 * Version 2.1, January 2020

   Reference counting of rarray data is now done atomically, so
   copying rarrays should now be thread-safe.
   
   One character names of substructures, used in debugging rarray
   itself, were removed.

   Streaming operators for rarrays moved to the ra namespace.


Known issues
============

  * Defining RA_BOUNDSCHECK only activates partial bounds checking
    because some the optional but expensive index-range chekcing
    capability got lost in the rewrite.

  * To get a sub-array, you need to use the rarray::at() member
    function. 

  * Rarray objects automatically convert into T*const*... pointers.

  * Cannot force aligned of data yet.

Reporting Bugs
==============

Under the license, you are not required to report bugs, but are
encouraged to do so.  If you find a bug, you may report it to
vanzonr@gmail.com. Errors or omissions in the documention can be
reported to the same address.

Files
=====

In the top directory
---------------------

rarray                 Header file defining runtime arrays (produced from headersources)
rarrayio               Header file defining I/O for rarrays (produced from headersources)

rarraydoc.tex          LaTeX source of the documentation
rarraydoc.pdf          Pdf format of the documentation

configure              A non-autotools configure script for compiling 
                       benchmarks; Creates config.mk

Makefile               Makefile to build unit tests, benchmarks, and
                       documentation pdf; Uses config.mk 

WARRANTY               File that expresses that there is no warranty

LICENSE                Text of the MIT license

AUTHOR                 Name and email address of the author

INSTALL                More hints on how to install and build tests and benchmarks.

READMEBENCHMARK.txt    Explanation of what the 2d and 4d benchmarks
                       actually do

README.md              This file.


In the directory 'headersources'
---------------------------------

elapsed.h              Header to measure time, used in benchmarks

rarray.h               Header source file defining runtime arrays
 
rarrayio.h             Header source file for I/O routines
 
rarraymacros.h         Internal macros and functions
rarraydelmacros.h

shared_buffer.h        Internal header that implements a reference-counted array
shared_shape.h         Internal header that implements a reference-counted shape structure
offsets.h              Internal header to compute pointer offsets within the shape structure


In the directory 'src'
-----------------------

hardinclude.cc         Code that creates generates the header files
                       from the internal headersources.

test_rarray.cc         Simple tests for rarray

testsuite.cc           Rarray unit test/regression test suite (using 'rut')

test_offsets.cc        Tests for internal offsets.h header

test_shared_buffer.cc  Tests for internal shared_buffer.h header

test_shared_shape.cc   Tests for internal shared_shape.h header 

benchmark2Daccess.cc   Benchmark code for 2d arrays comparing rarray,
                       blitz, eigen, boost, armadillo, dynamic and
                       automatic arrays

benchmark4Daccess.cc   Benchmark code for 4d arrays comparing rarray,
                       blitz, eigen, biist. dynamic and automatic
                       arrays

optbarrier.cc          Contains dummy function whose call acts as a 
                       barrier against code reordering; used in the
                       c++ benchmarks to avoid false comparisons

benchmark2Dfrtrn.f90   Fortran benchmark code for 2d arrays
 
benchmark4Dfrtrn.f90   Fortran benchmark code for 4d arrays

optbarrierf.f90        Fortran version of optbarrier.cc


In the directory 'compilerconfigs'
-----------------------------------

compiler.clang++.mk    Compiler dependence flags for the Makefile. Used
compiler.g++.mk        as input to create config.mk by configure
compiler.icpc.mk
compiler.xlC.mk
compiler.xlC_r.mk
compiler.xlc++.mk


In the directory 'rutsrc'
-------------------------

This contains code for the unit test library (only used for rarray unit tests).

Makefile               Makefile to build the rut library
  
In subdirectory 'src':
   
  rut.h                Header file for the unit test library
  rut.cc               Implementation file for the unit test library
  rutexample1.cc       Example of using rut
  ruttest.cc           Test of using rut (should fail)
  rutmpitest.cc        Test of using rut with mpi (preliminary)



- October 2019
