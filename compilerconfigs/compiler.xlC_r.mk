#
# ibm compiler settings
#
CXX=xlC_r
CXXFLAGS=-qlanglvl=extended0x
DBGFLAGS=-g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O2 -qinline=level=10 -qnoipa
FC=xlf
FFLAGS=-O5 -qnoipa
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
