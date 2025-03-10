#
# clang++
# Tested v13; need v15 for c++23, but may give compiler errors. v17 seems to work, and would allow std=c++23 instead of std=c++2b.)
#
CXX=clang++ -std=c++11
CXX17=clang++ -std=c++17
CXX23=clang++ -std=c++23
CXXFLAGS=-g -gdwarf-2
DBGFLAGS=-g -gdwarf-2
CPPFLAGS=-Wall -D__extern_always_inline=inline 
CPPFLAGSOPT=-DNDEBUG -D__extern_always_inline=inline 
CXXFLAGSOPT=-O3 -fstrict-aliasing -march=native 
#FC was gfortran
FC=flang
FFLAGS=-O3 -fstrict-aliasing -march=native
LDFLAGSCOV=-fprofile-instr-generate -fcoverage-mapping
CXXFLAGSCOV=-fprofile-instr-generate -fcoverage-mapping
LIBSCOV=
GCOV=llvm-profdata merge -sparse default.profraw  -o default.data && llvm-cov show -instr-profile=default.data
FILTERCOV=grep -hs ^ -
