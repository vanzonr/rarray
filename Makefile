# 
# Makefile - make file for rarray
#
# Copyright (c) 2013-2016  Ramses van Zon
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
# To build and run unit tests:            make test
# To build and run old unit tests:        make oldtest
# To run unit tests through valgrind:     make valgrindtest
# To check code coverage of tests:        make covertest
# To build and run benchmark test:        make benchmark
# To build the documentation:             make doc
# To test the code in the documentation:  make doctest
# To install the header files:            make install [PREFIX=directory]
#

-include config.mk

PREFIX?=/usr

CCL=${CXX}  
LDFLAGS?=-g
LDLIBS?= 
LDFLAGSOPT?=
LDLIBSOPT?=
CPPFLAGS=-I.
CXXFLAGSOPT?=-O2
CHECKCPPFLAGS=-DRA_BOUNDSCHECK
#MORECPPFLAGSOPT=-DBOOST_DISABLE_ASSERTS -DNDEBUG -DEIGEN_NO_DEBUG -DNOARMADILLO
#-DNOBLITZ -DNOEIGEN3 -DNOARMADILLO -DNOBOOST
MORECPPFLAGSOPT?=-DNOBLITZ -DNOBOOST -DNOEIGEN3 -DNOARMADILLO
MORELDFLAGSOPT?=
FC?=gfortran
FFLAGS?=-O2

TESTNAME?=rarraytests
OLDTESTNAME?=oldboostlessrarraytestsuite
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
	@echo "  To build and run unit tests:            make test"
	@echo "  To run unit tests through valgrind:     make valgrindtest"
	@echo "  To check code coverage of tests:        make covertest"
	@echo "  To build and run benchmark test:        make benchmark"
	@echo "  To rebuild the documentation:           make doc"
	@echo "  To test the code in the documentation:  make doctest"
	@echo ""
	@echo "Only the first one is mandatory, the other six are all optional and"
	@echo "not required for installation."
	@echo ""
	@echo "Prerequisites (apart from standard unix tools):"
	@echo "  test, covertest, doctest    config.mk"
	@echo "  valgrindtest                config.mk + valgrind"
	@echo "  benchmark                   config.mk"
	@echo ""
	@echo "Here, config.mk is a file that must be created by running ./configure first. "
	@echo "The configure script detects your compiler and searches for the Blitz++, BOOST, "
	@echo "Armadillo and Eigen libraries for comparison in the benchmarks but will skip "
	@echo "them if they are not installed."
	@echo ""
	@echo ""
	@echo "Note: The config.mk file is not required for installation, because rarray"
	@echo "is a header-only library."

all: test valgrindtest covertest benchmark doctest 

.PHONY: clean test covertest benchmark install doctest doc valgrindtest help

config.mk:
	@echo "Warning: Run 'configure' to create config.mk"
	@false

install: rarray rarrayio rarraydoc.pdf rarraymacros.h rarraydelmacros.h hardinclude
	mkdir -p ${PREFIX}/include
	./hardinclude rarray rarraymacros.h rarraydelmacros.h > ${PREFIX}/include/rarray
	cp -p rarrayio ${PREFIX}/include/rarrayio
	mkdir -p ${PREFIX}/share/rarray
	cp -p rarraydoc.pdf ${PREFIX}/share/rarray

hardinclude: hardinclude.cc

doc: rarraydoc-new.pdf

rarraydoc-old.pdf:
	mv rarraydoc.pdf rarraydoc-old.pdf

rarraydoc-new.pdf: rarraydoc.tex rarraydoc-old.pdf
	pdflatex rarraydoc.tex 
	pdflatex rarraydoc.tex 
	mv rarraydoc.pdf rarraydoc0.pdf
	qpdf --stream-data=uncompress rarraydoc0.pdf rarraydoc.pdf
	rm -f rarraydoc0.pdf
	cp -f rarraydoc.pdf rarraydoc-new.pdf

doctest: doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x doc11.x doc12.x config.mk
	./doc1.x
	./doc2.x
	./doc3.x
	./doc4.x
	./doc5.x
	./doc6.x
	./doc7.x
	./doc8.x
	./doc9.x
	./doc10.x
	./doc11.x
	./doc12.x

doc%.x: doc%.cc
	$(CXX) -I. $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

doc%.cc: doctestgenerator.sh
	sh doctestgenerator.sh

doctestgenerator.sh: rarraydoc.tex config.mk
	awk '/%TEST THIS/{a=1;n+=1;print "cat > doc" n ".cc << EOF";next}/%END TEST THIS/{a=0; print "EOF\n"}a' rarraydoc.tex | sed -e 's/^  //' -e 's/\\begin{verbatim}/#include <rarray>/' | grep -v verbatim> doctestgenerator.sh

test: $(TESTNAME) $(TESTXNAME)
	./$(TESTNAME) --report_level=detailed

oldtest: $(OLDTESTNAME)
	./$(OLDTESTNAME) --report_level=detailed

valgrindtest: $(TESTNAME)
	valgrind --tool=memcheck ./$(TESTNAME)

$(TESTNAME): $(TESTNAME).o config.mk
	$(CCL) $(TESTLDFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(OLDTESTNAME): $(OLDTESTNAME).o config.mk
	$(CCL) $(TESTLDFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(TESTNAME).o: $(TESTNAME).cc rarray rarraymacros.h rarraydelmacros.h rarrayio config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CHECKCPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OLDTESTNAME).o: $(OLDTESTNAME).cc rarray rarraymacros.h rarraydelmacros.h rarrayio config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CHECKCPPFLAGS) $(CXXFLAGS) -c -o $@ $<

benchmark: benchmark2d benchmark4d

benchmark2d: $(BENCHMARK2DNAME) $(BENCHMARK2DNAMEF)
	@echo benchmark on a 2d array example
	@./$(BENCHMARK2DNAME) 1 > /dev/null
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
	@echo benchmark on a 4d array example
	@./$(BENCHMARK4DNAME) 1 >& /dev/null
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
	$(CCL) $(LDFLAGSOPT) -o $@ $(BENCHMARK2DNAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK4DNAME): $(BENCHMARK4DNAME).o $(PASS).o config.mk
	$(CCL) $(LDFLAGSOPT) -o $@ $(BENCHMARK4DNAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK2DNAMEF): $(BENCHMARK2DNAMEF).f90 $(PASS)f.o config.mk
	$(FC) $(FFLAGS) -o $@ $(BENCHMARK2DNAMEF).f90 $(PASS)f.o 

$(BENCHMARK4DNAMEF): $(BENCHMARK4DNAMEF).f90 $(PASS)f.o config.mk
	$(FC) $(FFLAGS) -o $@ $(BENCHMARK4DNAMEF).f90 $(PASS)f.o 

$(PASS)f.o: $(PASS)f.f90 config.mk
	$(FC) -c -O0 -g -o $@ $<

$(BENCHMARK2DNAME).o: $(BENCHMARK2DNAME).cc rarray rarraymacros.h rarraydelmacros.h elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(BENCHMARK4DNAME).o: $(BENCHMARK4DNAME).cc rarray rarraymacros.h rarraydelmacros.h elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(PASS).o: $(PASS).cc config.mk
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

covertest: \
 output_from_test.txt  \
 coverage_in_test.txt  \
 coverage_in_code.txt  \
 missing_from_test.txt \
 summary

coverage_in_code.txt: rarray rarraymacros.h rarraydelmacros.h rarrayio
	@echo "Extracting lines from array.h that generate profile messages"
	\grep -n RA_IFTRACESAY rarray | grep -v '#define' | grep -v '#undef' | sed -e 's/   RA_IFTRACESAY("//' -e 's/");.*//' -e 's/\/\*\*\///' -e 's/\/\*!!!!\*\///'  | sort -n | sed 's/^[0-9]*: //'> coverage_in_code.txt

coverage_in_test.txt: output_from_test.txt output_from_nitest.txt 
	@echo "Filtering profile messages from test output"
	grep -h IFTRACE output_from_test.txt output_from_nitest.txt | sort -u | sed 's/IFTRACE rarray@//' | tr '\t' ' ' | sort -n | sed 's/^[0-9]*: //' > coverage_in_test.txt

output_from_test.txt: profiletests
	@echo "Run tests with profile messages on"
	./profiletests 2> output_from_test.txt

output_from_nitest.txt: profilenitests
	@echo "Run tests with profile messages on"
	./profilenitests 2> output_from_nitest.txt

missing_from_test.txt: coverage_in_code.txt coverage_in_test.txt
	@echo "Determine profiling lines in array.h that did not get executed"
	sort coverage_in_code.txt > _coverage_in_code.txt 
	sort coverage_in_test.txt > _coverage_in_test.txt
	comm -2 -3 _coverage_in_code.txt _coverage_in_test.txt | sort -n > missing_from_test.txt
	rm -f _coverage_in_code.txt _coverage_in_test.txt

profiletests: $(TESTNAME).cc rarray rarraymacros.h rarraydelmacros.h rarrayio config.mk
	@echo "Compile $(TESTNAME).cc and rarray with profile messages on"
	$(CXX) $(CXXFLAGS) ${TESTCPPFLAGS} $(CPPFLAGS) -DRA_TRACETEST $(TESTNAME).cc -o profiletests ${TESTLDFLAGS} 

profilenitests: $(TESTNAME).cc rarray rarraymacros.h rarraydelmacros.h rarrayio config.mk
	@echo "Compile $(TESTNAME).cc and rarray with profile messages on and skipping intermediate objects for indexing"
	$(CXX) -DRA_SKIPINTERMEDIATE $(CXXFLAGS) $(TESTCPPFLAGS) $(CPPFLAGS) -DRA_TRACETEST $(TESTNAME).cc -o profilenitests  $(TESTLDFLAGS)

summary: coverage_in_code.txt coverage_in_test.txt missing_from_test.txt
	@echo "Summary:"
	@wc -l coverage_in_code.txt
	@wc -l coverage_in_test.txt
	@wc -l missing_from_test.txt

clean:
	rm -f $(TESTNAME).o $(OLDTESTNAME).o rarrayextest.o $(TESTNAME)-cov.o $(TEXTXNAME).o $(TESTNAME)-ni-cov.o $(BENCHMARK4DNAME).o $(BENCHMARK2DNAME).o $(PASS).o $(PASS)f.o \
	profiletests profilenitests output_from_test.txt output_from_nitest.txt coverage_in_code.txt coverage_in_test.txt missing_from_test.txt  \
	doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x doc11.x doc12.x \
	doc1.cc doc2.cc doc3.cc doc4.cc doc5.cc doc6.cc doc7.cc doc8.cc doc9.cc doc10.cc doc11.cc doc12.cc doctestgenerator.sh \
	rarraydoc.aux rarraydoc.log rarraydoc.out rarraydoc.dvi

distclean: clean
	rm -f config.mk $(TESTNAME) $(OLDTESTNAME) $(BENCHMARK2DNAME) $(BENCHMARK4DNAME) $(BENCHMARK2DNAMEF) $(BENCHMARK4DNAMEF) $(TESTXNAME)


