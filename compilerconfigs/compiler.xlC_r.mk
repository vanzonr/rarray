#
# ibm compiler settings
#
CXX=xlC_r -qlanglvl=extended0x
CXX17=xlC_r -qlanglvl=extended0x
CXX23=:
CXXFLAGS=
DBGFLAGS=-g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O4 -qinline=level=10 -qnoipa
FC=xlf
FFLAGS=-O5 -qnoipa
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
