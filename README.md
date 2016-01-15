rarray - multidimensional array class template
==============================================

rarray is a C++ library for multidimensional arrays.  It is a
header-only implementation that uses templates, which allows most
compilers to generate fast code.  The current version is 1.0.

rarray is open-source, and is released under the MIT license. This
library is distributed WITHOUT ANY WARRANTY. For details, see the file
named 'LICENSE', and license statements in the source files.

Installation
============

To use this header-only library, one merely needs to copy the header
file rarray to the directory of the source files that include it,
or to a default directory where the compiler will look for header
files (e.g. /usr/include or /usr/local/include). 

Rather than do so by hand, you can use the included Makefile, which
can also compile the unit tests and benchmarks.  The configure command
sets up the makefile for your compiler.  You can type

  ./configure

or 
   ./configure --prefix=PATH

for a non-default installation file.  To pick your compiler, have the
CXX environment variable point to the right compiler command.

If you have a recent gnu, intel, ibm or clang compiler, and the 'make'
command, then the Makefile should be enough to compile the unit tests
and benchmarks:

   make test
   make benchmark

To create the (pdf) documentation, you need to have pdflatex
installed, and type

   make doc

To install the header and documentation to PATH/include and
PATH/share/doc, respectively, type

   make install

Be aware that the Makefile has not been extensively tested.

The rarray library has been tested with the GNU g++ compiler version
4.8.0 and up, the Intel C++ compiler version 13, IBM's XL C++ compiler
version 12 and up, and clang 3.5.

Documentation
=============

Documentation can be found in the pdf file 'rarraydoc.pdf', which is
generated from the LaTeX file rarraydoc.tex.

Reporting Bugs
==============

Under the license, you are not required to report bugs, but are
encouraged to do so.  If you find a bug, you may report it to
vanzonr@gmail.com. Errors or omissions in the documention can be
reported to the same address.

Files
=====

rarray.h               The (header-only) library defining runtime arrays.

rarrayio.h             Additional I/O routines for the runtime arrays

rarraymacros.h         Internal header files
rarraydelmacros.h

rarraydoc.tex          LaTeX source of the documentation
rarraydoc.pdf          Pdf format of the documentation

rarraytests.cc         regression test suite (using Boost.Test)

benchmark2Daccess.cc   benchmark code for 2d arrays comparing rarray,
                       blitz, eigen, boost, armadillo, dynamic and
                       automatic arrays

benchmark2Dfrtrn.f90   fortran benchmark code for 2d arrays for
                       comparison
 
benchmark4Daccess.cc   benchmark code for 4d arrays comparing rarray,
                       blitz, eigen, biist. dynamic and automatic
                       arrays

benchmark4Dfrtrn.f90   fortran benchmark code for 4d arrays for
                       comparison

elasped.h              Header file used by the benchmarks to measure
                       elapsed time

optbarrier.cc          Contains dummy function whose call acts as a 
                       barrier against code reordering; used in the
                       c++ benchmarks to avoid skewed comparisons

optbarrierf.f90        Fortran version of optbarrier.cc

configure              A non-autotools configure script for compiling 
                       benchmarks; Creates config.mk

compiler.clang++.mk    Compiler dependence flags for the Makefile. Used
compiler.g++.mk        as input to create config.mk by configure
compiler.icpc.mk
compiler.xlC.mk
compiler.xlc++.mk

Makefile               Makefile to build example, regression tests, 
                       benchmarks, and documentation pdf; Reads in config.mk

WARRANTY               File that expresses that there is no warranty

LICENSE                Text of the MIT license

AUTHOR                 Name and email address of the author

READMEBENCHMARK.txt    Explanation of what the 2d and 4d benchmarks
                       actually do

npyexample.cc          Rough example on how to write a rarray to a
                       numpy file.

README.md              This file.


- 19 December 2014
