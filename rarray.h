//
// rarray.h - Runtime arrays: template classes for pointer-based,
//            runtime dimensionalized, multi-dimensional
//            arrays.  Documentation in rarraydoc.pdf
//
// Copyright (c) 2013-2015  Ramses van Zon
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef RARRAY_H
#define RARRAY_H

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

// When running rarraytestsuite.cc compiled with -DRA_TRACETEST, the
// following macro produced output to be used to determine which
// functions are exercised.
#ifdef RA_TRACETEST
#define RA_IFTRACESAY(a) std::cerr << "IFTRACE " << __FILE__ << '@' << __LINE__<< ":\t" << a << std::endl;
#else
#define RA_IFTRACESAY(a) 
#endif

// Compiling with -DRA_BOUNDSCHECK switches on the checkOrSay macro to
// check its first argument and throw an exception if it is not true.
// checkOrSay is intended to be used for bound checks.
#ifdef RA_BOUNDSCHECK
#define RA_CHECKORSAY(a, b) if (not(a)) throw std::out_of_range(std::string(b) + " in function " + std::string(__PRETTY_FUNCTION__))
// BOUNDCHECK is incompatible with SKIPINTERMEDIATE (see below)
#ifdef RA_SKIPINTERMEDIATE
#undef RA_SKIPINTERMEDIATE
#endif
#else
#define RA_CHECKORSAY(a, b) 
#endif

#if __cplusplus <= 199711L
#define RA_NULLPTR 0 
// In C++03, there is no move semantics. Rarrays need move semantics to be return-able from functions. The RA_MOVE macros helps here
#define RA_MOVE(x) x.move()
// usage example: rarray<float,2> matrix(int n, int m) { rarray<int,2> a(n,m); return RA_MOVE(a); }
#else
#define RA_NULLPTR nullptr
// I will implement a move semantics for c++11 soon, so that
// move=semantics automatically apply to rarrays returned from
// functions, but haven't gotten to that yet so 'return RA_MOVE(x)'
// will still be translated as 'return x.move()' for now.
#define RA_MOVE(x) x.move()
#endif

// For g++ and icpc, RA_INLINE forces inlining, even without optimization.
// In all other cases, RA_INLINE=inline, and inlining may not occur.
// Note for xlC: 
//    In version 10, you need "-O4" to get full inlining.
//    In version 11, "-O2 -qinline=level=6" suffices.
//
#if not defined(RA_INLINE)
# if defined(__INTEL_COMPILER)
#   define RA_INLINE  __forceinline
# elif defined(__GNUC__)
#   define RA_INLINE inline __attribute__((always_inline)) 
# else
#   define RA_INLINE inline
# endif
#endif

// routines using INLINEF will be forced to inline
// routines using INLINE_ will not: these were deemed to expensive to inline from a compilation point of view
#define RA_INLINEF RA_INLINE
#define RA_INLINE_ inline

// Forward definitions of ra::rarray<T,R> and ra::subrarray<T,R>
namespace ra { template<typename T,int R> class rarray;    }
namespace ra { template<typename T,int R> class subrarray; }
namespace ra { template<typename T>       class CommaOp;   }

// Define internal types needed by class rarray, in a separate namespace

namespace ra
{

// Need a generic way to express a pointer-to-a-pointer-to-a-pointer etc.
// 
//    PointerArray<T,1>::type   =    T*
//    PointerArray<T,2>::type   =    T*const*
//    PointerArray<T,3>::type   =    T*const*const*
//    .. 
//    PointerArray<T,1>::noconst_type   =    T*
//    PointerArray<T,2>::noconst_type   =    T**
//    PointerArray<T,3>::noconst_type   =    T*** 
//    ...
template<typename T,int R> 
struct PointerArray {
    typedef typename PointerArray<T,R-1>::type const*    type;         // const shape, recursive
    typedef typename PointerArray<T,R-1>::noconst_type*  noconst_type; // non-const variant
};
template<typename T> 
struct PointerArray<T,1> { // We end the recursion by specifically defining the R=1 case
    typedef T* type;         // no const as this would express that the elements are constant
    typedef T* noconst_type; 
};

// Need a way to remove the const from a type:
// Unconst<T>::type = T , unless T=const U, in which case Unconst<const T>::type = T .
template<typename T> 
struct Unconst { 
    typedef T type; // non-stripping, intended for typed without const
};
template<typename T>  
struct Unconst<const T> {  // Override non-const-stripped type with a stripped one which applies for types with const only.
    typedef T type; // no const!
};

// Iterator<T> acts like T* but allows (optional) bounds checking switched on by RA_BOUNDSCHECK
template<typename T>
class Iterator {
  public:
    RA_INLINEF T&        operator*   () const;                                    // dereference
    RA_INLINEF Iterator& operator++  ();                                          // pre-incrememt
    RA_INLINE_ Iterator  operator++ (int);                                        // post-increment
    RA_INLINE_ bool      operator== (const Iterator<T>& other) const;             // comparison: equal to
    RA_INLINEF bool      operator!= (const Iterator<T>& other) const;             // comparison: not equal to
    RA_INLINE_ bool      operator<  (const Iterator<T>& other) const;             // comparison: less than
    RA_INLINE_ bool      operator>  (const Iterator<T>& other) const;             // comparison: greater than
    RA_INLINE_ bool      operator<= (const Iterator<T>& other) const;             // comparison: less than or equal to
    RA_INLINE_ bool      operator>= (const Iterator<T>& other) const;             // comparison: greater than or equal to
  private:
    T*  pointer_;
    T*  pointer_min_;
    T*  pointer_max_plus_one_;
    RA_INLINE_ Iterator(T* ptr, int size);
    template<typename,int> friend class rarray;
    template<typename,int> friend class subrarray;
};


//------------------------------------------------//
//                                                //
//               I N T E R F A C E                //
//                                                //
//------------------------------------------------//

// A boolean that cannot be mistaken for an int.
class RAbool {
  private:  
    bool value;
  public:
    explicit RAbool(bool b): value(b) {}
    RAbool(const RAbool& other): value(other.value) {}
    RAbool& operator=(const RAbool& other) {
        value = other.value;
        return *this;
    }
    bool operator==(const RAbool& other) const {
      return other.value == value;
    }
    bool operator!=(const RAbool& other) const {
      return other.value != value;
    }
    bool as_bool() const {
      return value;
    }
};
#define RA_TRUE  ra::RAbool(true)
#define RA_FALSE ra::RAbool(false)
#define RA_FORCE ra::RAbool(true)

template<typename T,int R> 
class rarray {

  public: 
    typedef int                                       difference_type;  // difference type for indices
    typedef int                                       size_type;        // type of indices
    typedef Iterator<T>                               iterator;         // iterator type
    typedef Iterator<const T>                         const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,R>::type          parray_t;         // shorthand for T*const*const*...
    typedef typename PointerArray<T,R>::noconst_type  noconst_parray_t; // shorthand for T***...

    RA_INLINE_ rarray();                                                          // constructor leaving rarray undefined 
    RA_INLINE_ rarray(int n0, int n1);                                            // constructor creating its own buffer for R=2
    RA_INLINE_ rarray(int n0, int n1, int n2);                                                                            // R=3
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3);                                                                    // R=4
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4);                                                            // R=5
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4, int n5);                                                    // R=6
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6);                                            // R=7
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7);                                    // R=8
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);                            // R=9 
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);                    // R=10
    RA_INLINE_ rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);           // R=11
    RA_INLINE_ rarray(const int* extent);                                                                                 // R>11
    RA_INLINE_ rarray(T* buffer, int n0, int n1);                                 // constructor from an existing buffer for R=2
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2);                                                                 // R=3
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3);                                                         // R=4
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4);                                                 // R=5
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5);                                         // R=6
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6);                                 // R=7 
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7);                         // R=8
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);                 // R=9
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);         // R=10
    RA_INLINE_ rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);// R=11
    RA_INLINE_ rarray(T* buffer, const int* extent);                                                                      // R>11
    RA_INLINEF rarray(const rarray<T,R> &a);                                      // copy constructor
    RA_INLINEF rarray(const subrarray<T,R> &a);                                   // copy constructor
    RA_INLINE_ rarray<T,R>& operator=(const rarray<T,R> &a);                      // array assignment operator
    RA_INLINE_ rarray<T,R>& operator=(const subrarray<T,R> &a);                   // array assignment operator
    RA_INLINE_ CommaOp<T> operator=(const T& e);                                  // Comma separated element assignment
    RA_INLINEF ~rarray();                                                         // destructor
    RA_INLINEF void clear();                                                      // clean up routine, make undefined
    RA_INLINE_ void reshape(int n0, int n1, RAbool force=RA_FALSE);               // reshape keeping the underlying data for R=2
    RA_INLINE_ void reshape(int n0, int n1, int n2, RAbool force=RA_FALSE);                                               // R=3
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, RAbool force=RA_FALSE);                                       // R=4
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, RAbool force=RA_FALSE);                               // R=5
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, int n5, RAbool force=RA_FALSE);                       // R=6
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, RAbool force=RA_FALSE);               // R=7
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, RAbool force=RA_FALSE);       // R=8
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, RAbool force=RA_FALSE);//R=9
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, RAbool force=RA_FALSE);//R=10
    RA_INLINE_ void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, RAbool force=RA_FALSE);// R=11
    RA_INLINE_ void reshape(const int* extent, RAbool force=RA_FALSE);                                                    // R>11

    RA_INLINE_ bool                is_clear()           const;                    // check if undefined
    RA_INLINE_ rarray<T,R>         copy()               const;                    // return a copy
    RA_INLINE_ int                 extent(int i)        const;                    // retrieve array size in dimension i
    RA_INLINE_ const int*          shape()              const;                    // retrieve array sizes in all dimensions
    RA_INLINE_ int                 size()               const;                    // retrieve the total number of elements  
    RA_INLINE_ T*                  data();                                        // return a T* to the internal data
    RA_INLINE_ const T*            data()               const;                    // return a T* to the internal data
    RA_INLINE_ parray_t            ptr_array()          const;                    // return a T*const*.. acting similarly to this rarray when using []:
    RA_INLINE_ noconst_parray_t    noconst_ptr_array()  const;                    // return a T**.. acting similarly to this rarray when using []:    
    RA_INLINE_ rarray<const T,R>&  const_ref()          const;                    // create a reference to this that treats elements as constant:
    RA_INLINE_ void                fill(const T& value);                          // fill with uniform value

    RA_INLINE_ iterator            begin();                                       // start of the content
    RA_INLINE_ const_iterator      begin()              const;                    // start of the content, when *this is constant
    RA_INLINE_ const_iterator      cbegin()             const;                    // start of the content, when *this can be constant and you need to be explicit
    RA_INLINE_ iterator            end();                                         // end of the content
    RA_INLINE_ const_iterator      end()                const;                    // end of the content, when *this is constant
    RA_INLINE_ const_iterator      cend()               const;                    // end of the content, when *this is constant and you need to be explicit about that
    RA_INLINE_ int                 index(const T& a, int i) const;                // if a an element in the array, get index i of that element
    RA_INLINE_ int                 index(const iterator& iter, int i);            // if i points at an element in the array, get index i of that element
    RA_INLINE_ int                 index(const const_iterator& iter, int i) const;// if i points at an element in the array, get index i of that element
    RA_INLINE_ int*                index(const T& a, int* index) const;           // if a an element in the array, get the indices of that element
    RA_INLINE_ int*                index(const iterator& i, int* index);          // if i points at an element in the array, get the indices of that element
    RA_INLINE_ int*                index(const const_iterator& i, int* ind) const;// if i points at an element in the array, get the indices of that element

    RA_INLINEF rarray<T,R>&        move();                                        // convert this rarray to a temp value

    // access elements 
   #ifndef RA_SKIPINTERMEDIATE
    // through intermediate object:
    RA_INLINEF subrarray<T,R-1> operator[](int i);
    RA_INLINEF subrarray<const T,R-1> operator[](int i) const;
   #else
    // or if RA_SKIPINTERMEDIATE is set, by implicit case to a T**..:
    RA_INLINEF operator typename PointerArray<T,R>::type (); 
    RA_INLINEF operator typename PointerArray<const T,R>::type () const; 
   #endif


  private:
    parray_t     parray_;                                              // start of the array of pointers
    int*         extent_;                                              // array of number of elements in each dimension
    bool         ismine_;                                              // does the container own the data buffer?
    mutable bool cleans_;                                              // alternative to ref counting: am I the one that cleans?
    bool         tmpval_;                                              // to mimic move semantics: am I a temporary value?
                                                                       // Temporary values pass their 'cleans_' value on when
                                                                       // copied, once. None-temporary values (the default) do not.
                                                                       // When returning a rarray 'a' from a function, use
                                                                       // 'return RA_MOVE(a);'

    RA_INLINEF T*   get_buffer() const;                                           // get start of current contiguous buffer
    RA_INLINE_ void init_shallow(parray_t parray, bool& cleans, bool tmpval); // setup new rarray object
    RA_INLINE_ void init_shallow(parray_t parray);             // setup new rarray object
    RA_INLINE_ void init_parray(T* buffer, const int* extent);                    // setup new rarray object
    RA_INLINE_ void init_data(const int* extent, int extenttot);                  // setup new rarray object
    static RA_INLINE_ parray_t new_except_base(T* buffer, const int* extent);     // allocate the chain of pointers, except the base
    static RA_INLINE_ T*       base(parray_t parray);                             // find base of a chain of pointers:

    friend class subrarray<T,R>;

}; // end definition rarray<T,R>

// Define rarray<T,1> to stop the recursion in operator[]:

template<typename T> 
class rarray<T,1> {

  public:
    enum { R = 1 };

    typedef int                                       difference_type;  // difference type for indices
    typedef int                                       size_type;        // type of indices
    typedef Iterator<T>                               iterator;         // iterator type
    typedef Iterator<const T>                         const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,1>::type          parray_t;         // conforming shorthand for T*
    typedef typename PointerArray<T,1>::noconst_type  noconst_parray_t; // conforming shorthand for T*

    RA_INLINEF rarray();                                                          // constructor leaving object undefined
    RA_INLINE_ rarray(int n0);                                                    // constructor creating own buffer
    RA_INLINE_ rarray(const int* extent);                                         // constructor creating own buffer from pointer, for conformity
    RA_INLINE_ rarray(T* buffer, int n0);                                         // constructor from an existing buffer:
    RA_INLINE_ rarray(T* buffer, const int* extent);                              // constructor from an existing buffer for conformity    
    RA_INLINEF rarray(const rarray<T,1> &a);                                      // copy constructor
    RA_INLINEF rarray(const subrarray<T,1> &a);                                   // copy constructor    
    RA_INLINE_ rarray<T,1>& operator=(const rarray<T,1> &a);                      // assignment operator
    RA_INLINE_ rarray<T,1>& operator=(const subrarray<T,1> &a);                   // assignment operator
    RA_INLINE_ CommaOp<T> operator=( const T& e );                                // Comma separated element assignment
    RA_INLINEF ~rarray();                                                         // destructor
    RA_INLINEF void clear();                                                      // clean up routine, make undefined again
    RA_INLINE_ void reshape(int n0, RAbool force=RA_FALSE);                       // to change shape (only shrinking is defined)
    RA_INLINE_ void reshape(const int* extent, RAbool force=RA_FALSE);            // for conformity
    RA_INLINE_ bool                is_clear()           const;                    // check if undefined
    RA_INLINE_ rarray<T,1>         copy()               const;                    // return a copy
    RA_INLINE_ int                 extent(int i)        const;                    // retrieve array size in dimension i
    RA_INLINE_ const int*          shape()              const;                    // retrieve array sizes in all dimensions
    RA_INLINE_ int                 size()               const;                    // retrieve the total number of elements
    RA_INLINE_ T*                  data();                                        // return T* to the internal data
    RA_INLINE_ const T*            data()               const;                    // return T* to the internal data
    RA_INLINE_ parray_t            ptr_array()          const;                    // return T*const*... acting similarly to this rarray when using []
    RA_INLINE_ noconst_parray_t    noconst_ptr_array()  const;                    // return  T**... acting similarly to this rarray when using []
    RA_INLINE_ rarray<const T,1>&  const_ref()          const;                    // create reference to this that treats elements as constant
    RA_INLINE_ void                fill(const T& value);                          // fill with uniform value
    RA_INLINE_ iterator            begin();                                       // start of the content
    RA_INLINE_ const_iterator      begin()              const;                    // start of the content, when *this is constant
    RA_INLINE_ const_iterator      cbegin()             const;                    // start of the content, when *this is constant and you need to be explicit
    RA_INLINE_ iterator            end();                                         // end of the content
    RA_INLINE_ const_iterator      end()                const;                    // end of the content, when *this is constant
    RA_INLINE_ const_iterator      cend()               const;                    // end of the content, when *this is constant, and you need to be explicit about it
    RA_INLINE_ int                 index(const T& a, int i) const;                // if a an element in the array, get index i of that element
    RA_INLINE_ int                 index(const iterator& iter, int i);            // if i points at an element in the array, get index i of that element
    RA_INLINE_ int                 index(const const_iterator& iter, int i) const;// if i points at an element in the array, get index i of that element
    RA_INLINE_ int*                index(const T& a, int* index) const;           // if a an element in the array, get the indices of that element
    RA_INLINE_ int*                index(const iterator& i, int* index);          // if i points at an element in the array, get the indices of that element
    RA_INLINE_ int*                index(const const_iterator& i, int* ind) const;// if i points at an element in the array, get the indices of that element

    // accesselements through intermediate object:
   #ifndef RA_SKIPINTERMEDIATE
    // through a T& pointer a the element:
    RA_INLINEF T& operator[](int i);
    RA_INLINEF const T& operator[](int i) const;
   #else
    // through implicit conversion to a pointer:
    RA_INLINEF operator typename PointerArray<T,1>::type ();
    RA_INLINEF operator typename PointerArray<const T,1>::type () const;
   #endif

    RA_INLINEF rarray<T,1>& move();                    // convert this rarray to a temp value

  private:
    parray_t     parray_;                                              // start of the array of pointers
    int*         extent_;                                              // array of number of elements in each dimension
    bool         ismine_;                                              // does the container own the data buffer?
    mutable bool cleans_;                                              // alternative to ref counting: I am the one that cleans?
    bool tmpval_;                                              // to mimic move semantics: am I a temporary value?
                                                                       // Temporary values pass their 'cleans_' value on when
                                                                       // copied, once. None-temporary values (the default) do not.
                                                                       // When returning a rarray 'a' from a function, use
                                                                       // 'return RA_MOVE(a);'

    RA_INLINEF T*   get_buffer() const;                                           // get start of current contiguous buffer  
    RA_INLINE_ void init_shallow(parray_t parray, bool& cleans, bool tmpval); // setup new rarray object
    RA_INLINE_ void init_shallow(parray_t parray);             // setup new rarray object
    RA_INLINE_ void init_parray(T* buffer, const int* extent);                    // setup new rarray object
    RA_INLINE_ void init_data(const int* extent, int extenttot);                  // setup new rarray object
    static RA_INLINE_ parray_t new_except_base(T* buffer, const int* extent);     // allocate the chain of pointers, except the base
    static RA_INLINE_ T* base(parray_t parray) ;                                  // find base of a chain of pointers

    friend class subrarray<T,1>;

}; // end definition rarray<T,1>

} // end namespace ra

// Input/output streaming operators in global namespace
template<typename T,int R> RA_INLINE_ std::istream& operator>>(std::istream &i, ra::rarray<T,R>& r);
template<typename T,int R> RA_INLINE_ std::ostream& operator<<(std::ostream &o, const ra::rarray<T,R>& r);
template<typename T,int R> RA_INLINE_ std::ostream& operator<<(std::ostream &o, const ra::subrarray<T,R>& r);

namespace ra { // continuing namespace ra

// Definition class subrarray <T,R>

// subrarray class: interface like rarray without assignment. Really
// only a reference to one, used in intermediate expressions produced
// by square brackets.

template<typename T,int R> 
class subrarray {

  public:
    typedef int                                       difference_type;  // difference type for indices
    typedef int                                       size_type;        // type of indices
    typedef Iterator<T>                               iterator;         // iterator type
    typedef Iterator<const T>                         const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,R>::type          parray_t;         // shorthand for T*const*const*...
    typedef typename PointerArray<T,R>::noconst_type  noconst_parray_t; // shorthand for T***...
    
    RA_INLINE_ int                  extent(int i)        const;                   // retrieve array size in dimension i
    RA_INLINE_ const int*           shape()              const;                   // retrieve array sizes in all dimensions    
    RA_INLINE_ int                  size()               const;                   // retrieve the total number of elements
    RA_INLINE_ T*                   data()               const;                   // return T* to the internal pointer to the data
    RA_INLINE_ parray_t             ptr_array()          const;                   // return T*const*.. acting similarly when using []
    RA_INLINE_ noconst_parray_t     noconst_ptr_array()  const;                   // return T**.. acting similarly to this rarray when using []
    RA_INLINE_ subrarray<const T,R>& const_ref()         const;                   // create a reference to this treating elements as constant
    RA_INLINE_ void                 fill(const T& value);                         // fill with uniform value
    RA_INLINE_ iterator             begin()              const;                   // start of the *content*
    RA_INLINE_ iterator             end()                const;                   // end of the *content*
    RA_INLINE_ const_iterator       cbegin()             const;                   // start of the *content* (const version)
    RA_INLINE_ const_iterator       cend()               const;                   // end of the *content* (const version)
    RA_INLINE_ int                  index(const T& a, int i) const;               // if a an element in the array, get index i of that element
    RA_INLINE_ int                  index(const iterator& iter, int i);           // if i points at an element in the array, get index i of that element
    RA_INLINE_ int                  index(const const_iterator& iter, int i)const;// if i points at an element in the array, get index i of that element
    RA_INLINE_ int*                 index(const T& a, int* index) const;          // if a an element in the array, get the indices of that element
    RA_INLINE_ int*                 index(const iterator& i, int* index);         // if i points at an element in the array, get the indices of that element
    RA_INLINE_ int*                 index(const const_iterator& i, int* ind)const;// if i points at an element in the array, get the indices of that element
    RA_INLINEF subrarray<T,R-1>     operator[](int i)    const;                   // element access
    RA_INLINE_ CommaOp<T>           operator=(const T& e);                        // Comma separated element assignment

  private:
    parray_t   const  parray_;                                     // start of the pointer array
    const int* const  extent_;                                     // number of elements in each dimension

    friend class rarray<T,R>;                                      // constructor can only
    friend class rarray<T,R+1>;                                    // be called by these
    friend class subrarray<T,R+1>;                                 // classes.
    friend class rarray<typename Unconst<T>::type,R+1>;

    RA_INLINEF      subrarray(parray_t parray, const int* extent); // constructor, called by
    RA_INLINEF T*   get_buffer() const;                            // get start of current contiguous buffer

}; // end definition subrarray<T,R>

// Define subrarray<T,1> to stop the recursion in operator[]:

template<typename T> class subrarray<T,1> {

  public:
    enum { R = 1 };

    typedef int                                      difference_type;  // difference type for indices
    typedef int                                      size_type;        // type of indices
    typedef Iterator<T>                              iterator;         // iterator type
    typedef Iterator<const T>                        const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,1>::type         parray_t;         // conforming shorthand for T*
    typedef typename PointerArray<T,1>::noconst_type noconst_parray_t; // conforming shorthand for T*

    RA_INLINE_ int                  extent(int i)        const;                   // retrieve array size in dimension i
    RA_INLINE_ const int*           shape()              const;                   // retrieve array sizes in all dimensions
    RA_INLINE_ int                  size()               const;                   // retrieve the total number of elements
    RA_INLINE_ T*                   data()               const;                   // return T* to the internal pointer to the data
    RA_INLINE_ parray_t             ptr_array()          const;                   // return T*const*.. acting similarly to this rarray when using []
    RA_INLINE_ noconst_parray_t     noconst_ptr_array()  const;                   // return T**.. acting similarly to this rarray when using []
    RA_INLINE_ subrarray<const T,1>&const_ref()          const;                   // create a reference to this that treats elements as constant
    RA_INLINE_ void                 fill(const T& value);                         // fill with uniform value
    RA_INLINE_ iterator             begin()              const;                   // start of the *content*
    RA_INLINE_ iterator             end()                const;                   // end of the *content*
    RA_INLINE_ const_iterator       cbegin()             const;                   // start of the *content* (const version)
    RA_INLINE_ const_iterator       cend()               const;                   // end of the *content* (const version)
    RA_INLINE_ int                  index(const T& a, int i) const;               // if a an element in the array, get index i of that element
    RA_INLINE_ int                  index(const iterator& iter, int i);           // if i points at an element in the array, get index i of that element
    RA_INLINE_ int                  index(const const_iterator& iter, int i)const;// if i points at an element in the array, get index i of that element
    RA_INLINE_ int*                 index(const T& a, int* index) const;          // if a an element in the array, get the indices of that element
    RA_INLINE_ int*                 index(const iterator& i, int* index);         // if i points at an element in the array, get the indices of that element
    RA_INLINE_ int*                 index(const const_iterator& i, int* ind)const;// if i points at an element in the array, get the indices of that element
    RA_INLINEF T&                   operator[](int i)    const;                   // element access
    RA_INLINE_ CommaOp<T>           operator=(const T& e);                        // Comma separated element assignment

  private:
    parray_t   const   parray_;                                        // start of the pointer array
    const int* const   extent_;                                        // number of elements in each dimension

    RA_INLINEF subrarray(parray_t parray, const int* extent);          // constructor    
    friend class rarray<T,1>; 
    friend class rarray<T,2>;
    friend class subrarray<T,2>;
    friend class rarray<typename Unconst<T>::type,2>;

    RA_INLINEF T*   get_buffer() const;                                // get start of current contiguous buffer

};  // end of class template definition of subrarray<T,1>.

template<typename T,int R> RA_INLINE_ std::ostream& text_output(std::ostream &o, const rarray<T,R>& r);
template<typename T>       RA_INLINE_ std::ostream& text_output(std::ostream &o, const rarray<T,1>& r);
template<typename T,int R> RA_INLINE_ std::ostream& text_output(std::ostream &o, const subrarray<T,R>& r);
template<typename T>       RA_INLINE_ std::ostream& text_output(std::ostream &o, const subrarray<T,1>& r);

// Class to facilitate assignment from a comma separated list
template<typename T>
class CommaOp {
  public:
    RA_INLINEF CommaOp& operator,(const T& e);                         // puts the next number into the array.
  private:
    RA_INLINEF CommaOp(T* ptr, T* last); 
    T *ptr_;                                                           // points to next element to be filled
    T * const last_;                                                   // points to last element
    template<typename,int> friend class rarray;
    template<typename,int> friend class subrarray;
};

// We need to be able to get a reference in a pointer-to-pointer structure given indices.
//
// Deref<T,1>(T*   p, int* i) where i->{n1}        =  p[n1]
// Deref<T,2>(T**  p, int* i) where i->{n1,n2}     =  p[n1][n2]
// Deref<T,3>(T*** p, int* i) where i->{n1,n2,n3}  =  p[n1][n2][n3]
//...
template<typename T, int R>
struct Deref {
    static RA_INLINEF T& access(typename PointerArray<T,R>::type p, const int* indices);
};
template<typename T>
struct Deref<T,1>  // R=1 is special
{
    static RA_INLINEF T& access(typename PointerArray<T,1>::type p, const int* indices);
};

// Convert a string to a value, needed for operator>> .
template<typename T>
struct StringToValue {
    static RA_INLINE_ void get(const std::string& input, T& output);
};
template<>
struct StringToValue<std::string> {
    static RA_INLINE_ void get(const std::string& input, std::string& output);
};

// Function prototype of helper routine used by operator>>:
template<typename T, int R> RA_INLINE_ 
void read_and_parse_shape(std::istream & in, int* shape, typename PointerArray<T,R>::type p = 0);

// Template functions to detemine the dimensions of automatic arrays, for use in the EXTENT macro
// To be able to determine the first dimension, these need to get pass the total size in bytes (byte_size) of such an automatic array
template<typename A>                                                              RA_INLINE_ int extent_given_byte_size(A a[], int i, int byte_size);                               //for 1d array
template<typename A,int Z>                                                        RA_INLINE_ int extent_given_byte_size(A a[][Z], int i, int byte_size);                            //for 2d array
template<typename A,int Y,int Z>                                                  RA_INLINE_ int extent_given_byte_size(A a[][Y][Z], int i, int byte_size);                         //for 3d array
template<typename A,int X,int Y,int Z>                                            RA_INLINE_ int extent_given_byte_size(A a[][X][Y][Z], int i, int byte_size);                      //for 4d array
template<typename A,int W,int X,int Y,int Z>                                      RA_INLINE_ int extent_given_byte_size(A a[][W][X][Y][Z], int i, int byte_size);                   //for 5d array
template<typename A,int V,int W,int X,int Y,int Z>                                RA_INLINE_ int extent_given_byte_size(A a[][V][W][X][Y][Z], int i, int byte_size);                //for 6d array
template<typename A,int U,int V,int W,int X,int Y,int Z>                          RA_INLINE_ int extent_given_byte_size(A a[][U][V][W][X][Y][Z], int i, int byte_size);             //for 7d array
template<typename A,int T,int U,int V,int W,int X,int Y,int Z>                    RA_INLINE_ int extent_given_byte_size(A a[][T][U][V][W][X][Y][Z], int i, int byte_size);          //for 8d array
template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z>              RA_INLINE_ int extent_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int i, int byte_size);       //for 9d array
template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>        RA_INLINE_ int extent_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int i, int byte_size);    //for 10d array
template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>  RA_INLINE_ int extent_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int i, int byte_size); //for 11d array
template<typename A,int R>                                                        RA_INLINE_ int extent_given_byte_size(const rarray<A,R>& a, int i, int byte_size); // use rarray's extent function
// Template functions to convert automatic arrays, for conversion with RARRAY macro
// To be able to determine the first dimension, these need to get pass the total size in bytes (byte_size) of such an automatic array
template<typename A>                                                              RA_INLINE_ rarray<A,1>  make_rarray_given_byte_size(A a[], int byte_size);                              //for 1d array
template<typename A,int Z>                                                        RA_INLINE_ rarray<A,2>  make_rarray_given_byte_size(A a[][Z], int byte_size);                           //for 2d array
template<typename A,int Y,int Z>                                                  RA_INLINE_ rarray<A,3>  make_rarray_given_byte_size(A a[][Y][Z], int byte_size);                        //for 3d array
template<typename A,int X,int Y,int Z>                                            RA_INLINE_ rarray<A,4>  make_rarray_given_byte_size(A a[][X][Y][Z], int byte_size);                     //for 4d array
template<typename A,int W,int X,int Y,int Z>                                      RA_INLINE_ rarray<A,5>  make_rarray_given_byte_size(A a[][W][X][Y][Z], int byte_size);                  //for 5d array
template<typename A,int V,int W,int X,int Y,int Z>                                RA_INLINE_ rarray<A,6>  make_rarray_given_byte_size(A a[][V][W][X][Y][Z], int byte_size);               //for 6d array
template<typename A,int U,int V,int W,int X,int Y,int Z>                          RA_INLINE_ rarray<A,7>  make_rarray_given_byte_size(A a[][U][V][W][X][Y][Z], int byte_size);            //for 7d array
template<typename A,int T,int U,int V,int W,int X,int Y,int Z>                    RA_INLINE_ rarray<A,8>  make_rarray_given_byte_size(A a[][T][U][V][W][X][Y][Z], int byte_size);         //for 8d array
template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z>              RA_INLINE_ rarray<A,9>  make_rarray_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int byte_size);      //for 9d array
template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>        RA_INLINE_ rarray<A,10> make_rarray_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int byte_size);   //for 10d array
template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>  RA_INLINE_ rarray<A,11> make_rarray_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int byte_size);//for 11d array
template<typename A,int R>                                                        RA_INLINE_ rarray<A,R> make_rarray_given_byte_size(rarray<A,R> a, int byte_size); // trivial action for rarray

} // end namespace ra


//------------------------------------------------//
//                                                //
//          I M P L E M E N T A T I O N           //
//                                                //
//------------------------------------------------//

// Helper macros to define methods with the same body without typing
// it twice, the R=1 case being specialized this is necessary to avoid
// a lot of duplication (or even quadruplication) of code. 
// Note that R=1 has to be specialized so brackets work well.  In the
// 'body', non-enclosed commas need to be replaced by COMMA for his to
// work.

#define RA_COMMA ,
#define RA_DUPLICATE_BODY(header1,header2,body)                         \
    header1 body                                                        \
    header2 body
#define RA_QUADRUPLICATE_BODY(header1,header2,header3,header4,body)     \
    header1 body                                                        \
    header2 body                                                        \
    header3 body                                                        \
    header4 body
#define RA_SEXTUPLICATE_BODY(header1,header2,header3,header4,header5,header6,body) \
    header1 body                                                        \
    header2 body                                                        \
    header3 body                                                        \
    header4 body                                                        \
    header5 body                                                        \
    header6 body
#define RA_OCTUPLICATE_BODY(header1,header2,header3,header4,header5,header6,header7,header8,body) \
    header1 body                                                        \
    header2 body                                                        \
    header3 body                                                        \
    header4 body                                                        \
    header5 body                                                        \
    header6 body                                                        \
    header7 body                                                        \
    header8 body

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// default constructor, puts rarray in 'undefined' state

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::rarray<T RA_COMMA R>::rarray(),
template<typename T>                RA_INLINE_ ra::rarray<T RA_COMMA 1>::rarray(),
  : parray_(RA_NULLPTR) RA_COMMA
    extent_(RA_NULLPTR) RA_COMMA
    ismine_(false)      RA_COMMA
    cleans_(false)      RA_COMMA
    tmpval_(false) 
{
    // constructor for an undefined array
    RA_IFTRACESAY("rarray<T,R>::rarray()");
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray constructors that allocate the array data of dimensions given as arguments

template<typename T> RA_INLINE_ 
ra::rarray<T,1>::rarray(int n0)
{
    // constructor for rank==1
    RA_IFTRACESAY("rarray<T,1>::rarray(int)");
    init_data(&n0, n0);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1)
{
    // constructor for R=2
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int)");
    RA_CHECKORSAY(R==2, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1};
    init_data(extent, n0*n1);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2)
{
    // constructor for R=3
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int)");
    RA_CHECKORSAY(R==3, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2};
    init_data(extent, n0*n1*n2);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3)
{
    // constructor for R=4
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int)");
    RA_CHECKORSAY(R==4, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3};
    init_data(extent, n0*n1*n2*n3);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4)
{
    // constructor for R=5
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int)");
    RA_CHECKORSAY(R==5, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4};
    init_data(extent, n0*n1*n2*n3*n4);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5)
{
    // constructor for R=6
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int, int)");
    RA_CHECKORSAY(R==6, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5};
    init_data(extent, n0*n1*n2*n3*n4*n5);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6)
{
    // constructor for R=7
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==7, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7)
{
    // constructor for R=8
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==8, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
    // constructor for R=9
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==9, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7*n8);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    // constructor for R=10
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==10, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7*n8*n9);
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10)
{
    // constructor for R=11
    RA_IFTRACESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int, int, int. int)");
    RA_CHECKORSAY(R==11, "wrong rank in constructor");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0 and n10!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray constructors that allocate the array data of dimensions given as a c-array

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(const int* extent) 
{
    // constructor for any R (the only way for R>11)
    RA_IFTRACESAY("rarray<T,R>::rarray(const int*)");
    int volume = 1;
    for (int i=0; i<R; i++)
        volume *= extent[i];
    RA_CHECKORSAY(volume!=0,"zero shape in constructor not allowed");
    init_data(extent, volume);
}

template<typename T> RA_INLINE_ 
ra::rarray<T,1>::rarray(const int* extent) 
{
    // constructor from an array of 1 extent: here just for uniformity
    RA_IFTRACESAY("rarray<T,1>::rarray(const int*)");
    RA_CHECKORSAY(*extent!=0,"zero shape in constructor not allowed");
    init_data(extent, extent[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray constructors from existing buffer with dimensions as arguments

template<typename T> RA_INLINE_ 
ra::rarray<T,1>::rarray(T* buffer, int n0)
{
    // constructor for rank==1
    RA_IFTRACESAY("rarray<T,1>::rarray(T*, int)");
    const int extent[1] = {n0};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1) 
{
    // constructor for R=2
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int)");
    RA_CHECKORSAY(R==2, "wrong rank in constructor");
    const int extent[R] = {n0,n1};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2) 
{
    // constructor for R=3
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int)");
    RA_CHECKORSAY(R==3, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3) 
{
    // constructor for R=4
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int)");
    RA_CHECKORSAY(R==4, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4) 
{
    // constructor for R=5
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int)");
    RA_CHECKORSAY(R==5, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5) 
{
    // constructor for R=6
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==6, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6) 
{
    // constructor for R=7
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==7, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7)
{
    // constructor for R=8
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==8, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
    // constructor for R=9
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==9, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    // constructor for R=10
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==10, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10)
{
    // constructor for R=11
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==11, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10};
    init_parray(buffer, extent);
    ismine_ = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray constructors from existing buffer with dimensions in a single-argument c-array

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::rarray<T RA_COMMA R>::rarray(T* buffer, const int* extent),
template<typename T>                RA_INLINE_ ra::rarray<T RA_COMMA 1>::rarray(T* buffer, const int* extent), 
{
    // constructor for any I (the only way for R>11)
    RA_IFTRACESAY("rarray<T,R>::rarray(T*, const int*)");
    init_parray(buffer, extent);
    ismine_ = false;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// subrarray constructors

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::subrarray<T RA_COMMA R>::subrarray(parray_t parray, const int* extent),
template<typename T>                RA_INLINEF ra::subrarray<T RA_COMMA 1>::subrarray(parray_t parray, const int* extent),
  : parray_(parray) RA_COMMA
    extent_(extent) 
{
    // constructor of subrarray; private, to be called from operator[] of rarrays
    RA_IFTRACESAY("subrarray<T,R>::subrarray(parray_t, const int*)");
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray destructors

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>::~rarray(),
template<typename T>                RA_INLINEF ra::rarray<T RA_COMMA 1>::~rarray(),
{
    // rarray<T,R> destructor (note: subrarray has none)
    RA_IFTRACESAY("rarray<T,R>::~rarray()");
    clear();
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray and subrarray comma-separated assignment

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::CommaOp<T> ra::rarray<T RA_COMMA R>::operator=(const T& e),
template<typename T>                RA_INLINE_ ra::CommaOp<T> ra::rarray<T RA_COMMA 1>::operator=(const T& e),
template<typename T RA_COMMA int R> RA_INLINE_ ra::CommaOp<T> ra::subrarray<T RA_COMMA R>::operator=(const T& e),
template<typename T>                RA_INLINE_ ra::CommaOp<T> ra::subrarray<T RA_COMMA 1>::operator=(const T& e),
{
    // Comma separated element assignment: puts the first one in and prepares for more
    RA_IFTRACESAY("CommaOp<T> rarray<T,R>::operator=(const T&)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "assignment to unsized array");
    RA_CHECKORSAY(size()>0,"assignment with more elements than in array");
    T* first = get_buffer();
    *first = e;
    ra::CommaOp<T> co(first+1 RA_COMMA first+size()-1); 
    return co;
 })

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// comma operator constructor

template<typename T> RA_INLINE_ ra::CommaOp<T>::CommaOp(T* ptr, T* last)
: ptr_(ptr), last_(last)
{ 
    // fill remainder of array with zeros
    RA_IFTRACESAY("CommaOp<T>::CommaOp(T*,T*)");
    RA_CHECKORSAY(ptr_!=RA_NULLPTR and last_!=RA_NULLPTR, "invalid comma operator");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// comma operator; set next element

template<typename T> RA_INLINE_ ra::CommaOp<T>& ra::CommaOp<T>::operator,(const T& e)
{ 
    // puts the next number into the array.
    RA_IFTRACESAY("CommaOp<T>& CommaOp<T>::operator,(const T&e)");
    RA_CHECKORSAY(ptr_!=RA_NULLPTR and last_!=RA_NULLPTR, "invalid comma operator");
    RA_CHECKORSAY(ptr_<=last_, "assignment with more elements than in array");
    *ptr_++ = e;
    return *this; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// is_clear

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ bool ra::rarray<T RA_COMMA R>::is_clear() const,
template<typename T>                RA_INLINE_ bool ra::rarray<T RA_COMMA 1>::is_clear() const,
{
    // check if undefined
    RA_IFTRACESAY("bool rarray<T,R>::is_clear()");
    return parray_ == RA_NULLPTR;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray copy constructor

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>::rarray(const rarray<T RA_COMMA R> &a),
template<typename T>                RA_INLINEF ra::rarray<T RA_COMMA 1>::rarray(const rarray<T RA_COMMA 1> &a),
{
    // copy constructor
    RA_IFTRACESAY("rarray<T,R>::rarray(const rarray<T,R>&)");
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.cleans_, a.tmpval_);
    ismine_ = a.ismine_;
})

// rarray copy constructor from subrarray

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>::rarray(const ra::subrarray<T RA_COMMA R> &a),
template<typename T>                RA_INLINEF ra::rarray<T RA_COMMA 1>::rarray(const ra::subrarray<T RA_COMMA 1> &a), 
{
    // copy constructor
    RA_IFTRACESAY("rarray<T,R>::rarray(const subrarray<T,R>&)");
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rarray deep copy

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::rarray<T RA_COMMA R> ra::rarray<T RA_COMMA R>::copy() const,
template<typename T>                RA_INLINE_ ra::rarray<T RA_COMMA 1> ra::rarray<T RA_COMMA 1>::copy() const, 
{
    // create an independent, deep copy (regular copy is shallow)
    RA_IFTRACESAY("rarray<T,R> rarray<T,R>::copy() const");
    if (parray_ != RA_NULLPTR) { 
        // if initialized
        ra::rarray<T RA_COMMA R> result(extent_);
        T* bufbegin = get_buffer();
        T* bufend = bufbegin+size();
        T* newbuf = result.get_buffer();
        std::copy(bufbegin, bufend, newbuf);
        return RA_MOVE(result);
    } else {
        // else return undefined copy
        return RA_MOVE(rarray());
    }
 })

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// extent of (sub)rarray

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ int ra::rarray<T RA_COMMA R>::extent(int i) const,
template<typename T>                RA_INLINE_ int ra::rarray<T RA_COMMA 1>::extent(int i) const,
template<typename T RA_COMMA int R> RA_INLINE_ int ra::subrarray<T RA_COMMA R>::extent(int i) const,
template<typename T>                RA_INLINE_ int ra::subrarray<T RA_COMMA 1>::extent(int i) const, 
{
    // retrieve array size in dimension i   
    RA_IFTRACESAY("int sub/rarray<T,R>::extent(int) const");
    RA_CHECKORSAY(i >=0 and i < R, "wrong dimension");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return extent_[i];
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// shape of (sub)rarray

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ const int* ra::rarray<T RA_COMMA R>::shape() const,
template<typename T>                RA_INLINE_ const int* ra::rarray<T RA_COMMA 1>::shape() const,
template<typename T RA_COMMA int R> RA_INLINE_ const int* ra::subrarray<T RA_COMMA R>::shape() const,
template<typename T>                RA_INLINE_ const int* ra::subrarray<T RA_COMMA 1>::shape() const,
{
    // retrieve pointer to array of extents
    RA_IFTRACESAY("const int* sub/rarray<T,R>::shape() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return extent_;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// size of (sub)rarray

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ int ra::rarray<T RA_COMMA R>::size() const, 
template<typename T>                RA_INLINE_ int ra::rarray<T RA_COMMA 1>::size() const,
template<typename T RA_COMMA int R> RA_INLINE_ int ra::subrarray<T RA_COMMA R>::size() const,
template<typename T>                RA_INLINE_ int ra::subrarray<T RA_COMMA 1>::size() const,
{ 
    // Retrieve array sizes in all dimensions. 
    // Needs to be computed, as this information is not stored.
    RA_IFTRACESAY("int sub/rarray<T,R>::size() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    int result = 1;
    for (int i=0; i<R; i++) 
        result *= extent_[i];
    return result;
})

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// data of (sub)rarray

RA_SEXTUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_       T* ra::rarray<T RA_COMMA R>::data(),
template<typename T RA_COMMA int R> RA_INLINE_ const T* ra::rarray<T RA_COMMA R>::data() const,
template<typename T>                RA_INLINE_       T* ra::rarray<T RA_COMMA 1>::data(),
template<typename T>                RA_INLINE_ const T* ra::rarray<T RA_COMMA 1>::data() const,
template<typename T RA_COMMA int R> RA_INLINE_       T* ra::subrarray<T RA_COMMA R>::data() const,
template<typename T>                RA_INLINE_       T* ra::subrarray<T RA_COMMA 1>::data() const,
{
    // return pointer of type T* to the internal data
    RA_IFTRACESAY("(const) T* rarray<T,R>::data() (const)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return get_buffer();
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// (sub)rarray begin methods

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::iterator ra::rarray<T RA_COMMA R>::begin(),
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::iterator ra::rarray<T RA_COMMA 1>::begin(),
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::subrarray<T RA_COMMA R>::iterator ra::subrarray<T RA_COMMA R>::begin() const,
template<typename T>                RA_INLINE_ typename ra::subrarray<T RA_COMMA 1>::iterator ra::subrarray<T RA_COMMA 1>::begin() const,
{
    // get iterator to first element. iterators remember their size
    RA_IFTRACESAY("iterator rarray<T,R>::begin()");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return iterator(get_buffer(), size());
})

RA_SEXTUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::const_iterator ra::rarray<T RA_COMMA R>::begin() const,
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::const_iterator ra::rarray<T RA_COMMA R>::cbegin() const, 
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::const_iterator ra::rarray<T RA_COMMA 1>::begin() const, 
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::const_iterator ra::rarray<T RA_COMMA 1>::cbegin() const, 
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::subrarray<T RA_COMMA R>::const_iterator ra::subrarray<T RA_COMMA R>::cbegin() const,
template<typename T>                RA_INLINE_ typename ra::subrarray<T RA_COMMA 1>::const_iterator ra::subrarray<T RA_COMMA 1>::cbegin() const,
{
    // get const iterator to first element. iterators remember their size.
    RA_IFTRACESAY("const_iterator rarray<T,R>::begin() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return const_iterator(get_buffer(), size());
 })

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// (sub)rarray end methods

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::iterator ra::rarray<T RA_COMMA R>::end(),
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::iterator ra::rarray<T RA_COMMA 1>::end(),
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::subrarray<T RA_COMMA R>::iterator ra::subrarray<T RA_COMMA R>::end() const,
template<typename T>                RA_INLINE_ typename ra::subrarray<T RA_COMMA 1>::iterator ra::subrarray<T RA_COMMA 1>::end() const,
{
    // get iterator to just past last element. as there is nothing there, this iterators has no size
    RA_IFTRACESAY("iterator rarray<T,R>::end()");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return iterator(get_buffer()+size(), 0);
})

RA_SEXTUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::const_iterator ra::rarray<T RA_COMMA R>::end() const, 
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::const_iterator ra::rarray<T RA_COMMA R>::cend() const, 
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::const_iterator ra::rarray<T RA_COMMA 1>::end() const, 
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::const_iterator ra::rarray<T RA_COMMA 1>::cend() const, 
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::subrarray<T RA_COMMA R>::const_iterator ra::subrarray<T RA_COMMA R>::cend() const,
template<typename T>                RA_INLINE_ typename ra::subrarray<T RA_COMMA 1>::const_iterator ra::subrarray<T RA_COMMA 1>::cend() const,
{
    // get const iterator to just past last element. as there is nothing there, this iterators has no size
    RA_IFTRACESAY("const_iterator rarray<T,R>::(c)end() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return const_iterator(get_buffer()+size(), 0);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// retrieve indices of an element of a (sub)rarray determined from an iterator pointing at the element

RA_OCTUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ int*    ra::rarray<T RA_COMMA R>::index(const iterator&i, int* ind),
template<typename T RA_COMMA int R> RA_INLINE_ int*    ra::rarray<T RA_COMMA R>::index(const const_iterator&i, int* ind) const,
template<typename T>                RA_INLINE_ int*    ra::rarray<T RA_COMMA 1>::index(const iterator&i, int* ind),
template<typename T>                RA_INLINE_ int*    ra::rarray<T RA_COMMA 1>::index(const const_iterator&i, int* ind) const,
template<typename T RA_COMMA int R> RA_INLINE_ int* ra::subrarray<T RA_COMMA R>::index(const iterator&i, int* ind),
template<typename T RA_COMMA int R> RA_INLINE_ int* ra::subrarray<T RA_COMMA R>::index(const const_iterator&i, int* ind) const,
template<typename T>                RA_INLINE_ int* ra::subrarray<T RA_COMMA 1>::index(const iterator&i, int* ind),
template<typename T>                RA_INLINE_ int* ra::subrarray<T RA_COMMA 1>::index(const const_iterator&i, int* ind) const,
{
    // retrieve indexes within *this of the element pointed to by i. puts them in preexisting array and returns pointer
    RA_IFTRACESAY("int* rarray<T,R>::index((const_)iterator&,int*) (const)");
    return index(*i, ind);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// retrieve indices of an element of a (sub)rarray determine from a reference to the element

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ int*    ra::rarray<T RA_COMMA R>::index(const T& a, int* ind) const,
template<typename T>                RA_INLINE_ int*    ra::rarray<T RA_COMMA 1>::index(const T& a, int* ind) const,
template<typename T RA_COMMA int R> RA_INLINE_ int* ra::subrarray<T RA_COMMA R>::index(const T& a, int* ind) const,
template<typename T>                RA_INLINE_ int* ra::subrarray<T RA_COMMA 1>::index(const T& a, int* ind) const,
{
    // retrieve indexes within *this of the element a. puts them in preexisting array and returns pointer
    RA_IFTRACESAY("int* rarray<T,R>::index((const) T&,int*) (const)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    RA_CHECKORSAY(ind!=RA_NULLPTR, "invalid index buffer");
    int linearindex = &a - get_buffer();
    RA_CHECKORSAY(linearindex >=0 and linearindex < size(), "element not in array");
    int j = R;
    while (j-->0) {
        ind[j] = linearindex % extent_[j];
        linearindex /= extent_[j];
    }
    return ind;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// retrieve indices of an element

RA_OCTUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ int    ra::rarray<T RA_COMMA R>::index(const iterator&iter, int i),
template<typename T RA_COMMA int R> RA_INLINE_ int    ra::rarray<T RA_COMMA R>::index(const const_iterator&iter, int i) const,
template<typename T>                RA_INLINE_ int    ra::rarray<T RA_COMMA 1>::index(const iterator&iter, int i),
template<typename T>                RA_INLINE_ int    ra::rarray<T RA_COMMA 1>::index(const const_iterator&iter, int i) const,
template<typename T RA_COMMA int R> RA_INLINE_ int ra::subrarray<T RA_COMMA R>::index(const iterator&iter, int i),
template<typename T RA_COMMA int R> RA_INLINE_ int ra::subrarray<T RA_COMMA R>::index(const const_iterator&iter, int i) const,
template<typename T>                RA_INLINE_ int ra::subrarray<T RA_COMMA 1>::index(const iterator&iter, int i),
template<typename T>                RA_INLINE_ int ra::subrarray<T RA_COMMA 1>::index(const const_iterator&iter, int i) const,
{
    // retrieve index in dimension i within *this of the element pointed to by i
    RA_IFTRACESAY("in* rarray<T,R>::index((const_)iterator&,int) (const)");
    return index(*iter, i);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// retrieve indices of an element

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ int    ra::rarray<T RA_COMMA R>::index(const T& a, int i) const,
template<typename T>                RA_INLINE_ int    ra::rarray<T RA_COMMA 1>::index(const T& a, int i) const,
template<typename T RA_COMMA int R> RA_INLINE_ int ra::subrarray<T RA_COMMA R>::index(const T& a, int i) const,
template<typename T>                RA_INLINE_ int ra::subrarray<T RA_COMMA 1>::index(const T& a, int i) const,
{
    // retrieve index in dimension i within *this of the element a
    RA_IFTRACESAY("int rarray<T,R>::index((const) T&,int) (const)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    RA_CHECKORSAY(i >=0 and i < R, "wrong dimension");
    int linearindex = &a - get_buffer();
    RA_CHECKORSAY(linearindex >=0 and linearindex < size(), "element not in array");
    for (int j = R-1; j > i; j--) 
        linearindex /= extent_[j];
    return linearindex % extent_[i];

})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// pointer-to-pointer structure

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::parray_t    ra::rarray<T RA_COMMA R>::ptr_array() const,
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::parray_t    ra::rarray<T RA_COMMA 1>::ptr_array() const, 
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::subrarray<T RA_COMMA R>::parray_t ra::subrarray<T RA_COMMA R>::ptr_array() const,
template<typename T>                RA_INLINE_ typename ra::subrarray<T RA_COMMA 1>::parray_t ra::subrarray<T RA_COMMA 1>::ptr_array() const,
{
    // return T*const*.. acting similarly to this rarray when using []
    RA_IFTRACESAY("sub/rarray<T,R>::parray_t sub/rarray<T,R>::ptr_array() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined array");
    return parray_;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// pointer-to-pointer structure, without const

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::rarray<T RA_COMMA R>::noconst_parray_t    ra::rarray<T RA_COMMA R>::noconst_ptr_array() const, 
template<typename T>                RA_INLINE_ typename ra::rarray<T RA_COMMA 1>::noconst_parray_t    ra::rarray<T RA_COMMA 1>::noconst_ptr_array() const, 
template<typename T RA_COMMA int R> RA_INLINE_ typename ra::subrarray<T RA_COMMA R>::noconst_parray_t ra::subrarray<T RA_COMMA R>::noconst_ptr_array() const, 
template<typename T>                RA_INLINE_ typename ra::subrarray<T RA_COMMA 1>::noconst_parray_t ra::subrarray<T RA_COMMA 1>::noconst_ptr_array() const, 
{
    // return T**.. acting similarly to this rarray when using []
    RA_IFTRACESAY("rarray<T,R>::noconst_parray_t sub/rarray<T,R>::noconst_ptr_array() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return const_cast<noconst_parray_t>(parray_);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// refererence as a const T

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::rarray<const T RA_COMMA R>& ra::rarray<T RA_COMMA R>::const_ref() const,
template<typename T>                RA_INLINE_ ra::rarray<const T RA_COMMA 1>& ra::rarray<T RA_COMMA 1>::const_ref() const,
{
    // create a reference to this that treats elements as constant
    RA_IFTRACESAY("rarray<const T,R>& rarray<T,R>::const_ref() const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return (rarray<const T RA_COMMA R>&)(*this);
 })

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::subrarray<const T RA_COMMA R>& ra::subrarray<T RA_COMMA R>::const_ref() const,
template<typename T>                RA_INLINE_ ra::subrarray<const T RA_COMMA 1>& ra::subrarray<T RA_COMMA 1>::const_ref() const,
{
    // create a reference to this that treats elements as constant
    RA_IFTRACESAY("subrarray<const T,R>& subrarray<T,R>::const_ref() const");
    return (subrarray<const T,R>&)(*this);
 })

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ void    ra::rarray<T RA_COMMA R>::fill(const T& value),
template<typename T>                RA_INLINE_ void    ra::rarray<T RA_COMMA 1>::fill(const T& value),
template<typename T RA_COMMA int R> RA_INLINE_ void ra::subrarray<T RA_COMMA R>::fill(const T& value),
template<typename T>                RA_INLINE_ void ra::subrarray<T RA_COMMA 1>::fill(const T& value),
{
    // fill with constant value using iterators
    RA_IFTRACESAY("void sub/rarray<T,R>::fill(const T&)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    for (iterator i=begin(); i!=end(); i++)
        *i = value;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RA_SKIPINTERMEDIATE
template<typename T,int R> RA_INLINEF ra::subrarray<const T,R-1> ra::rarray<T,R>::operator[](int i) const
{
    RA_IFTRACESAY("subrarray<const T,R-1> rarray<T,R>::operator[](int) const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    RA_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return ra::subrarray<const T,R-1>(parray_[i], extent_+1);
}
template<typename T,int R> RA_INLINEF ra::subrarray<T,R-1> ra::rarray<T,R>::operator[](int i)
{
    RA_IFTRACESAY("subrarray<T,R-1> rarray<T,R>::operator[](int)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    RA_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return ra::subrarray<T,R-1>(parray_[i], extent_+1);
}
RA_DUPLICATE_BODY(
template<typename T> RA_INLINEF const T& ra::rarray<T RA_COMMA 1>::operator[](int i) const,
template<typename T> RA_INLINEF       T& ra::rarray<T RA_COMMA 1>::operator[](int i),
{
    RA_IFTRACESAY("(const) T& rarray<T,1>::operator[](int) (const)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    RA_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
})
#else
RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>::operator typename ra::PointerArray<const T RA_COMMA R>::type () const,
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>::operator typename ra::PointerArray<T RA_COMMA R>::type (),
{
    RA_IFTRACESAY("rarray<T,R>::operator typename PointerArray<const T,R>::parray_t () const");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return parray_;
})
RA_DUPLICATE_BODY(
template<typename T> RA_INLINEF ra::rarray<T RA_COMMA 1>::operator typename ra::PointerArray<const T RA_COMMA 1>::type () const, 
template<typename T> RA_INLINEF ra::rarray<T RA_COMMA 1>::operator typename ra::PointerArray<T RA_COMMA 1>::type (),
{
    RA_IFTRACESAY("rarray<T,1>::operator PointerArray<(const) T,1>::type () (const)");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    return parray_;
})
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// subrarray brackets

template<typename T,int R> RA_INLINEF ra::subrarray<T,R-1> ra::subrarray<T,R>::operator[](int i) const
{
    // subrarray methods for element access
    RA_IFTRACESAY("subrarray<T,R-1> subrarray<T,R>::operator[](int)");
    RA_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");        
    return subrarray<T,R-1>(parray_[i], extent_+1);
}

template<typename T> RA_INLINEF T& ra::subrarray<T,1>::operator[](int i) const
{
    // subrarray methods for element access, R==1 version
    RA_IFTRACESAY("T& subrarray<T,1>::operator[](int)");
    RA_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>& ra::rarray<T RA_COMMA R>::operator=(const ra::rarray<T RA_COMMA R> &a),
template<typename T>                RA_INLINEF ra::rarray<T RA_COMMA 1>& ra::rarray<T RA_COMMA 1>::operator=(const ra::rarray<T RA_COMMA 1> &a),
{
    // rarray assignment operator from other rarray
    RA_IFTRACESAY("rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R>&)");
    if (&a != this) {
        clear();
        extent_ = const_cast<int*>(a.extent_);
        init_shallow(a.parray_, a.cleans_, a.tmpval_);
        ismine_ = a.ismine_;
    }
    return *this;
})

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF ra::rarray<T RA_COMMA R>& ra::rarray<T RA_COMMA R>::operator=(const ra::subrarray<T RA_COMMA R> &a),
template<typename T>                RA_INLINEF ra::rarray<T RA_COMMA 1>& ra::rarray<T RA_COMMA 1>::operator=(const ra::subrarray<T RA_COMMA 1> &a),
{
    // rarray assignment operator from subrarray
    RA_IFTRACESAY("rarray<T,R>& rarray<T,R>::operator=(const subrarray<T,R>&)");
    clear();
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_);
    return *this;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_QUADRUPLICATE_BODY(
template<typename T RA_COMMA int R> T* ra::rarray<T RA_COMMA R>::get_buffer() const,
template<typename T>                T* ra::rarray<T RA_COMMA 1>::get_buffer() const,
template<typename T RA_COMMA int R> T* ra::subrarray<T RA_COMMA R>::get_buffer() const,
template<typename T>                T* ra::subrarray<T RA_COMMA 1>::get_buffer() const,
{
    RA_IFTRACESAY("T* (sub)rarray<T,R>::get_buffer()");
    return ra::rarray<T RA_COMMA R>::base(parray_);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ void ra::rarray<T RA_COMMA R>::init_shallow(parray_t parray, bool& cleans, bool tmpval),
template<typename T>                RA_INLINE_ void ra::rarray<T RA_COMMA 1>::init_shallow(parray_t parray, bool& cleans, bool tmpval),
{
    // shallow init function : reuses buffer and parray
    RA_IFTRACESAY("void rarray<T,R>::init_shallow(parray_t, bool&, bool)");
    RA_CHECKORSAY(      parray != RA_NULLPTR, "null pointer");
    RA_CHECKORSAY(base(parray) != RA_NULLPTR, "null pointer");
    parray_ = parray;
    tmpval_ = false;
    if (tmpval) {
        cleans_ = cleans;
        cleans = false;
    } else {
        cleans_ = false;
    }
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ void ra::rarray<T RA_COMMA R>::init_shallow(parray_t parray),
template<typename T>                RA_INLINE_ void ra::rarray<T RA_COMMA 1>::init_shallow(parray_t parray),
{
    // shallow init function for subrarray: reuses buffer and parray
    RA_IFTRACESAY("void rarray<T,R>::init_shallow(parray_t)");
    RA_CHECKORSAY(      parray != RA_NULLPTR, "null pointer");
    RA_CHECKORSAY(base(parray) != RA_NULLPTR, "null pointer");
    parray_ = parray;
    tmpval_ = false;
    cleans_ = false;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ void ra::rarray<T RA_COMMA R>::init_parray(T* buffer, const int* extent),
template<typename T>                RA_INLINE_ void ra::rarray<T RA_COMMA 1>::init_parray(T* buffer, const int* extent),
{
    // init functions that reuses a buffer but not a parray
    RA_IFTRACESAY("void rarray<T,R>::init_parray(T*, const int*)");
    RA_CHECKORSAY(buffer != RA_NULLPTR, "null pointer");
    RA_CHECKORSAY(extent != RA_NULLPTR, "null pointer");
    parray_t parray = new_except_base(buffer, extent);
    extent_ = new int[R];
    for (int i=0;i<R;i++)
        extent_[i] = extent[i];
    bool oldcleans = true;
    init_shallow(parray, oldcleans, true);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ void ra::rarray<T RA_COMMA R>::init_data(const int* extent, int extenttot),
template<typename T>                RA_INLINE_ void ra::rarray<T RA_COMMA 1>::init_data(const int* extent, int extenttot),
{
    // init functions that need new buffer and parray
    RA_IFTRACESAY("void rarray<T,R>::init_data(const int*, int)");
    RA_CHECKORSAY( extent != RA_NULLPTR, "null pointer");
    RA_CHECKORSAY( extenttot >= 0, "negative number of elements");
    T* buffer = new T[extenttot];
    init_parray(buffer, extent);
    ismine_ = true;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// convert rarray to a movable array

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ ra::rarray<T RA_COMMA R>& ra::rarray<T RA_COMMA R>::move(),
template<typename T>                RA_INLINE_ ra::rarray<T RA_COMMA 1>& ra::rarray<T RA_COMMA 1>::move(),
{
    // create a reference to this that treats elements as constant
    RA_IFTRACESAY("rarray<T,R>& rarray<T,R>::move()");
    RA_CHECKORSAY(parray_!=RA_NULLPTR, "attempt at using undefined rarray");
    tmpval_ = true;
    return *this;
 })

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(const int* extent, RAbool force)
{
    // common method to reshape an array (takes an c-array argument)
    RA_IFTRACESAY("void rarray<T,R>::reshape(const int* extent, RAbool force)");
    RA_CHECKORSAY(parray_ != RA_NULLPTR, "null pointer");
    RA_CHECKORSAY(not cleans_ or force.as_bool(), "reshape only allowed on shallow copy of rarray");
    int tot1 = 1, tot2 = 1;
    for (int i=0;i<R;i++) {
        tot1 *= extent_[i];
        tot2 *= extent[i];
    }
    RA_CHECKORSAY(tot2 <= tot1, "reshaping beyond underlying memory buffer");
    if (parray_ != RA_NULLPTR and (not cleans_ or force.as_bool()))  // cannot reshape originals unless forced
    {
        if (not cleans_) {
            T* buffer = get_buffer();    // get buffer address
            ismine_ = false;             // protect buffer from being deleted
            clear();                     // deallocate everything else
            init_parray(buffer, extent); // reallocate pointer arrays
        } else {
            T* buffer = get_buffer();
            delete[] parray_;
            for (int i=0;i<R;i++)
                extent_[i] = extent[i];
            parray_ = new_except_base(buffer, extent_);
        }
    }
}

template<typename T> RA_INLINE_
void ra::rarray<T,1>::reshape(const int* extent, RAbool force)
{
    RA_IFTRACESAY("void rarray<T,1>::reshape(const int* extent, RAbool force)");
    RA_CHECKORSAY(parray_ != RA_NULLPTR, "null pointer");
    RA_CHECKORSAY(*extent <= *extent_, "reshaping beyond underlying memory buffer");
    RA_CHECKORSAY(not cleans_ or force.as_bool(), "reshape only allowed on shallow copy of rarray");
    if (parray_ != RA_NULLPTR and (not cleans_ or force.as_bool()))  // cannot reshape originals unless forced
    {
        if (not cleans_) {
            T* buffer = get_buffer();    // get buffer address        
            ismine_ = false;             // protect buffer from being deleted
            clear();                      // deallocate everything else
            init_parray(buffer, extent); // reallocate pointer arrays
        } else {
            *extent_ = *extent;
        }
    }
}

template<typename T> RA_INLINE_ 
void ra::rarray<T,1>::reshape(int n0, RAbool force)
{
    // constructor for R=1
    RA_IFTRACESAY("void rarray<T,1>::reshape(int)");
    RA_CHECKORSAY(R==1, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0, "zero shape not allowed");
    const int extent[1] = {n0};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_
void ra::rarray<T,R>::reshape(int n0, int n1, RAbool force) 
{
    // reshape method only for R=2
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int)");
    RA_CHECKORSAY(R==2, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0, "zero shape not allowed");
    // reuse common method
    const int extent[R] = {n0,n1};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, RAbool force)
{
    // reshape method only for R=3
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int)");
    RA_CHECKORSAY(R==3, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, RAbool force) 
{
    // reshape method only for R=4
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int)");
    RA_CHECKORSAY(R==4, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, RAbool force)
{
    // reshape method only for R=5
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int)");
    RA_CHECKORSAY(R==5, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, RAbool force)
{
    // reshape method only for R=6
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int, int)");
    RA_CHECKORSAY(R==6, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, RAbool force)
{
    // reshape method only for R=7
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==7, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, RAbool force)
{
    // reshape method only for R=8
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==8, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, RAbool force)
{
    // reshape method only for R=9
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==9, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, RAbool force)
{
    // reshape method only for R=10
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==10, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};
    reshape(extent, force);
}

template<typename T,int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, RAbool force)
{
    // reshape method only for R=11
    RA_IFTRACESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int, int, int, int)");
    RA_CHECKORSAY(R==11, "wrong rank in reshape");
    RA_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0 and n10!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10};
    reshape(extent, force);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DELETE(x) std::cerr << "Deleting array " #x " at " << x << '\n'; delete[] x;
RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINEF void ra::rarray<T RA_COMMA R>::clear(),
template<typename T>                RA_INLINEF void ra::rarray<T RA_COMMA 1>::clear(),
{
    //  rarray private cleanup routine
    RA_IFTRACESAY("void rarray<T,R>::clear()");
    if (parray_ != RA_NULLPTR and cleans_) {
        if (ismine_) 
            delete[] get_buffer(); 
        if (R > 1)
            delete[] parray_;
        delete[] extent_;
    }
    parray_ = RA_NULLPTR;
    cleans_ = false;
    tmpval_ = false;
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_
typename ra::rarray<T,R>::parray_t 
ra::rarray<T,R>::new_except_base(T* buffer, const int* extent) 
{
    // rarray private method to allocate the chain of pointers, except the base
    RA_IFTRACESAY("parray_t rarray<T,R>::new_except_base(T*, const int*)");
    if (R>1) {
        typedef typename ra::Unconst<T>::type* bufstart_t;       
        bufstart_t bufstart = const_cast<bufstart_t>(buffer);
        int        nalloc   = 0;
        char**     result;
        for (int i=R-1; i--; )
            nalloc = extent[i]*(1+nalloc);
        
        char**  palloc = new char*[nalloc];        
        int     extenttot = 1;
        char*** ptr_array    = reinterpret_cast<char***>(&result);
        for (int i=0; i<R-1; i++) {
            for (int j=0; j<extenttot; j++)
                ptr_array[j] = palloc + j*extent[i];
            extenttot *= extent[i];
            if (extenttot==0) {
                break; // really should not be allowed and give an error
            } else {
                ptr_array     = reinterpret_cast<char***>(*ptr_array);
                palloc += extenttot;
            }
        }
        for (int j=0; j<extenttot; j++)
            ptr_array[j] = reinterpret_cast<char**>(bufstart + j*extent[R-1]);
        return reinterpret_cast<parray_t>(result);
    } else {
        return reinterpret_cast<parray_t>(buffer);
    }
}

template<typename T> RA_INLINE_
typename ra::rarray<T,1>::parray_t 
ra::rarray<T,1>::new_except_base(T* buffer, const int* extent) 
{
    RA_IFTRACESAY("parray_t rarray<T,1>::new_except_base(T*, const int*)");
    return reinterpret_cast<parray_t>(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_
T* ra::rarray<T,R>::base(parray_t parray) 
{
    // rarray private method to find base of a chain of pointers
    RA_IFTRACESAY("T* rarray<T,R>::base(parray_t)");
    if (parray != RA_NULLPTR) {
        const char* result1 = reinterpret_cast<const char*>(parray);
        char* result2 = const_cast<char*>(result1);
        char** result = reinterpret_cast<char**>(result2);    
        for (int i=0; i < R-1; i++)
            result = reinterpret_cast<char**>(*result);
        return reinterpret_cast<T*>(result);
    } else
        return RA_NULLPTR;
}

template<typename T> RA_INLINE_
T* ra::rarray<T,1>::base(parray_t parray)
{
    // rarray private method to find base of a chain of pointers: different for R==1
    RA_IFTRACESAY("T* rarray<T,1>::base(parray_t)");
    const char*  result1 = reinterpret_cast<const char*>(parray);
          char*  result2 = const_cast<char*>(result1);
          char** result  = reinterpret_cast<char**>(result2);
    return reinterpret_cast<T*>(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINEF 
T& ra::Iterator<T>::operator*() const
{
    // Iterator dereference
    RA_IFTRACESAY("T& Iterator<T>::operator*()");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_ < pointer_max_plus_one_, "iterator out of bounds");
    return *pointer_;
}           

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINEF 
ra::Iterator<T>& ra::Iterator<T>::operator++()
{
    // Iterator pre-incrememt
    RA_IFTRACESAY("Iterator<T> Iterator<T>::operator++()");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_ < pointer_max_plus_one_, "iterator out of bounds");
    ++pointer_;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_
ra::Iterator<T> ra::Iterator<T>::operator++(int)
{
    // Iterator post-increment
    RA_IFTRACESAY("Iterator<T> Iterator<T>::operator++(int)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_ < pointer_max_plus_one_, "iterator out of bounds");
    ra::Iterator<T> saved = *this;
    ++pointer_;
    return saved;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_
bool ra::Iterator<T>::operator==(const Iterator<T>& other) const
{
    // Iterator comparison: equal to
    RA_IFTRACESAY("bool Iterator<T>::operator==(const Iterator<T>&)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    RA_CHECKORSAY(other.pointer_ != RA_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ == other.pointer_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINEF 
bool ra::Iterator<T>::operator!=(const Iterator<T>& other) const
{
    // Iterator comparison: not equal to
    RA_IFTRACESAY("bool Iterator<T>::operator!=(const Iterator<T>&)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    RA_CHECKORSAY(other.pointer_ != RA_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ != other.pointer_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_ 
bool ra::Iterator<T>::operator<(const Iterator<T>& other) const
{
    // Iterator comparison: less than
    RA_IFTRACESAY("bool Iterator<T>::operator<(const Iterator<T>&)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    RA_CHECKORSAY(other.pointer_ != RA_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ < other.pointer_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_ 
bool ra::Iterator<T>::operator>(const Iterator<T>& other) const
{
    // Iterator comparison: greater than
    RA_IFTRACESAY("bool Iterator<T>::operator>(const Iterator<T>&)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    RA_CHECKORSAY(other.pointer_ != RA_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ > other.pointer_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_ 
bool ra::Iterator<T>::operator<=(const Iterator<T>& other) const
{
    // Iterator comparison: less than or equal to
    RA_IFTRACESAY("bool Iterator<T>::operator<=(const Iterator<T>&)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    RA_CHECKORSAY(other.pointer_ != RA_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ <= other.pointer_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_ 
bool ra::Iterator<T>::operator>=(const Iterator<T>& other) const
{
    // Iterator comparison: greater than or equal to
    RA_IFTRACESAY("bool Iterator<T>::operator>=(const Iterator<T>&)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    RA_CHECKORSAY(other.pointer_ != RA_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ >= other.pointer_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> RA_INLINE_ 
ra::Iterator<T>::Iterator(T* ptr, int size)
  : pointer_(ptr),
    pointer_min_(ptr),
    pointer_max_plus_one_(ptr+size)
{
    RA_IFTRACESAY("Iterator<T>::Iterator(T*,int)");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and size >=0, "invalid construction of iterator");
    RA_CHECKORSAY(pointer_ != RA_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename A> RA_INLINE_ 
int ra::extent_given_byte_size(A a[], int i, int byte_size) 
{
    // find shape of automatic arrays, given the size of the array
    RA_IFTRACESAY("int extent_given_byte_size(A[],int,int)");
    RA_CHECKORSAY(i>=0 and i<1, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A);
        default: return 1;
    }
}

template<typename A,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<2, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/Z;
        case 1:  return Z;
        default: return 1;
    }
}

template<typename A,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<3, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/Z/Y;
        case 1:  return Y;
        case 2:  return Z;
        default: return 1;
    }
}

template<typename A,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<4, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/X/Z/Y;
        case 1:  return X;
        case 2:  return Y;
        case 3:  return Z;
        default: return 1;
    }
}

template<typename A,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<5, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/W/X/Z/Y;
        case 1:  return W;
        case 2:  return X;
        case 3:  return Y;
        case 4:  return Z;
        default: return 1;
    }
}

template<typename A,int V,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][V][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][V][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<6, "wrong dimension");
    switch (i) {
       case 0:  return byte_size/sizeof(A)/V/W/X/Z/Y;
       case 1:  return V;
       case 2:  return W;
       case 3:  return X;
       case 4:  return Y;
       case 5:  return Z;
       default: return 1;
    }
}

template<typename A,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][U][V][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][U][V][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<7, "wrong dimension");
    switch (i) {
       case 0:  return byte_size/sizeof(A)/U/V/W/X/Z/Y;
       case 1:  return U;
       case 2:  return V;
       case 3:  return W;
       case 4:  return X;
       case 5:  return Y;
       case 6:  return Z;
       default: return 1;
    }
}

template<typename A,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][T][U][V][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<8, "wrong dimension");
    switch (i) {
       case 0:  return byte_size/sizeof(A)/T/U/V/W/X/Z/Y;
       case 1:  return T;
       case 2:  return U;
       case 3:  return V;
       case 4:  return W;
       case 5:  return X;
       case 6:  return Y;
       case 7:  return Z;
       default: return 1;
    }
}

template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][S][T][U][V][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<9, "wrong dimension");
    switch (i) {
       case 0:  return byte_size/sizeof(A)/S/T/U/V/W/X/Z/Y;
       case 1:  return S;
       case 2:  return T;
       case 3:  return U;
       case 4:  return V;
       case 5:  return W;
       case 6:  return X;
       case 7:  return Y;
       case 8:  return Z;
       default: return 1;
    }
}

template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][R][S][T][U][V][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<10, "wrong dimension");
    switch (i) {
       case 0:  return byte_size/sizeof(A)/R/S/T/U/V/W/X/Z/Y;
       case 1:  return R;
       case 2:  return S;
       case 3:  return T;
       case 4:  return U;
       case 5:  return V;
       case 6:  return W;
       case 7:  return X;
       case 8:  return Y;
       case 9:  return Z;
       default: return 1;
    }
}

template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(A[][Q][R][S][T][U][V][W][X][Y][Z],int,int)");
    RA_CHECKORSAY(i>=0 and i<11, "wrong dimension");
    switch (i) {
       case 0:   return byte_size/sizeof(A)/Q/R/S/T/U/V/W/X/Z/Y;
       case 1:   return Q;
       case 2:   return R;
       case 3:   return S;
       case 4:   return T;
       case 5:   return U;
       case 6:   return V;
       case 7:   return W;
       case 8:   return X;
       case 9:   return Y;
       case 10:  return Z;
       default: return 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename A,int R> RA_INLINE_ 
int ra::extent_given_byte_size(const ra::rarray<A,R>& a, int i, int byte_size) 
{
    RA_IFTRACESAY("int extent_given_byte_size(const rarray<A,R>&,int,int)");
    return a.extent(i);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// convert to rarray

template<typename A> RA_INLINE_ 
ra::rarray<A,1> ra::make_rarray_given_byte_size(A a[], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,1> make_rarray_given_byte_size(A[],int)");
    const int z = byte_size/sizeof(A);
    return RA_MOVE(ra::rarray<A RA_COMMA 1>(a,z));
}

template<typename A,int Z> RA_INLINE_ 
ra::rarray<A,2> ra::make_rarray_given_byte_size(A a[][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,2> make_rarray_given_byte_size(A[][Z],int)");
    const int y = byte_size/sizeof(A)/Z;
    return RA_MOVE(ra::rarray<A RA_COMMA 2>(*a,y,Z));
}

template<typename A,int Y,int Z> RA_INLINE_ 
ra::rarray<A,3> ra::make_rarray_given_byte_size(A a[][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,3> make_rarray_given_byte_size(A[][Y][Z],int)");
    const int x = byte_size/sizeof(A)/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 3>(**a,x,Y,Z));
}

template<typename A,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,4> ra::make_rarray_given_byte_size(A a[][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,4> make_rarray_given_byte_size(A[][X][Y][Z],int)");
    const int w = byte_size/sizeof(A)/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 4>(***a,w,X,Y,Z));
}

template<typename A,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,5> ra::make_rarray_given_byte_size(A a[][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,5> make_rarray_given_byte_size(A[][W][X][Y][Z],int)");
    const int v = byte_size/sizeof(A)/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 5>(****a,v,W,X,Y,Z));
}

template<typename A,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,6> ra::make_rarray_given_byte_size(A a[][V][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,6> make_rarray_given_byte_size(A[][V][W][X][Y][Z],int)");
    const int u = byte_size/sizeof(A)/V/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 6>(*****a,u,V,W,X,Y,Z));
}

template<typename A,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,7> ra::make_rarray_given_byte_size(A a[][U][V][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,7> make_rarray_given_byte_size(A[][U][V][W][X][Y][Z],int)");
    const int t = byte_size/sizeof(A)/U/V/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 7>(******a,t,U,V,W,X,Y,Z));
}

template<typename A,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,8> ra::make_rarray_given_byte_size(A a[][T][U][V][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,8> make_rarray_given_byte_size(A[][T][U][V][W][X][Y][Z],int)");
    const int s = byte_size/sizeof(A)/T/U/V/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 8>(*******a,s,T,U,V,W,X,Y,Z));
}

template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,9> ra::make_rarray_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,9> make_rarray_given_byte_size(A[][Q][R][S][T][U][V][W][X][Y][Z],int)");
    const int r = byte_size/sizeof(A)/S/T/U/V/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 9>(********a,r,S,T,U,V,W,X,Y,Z));
}

template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,10> ra::make_rarray_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,10> make_rarray_given_byte_size(A[][R][S][T][U][V][W][X][Y][Z],int)");
    const int q = byte_size/sizeof(A)/R/S/T/U/V/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 10>(*********a,q,R,S,T,U,V,W,X,Y,Z));
}

template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,11> ra::make_rarray_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    RA_IFTRACESAY("rarray<A,11> make_rarray_given_byte_size(A[][Q][R][S][T][U][V][W][X][Y][Z],int)");
    const int p = byte_size/sizeof(A)/Q/R/S/T/U/V/W/X/Z/Y;
    return RA_MOVE(ra::rarray<A RA_COMMA 11>(**********a,p,Q,R,S,T,U,V,W,X,Y,Z));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R> ra::make_rarray_given_byte_size(ra::rarray<T,R> a, int byte_size) 
{
    RA_IFTRACESAY("rarray<T,R> make_rarray_given_byte_size(rarray<T,R>,int)");
    return RA_MOVE(a); // move shouldn't be necessary as a is a copy already ?
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RA_DUPLICATE_BODY(
template<typename T RA_COMMA int R> RA_INLINE_ std::ostream& operator<<(std::ostream &o, const ra::rarray<T RA_COMMA R>& r),
template<typename T RA_COMMA int R> RA_INLINE_ std::ostream& operator<<(std::ostream &o, const ra::subrarray<T RA_COMMA R>& r),
{
    RA_IFTRACESAY("std::ostream& operator<<(std::ostream&,const rarray<T,R>&)");
    return ra::text_output(o,r);
})

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_ 
std::ostream& ra::text_output(std::ostream &o, const ra::rarray<T,R>& r)
{
    RA_IFTRACESAY("std::ostream& operator<<(std::ostream&,const rarray<T,R>&)");
    if (not r.is_clear()) {
        o << '{';
        for (int i=0; i<r.extent(0); i++)  {
            if (i>0)
                o << ',';
            #ifndef RA_SKIPINTERMEDIATE
            o << r[i];
            #else
            T* subdata = RA_NULLPTR;
            if (r[i] != RA_NULLPTR) {
                const char* result1 = reinterpret_cast<const char*>(r[i]);
                char* result2 = const_cast<char*>(result1);
                char** result = reinterpret_cast<char**>(result2);
                for (int i=0; i < R-2; i++)
                    result = reinterpret_cast<char**>(*result);
                subdata = reinterpret_cast<T*>(result);
            }
            o << ra::rarray<T,R-1>(subdata, r.shape()+1);
            #endif
        }
        o << '}';
    } else {
        for (int i=0; i<R; i++) 
            o << '{';
        for (int i=0; i<R; i++) 
            o << '}';
    }
    return o;
}

template<typename T> RA_INLINE_ 
std::ostream& ra::text_output(std::ostream &o, const ra::rarray<T,1>& r)
{
    RA_IFTRACESAY("std::ostream& operator<<(std::ostream&,const rarray<T,1>&)");
    if (not r.is_clear()) {
        o << '{';
        for (int i=0; i<r.extent(0); i++) {
            if (i) o << ',';
            std::stringstream strstr;
            std::string result;
            strstr << r[i];
            result = strstr.str();
            if (result.find_first_of("{,}#") != std::string::npos)
                o << '#' << result.size() << ':';
            o << result;
        }
        o << '}';
    } else 
        o << "{}";
    return o;
}

template<typename T,int R> RA_INLINE_
std::ostream& ra::text_output(std::ostream &o, const ra::subrarray<T,R>& r)
{
    RA_IFTRACESAY("std::ostream& operator<<(std::ostream&,const subrarray<T,R>&)");
    o << '{' << r[0];
    for (int i=1; i<r.extent(0); i++) 
        o << ',' << r[i];
    o << '}';
    return o;
}

template<typename T> RA_INLINE_
std::ostream& ra::text_output(std::ostream &o, const ra::subrarray<T,1>& r)
{
    RA_IFTRACESAY("std::ostream& operator<<(std::ostream&,const subrarray<T,1>&)");
    o << '{';
    for (int i=0; i<r.extent(0); i++) {
        if (i) o << ',';
        std::stringstream strstr;
        std::string result;
        strstr << r[i];
        result = strstr.str();
        if (result.find_first_of("{,}#") != std::string::npos)
            o << '#' << result.size() << ':';
        o << result;
    }
    o << '}';
    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// helper routines to convert a string to any data type

template<typename T> RA_INLINE_
void ra::StringToValue<T>::get(const std::string& input, T& output) 
{
    RA_IFTRACESAY("void StringToValue::get(const std::string&,T&)");
    std::stringstream str(input); // use streaming operators by default
    str >> output; // won't work with strings as they get truncated at first whitespace
}

RA_INLINE_ void ra::StringToValue<std::string>::get(const std::string& input, std::string& output)
{
    RA_IFTRACESAY("void StringToValue::get(const std::string&,std::string&)");
    output = input;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINEF 
T& ra::Deref<T,R>::access(typename PointerArray<T,R>::type p, const int* indices) 
{
    RA_IFTRACESAY("Deref<T,R>::access(PointerArray<T,R>::type,const int*)");
    return Deref<T,R-1>::access(p[indices[0]-1], indices+1);
}

template<typename T> RA_INLINEF 
T& ra::Deref<T,1>::access(typename PointerArray<T,1>::type p, const int* indices) 
{
    RA_IFTRACESAY("Deref<T,1>::access(PointerArray<T,1>::type,const int*)");
    return p[indices[0]-1];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_
void ra::read_and_parse_shape(std::istream &                             in, 
                                    int*                                       shape, 
                                    typename ra::PointerArray<T,R>::type p)
{
    RA_IFTRACESAY("void read_and_parse_shape(std::istream&,int*,PointerArray<T,R>::type)");
    size_t init_file_ptr = in.tellg();
    try {
        int current_shape[R];
        for (int i=0; i<R; i++) {
            current_shape[i] = 1;
            if (shape)
                shape[i] = 0;
            if (in.get() != '{') 
                throw std::istream::failure("Format error");
        }
        int current_depth = R-1; // start scanning the deepest level
        while (current_depth>=0) {
            if (current_depth==R-1) {
                char         lastchar;
                std::string  word = "";
                do {
                    lastchar = in.get();
                    if (lastchar != ',' and lastchar != '}')
                        word += lastchar;
                    if (word == "#") {
                        word="";
                        std::string skipstr;
                        do {
                            skipstr += (lastchar = in.get());
                        } while (lastchar!=':');
                        int skip = atoi(skipstr.c_str());
                        for (int i=0; i<skip; i++) 
                            word += in.get();
                        lastchar = in.get();
                    }
                    if (lastchar == ',') {
                        if (p) 
                            StringToValue<T>::get(word,
                                                  Deref<T,R>::access(p, current_shape));
                        word="";
                        current_shape[current_depth]++;
                    }
                } while (lastchar != '}');
                if (p) 
                    StringToValue<T>::get(word,
                                          Deref<T,R>::access(p, current_shape));
                if (shape)
                    if (shape[current_depth] < current_shape[current_depth])
                        shape[current_depth] = current_shape[current_depth];
                current_depth--;
            } else {
                switch (in.get()) {
                  case ',':
                    current_shape[current_depth]++;
                    break;
                  case '{':
                    current_depth++;
                    current_shape[current_depth] = 1;
                    break;
                  case '}':
                      if (shape)
                          if (shape[current_depth] < current_shape[current_depth])
                              shape[current_depth] = current_shape[current_depth];
                    current_depth--;
                    break;
                  default:
                    throw std::istream::failure("Format error");
                }
            }
        }    
    }
    catch (std::istream::failure& e) {
        in.seekg(init_file_ptr, in.beg);// upon failure, undo characters read in
        throw e;                        // and pass on the error
    }
    if (p==0)
        in.seekg(init_file_ptr, in.beg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_
std::istream& operator>>(std::istream &in, ra::rarray<T,R>& r)
{
    RA_IFTRACESAY("std::istream& operator>>(std::istream&,rarray<T,R>&)");
    int extent[R] = {0};
    size_t init_file_ptr = in.tellg();
    try {
        // skip initial white space
	if (in.flags() && std::ios::skipws) {
            char lastchar;
            do {
                lastchar = in.get();
            } while (lastchar==' ' or lastchar== '\t' or lastchar=='\n');
            in.putback(lastchar);
        }
        // read the shape
        ra::read_and_parse_shape<T,R>(in, extent, 0);
        // allocate array
        r = RA_MOVE(ra::rarray<T RA_COMMA R>(extent));
        // fill array
        ra::read_and_parse_shape<T,R>(in, 0, r.ptr_array());
        return in;
    }
    catch (std::istream::failure& e) {
        in.seekg(init_file_ptr, in.beg);// upon failure, undo characters read in
        throw e;                        // and pass on the error
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Get rid of the macros
#undef RA_IFTRACESAY
#undef RA_CHECKORSAY
#undef RA_DUPLICATE_BODY
#undef RA_QUADRUPLICATE_BODY
#undef RA_SEXTUPLICATE_BODY
#undef RA_OCTUPLICATE_BODY
#undef RA_COMMA
#undef RA_INLINE
#undef RA_INLINEF
#undef RA_INLINE_
// Global namespace stuff
// (also in global namespace: operator<< and operator>> for rarray and subrarray)

#define EXTENT(A,I)  ra::extent_given_byte_size(A,I,sizeof(A))
#define RARRAY(A)    ra::make_rarray_given_byte_size(A,sizeof(A))
#define INDEX(A,X,I) RARRAY(A).index(X,I)
#define as_rarray(A) ra::make_rarray_given_byte_size(A,sizeof(A)) 

// for now:
using ra::rarray; 

#endif
