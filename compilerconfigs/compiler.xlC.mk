#
# xlC compiler settings for power6
#
CXX=xlC
CXXFLAGS=-qlanglvl=extended0x
DBGFLAGS=-g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -qarch=pwr6 -qtune=pwr6 -qinline=level=10
FC=xlf
FFLAGS=-O3 -qarch=pwr6 -qtune=pwr6
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
