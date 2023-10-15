#
# clang++
# Tested v13; need v15 for c++23, but may give compiler errors. v17 seems to work, and would allow std=c++23 instead of std=c++2b.)
#
CXX=clang++ -std=c++11
CXX14=clang++ -std=c++14
CXX23=clang++ -std=c++2b
CXXFLAGS=-g
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
FILTERCOV=grep -hs ^ -
