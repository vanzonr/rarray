Benchmarks
==========

The first and primary purpose of rarray is to provide runtime
multidimensional arrays for C++ that are at least as fast in accessing
elements as multi-dimensional arrays that you would create yourself
with nested new statements.  The benchmarks for rarray are therefore
focussed on element access.

rarray was designed to be interoperable with standardized linear
algebra routines in libraries such as cblas and lapacke (try and get
an implementation optimized for your routine, eg MKL for intel
processors). This is accomplished by insisting that the data for each
array is stored in contiguous memory locations (in row-major format,
to be precise).  Because of this operability, there are no linear
algebra benchmarks with rarray. If you want one, get a standard c
version of such a benchmark and plug in a rarray.

There are currently 2 benchmarks to test the speed of element access,
one involving 2d arrays and one involving 4d arrays (a 3d array is
planned). 

  * benchmark2Daccess:

      - construct three 10,000x10,000 matrices of floats called A, B,
        and C.
      - for r=0,1,2:
        - fill A and B:
           A_{ij} = i+r
           B_{ij} = j+r/2
        - add A and B element-wise, storing the result in C
        - sum up all the elements of C
      - accumulate the results of the sum of C's elements for r=0,1,2.
      - compare with exact result

  * benchmark4Daccess

      - construct three 4d arrays of 100x100x100x100 floats 
        called A, B, and C.
      - for r=0,1,2:
        - fill A and B:
           A_{ijkl} = l+i+r
           B_{ijkl} = k+j+r/2
        - add A and B element-wise, storing the result in C
        - sum up all the elements of C
      - accumulate the results of the sum of C's elements for r=0,1,2.
      - compare with exact result


Comparison
==========

For comparison, versions of these benchmarks are implementated for the
following libraries or techniques:

1. rarray
2. automatic arrays
3. dynamic arrays
4. boost's multi_array
5. armadillo
6. std::vector
7. blitz (version 1)
8. blitz (version 2)
9. eigen3

There is a Fortran implementation of the same algorithm as well.

Different libraries use different (default) ordering of elements
(i.e. row-major vs column-major). In order to be fair, the benchmark
implementations have been adapted to lead to non-strided memory
access.

The benchmarks can be compiled with the Makefile, after running
'configure'.


