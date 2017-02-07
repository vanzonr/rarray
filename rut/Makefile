# Makefile
#
# Part of RUT (rarray unit test)
#
# Ramses van Zon, 2017
#
CXX=g++
CXXFLAGS=-g -O0
LDLIBS=
LDFLAGS=
CPPFLAGS=

all: ruttest

ruttest: ruttest.o rut.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ruttest.o: ruttest.cc rut.h

rut.o: rut.cc rut.h
	$(CXX) -c -Os -o $@ $<

test: ruttest
	./ruttest

clean:
	\rm -f ruttest.o rut.o
