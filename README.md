rarray - reference-counted multidimensional arrays
==================================================

rarray is a C++ library for reference counted multidimensional arrays.  
It is a header-only implementation that uses templates, which allows 
most compilers to generate fast code.

The latest release version is 2.7.0.

rarray is open-source, and is released under the MIT license. This
library is distributed WITHOUT ANY WARRANTY. For details, see the file
named 'LICENSE', and license statements in the source files.

For details on the installation procedure see the file named 'INSTALL'.

For the impatient: the what, why and how of rarray
==================================================

What
-----
Reference-counted and non-owning multidimensional arrays with runtime dimensions. 

What not
--------
No strides, no linear algebra, overloaded operators etc.

Why
----
Usually faster than alternatives.    
Uses the same accessors as automatic arrays.    
Requires only the C++-11 standard.    
Data is contiguous to allow interfacing with libraries like BLAS, LAPACK, FFTW, etc.

How
----
The header file `rarray` provides the type `rarray<T,R>`, where `T` is
any type and `R` is the rank. Element access uses repeated square
brackets. Copying rarrays or passing them to functions mean shallow
copies, unless explicitly asking for a deep copy. Streaming I/O is
also supported.


Documentation
=============

The rarray library comes with a tutorial tutorial.md. Code documentation can be generated using doxygen with

    make doc

Background information on the design of the library can be found in background.tex.

Installation
============

    mkdir build
    cd build
    ../configure --prefix=<PREFIXDIR>
    make install

Or copy the file "`rarray`" to `<PREFIXDIR>/include`.

The configure-make recipe also allows running tests; type "make help"
to see the options.

If you have sudo permissions, you can also do "`sudo make install`" to
install the header and documentation to /usr/include and
/usr/share/doc, respectively. Note that this will fail on recent MacOS
versions, in which case, try "`sudo make install PREFIX=/usr/local`".

Development
===========

To modify rarray, do not edit the rarray header file, as this is a
generated file. Instead, edit the files in the `headersources`
directory.  You can use the included Makefile to assemble the rarray
headers with `make headers`.

The Makefile can compile and run the unit tests and benchmarks. Simply do:

    ./configure
    make test
    make benchmarks

The `configure` command should work under Linux if you have a recent
GNU, Intel, IBM or Clang compiler.  Note that to pick your compiler,
you may have the set the CXX environment variable point to the right
compiler command (e.g. `export CXX=clang++`) before running `configure`.

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

     - Reference counting of rarray data is now done atomically.
   
     - One character names of substructures for debug were removed.

     - Streaming operators for rarrays moved to the ra namespace.

   In version 2.1.1, the undocumented rlinear function was renamed
   linspace and added to the documentation, as was xrange.

 * Version 2.2, February 2020

     - Several bug fixed with running and installing rarray on MacOS.

     - The rarray unit test library 'rut' was dropped in favour of
       'catch2', which does everything that 'rut' was intended to do,
       but better. Since 'catch2' is header only, this makes running
       the tests on different platforms much easier.

 * Version 2.3.0, January 2022

     - Returned to a single-header implementation by incorporating the
       rarrayio header into the rarray header.

 * Version 2.4.0, December 2022

     - Eliminated dead code and added exception safety.

     - Fixed bug for compound data types.
   
     - Support added for Intel OneAPI's icpx C++ compiler.

   2.4.1 was a bug fix for the at() function.
   
 * Version 2.5.0, February 2023

     - Bounds checking reinstated.

     - Can now get a subarray with square brackets.
 
     - Rarray objects no longer automatically converted into
       T*const*... pointers.

     - More extensive unit and coverage tests.

     - 'is_clear' renamed to 'empty'.

 * Version 2.5.1, May 2023 
     
     - Bug fixes, most importantely bounds checking of 1d arrays
     
     - Expanded tests for 100% code coverage
     
     - Comparison with mdspan (C++23)
     
     - Updated documentation.

 * Version 2.6.0, November 2023

     - Bug fixes (inlining, compiler settings, exception safety).
     
     - Support for the multidimensional subscript operator for c++23
       compilers.
       
     - More complete support for rarray<const T,R>.

     - Implicit conversion operator from rarray<T,R> to
       rarray<const T,R>.

   Version 2.6.1 from December 2023 is functionally the same, but with
   substantial code cleanup.  The stub header file rarrayio was
   removed.

 * Version 2.7.0, January 2024

     - Added fill and form methods.
     
     - Better documentation, now in markdown and doxygen.

Known issues
============

  * Cannot force alignment of data yet.

  * The behavior of rarrays of types whose destructor throws an
    exception is undefined (note: destructors should not throw
    exceptions anyway).

  * `xrange` result does not act as a range in C++23 mode for
    clang-based compilers (note: rarray only promisses C++11).
  
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

configure              A non-autotools configure script for compiling 
                       benchmarks; Creates config.mk

Makefile               Makefile to build unit tests, benchmarks, and
                       documentation pdf; Uses config.mk if it exists

VERSION                Contains the tag of most recently release version.
                       Note: If cloned from the github repository, the
                             code may differ due to subsequent
                             commits. Do a "git checkout TAG",
                             e.g. "git checkout 2.7.0" to get the
                             exact release.

WARRANTY               File that expresses that there is no warranty

LICENSE                Text of the MIT license

AUTHOR                 Name and email address of the author

INSTALL                More hints on how to install and build tests and benchmarks.

READMEBENCHMARK.txt    Explanation of what the 2d and 4d benchmarks
                       actually do

README.md              This file.

tutorial.md            Explains how to use rarray

Doxyfile.devel         Doxygen file for developer documentation

rarraydoc.tex          LaTeX source of the background documentation

rarraydoc.pdf          Pdf format of the background documentation

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

versionheader.h        To include the rarray version in the header (since v2.4.0)


In the directory 'src'
-----------------------

hardinclude.cc         Code that creates generates the header files
                       from the internal headersources.

test_rarray.cc         Simple tests for rarray

testsuite.cc           Rarray unit test/regression test suite (using catch2)

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

Compiler dependence flags for the Makefile. Used as input to create config.mk by configure.

compiler.clang++.mk    

compiler.g++.mk        

compiler.icpc.mk

compiler.icpx.mk

compiler.nvc++.mk

compiler.pgc++.mk

compiler.xlC.mk

compiler.xlC_r.mk

compiler.xlc++.mk


- January 2024
