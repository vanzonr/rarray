#
# ibm compiler settings
#
CXX=xlc++ -qlanglvl=extended0x
CXX17=:
CXX23=:
CXXFLAGS=-g
DBGFLAGS=-g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -qinline=level=10 -qnoipa
FC=xlf
FFLAGS=-O5 -qnoipa
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
