# Makefile
#
# Part of RUT (rarray unit test)
#
# Ramses van Zon, 2017
#

   VPATH=src
     LIB=lib

      AR=ar rcs
      SL=ln -s
      RM=rm -f

  MPICXX?=mpicxx
     CXX?=g++
 CXXFLAGS=
   LDLIBS=-lrut
MPILDLIBS=-lrutmpi
  LDFLAGS=-L$(LIB)
 CPPFLAGS=

  PREFIX?=/usr

   LIBTAG=$(LIB)/.tag

all: ruttest rutmpitest rutexample1 $(LIB)/.librut.so.tag $(LIB)/librut.a $(LIB)/.librutmpi.so.tag $(LIB)/librutmpi.a 

ruttest: ruttest.o $(LIB)/.librut.so.tag
	$(CXX) $(LDFLAGS) -o $@ $< $(LDLIBS)

rutexample1: rutexample1.o $(LIB)/.librut.so.tag
	$(CXX) $(LDFLAGS) -o $@ $< $(LDLIBS)

ruttest.o: ruttest.cc rut.h

rutexaple1.o: rutexmple1.cc rut.h

rutmpitest: rutmpitest.o $(LIB)/.librutmpi.so.tag
	$(MPICXX) $(LDFLAGS) -o $@ $< $(MPILDLIBS)

rutmpitest.o: rutmpitest.cc rut.h
	$(MPICXX) -D_MPI $(CXXFLAGS) -c -o $@ $<

rut.o: rut.cc rut.h
	$(CXX) $(CXXFLAGS) -fpic -c -Os -o $@ $<

rutmpi.o: rut.cc rut.h
	$(MPICXX) -D_MPI $(CXXFLAGS) -fpic -c -Os -o $@ $<

$(LIB)/librut.a: rut.o $(LIBTAG)
	$(AR) $@ $<

$(LIB)/librutmpi.a: rutmpi.o $(LIBTAG)
	$(AR) $@ $<

$(LIBTAG):
	mkdir -p $(LIB) 
	touch $(LIB)/.tag

$(LIB)/.librut.so.tag: rut.o $(LIBTAG)
	$(CXX) $(LDFLAGS) -shared -Wl,-soname,librut.so.1 -o $(LIB)/librut.so.1.0 rut.o
	test -h $(LIB)/librut.so.1 || $(SL) $(LIB)/librut.so.1.0 $(LIB)/librut.so.1
	test -h $(LIB)/librut.so || $(SL) $(LIB)/librut.so.1 $(LIB)/librut.so
	touch $(LIB)/.librut.so.tag

$(LIB)/.librutmpi.so.tag: rutmpi.o $(LIBTAG)
	$(CXX) $(LDFLAGS) -shared -Wl,-soname,librutmpi.so.1 -o $(LIB)/librutmpi.so.1.0 rutmpi.o
	test -h $(LIB)/librutmpi.so.1 || $(SL) $(LIB)/librutmpi.so.1.0 $(LIB)/librutmpi.so.1
	test -h $(LIB)/librutmpi.so || $(SL) $(LIB)/librutmpi.so.1 $(LIB)/librutmpi.so
	touch $(LIB)/.librutmpi.so.tag

test: ruttest
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB) ./ruttest

mpitest: rutmpitest
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB) mpirun -n 4 ./rutmpitest

.PHONY: clean distclean install

clean:
	$(RM) -f ruttest.o rutmpitest.o rut.o rutmpi.o ruttest rutmpitest $(LIB)/.librut.so.tag $(LIB)/.librutmpi.so.tag  rutexample1.o $(LIBTAG)
	rmdir $(LIB)

distclean: clean
	$(RM) -f rutexample1 ruttest rutmpitest $(LIB)/librut.so $(LIB)/librut.so.1 $(LIB)/librut.so.1.0 $(LIB)/librutmpi.so $(LIB)/librutmpi.so.1 $(LIB)/librutmpi.so.1.0 $(LIB)/librutmpi.a  $(LIB)/librut.a

install: lib/librutmpi.so.1.0 lib/librutmpi.a lib/librut.so.1.0 lib/librut.a rut.h
	cp -f lib/librutmpi.so.1.0 lib/librutmpi.a lib/librut.so.1.0 lib/librut.a $(PREFIX)/lib
	test -h $(PREFIX)/lib/librut.so.1 || $(SL) $(PREFIX)/lib/librut.so.1.0 $(PREFIX)/lib/librut.so.1
	test -h $(PREFIX)/lib/librut.so || $(SL) $(PREFIX)/lib/librut.so.1 $(PREFIX)/lib/librut.so
	test -h $(PREFIX)/lib/librutmpi.so.1 || $(SL) $(PREFIX)/lib/librutmpi.so.1.0 $(PREFIX)/lib/librutmpi.so.1
	test -h $(PREFIX)/lib/librutpi.so || $(SL) $(PREFIX)/lib/librutmpi.so.1 $(PREFIX)/lib/librutmpi.so
	cp -f src/rut.h $(PREFIX)/include
