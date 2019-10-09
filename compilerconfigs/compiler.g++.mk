#
# g++ 4.6+ settings
#
CXX=g++
CXXFLAGS=-g -std=c++0x 
#-std=c++11 works for g++ 4.8+
CPPFLAGS=-Wall
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -fstrict-aliasing -ffast-math -march=native
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -ffast-math -march=native
