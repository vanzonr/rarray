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
 CXXFLAGS=-g -O0
   LDLIBS=-lrut
MPILDLIBS=-lrutmpi
  LDFLAGS=-L$(LIB)
 CPPFLAGS=

  PREFIX?=/usr

all: ruttest rutmpitest rutexample1 $(LIB)/librut.so $(LIB)/librut.a $(LIB)/librutmpi.so $(LIB)/librutmpi.a 

ruttest: ruttest.o $(LIB)/librut.so
	$(CXX) $(LDFLAGS) -o $@ $< $(LDLIBS)

rutexample1: rutexample1.o $(LIB)/librut.so
	$(CXX) $(LDFLAGS) -o $@ $< $(LDLIBS)

ruttest.o: ruttest.cc rut.h

rutexaple1.o: rutexmple1.cc rut.h

rutmpitest: rutmpitest.o $(LIB)/librutmpi.so
	$(MPICXX) $(LDFLAGS) -o $@ $< $(MPILDLIBS)

rutmpitest.o: rutmpitest.cc rut.h
	$(MPICXX) -D_MPI $(CXXFLAGS) -c -o $@ $<

rut.o: rut.cc rut.h
	$(CXX) $(CXXFLAGS) -fpic -c -Os -o $@ $<

rutmpi.o: rut.cc rut.h
	$(MPICXX) -D_MPI $(CXXFLAGS) -fpic -c -Os -o $@ $<

$(LIB)/librut.a: rut.o  $(LIB)
	$(AR) $@ $<

$(LIB)/librutmpi.a: rutmpi.o  $(LIB)
	$(AR) $@ $<

$(LIB):
	mkdir -p $(LIB)

$(LIB)/librut.so.1.0: rut.o $(LIB)
	$(CXX) $(LDFLAGS) -shared -Wl,-soname,librut.so.1 -o $@ $<

$(LIB)/librutmpi.so.1.0: rutmpi.o $(LIB)
	$(CXX) $(LDFLAGS) -shared -Wl,-soname,librutmpi.so.1 -o $@ $<

$(LIB)/librut.so.1: $(LIB)/librut.so.1.0 $(LIB)
	test -s $@ || $(SL) librut.so.1.0 $@

$(LIB)/librut.so: $(LIB)/librut.so.1 $(LIB)
	test -s $@ || $(SL) librut.so.1 $@

$(LIB)/librutmpi.so.1: $(LIB)/librutmpi.so.1.0 $(LIB)
	test -s $@ || $(SL) librutmpi.so.1.0 $@

$(LIB)/librutmpi.so: $(LIB)/librutmpi.so.1 $(LIB)
	test -s $@ || $(SL) librutmpi.so.1 $@

test: ruttest
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB) ./ruttest

mpitest: rutmpitest
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(LIB) mpirun -n 4 ./rutmpitest

clean:
	$(RM) -f ruttest.o rutmpitest.o rut.o rutmpi.o ruttest rutmpitest

install: lib/librutmpi.so.1.0 lib/librutmpi.a lib/librut.so.1.0 lib/librut.a rut.h
	cp -f lib/librutmpi.so.1.0 lib/librutmpi.a lib/librut.so.1.0 lib/librut.a $(PREFIX)/lib
	test -s $(PREFIX)/lib/librut.so.1 || $(SL) $(PREFIX)/lib/librut.so.1.0 $(PREFIX)/lib/librut.so.1
	test -s $(PREFIX)/lib/librut.so || $(SL) $(PREFIX)/lib/librut.so.1 $(PREFIX)/lib/librut.so
	test -s $(PREFIX)/lib/librutmpi.so.1 || $(SL) $(PREFIX)/lib/librutmpi.so.1.0 $(PREFIX)/lib/librutmpi.so.1
	test -s $(PREFIX)/lib/librutpi.so || $(SL) $(PREFIX)/lib/librutmpi.so.1 $(PREFIX)/lib/librutmpi.so
	cp -f src/rut.h $(PREFIX)/include
