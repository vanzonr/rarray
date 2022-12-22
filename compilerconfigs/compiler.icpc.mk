#
# intel compiler settings
# (tested with versions 13.1.0-15)
#
CXX=icpc
CXXFLAGS=-O0 -std=c++11
DBGFLAGS=-g
CPPFLAGS=-Wall
CPPFLAGSOPT=-DNDEBUG 
CXXFLAGSOPT=-O3 -xhost -ansi-alias -no-prec-div -funroll-loops
FC=ifort
FFLAGS=-O3 -xhost -ansi-alias -no-prec-div -funroll-loops
