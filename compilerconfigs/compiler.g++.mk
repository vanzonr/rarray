#
# g++ 4.8+ settings
#
CXX=g++
CXXFLAGS=-g -std=c++11 
CPPFLAGS=-Wall -Wshadow -Wconversion
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -fstrict-aliasing -march=native
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -march=native
