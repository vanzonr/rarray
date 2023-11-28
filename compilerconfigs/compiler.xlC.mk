#
# xlC compiler settings for power6
#
CXX=xlC -qlanglvl=extended0x
CXX17=xlC -qlanglvl=extended0x
CXX23=:
CXXFLAGS=-g
DBGFLAGS=-g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O4 -qarch=pwr6 -qtune=pwr6 -qinline=level=10
FC=xlf
FFLAGS=-O3 -qarch=pwr6 -qtune=pwr6
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
