#
# clang-based intel compiler settings
# (c++11: tested with oneAPI 2021.4.0, and 2022.1.0)
# (c++23: tested with oneAPI 2023.1.0)
#
CXX=icpx -std=c++11
CXX17=icpx -std=c++17
CXX23=icpx -std=c++23
CXXFLAGS=-O0
DBGFLAGS=-g
CPPFLAGS=-Wall
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -xhost -ansi-alias -funroll-loops
FC=ifx
FFLAGS=-O3 -xhost -ansi-alias -funroll-loops
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
