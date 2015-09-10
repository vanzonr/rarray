# 
# Makefile - make file for rarray.h
#
# Copyright (c) 2013-2015  Ramses van Zon
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
# To run unit tests through valgrind:     make valgrindtest
# To check code coverage of tests:        make covertest
# To build and run benchmark test:        make benchmark
# To build the documentation:             make doc
# To test the code in the documentation:  make doctest
# To install rarray.h:                    make [PREFIX=directory]
#

include config.mk

PREFIX?=/usr

CCL=${CXX}  
LDFLAGS?=-g
LDLIBS?= 
LDFLAGSOPT?=
LDLIBSOPT?=
CXXFLAGSOPT?=-O2
CHECKCPPFLAGS=-DRA_BOUNDSCHECK
#MORECPPFLAGSOPT=-DBOOST_DISABLE_ASSERTS -DNDEBUG -DEIGEN_NO_DEBUG -DNOARMADILLO
#-DNOBLITZ -DNOEIGEN3 -DNOARMADILLO -DNOBOOST
MORECPPFLAGSOPT?=-DNOBLITZ -DNOBOOST -DNOEIGEN3 -DNOARMADILLO
MORELDFLAGSOPT?=
FC?=gfortran
FFLAGS?=-O2

TESTNAME?=rarraytests
TESTXNAME?=rarrayextest
BENCHMARK2DNAME=benchmark2Daccess
BENCHMARK4DNAME=benchmark4Daccess
BENCHMARK2DNAMEF=benchmark2Dfrtrn
BENCHMARK4DNAMEF=benchmark4Dfrtrn
PASS=optbarrier

all: test valgrindtest covertest benchmark doctest rarrayextest

rarrayextest: rarrayextest.cc rarray.h rarrayex.h rarraymacros.h rarraydelmacros.h
	$(CXX) -std=c++11 -DRA_BOUNDSCHECK -o $@ $<
	./rarrayextest

.PHONY: clean test covertest benchmark install doctest doc valgrindtest

config.mk:
	@echo "Error: Run 'configure' to create config.mk"
	@false

install: rarray.h rarrayio.h rarrayex.h rarraydoc.pdf rarraymacros.h rarraydelmacros.h
	mkdir -p ${PREFIX}/include
	cp rarray.h ${PREFIX}/include/rarray.h
	cp rarrayio.h ${PREFIX}/include/rarrayio.h
	cp rarrayex.h ${PREFIX}/include/rarrayex.h
	cp rarraymacros.h ${PREFIX}/include/rarraymacros.h
	cp rarraydelmacros.h ${PREFIX}/include/rarraydelmacros.h
	mkdir -p ${PREFIX}/share/rarray
	cp rarraydoc.pdf ${PREFIX}/share/rarray

doc: rarraydoc.pdf

rarraydoc.pdf: rarraydoc.tex
	pdflatex rarraydoc.tex 
	mv rarraydoc.pdf rarraydoc0.pdf
	qpdf --stream-data=uncompress rarraydoc0.pdf rarraydoc.pdf
	rm -f rarraydoc0.pdf

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
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

doc%.cc: doctestgenerator.sh
	sh doctestgenerator.sh

doctestgenerator.sh: rarraydoc.tex config.mk
	awk '/%TEST THIS/{a=1;n+=1;print "cat > doc" n ".cc << EOF";next}/%END TEST THIS/{a=0; print "EOF\n"}a' rarraydoc.tex | sed -e 's/^  //' -e 's/\\begin{verbatim}/#include "rarray.h"/' | grep -v verbatim> doctestgenerator.sh

test: $(TESTNAME) $(TESTXNAME)
	./$(TESTNAME) --report_level=detailed
	./$(TESTXNAME) --report_level=detailed

valgrindtest: $(TESTNAME)
	valgrind --tool=memcheck $(TESTNAME)

rarraytests: $(TESTNAME).o config.mk
	$(CCL) $(TESTLDFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(TESTNAME).o: $(TESTNAME).cc rarray.h rarraymacros.h rarraydelmacros.h rarrayio.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CHECKCPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TESTXNAME).o: $(TESTXNAME).cc rarrayex.h rarray.h rarraymacros.h rarraydelmacros.h rarrayio.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CHECKCPPFLAGS) $(CXXFLAGS) -c -o $@ $<

benchmark: benchmark2d benchmark4d

benchmark2d: $(BENCHMARK2DNAME) $(BENCHMARK2DNAMEF)
	@echo benchmark on a 2d array example
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

benchmark4d: $(BENCHMARK4DNAME) $(BENCHMARK4DNAMEF)
	@echo benchmark on a 4d array example
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

$(BENCHMARK2DNAME).o: $(BENCHMARK2DNAME).cc rarray.h rarraymacros.h rarraydelmacros.h elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(BENCHMARK4DNAME).o: $(BENCHMARK4DNAME).cc rarray.h rarraymacros.h rarraydelmacros.h  elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(PASS).o: $(PASS).cc config.mk
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

covertest: \
 output_from_test.txt  \
 coverage_in_test.txt  \
 coverage_in_code.txt  \
 missing_from_test.txt \
 summary

coverage_in_code.txt: rarray.h rarraymacros.h rarraydelmacros.h rarrayio.h
	@echo "Extracting lines from array.h that generate profile messages"
	\grep -n RA_IFTRACESAY rarray.h | grep -v '#define' | grep -v '#undef' | sed -e 's/   RA_IFTRACESAY("//' -e 's/");.*//' -e 's/\/\*\*\///' -e 's/\/\*!!!!\*\///'  | sort -n | sed 's/^[0-9]*: //'> coverage_in_code.txt

coverage_in_test.txt: output_from_test.txt output_from_nitest.txt 
	@echo "Filtering profile messages from test output"
	grep -h IFTRACE output_from_test.txt output_from_nitest.txt | sort -u | sed 's/IFTRACE rarray.h@//' | tr '\t' ' ' | sort -n | sed 's/^[0-9]*: //' > coverage_in_test.txt

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

profiletests: $(TESTNAME).cc rarray.h rarraymacros.h rarraydelmacros.h rarrayio.h config.mk
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on"
	$(CXX) $(CXXFLAGS) ${TESTCPPFLAGS} $(CPPFLAGS) -DRA_TRACETEST $(TESTNAME).cc -o profiletests ${TESTLDFLAGS} 

profilenitests: $(TESTNAME).cc rarray.h rarraymacros.h rarraydelmacros.h rarrayio.h config.mk
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on and skipping intermediate objects for indexing"
	$(CXX) -DRA_SKIPINTERMEDIATE $(CXXFLAGS) $(TESTCPPFLAGS) $(CPPFLAGS) -DRA_TRACETEST $(TESTNAME).cc -o profilenitests  $(TESTLDFLAGS)

summary: coverage_in_code.txt coverage_in_test.txt missing_from_test.txt
	@echo "Summary:"
	@wc -l coverage_in_code.txt
	@wc -l coverage_in_test.txt
	@wc -l missing_from_test.txt

clean:
	rm -f $(TESTNAME).o rarrayextest.o $(TESTNAME)-cov.o $(TEXTXNAME).o $(TESTNAME)-ni-cov.o $(BENCHMARK4DNAME).o $(BENCHMARK2DNAME).o $(PASS).o $(PASS)f.o \
	profiletests profilenitests output_from_test.txt output_from_nitest.txt coverage_in_code.txt coverage_in_test.txt missing_from_test.txt  \
	doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x doc11.x doc12.x \
	doc1.cc doc2.cc doc3.cc doc4.cc doc5.cc doc6.cc doc7.cc doc8.cc doc9.cc doc10.cc doc11.cc doc12.cc doctestgenerator.sh \
	rarraydoc.aux rarraydoc.log rarraydoc.out rarraydoc.dvi

distclean: clean
	rm -f config.mk $(TESTNAME) $(BENCHMARK2DNAME) $(BENCHMARK4DNAME) $(BENCHMARK2DNAMEF) $(BENCHMARK4DNAMEF) $(TESTXNAME)


