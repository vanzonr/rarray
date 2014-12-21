#
# intel compiler settings
# (tested with versions 13.1.0-14.0.0)
#
CXX=icpc
CXXFLAGS=-O0 -g -std=c++11
#note: switching off optimization for tests, not for benchmarks
CPPFLAGS=-Wall
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -xhost -ansi-alias -no-prec-div -fp-model fast=2 -funroll-loops
FC=ifort
FFLAGS=-O3 -xhost -ansi-alias -no-prec-div -fp-model fast=2 -funroll-loops
