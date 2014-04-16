#
# intel compiler settings
#
CXX=xlc++
CXXFLAGS=-g -qlanglvl=extended0x
CPPFLAGS=-D__PRETTY_FUNCTION__=__FUNCTION__
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -qarch=pwr6 -qtune=pwr6 -qinline=level=10
FC=xlf
FFLAGS=-O3 -qarch=pwr6 -qtune=pwr6
