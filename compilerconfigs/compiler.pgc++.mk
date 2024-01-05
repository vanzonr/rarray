#
# PGI compiler settings
#
CXX=pgc++ --c++11 --diag_suppress declared_but_not_referenced 
CXX17=pgc++ --c++17 --diag_suppress declared_but_not_referenced
CXX23=:
CXXFLAGS=
DBGFLAGS=-O0 -g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O4 -Munroll=c:1 -Mnoframe -Mlre -Mautoinline -Mvect=sse -Mcache_align -Mflushz -Mpre -Mneginfo=all
FC=pgfortran
FFLAGS=-Ofast
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
