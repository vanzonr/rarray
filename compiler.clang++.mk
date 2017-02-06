#
# clang++ 3.5
#
CXX=clang++
CXXFLAGS=-g -std=c++11 
CPPFLAGS=-Wall -D__extern_always_inline=inline 
CPPFLAGSOPT=-DNDEBUG -D__extern_always_inline=inline 
CXXFLAGSOPT=-O3 -fstrict-aliasing -ffast-math -march=native
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -ffast-math -march=native
