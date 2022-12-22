#
# intel compiler settings
# (tested with oneAPI 2021.4.0, and 2022.1.0)
#
CXX=icpx
CXXFLAGS=-O0 -std=c++11
DBGFLAGS=-g
CPPFLAGS=-Wall
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -xhost -ansi-alias -funroll-loops
FC=ifx
FFLAGS=-O3 -xhost -ansi-alias -funroll-loops
