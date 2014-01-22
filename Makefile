# Makefile for rarray.h
#
# To build and run unit tests:            make test
# To run unit tests through valgrind:     make valgrindtest
# To check code coverage of tests:        make covertest
# To build and run benchmark test:        make benchmark
# To build the documentation:             make doc
# To test the code in the documentation:  make doctest
# To install rarray.h:                    make [PREFIX=directory]
#

include compiler.mk

PREFIX?=/usr/local

FC?=gfortran
CCL?=${CXX}  
LDFLAGS?=-g
LDLIBS?= 
LDFLAGSOPT?=
LDLIBSOPT?=
MORECPPFLAGS=-DAR_BOUNDSCHECK
CXXFLAGSOPT?=-O2
FFLAGSOPT?=-O2
MORECPPFLAGSOPT=-DBOOST_DISABLE_ASSERTS

TESTNAME=rarraytestsuite
BENCHMARK2NAME=rarray2dspeed
BENCHMARK4NAME=rarray4dspeed
PASS=pass

all: test valgrindtest covertest benchmark doctest

.PHONY: clean test covertest benchmark install doctest doc valgrindtest

install: src/rarray.h doc/rarraydoc.pdf
	mkdir -p ${PREFIX}/include
	cp src/rarray.h ${PREFIX}/include/rarrar.h
	mkdir -p ${PREFIX}/share/doc/rarray
	cp doc/rarraydoc.pdf ${PREFIX}/share/doc/rarray

doc: doc/rarraydoc.pdf

doc/rarraydoc.pdf: doc/rarraydoc.tex
	(cd doc; pdflatex rarraydoc.tex; cd ..)

doctest: doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x
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
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -Isrc -o $@ $< $(LDLIBS)

doc%.cc: doctestgenerator.sh
	sh doctestgenerator.sh

doctestgenerator.sh: doc/rarraydoc.tex 
	awk '/%TEST THIS/{a=1;n+=1;print "cat > doc" n ".cc << EOF";next}/%END TEST THIS/{a=0; print "EOF\n"}a' doc/rarraydoc.tex | sed -e 's/^  //' -e 's/\\begin{verbatim}/#include "rarray.h"/' | grep -v verbatim> doctestgenerator.sh

test: $(TESTNAME)
	./$(TESTNAME)

valgrindtest: $(TESTNAME)
	valgrind --tool=memcheck $(TESTNAME)

$(TESTNAME): $(TESTNAME).o compiler.mk
	$(CCL) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(TESTNAME).o: src/$(TESTNAME).cc src/rarray.h compiler.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CXXFLAGS) -c -o $@ $<

benchmark: benchmark2 benchmark4

benchmark2: $(BENCHMARK2NAME) $(BENCHMARK2NAME)f
	@echo benchmark on a 2d array example

	@(ulimit -s unlimited; ./$(BENCHMARK2NAME) 2) 
	@./$(BENCHMARK2NAME) 3
	@./$(BENCHMARK2NAME) 4
	@./$(BENCHMARK2NAME) 6
	@./$(BENCHMARK2NAME) 7
	@./$(BENCHMARK2NAME) 8
	@./$(BENCHMARK2NAME) 9
	@./$(BENCHMARK2NAME)f

benchmark4: $(BENCHMARK4NAME) $(BENCHMARK4NAME)f
	@echo benchmark on a 4d array example
	@./$(BENCHMARK4NAME) 0
	@./$(BENCHMARK4NAME) 1
	@(ulimit -s unlimited; ./$(BENCHMARK4NAME) 2) 
	@./$(BENCHMARK4NAME) 3
	@./$(BENCHMARK4NAME) 4
	@./$(BENCHMARK4NAME) 6
	@./$(BENCHMARK4NAME) 7
	@./$(BENCHMARK4NAME) 8
	@./$(BENCHMARK4NAME) 9
	@./$(BENCHMARK4NAME)f

$(BENCHMARK2NAME): $(BENCHMARK2NAME).o $(PASS).o compiler.mk
	$(CCL) $(LDFLAGSOPT) -o $@ $(BENCHMARK2NAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK4NAME): $(BENCHMARK4NAME).o $(PASS).o compiler.mk
	$(CCL) $(LDFLAGSOPT) -o $@ $(BENCHMARK4NAME).o $(PASS).o $(LDLIBS)

$(BENCHMARK2NAME)f: src/$(BENCHMARK2NAME)f.f90 $(PASS)f.o compiler.mk
	$(FC) $(FFLAGSOPT) -o $@ src/$(BENCHMARK2NAME)f.f90 $(PASS)f.o 

$(BENCHMARK4NAME)f: src/$(BENCHMARK4NAME)f.f90 $(PASS)f.o compiler.mk
	$(FC) $(FFLAGSOPT) -o $@ src/$(BENCHMARK4NAME)f.f90 $(PASS)f.o 

$(PASS)f.o: src/$(PASS)f.f90 compiler.mk
	$(FC) -c -O0 -g -o $@ $<

$(BENCHMARK2NAME).o: src/$(BENCHMARK2NAME).cc src/rarray.h compiler.mk
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(BENCHMARK4NAME).o: src/$(BENCHMARK4NAME).cc src/rarray.h compiler.mk
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(MORECPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(PASS).o: src/$(PASS).cc compiler.mk
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CXXFLAGS) -c -o $@ $<

covertest: \
 output_from_test.txt  \
 coverage_in_test.txt  \
 coverage_in_code.txt  \
 missing_from_test.txt \
 summary

coverage_in_code.txt: src/rarray.h
	@echo "Extracting lines from array.h that generate profile messages"
	\grep -n AR_PROFILESAY src/rarray.h | grep -v '#define' | grep -v '#undef' | sed -e 's/   AR_PROFILESAY("//' -e 's/");.*//' -e 's/\/\*\*\///' -e 's/\/\*!!!!\*\///'  | sort -n | sed 's/^[0-9]*: //'> coverage_in_code.txt

coverage_in_test.txt: output_from_test.txt output_from_nitest.txt 
	@echo "Filtering profile messages from test output"
	grep -h PROFILE output_from_test.txt output_from_nitest.txt | sort -u | sed 's/PROFILE src\/rarray.h@//' | tr '\t' ' ' | sort -n | sed 's/^[0-9]*: //' > coverage_in_test.txt

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

profiletests: src/$(TESTNAME).cc src/rarray.h compiler.mk
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(MORECPPFLAGS) -DAR_TRACETEST src/$(TESTNAME).cc -o profiletests

profilenitests: src/$(TESTNAME).cc src/rarray.h compiler.mk
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on and skipping intermediate objects for indexing"
	$(CXX) -DAR_SKIPINTERMEDIATE $(CXXFLAGS) $(CPPFLAGS) -DAR_TRACETEST src/$(TESTNAME).cc -o profilenitests

summary: coverage_in_code.txt coverage_in_test.txt missing_from_test.txt
c	@echo "Summary:"
	@wc -l coverage_in_code.txt
	@wc -l coverage_in_test.txt
	@wc -l missing_from_test.txt

clean:
	rm -f $(TESTNAME).o $(TESTNAME)-cov.o $(TESTNAME)-ni-cov.o $(BENCHMARK4NAME).o $(BENCHMARK2NAME).o $(PASS).o $(PASS)f.o \
	profiletests profilenitests output_from_test.txt output_from_nitest.txt coverage_in_code.txt coverage_in_test.txt missing_from_test.txt  \
	doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x \
	doc1.cc doc2.cc doc3.cc doc4.cc doc5.cc doc6.cc doc7.cc doc8.cc doc9.cc doc10.cc doctestgenerator.sh \
	doc/rarraydoc.aux doc/rarraydoc.log doc/rarraydoc.out doc/rarraydoc.pdf


