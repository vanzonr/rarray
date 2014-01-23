# 
# Makefile - make file for rarray.h
#
# Copyright (c) 2013-2014  Ramses van Zon
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
MORECPPFLAGS=-DAR_BOUNDSCHECK
MORECPPFLAGSOPT=-DBOOST_DISABLE_ASSERTS -DNDEBUG -DEIGEN_NO_DEBUG
FC?=gfortran
FFLAGS?=-O2

TESTNAME=rarraytestsuite
BENCHMARK2NAME=rarray2dspeed
BENCHMARK4NAME=rarray4dspeed
BENCHMARK2NAMEF=frtrn2dspeed
BENCHMARK4NAMEF=frtrn4dspeed
PASS=optbarrier

all: test valgrindtest covertest benchmark doctest

.PHONY: clean test covertest benchmark install doctest doc valgrindtest

config.mk:
	@echo "Error: Run 'configure' to create config.mk"
	@false

install: rarray.h rarray.pdf
	mkdir -p ${PREFIX}/include
	cp rarray.h ${PREFIX}/include/rarrar.h
	mkdir -p ${PREFIX}/share/rarray
	cp rarray.pdf ${PREFIX}/share/rarray

doc: rarray.pdf

rarray.pdf: rarray.tex
	pdflatex rarray.tex

doctest: doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x config.mk
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

doc%.x: doc%.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

doc%.cc: doctestgenerator.sh
	sh doctestgenerator.sh

doctestgenerator.sh: rarray.tex config.mk
	awk '/%TEST THIS/{a=1;n+=1;print "cat > doc" n ".cc << EOF";next}/%END TEST THIS/{a=0; print "EOF\n"}a' rarray.tex | sed -e 's/^  //' -e 's/\\begin{verbatim}/#include "rarray.h"/' | grep -v verbatim> doctestgenerator.sh

test: $(TESTNAME)
	./$(TESTNAME)

valgrindtest: $(TESTNAME)
	valgrind --tool=memcheck $(TESTNAME)

$(TESTNAME): $(TESTNAME).o config.mk
	$(CCL) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(TESTNAME).o: $(TESTNAME).cc rarray.h config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CXXFLAGS) -c -o $@ $<

benchmark: benchmark2 benchmark4

benchmark2: $(BENCHMARK2NAME) $(BENCHMARK2NAMEF)
	@echo benchmark on a 2d array example
	@./$(BENCHMARK2NAME) 1
	@(ulimit -s unlimited; ./$(BENCHMARK2NAME) 2) 
	@./$(BENCHMARK2NAME) 3
	@./$(BENCHMARK2NAME) 4
	@./$(BENCHMARK2NAME) 6
	@./$(BENCHMARK2NAME) 7
	@./$(BENCHMARK2NAME) 8
	@./$(BENCHMARK2NAME) 9
	@./$(BENCHMARK2NAMEF)

benchmark4: $(BENCHMARK4NAME) $(BENCHMARK4NAMEF)
	@echo benchmark on a 4d array example
	@./$(BENCHMARK4NAME) 1
	@(ulimit -s unlimited; ./$(BENCHMARK4NAME) 2) 
	@./$(BENCHMARK4NAME) 3
	@./$(BENCHMARK4NAME) 4
	@./$(BENCHMARK4NAME) 6
	@./$(BENCHMARK4NAME) 7
	@./$(BENCHMARK4NAME) 8
	@./$(BENCHMARK4NAME) 9
	@./$(BENCHMARK4NAMEF)

$(BENCHMARK2NAME): $(BENCHMARK2NAME).o $(PASS).o config.mk
	$(CCL) $(LDFLAGSOPT) -o $@ $(BENCHMARK2NAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK4NAME): $(BENCHMARK4NAME).o $(PASS).o config.mk
	$(CCL) $(LDFLAGSOPT) -o $@ $(BENCHMARK4NAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK2NAMEF): $(BENCHMARK2NAMEF).f90 $(PASS)f.o config.mk
	$(FC) $(FFLAGS) -o $@ $(BENCHMARK2NAMEF).f90 $(PASS)f.o 

$(BENCHMARK4NAMEF): $(BENCHMARK4NAMEF).f90 $(PASS)f.o config.mk
	$(FC) $(FFLAGS) -o $@ $(BENCHMARK4NAMEF).f90 $(PASS)f.o 

$(PASS)f.o: $(PASS)f.f90 config.mk
	$(FC) -c -O0 -g -o $@ $<

$(BENCHMARK2NAME).o: $(BENCHMARK2NAME).cc rarray.h elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(BENCHMARK4NAME).o: $(BENCHMARK4NAME).cc rarray.h elapsed.h config.mk
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(PASS).o: $(PASS).cc config.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CXXFLAGS) -c -o $@ $<

covertest: \
 output_from_test.txt  \
 coverage_in_test.txt  \
 coverage_in_code.txt  \
 missing_from_test.txt \
 summary

coverage_in_code.txt: rarray.h
	@echo "Extracting lines from array.h that generate profile messages"
	\grep -n AR_PROFILESAY rarray.h | grep -v '#define' | grep -v '#undef' | sed -e 's/   AR_PROFILESAY("//' -e 's/");.*//' -e 's/\/\*\*\///' -e 's/\/\*!!!!\*\///'  | sort -n | sed 's/^[0-9]*: //'> coverage_in_code.txt

coverage_in_test.txt: output_from_test.txt output_from_nitest.txt 
	@echo "Filtering profile messages from test output"
	grep -h PROFILE output_from_test.txt output_from_nitest.txt | sort -u | sed 's/PROFILE rarray.h@//' | tr '\t' ' ' | sort -n | sed 's/^[0-9]*: //' > coverage_in_test.txt

output_from_test.txt: profiletests
	@echo "Run tests with profile messages on"
	profiletests 2> output_from_test.txt

output_from_nitest.txt: profilenitests
	@echo "Run tests with profile messages on"
	profilenitests 2> output_from_nitest.txt

missing_from_test.txt: coverage_in_code.txt coverage_in_test.txt
	@echo "Determine profiling lines in array.h that did not get executed"
	sort coverage_in_code.txt > _coverage_in_code.txt 
	sort coverage_in_test.txt > _coverage_in_test.txt
	comm -2 -3 _coverage_in_code.txt _coverage_in_test.txt | sort -n > missing_from_test.txt
	rm -f _coverage_in_code.txt _coverage_in_test.txt

profiletests: $(TESTNAME).cc rarray.h config.mk
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(MORECPPFLAGS) -DAR_TRACETEST $(TESTNAME).cc -o profiletests

profilenitests: $(TESTNAME).cc rarray.h config.mk
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on and skipping intermediate objects for indexing"
	$(CXX) -DAR_SKIPINTERMEDIATE $(CXXFLAGS) $(CPPFLAGS) -DAR_TRACETEST $(TESTNAME).cc -o profilenitests

summary: coverage_in_code.txt coverage_in_test.txt missing_from_test.txt
	@echo "Summary:"
	@wc -l coverage_in_code.txt
	@wc -l coverage_in_test.txt
	@wc -l missing_from_test.txt

clean:
	rm -f $(TESTNAME).o $(TESTNAME)-cov.o $(TESTNAME)-ni-cov.o $(BENCHMARK4NAME).o $(BENCHMARK2NAME).o $(PASS).o $(PASS)f.o \
	profiletests profilenitests output_from_test.txt output_from_nitest.txt coverage_in_code.txt coverage_in_test.txt missing_from_test.txt  \
	doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x \
	doc1.cc doc2.cc doc3.cc doc4.cc doc5.cc doc6.cc doc7.cc doc8.cc doc9.cc doc10.cc doctestgenerator.sh \
	rarray.aux rarray.log rarray.out rarray.dvi

distclean: clean
	rm -f config.mk $(TESTNAME) $(BENCHMARK2NAME) $(BENCHMARK4NAME) $(BENCHMARK2NAMEF) $(BENCHMARK4NAMEF) rarray.pdf


