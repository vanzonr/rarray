#
# ibm compiler settings
#
CXX=xlc++
CXXFLAGS=-qsuppress=1501-510 -g -qlanglvl=extended0x
CPPFLAGS=-D__PRETTY_FUNCTION__=__FUNCTION__
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-qsuppress=1501-510 -O5 -qinline=level=10 -qsuppress=1500-036 -qnoipa
FC=xlf
FFLAGS=-O5 -qsuppress=1500-036 -qsuppress=1501-036 -qnoipa -qsuppress=1501-510
