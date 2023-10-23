#
# intel compiler settings
# (tested with versions 13.1.0-15)
#
CXX=icpc -std=c++11
CXX17=icpc -std=c++17
CXX23=:
CXXFLAGS=-O0
DBGFLAGS=-g
CPPFLAGS=-Wall
CPPFLAGSOPT=-DNDEBUG 
CXXFLAGSOPT=-O3 -xhost -ansi-alias -no-prec-div -funroll-loops
FC=ifort
FFLAGS=-O3 -xhost -ansi-alias -no-prec-div -funroll-loops
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
