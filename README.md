rarray - multidimensional array class template
==============================================

rarray is a C++ library for multidimensional arrays.  It is a
header-only implementation that uses templates, which allows most
compilers to generate fast code.  The current version is 1.0

rarray is open-source, and is released under the MIT license. This
library is distributed WITHOUT ANY WARRANTY. For details, see the file
named 'LICENSE', and license statements in the source files.

Installation
============

To use this header-only library, one merely needs to copy the header
file rarray to the directory of the source files that include it,
or to a default directory where the compiler will look for header
files (e.g. /usr/local/include). 

Rather than do so by hand, you can use the included Makefile, which
can also compile the unit tests and benchmarks.  The configure command
sets up the makefile for your compiler.  You can type

  ./configure

or 
   ./configure --prefix=PATH

for a non-default installation file.  To pick your compiler, have the
CXX environment variable point to the right compiler command.

If you have a recent gnu, intel, ibm or clang compiler, the Makefile
should be able to compile the unit tests and benchmarks:

   make -j1 test
   make -j1 benchmark

To create the (pdf) documentation, you need to have pdflatex
installed, and type

   make doc

To in install the header and documentation to PATH/include and PATH/share/doc, respectively, type

   make install

Be aware that the Makefile has not been extensively tested.

The rarray library has been tested with the GNU g++ compiler version
4.8, the Intel C++ compiler version 13, IBM's XL C++ compiler version
12 and up, and clang 3.5.

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

rarray.h:           The header-library

rarraydoc.tex:      LaTeX source of the documentation

rarraytestsuite.cc: regression test suite (not using Boost.Test yet)

rarray2dspeed.cc:   benchmark code for 2d arrays comparing rarray,
                    blitz, eigen, dynamic and automatic arrays

rarray2dspeedf.f90: fortran benchmark code for 2d arrays for comparison.

rarray4dspeed.cc:   benchmark code for 4d arrays comparing rarray,
                    blitz, eigen, dynamic and automatic arrays

rarray4dspeedf.f90: fortran benchmark code for 4d arrays for comparison.

configure:          A non-autotools configure script.

Makefile:           Makefile to build example, regression test and pdf

WARRANTEE:          File that expresses that there is no warrantee

LICENSE:            Text of the MIT license

AUTHOR:             Name and email address of the author

README:             This file

- 21 January 2014
