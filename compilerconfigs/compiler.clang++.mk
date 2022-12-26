#
# clang++ 3.5+
# (tested upto v13)
#
CXX=clang++
CXXFLAGS=-std=c++11
DBGFLAGS=-g
CPPFLAGS=-Wall -D__extern_always_inline=inline 
CPPFLAGSOPT=-DNDEBUG -D__extern_always_inline=inline 
CXXFLAGSOPT=-O3 -fstrict-aliasing -march=native 
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -march=native
LDFLAGSCOV=-fprofile-instr-generate -fcoverage-mapping
CXXFLAGSCOV=-fprofile-instr-generate -fcoverage-mapping
LIBSCOV=
GCOV=llvm-profdata merge -sparse default.profraw  -o default.data && llvm-cov show -instr-profile=default.data
