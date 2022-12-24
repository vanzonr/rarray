# 
# Makefile - make file for rarray
#
# Copyright (c) 2013-2022  Ramses van Zon
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# To install the header files:             make install [PREFIX=directory]
#
# This is the only required action.  If (optionally) you want to run
# the tests and benchmarks, first run './configure' to detect the
# compiler and certain libraries and create config.mk with the
# results, then you can do the following:
#
# To build and run tests:                 make test
# To build and run the benchmarks:        make benchmarks
# To rebuild the headers:                 make headers
# To rebuild the documentation:           make doc
#

-include config.mk

SRC=src
HS=headersources

PREFIX?=/usr

LDFLAGS?=
LDLIBS?= 
LDFLAGSOPT?=
LDLIBSOPT?=
CXXFLAGS?=-std=c++11 -Og
DBGFLAGS?=-g
CXXFLAGSOPT?=-O3
MORECPPFLAGSOPT?=-DNOBLITZ -DNOBOOST -DNOEIGEN3 -DNOARMADILLO -DNOOPENBLAS
MORELDFLAGSOPT?=
FC?=gfortran
FFLAGS?=-O2

CPPFLAGS+= -I. -I${HS}

# want to add coverage test later:
#LDFLAGSCOV=-fprofile-arcs -ftest-coverage
#CXXFLAGSCOV=-fprofile-arcs -ftest-coverage
#LIBSCOV=

RM=rm -f
VALGRIND?=valgrind --leak-check=full

BENCHMARK2DNAME=benchmark2Daccess
BENCHMARK4DNAME=benchmark4Daccess
BENCHMARK2DNAMEF=benchmark2Dfrtrn
BENCHMARK4DNAMEF=benchmark4Dfrtrn
PASS=optbarrier

help:
	@echo ""
	@echo "This makefile can install, test, and benchmark the rarray library."
	@echo ""
	@echo "  To install the header files:            make install [PREFIX=directory]"
	@echo ""
	@echo "This is the only required action.  If (optionally) you want to run tests and benchmarks, first run './configure' to detect the compiler and certain libraries and create config.mk with the results, then you can do the following:"
	@echo ""
	@echo "  To build and run tests:                 make test"
	@echo "  To build and run benchmarks:            make benchmarks"
	@echo "  To rebuild the headers:                 make headers"
	@echo "  To rebuild the documentation:           make doc"
	@echo ""

.PHONY: headers test benchmarks doc valgrindtest install clean distclean run_test_shared_buffer run_test_offsets run_test_shared_shape run_test_rarray run_valgrind_testsuite list benchmark2d benchmark4d run_testsuite run_testsuite_bc

headers: rarray rarrayio

all: headers test_shared_buffer test_offsets test_shared_shape test_rarray testsuite

test: run_testsuite run_testsuite_bc run_test_shared_buffer run_test_offsets run_test_shared_shape run_test_rarray run_valgrind_testsuite

benchmarks: benchmark2d benchmark4d

doc: rarraydoc.pdf

valgrindtest: run_test_shared_buffer run_test_offsets run_test_shared_shape run_test_rarray run_valgrind_testsuite 

hardinclude: ${SRC}/hardinclude.cc
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -o $@ $^

VERSION:
	git describe --abbrev=0 > $@

${HS}/versionheader.h: VERSION
	echo "// define rarray version (i.e. latest git tag)" > $@
	echo -n "#define RA_VERSION \"" >> $@
	cat VERSION | tr -d '\n' >> $@
	echo "\"" >> $@
	echo -n "#define RA_VERSION_NUMBER " >> $@
	cat VERSION | tr -dc '0-9.\n' | awk -F\. '{print 1000000*$$1 + 1000*$$2 + $$3}' >> $@

rarray: ${HS}/rarray.h ${HS}/rarraymacros.h ${HS}/rarraydelmacros.h ${HS}/shared_buffer.h ${HS}/shared_shape.h ${HS}/offsets.h ${HS}/rarrayio.h ${HS}/versionheader.h hardinclude
	cd ${HS} ; ../hardinclude rarray.h rarraymacros.h rarraydelmacros.h shared_buffer.h shared_shape.h rarrayio.h offsets.h versionheader.h > ../rarray

rarrayio: rarray
	echo '#include <rarray>' > rarrayio

install: rarray rarrayio rarraydoc.pdf
	mkdir -p ${PREFIX}/include
	cp -f rarray ${PREFIX}/include/rarray
	cp -f rarrayio ${PREFIX}/include/rarrayio
	mkdir -p ${PREFIX}/share/rarray
	cp -f rarraydoc.pdf ${PREFIX}/share/rarray

rarraydoc.pdf: rarraydoc.tex
	pdflatex $^
	pdflatex $^

testsuite: testsuite.o
	${CXX} ${LDFLAGS} ${LDFLAGSCOV} -o $@ $< ${LIBSCOV}

testsuite.o: ${SRC}/testsuite.cc rarray rarrayio catch.hpp
	${CXX} ${CPPFLAGS} ${CXXFLAGS} ${CXXFLAGSCOV} -g -O0 -c -o $@ $<

testsuite_bc: testsuite_bc.o
	${CXX} ${LDFLAGS} ${LDFLAGSCOV} -o $@ $< ${LIBSCOV}

testsuite_bc.o: ${SRC}/testsuite.cc rarray rarrayio catch.hpp
	${CXX} ${CPPFLAGS} ${CXXFLAGS} ${CXXFLAGSCOV} -DRA_BOUNDSCHECK -g -O0 -c -o $@ $<

catch.hpp:
	wget https://github.com/catchorg/Catch2/releases/download/v2.11.1/catch.hpp
	sed -i 's/\(static constexpr std::size_t sigStackSize = 32768\).*/\1;\/\//' catch.hpp

test_shared_buffer.o: ${SRC}/test_shared_buffer.cc ${HS}/shared_buffer.h
	${CXX} ${CPPFLAGS} ${DBGFLAGS} -c -o $@ $<

test_shared_buffer: test_shared_buffer.o
	${CXX} ${LDFLAGS} -o $@ $^

test_offsets.o: ${SRC}/test_offsets.cc ${HS}/offsets.h
	${CXX} ${CPPFLAGS} ${CXXFLAGS} ${DBGFLAGS} -c -o $@ $<

test_offsets: test_offsets.o
	${CXX} ${LDFLAGS} ${DBGFLAGS} -o $@ $^

test_shared_shape.o: ${SRC}/test_shared_shape.cc ${HS}/shared_shape.h ${HS}/offsets.h
	${CXX} ${CPPFLAGS} ${CXXFLAGS} ${DBGFLAGS} -c -o $@ $<

test_shared_shape: test_shared_shape.o
	${CXX} ${LDFLAGS} ${DBGFLAGS} -o $@ $^

test_rarray.o: ${SRC}/test_rarray.cc rarray
	${CXX} ${CPPFLAGS} ${CXXFLAGS} ${DBGFLAGS} -c -o $@ $<

test_rarray: test_rarray.o
	${CXX} ${LDFLAGS} -o $@ $^

clean:
	${RM} test_shared_buffer.o test_offsets.o test_shared_shape.o test_rarray.o testsuite.o benchmark2Daccess.o benchmark4Daccess.o benchmark2Dfrtrn.o benchmark4Dfrtrn.o optbarrier.o optbarrierf.o rarraydoc.log rarraydoc.out rarraydoc.aux rarraydoc.toc catch.hpp

distclean:
	make clean
	make rarray rarrayio doc	
	make clean
	${RM} test_shared_buffer test_offsets test_shared_shape test_rarray testsuite hardinclude benchmark2Daccess benchmark4Daccess benchmark2Dfrtrn benchmark4Dfrtrn config.mk

run_test_shared_buffer: test_shared_buffer
	${VALGRIND} ./test_shared_buffer

run_test_offsets: test_offsets
	${VALGRIND} ./test_offsets

run_test_shared_shape: test_shared_shape
	${VALGRIND} ./test_shared_shape

run_test_rarray: test_rarray
	${VALGRIND} ./test_rarray

run_testsuite: testsuite
	./testsuite

run_testsuite_bc: testsuite_bc
	./testsuite_bc

run_valgrind_testsuite: testsuite
	${VALGRIND} ./testsuite

list:
	@grep '^[^#[:space:]].*:' Makefile

benchmark2d: $(BENCHMARK2DNAME) $(BENCHMARK2DNAMEF)
	@echo Comparison benchmark on a 2d array example
	@./$(BENCHMARK2DNAME) 1 
	@(ulimit -s 4000000; ./$(BENCHMARK2DNAME) 2) 
	@./$(BENCHMARK2DNAME) 3
	@./$(BENCHMARK2DNAME) 4
	@./$(BENCHMARK2DNAME) 5
	@./$(BENCHMARK2DNAME) 6
	@./$(BENCHMARK2DNAME) 7
	@./$(BENCHMARK2DNAME) 8
	@./$(BENCHMARK2DNAME) 9
	@./$(BENCHMARK2DNAMEF)
	@./$(BENCHMARK2DNAME) 1

benchmark4d: $(BENCHMARK4DNAME) $(BENCHMARK4DNAMEF)
	@echo Comparison benchmark on a 4d array example
	@./$(BENCHMARK4DNAME) 1 
	@(ulimit -s 4000000; ./$(BENCHMARK4DNAME) 2) 
	@./$(BENCHMARK4DNAME) 3
	@./$(BENCHMARK4DNAME) 4
	@./$(BENCHMARK4DNAME) 5
	@./$(BENCHMARK4DNAME) 6
	@./$(BENCHMARK4DNAME) 7
	@./$(BENCHMARK4DNAME) 8
	@./$(BENCHMARK4DNAME) 9
	@./$(BENCHMARK4DNAMEF)
	@./$(BENCHMARK4DNAME) 1

$(BENCHMARK2DNAME): $(BENCHMARK2DNAME).o $(PASS).o config.mk
	$(CXX) $(LDFLAGSOPT) -o $@ $(BENCHMARK2DNAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK4DNAME): $(BENCHMARK4DNAME).o $(PASS).o config.mk
	$(CXX) $(LDFLAGSOPT) -o $@ $(BENCHMARK4DNAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK2DNAMEF): ${SRC}/$(BENCHMARK2DNAMEF).f90 $(PASS)f.o config.mk
	$(FC) $(FFLAGSOPT) -o $@ ${SRC}/$(BENCHMARK2DNAMEF).f90 $(PASS)f.o 

$(BENCHMARK4DNAMEF): ${SRC}/$(BENCHMARK4DNAMEF).f90 $(PASS)f.o config.mk
	$(FC) $(FFLAGSOPT) -o $@ ${SRC}/$(BENCHMARK4DNAMEF).f90 $(PASS)f.o 

$(PASS)f.o: ${SRC}/$(PASS)f.f90 config.mk
	$(FC) -c ${DBGFLAGS} -o $@ $<

$(BENCHMARK2DNAME).o: ${SRC}/$(BENCHMARK2DNAME).cc rarray ${HS}/rarraymacros.h ${HS}/rarraydelmacros.h ${HS}/elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGS) $(CXXFLAGSOPT) -c -o $@ $<

$(BENCHMARK4DNAME).o: ${SRC}/$(BENCHMARK4DNAME).cc rarray ${HS}/rarraymacros.h ${HS}/rarraydelmacros.h ${HS}/elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGS) $(CXXFLAGSOPT) -c -o $@ $<

$(PASS).o: ${SRC}/$(PASS).cc config.mk
	$(CXX) -c -o $@ $<
