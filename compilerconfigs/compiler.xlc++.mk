#
# ibm compiler settings
#
CXX=xlc++
CXXFLAGS=-qlanglvl=extended0x
DBGFLAGS=-g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -qinline=level=10 -qnoipa
FC=xlf
FFLAGS=-O5 -qnoipa
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
