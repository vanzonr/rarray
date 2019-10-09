# 
# Makefile - make file for rarray unit test library
#
# Copyright (c) 2017-2019  Ramses van Zon
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
# To install:
#   make installserialonly [PREFIX=directory]
# or, for use in mpi applications:
#   make install [PREFIX=directory]
#

VPATH=src

AR=ar rcs
SL=ln -s
RM=rm -f

MPICXX?=mpicxx
CXX?=g++
CXXFLAGS=-g -O0
LDLIBS=-lrut
MPILDLIBS=-lrutmpi

PREFIX?=/usr

all: ruttest rutmpitest rutexample1 libraries

libraries: seriallibrary mpilibrary
mpilibrary: .librutmpi.so.tag librut.a
seriallibrary: .librut.so.tag librut.a

library: .librut.so.tag librut.a .librutmpi.so.tag librutmpi.a

ruttest: ruttest.o .librut.so.tag
	${CXX} ${LDFLAGS} -L. -o $@ $< ${LDLIBS}

rutexample1: rutexample1.o .librut.so.tag
	${CXX} ${LDFLAGS} -L. -o $@ $< ${LDLIBS}

ruttest.o: ruttest.cc rut.h

rutexaple1.o: rutexmple1.cc rut.h

rutmpitest: rutmpitest.o .librutmpi.so.tag
	${MPICXX} ${LDFLAGS} -L. -o $@ $< ${MPILDLIBS}

rutmpitest.o: rutmpitest.cc rut.h
	${MPICXX} -D_MPI ${CXXFLAGS} -c -o $@ $<

rut.o: rut.cc rut.h
	${CXX} ${CXXFLAGS} -fpic -c -Os -o $@ $<

rutmpi.o: rut.cc rut.h
	${MPICXX} -D_MPI ${CXXFLAGS} -fpic -c -Os -o $@ $<

librut.a: rut.o
	${AR} $@ $<

librutmpi.a: rutmpi.o
	${AR} $@ $<

.librut.so.tag: rut.o
	${CXX} ${LDFLAGS} -shared -Wl,-soname,librut.so.1 -o librut.so.1.0 rut.o
	test -h librut.so.1 || ${SL} librut.so.1.0 librut.so.1
	test -h librut.so || ${SL} librut.so.1 librut.so
	touch .librut.so.tag

.librutmpi.so.tag: rutmpi.o
	${CXX} ${LDFLAGS} -shared -Wl,-soname,librutmpi.so.1 -o librutmpi.so.1.0 rutmpi.o
	test -h librutmpi.so.1 || ${SL} librutmpi.so.1.0 librutmpi.so.1
	test -h librutmpi.so || ${SL} librutmpi.so.1 librutmpi.so
	touch .librutmpi.so.tag

test: ruttest
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:. ./ruttest

mpitest: rutmpitest
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:. mpirun -n 4 ./rutmpitest

clean:
	${RM} ruttest.o rutmpitest.o rut.o rutmpi.o ruttest rutmpitest .librut.so.tag .librutmpi.so.tag rutexample1.o

distclean: clean
	${RM} rutexample1 ruttest rutmpitest librut.so librut.so.1 librut.so.1.0 librutmpi.so librutmpi.so.1 librutmpi.so.1.0 librutmpi.a librut.a

installserialonly: .librut.so.tag librut.a src/rut.h
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/include
	cp -f librut.so.1.0 librut.a ${PREFIX}/lib
	test -h ${PREFIX}/lib/librut.so.1 || ${SL} ${PREFIX}/lib/librut.so.1.0 ${PREFIX}/lib/librut.so.1
	test -h ${PREFIX}/lib/librut.so || ${SL} ${PREFIX}/lib/librut.so.1 ${PREFIX}/lib/librut.so
	cp -f src/rut.h ${PREFIX}/include

install: .librutmpi.so.tag librutmpi.a installserialonly
	cp -f librutmpi.so.1.0 librutmpi.a ${PREFIX}/lib
	test -h ${PREFIX}/lib/librutmpi.so.1 || ${SL} ${PREFIX}/lib/librutmpi.so.1.0 ${PREFIX}/lib/librutmpi.so.1
	test -h ${PREFIX}/lib/librutpi.so || ${SL} ${PREFIX}/lib/librutmpi.so.1 ${PREFIX}/lib/librutmpi.so

.PHONY: all library clean distclean install
