# Makefile for rarray.h
#
# To build and run unit tests:            make test
# To check code coverage of tests:        make covertest
# To build and run benchmark test:        make benchmark
# To build the documentation:             make doc
# To test the code in the documentation:  make doctest
# To install rarray.h:                    make [PREFIX=directory]
#
PREFIX?=/usr/local
CXX?=g++
CCL?=${CXX}  

MORECPPFLAGS=-DAR_BOUNDSCHECK -Wall
CXXFLAGS?=-g -std=c++11 
LDFLAGS?=-g
LDLIBS?= 

CPPFLAGSOPT?=-DNDEBUG
CXXFLAGSOPT?=-O3 -fstrict-aliasing -ffast-math -march=native
LDFLAGSOPT?=
LDLIBSOPT?=

TESTNAME=rarraytestsuite
BENCHMARKNAME=rarray4dspeed
PASS=pass

all: test covertest benchmark doctest

.PHONY: clean test covertest benchmark install doctest doc

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

$(TESTNAME): $(TESTNAME).o
	$(CCL) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(TESTNAME).o: src/$(TESTNAME).cc src/rarray.h
	$(CXX) $(CPPFLAGS) $(MORECPPFLAGS) $(CXXFLAGS) -c -o $@ $<


benchmark: $(BENCHMARKNAME)
	@echo benchmark on a 4d array example
	@./$(BENCHMARKNAME) 1
	@(ulimit -s unlimited; ./$(BENCHMARKNAME) 2) 
	@./$(BENCHMARKNAME) 3
	@./$(BENCHMARKNAME) 4
	@./$(BENCHMARKNAME) 6
	@./$(BENCHMARKNAME) 7
	@./$(BENCHMARKNAME) 8
	@./$(BENCHMARKNAME) 9

$(BENCHMARKNAME): $(BENCHMARKNAME).o $(PASS).o
	$(CCL) $(LDFLAGSOPT) -o $@ $^ $(LDLIBS)

$(BENCHMARKNAME).o: src/$(BENCHMARKNAME).cc src/rarray.h
	$(CXX) $(CPPFLAGS) $(CPPFLAGSOPT) $(CXXFLAGSOPT) -c -o $@ $<

$(PASS).o: src/$(PASS).cc
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

profiletests: src/$(TESTNAME).cc src/rarray.h
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(MORECPPFLAGS) -DAR_TRACETEST src/$(TESTNAME).cc -o profiletests

profilenitests: src/$(TESTNAME).cc src/rarray.h
	@echo "Compile $(TESTNAME).cc and rarray.h with profile messages on and skipping intermediate objects for indexing"
	$(CXX) -DAR_SKIPINTERMEDIATE $(CXXFLAGS) $(CPPFLAGS) -DAR_TRACETEST src/$(TESTNAME).cc -o profilenitests

summary: coverage_in_code.txt coverage_in_test.txt missing_from_test.txt
	@echo "Summary:"
	@wc -l coverage_in_code.txt
	@wc -l coverage_in_test.txt
	@wc -l missing_from_test.txt

clean:
	rm -f $(TESTNAME).o $(TESTNAME)-cov.o $(TESTNAME)-ni-cov.o $(BENCHMARKNAME).o $(PASS).o profiletests profilenitests \
	output_from_test.txt output_from_nitest.txt coverage_in_code.txt coverage_in_test.txt missing_from_test.txt  \
	doc1.x doc2.x doc3.x doc4.x doc5.x doc6.x doc7.x doc8.x doc9.x doc10.x \
	doc1.cc doc2.cc doc3.cc doc4.cc doc5.cc doc6.cc doc7.cc doc8.cc doc9.cc doc10.cc doctestgenerator.sh


