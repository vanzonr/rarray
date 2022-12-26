#
# g++ 4.8+ settings
#
CXX=g++
CXXFLAGS=-std=c++11 
DBGFLAGS=-g
CPPFLAGS=-Wall -Wshadow -Wconversion -Wfatal-errors
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -fstrict-aliasing -march=native
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -march=native
LDFLAGSCOV=
CXXFLAGSCOV=-fprofile-arcs -ftest-coverage
LIBSCOV=-lgcov
GCOV=gcov -trkm
