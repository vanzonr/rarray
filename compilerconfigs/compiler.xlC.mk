#
# intel compiler settings
#
CXX=xlC
CXXFLAGS=-g -qlanglvl=extended0x
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -qarch=pwr6 -qtune=pwr6 -qinline=level=10
FC=xlf
FFLAGS=-O3 -qarch=pwr6 -qtune=pwr6
