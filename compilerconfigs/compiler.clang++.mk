#
# clang++ 3.5+
# (tested upto v9)
#
CXX=clang++
CXXFLAGS=-std=c++11
DBGFLAGS=-g
CPPFLAGS=-Wall -D__extern_always_inline=inline 
CPPFLAGSOPT=-DNDEBUG -D__extern_always_inline=inline 
CXXFLAGSOPT=-O3 -fstrict-aliasing -march=native 
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -march=native
