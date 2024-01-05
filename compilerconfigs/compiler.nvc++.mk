#
# NVIDIA's nvhpc compiler settings
#
CXX=nvc++ --c++11 --diag_suppress declared_but_not_referenced 
CXX17=nvc++ --c++17 --diag_suppress declared_but_not_referenced
CXX23=:
CXXFLAGS=
DBGFLAGS=-O0 -g
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O4 -Munroll=c:1 -Mnoframe -Mlre -Mautoinline -Mvect=sse -Mcache_align -Mflushz -Mpre -Mneginfo=all
FC=nvfortran
FFLAGS=-Ofast
LDFLAGSCOV=
CXXFLAGSCOV=
LIBSCOV=
GCOV=:
