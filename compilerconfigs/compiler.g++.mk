#
# g++ 4.8+ settings
#
CXX=g++ -std=c++11
CXX17=g++ -std=c++17
CXX23=g++ -std=c++23
CXXFLAGS=-Og
DBGFLAGS=-g
CPPFLAGS=-Wall -Wshadow -Wconversion -Wfatal-errors
CPPFLAGSOPT=-DNDEBUG
CXXFLAGSOPT=-O3 -fstrict-aliasing -march=native -fno-signed-zeros -fno-trapping-math -fassociative-math
FC=gfortran
FFLAGS=-O3 -fstrict-aliasing -march=native -fno-signed-zeros -fno-trapping-math -fassociative-math
LDFLAGSCOV=
CXXFLAGSCOV=-fprofile-arcs -ftest-coverage
LIBSCOV=-lgcov
GCOV=gcov -trkm
FILTERCOV=awk
NOELIDECONSTRUCTORS=-fno-elide-constructors
