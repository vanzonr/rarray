//
// rarray.h - Runtime arrays: template classes for pointer-based,
//            runtime dimensionalized, multi-dimensional
//            arrays.  Documentation in docrarray.pdf
//
// Copyright (c) 2013-2014  Ramses van Zon
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

// When running rarraytestsuite.cc compiled with -DTRACETEST, the
// following macro produced output to be used to determine which
// functions are exercised.
#ifdef AR_TRACETEST
//#define AR_PROFILESAY(a) std::cerr << "PROFILE " << __FILE__ << '@' << __LINE__<< ":\t" << a << std::endl;
#define AR_PROFILESAY(a) std::cerr << "PROFILE " << __FILE__ << '@' << __LINE__<< ":\t" << a << std::endl;
#else
#define AR_PROFILESAY(a) 
#endif

// Compiling with -DBOUNDSCHECK switches on the checkOrSay macro to
// check its first argument and throw an exception if it is not true.
// checkOrSay is intended to be used for bound checks.
#ifdef AR_BOUNDSCHECK
#define AR_CHECKORSAY(a, b) if (not(a)) throw std::out_of_range(b)
// BOUNDCHECK is incompatible with SKIPINTERMEDIATE (see below)
#ifdef AR_SKIPINTERMEDIATE
#undef AR_SKIPINTERMEDIATE
#endif
#else
#define AR_CHECKORSAY(a, b) 
#endif

#if __cplusplus <= 199711L
#define AR_NULLPTR 0 
#else
#define AR_NULLPTR nullptr
#endif

// Forward definitions of ra::rarray<T,R> and radetail::subarray<T,R>
namespace ra       { template<typename T,int R> class rarray;   }
namespace radetail { template<typename T,int R> class subarray; }

// Define internal types needed by class rarray, in a separate namespace

namespace radetail
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

// Iterator<T> acts like T* but allows (optional) bounds checking switched on by AR_BOUNDSCHECK
template<typename T>
class Iterator {
  public:
    T&        operator*   ();                                          // dereference
    Iterator& operator++  ();                                          // pre-incrememt
    Iterator  operator++ (int);                                        // post-increment
    bool      operator== (const Iterator<T>& other);                   // comparison: equal to
    bool      operator!= (const Iterator<T>& other);                   // comparison: not equal to
    bool      operator<  (const Iterator<T>& other);                   // comparison: less than
    bool      operator>  (const Iterator<T>& other);                   // comparison: greater than
    bool      operator<= (const Iterator<T>& other);                   // comparison: less than or equal to
    bool      operator>= (const Iterator<T>& other);                   // comparison: greater than or equal to
  private:
    T*  pointer_;
    T*  pointer_min_;
    T*  pointer_max_plus_one_;
    Iterator(T* ptr, int size);
    template<typename S,int R> friend class ra::rarray;
    template<typename S,int R> friend class radetail::subarray;
};


}  // end namespace radetail (to be continued)


//------------------------------------------------//
//                                                //
//               I N T E R F A C E                //
//                                                //
//------------------------------------------------//


// class rarray<T,R> is the only thing in namespace ra

namespace ra {

template<typename T,int R> 
class rarray {

  public: 
    enum { rank = R };
    
    typedef int                                                difference_type;  // difference type for indices
    typedef int                                                size_type;        // type of indices
    typedef radetail::Iterator<T>                              iterator;         // iterator type
    typedef radetail::Iterator<const T>                        const_iterator;   // iterator type for constant access
    typedef typename radetail::PointerArray<T,R>::type         parray_t;         // shorthand for T*const*const*...
    typedef typename radetail::PointerArray<T,R>::noconst_type noconst_parray_t; // shorthand for T***...
    typedef rarray<T,R>                                        self_t;           // useful in some generic programmong

    rarray();                                                          // constructor leaving rarray undefined 
    rarray(int n0, int n1);                                            // constructor creating its own buffer for R=2
    rarray(int n0, int n1, int n2);                                                                            // R=3
    rarray(int n0, int n1, int n2, int n3);                                                                    // R=4
    rarray(int n0, int n1, int n2, int n3, int n4);                                                            // R=5
    rarray(int n0, int n1, int n2, int n3, int n4, int n5);                                                    // R=6
    rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6);                                            // R=7
    rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7);                                    // R=8
    rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);                            // R=9 
    rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);                    // R=10
    rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);           // R=11
    rarray(const int* extent);                                                                                 // R>11
    rarray(T* buffer, int n0, int n1);                                 // constructor from an existing buffer for R=2
    rarray(T* buffer, int n0, int n1, int n2);                                                                 // R=3
    rarray(T* buffer, int n0, int n1, int n2, int n3);                                                         // R=4
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4);                                                 // R=5
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5);                                         // R=6
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6);                                 // R=7 
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7);                         // R=8
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);                 // R=9
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);         // R=10
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);// R=11
    rarray(T* buffer, const int* extent);                                                                      // R>11
    rarray(const rarray<T,R> &a);                                      // copy constructor
    rarray(const radetail::subarray<T,R> &a);                          // copy constructor
    rarray<T,R>& operator=(const rarray<T,R> &a);                      // assignment operator
    rarray<T,R>& operator=(const radetail::subarray<T,R> &a);          // assignment operator
    ~rarray();                                                         // destructor
    void clear();                                                      // clean up routine, make undefined
    void reshape(int n0, int n1);                                      // reshape keeping the underlying data for R=2
    void reshape(int n0, int n1, int n2);                                                                      // R=3
    void reshape(int n0, int n1, int n2, int n3);                                                              // R=4
    void reshape(int n0, int n1, int n2, int n3, int n4);                                                      // R=5
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5);                                              // R=6
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6);                                      // R=7
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7);                              // R=8
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);                      // R=9
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);              // R=10
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10);     // R=11
    void reshape(const int* extent);                                                                           // R>11

    bool                is_clear()           const;                    // check if undefined
    rarray<T,R>         copy()               const;                    // return a copy
    int                 extent(int i)        const;                    // retrieve array size in dimension i
    const int*          shape()              const;                    // retrieve array sizes in all dimensions
    int                 size()               const;                    // retrieve the total number of elements  
    T*                  data();                                        // return a T* to the internal data
    const T*            data()               const;                    // return a T* to the internal data
    parray_t            ptr_array()          const;                    // return a T*const*.. acting similarly to this rarray when using []:
    noconst_parray_t    noconst_ptr_array()  const;                    // return a T**.. acting similarly to this rarray when using []:    
    rarray<const T,R>&  const_ref()          const;                    // create a reference to this that treats elements as constant:
    void                fill(const T& value);                          // fill with uniform value

    iterator            begin();                                       // start of the content
    const_iterator      begin()              const;                    // start of the content, when *this is constant
    const_iterator      cbegin()             const;                    // start of the content, when *this can be constant and you need to be explicit
    iterator            end();                                         // end of the content
    const_iterator      end()                const;                    // end of the content, when *this is constant
    const_iterator      cend()               const;                    // end of the content, when *this is constant and you need to be explicit about that
    int*                index(const T& a, int* index) const;           // if a an element in the array, get the indices of that element
    int*                index(const iterator& i, int* index);          // if i points at an element in the array, get the indices of that element
    int*                index(const const_iterator& i, int* ind) const;// if i points at an element in the array, get the indices of that element

    // access elements 
   #ifndef AR_SKIPINTERMEDIATE
    // through intermediate object:
    radetail::subarray<T,R-1> operator[](int i);
    radetail::subarray<const T,R-1> operator[](int i) const;
   #else
    // or if AR_SKIPINTERMEDIATE is set, by implicit case to a T**..:
    operator typename radetail::PointerArray<T,R>::type (); 
    operator typename radetail::PointerArray<const T,R>::type () const; 
   #endif

  private:
    parray_t  parray_;                                                 // start of the array of pointers
    int*      extent_;                                                 // array of number of elements in each dimension
    bool      ismine_;                                                 // does the container own the data buffer?
    bool      entire_;                                                 // true if not a subarray
    int*      rcount_;                                                 // reference count for the array if not a subarray

    T*   get_buffer() const;                                           // get start of current contiguous buffer
    void init_shallow(parray_t parray, const int* extent, bool entire, int* rcount); // setup new rarray object
    void init_shallow(parray_t parray, const int* extent);             // setup new rarray object
    void init_parray(T* buffer, const int* extent);                    // setup new rarray object
    void init_data(const int* extent, int extenttot);                  // setup new rarray object
    static parray_t new_except_base(T* buffer, const int* extent);     // allocate the chain of pointers, except the base
    static T*       base(parray_t parray);                             // find base of a chain of pointers:

    friend class radetail::subarray<T,R>;

}; // end definition rarray<T,R>

// Define rarray<T,1> to stop the recursion in operator[]:

template<typename T> 
class rarray<T,1> {

  public:
    enum { rank = 1 };

    typedef int                                                difference_type;  // difference type for indices
    typedef int                                                size_type;        // type of indices
    typedef radetail::Iterator<T>                              iterator;         // iterator type
    typedef radetail::Iterator<const T>                        const_iterator;   // iterator type for constant access
    typedef typename radetail::PointerArray<T,1>::type         parray_t;         // conforming shorthand for T*
    typedef typename radetail::PointerArray<T,1>::noconst_type noconst_parray_t; // conforming shorthand for T*
    typedef rarray<T,1>                                        self_t;           // useful in some generic programmong

    rarray();                                                          // constructor leaving object undefined
    rarray(int n0);                                                    // constructor creating own buffer
    rarray(const int* extent);                                         // constructor creating own buffer from pointer, for conformity
    rarray(T* buffer, int n0);                                         // constructor from an existing buffer:
    rarray(T* buffer, const int* extent);                              // constructor from an existing buffer for conformity    
    rarray(const rarray<T,1> &a);                                      // copy constructor
    rarray(const radetail::subarray<T,1> &a);                          // copy constructor    
    rarray<T,1>& operator=(const rarray<T,1> &a);                      // assignment operator
    rarray<T,1>& operator=(const radetail::subarray<T,1> &a);          // assignment operator
    ~rarray();                                                         // destructor
    void clear();                                                      // clean up routine, make undefined again
    void reshape(int n0);                                              // to change shape (only shrinking is defined)
    void reshape(const int* extent);                                   // for conformity

    bool                is_clear()           const;                    // check if undefined
    rarray<T,1>         copy()               const;                    // return a copy
    int                 extent(int i)        const;                    // retrieve array size in dimension i
    const int*          shape()              const;                    // retrieve array sizes in all dimensions
    int                 size()               const;                    // retrieve the total number of elements
    T*                  data();                                        // return T* to the internal data
    const T*            data()               const;                    // return T* to the internal data
    parray_t            ptr_array()          const;                    // return T*const*... acting similarly to this rarray when using []
    noconst_parray_t    noconst_ptr_array()  const;                    // return  T**... acting similarly to this rarray when using []
    rarray<const T,1>&  const_ref()          const;                    // create reference to this that treats elements as constant
    void                fill(const T& value);                          // fill with uniform value

    iterator            begin();                                       // start of the content
    const_iterator      begin()              const;                    // start of the content, when *this is constant
    const_iterator      cbegin()             const;                    // start of the content, when *this is constant and you need to be explicit
    iterator            end();                                         // end of the content
    const_iterator      end()                const;                    // end of the content, when *this is constant
    const_iterator      cend()               const;                    // end of the content, when *this is constant, and you need to be explicit about it
    int*                index(const T& a, int* index) const;           // if a an element in the array, get the indices of that element
    int*                index(const iterator& i, int* index);          // if i points at an element in the array, get the indices of that element
    int*                index(const const_iterator& i, int* ind) const;// if i points at an element in the array, get the indices of that element

    // accesselements through intermediate object:
   #ifndef AR_SKIPINTERMEDIATE
    // through a T& pointer a the element:
    T& operator[](int i);
    const T& operator[](int i) const;
   #else
    // through implicit conversion to a pointer:
    operator typename radetail::PointerArray<T,1>::type ();
    operator typename radetail::PointerArray<const T,1>::type () const;
   #endif

  private:
    parray_t  parray_;                                                 // start of the array of pointers
    int*      extent_;                                                 // array of number of elements in each dimension
    bool      ismine_;                                                 // does the container own the data buffer?
    bool      entire_;                                                 // true if not a subarray
    int*      rcount_;                                                 // reference count for the array if not a subarray

    T*   get_buffer() const;                                           // get start of current contiguous buffer  
    void init_shallow(parray_t parray, const int* extent, bool entire, int* rcount); // setup new rarray object
    void init_shallow(parray_t parray, const int* extent);             // setup new rarray object
    void init_parray(T* buffer, const int* extent);                    // setup new rarray object
    void init_data(const int* extent, int extenttot);                  // setup new rarray object
    static parray_t new_except_base(T* buffer, const int* extent);     // allocate the chain of pointers, except the base
    static T* base(parray_t parray) ;                                  // find base of a chain of pointers

    friend class radetail::subarray<T,1>;

}; // end definition rarray<T,1>

} // end namespace ra

// Input/output streaming operators in global namespace
template<typename T,int R>  std::istream& operator>>(std::istream &i, ra::rarray<T,R>& r);
template<typename T,int R>  std::ostream& operator<<(std::ostream &o, const ra::rarray<T,R>& r);
template<typename T,int R>  std::ostream& operator<<(std::ostream &o, const radetail::subarray<T,R>& r);

namespace radetail { // continuing namespace radetail

// Definition class subarray <T,R>

// subarray class: interface like rarray without assignment. Really
// only a reference to one, used in intermediate expressions produced
// by square brackets.

template<typename T,int R> 
class subarray {

  public:
    enum { rank = R };

    typedef int                                      difference_type;  // difference type for indices
    typedef int                                      size_type;        // type of indices
    typedef radetail::Iterator<T>                    iterator;         // iterator type
    typedef radetail::Iterator<const T>              const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,R>::type         parray_t;         // shorthand for T*const*const*...
    typedef typename PointerArray<T,R>::noconst_type noconst_parray_t; // shorthand for T***...
    
    int                  extent(int i)        const;                   // retrieve array size in dimension i
    const int*           shape()              const;                   // retrieve array sizes in all dimensions    
    int                  size()               const;                   // retrieve the total number of elements
    T*                   data()               const;                   // return T* to the internal pointer to the data
    parray_t             ptr_array()          const;                   // return T*const*.. acting similarly when using []
    noconst_parray_t     noconst_ptr_array()  const;                   // return T**.. acting similarly to this rarray when using []
    subarray<const T,R>& const_ref()          const;                   // create a reference to this treating elements as constant
    void                 fill(const T& value);                         // fill with uniform value
    iterator             begin()              const;                   // start of the *content*
    iterator             end()                const;                   // end of the *content*
    const_iterator       cbegin()             const;                   // start of the *content* (const version)
    const_iterator       cend()               const;                   // end of the *content* (const version)
    int*                 index(const T& a, int* index) const;          // if a an element in the array, get the indices of that element
    int*                 index(const iterator& i, int* index);         // if i points at an element in the array, get the indices of that element
    int*                 index(const const_iterator& i, int* ind)const;// if i points at an element in the array, get the indices of that element
    subarray<T,R-1>      operator[](int i)    const;                   // element access

  protected:
    parray_t   const  parray_;                                         // start of the pointer array
    const int* const  extent_;                                         // number of elements in each dimension

    subarray(parray_t parray, const int* extent);                      // constructor, called by
    friend class ra::rarray<T,R>;                                      // constructor can only
    friend class ra::rarray<T,R+1>;                                    // be called by these
    friend class subarray<T,R+1>;                                      // classes.
    friend class ra::rarray<typename Unconst<T>::type,R+1>;

}; // end definition subarray<T,R>

// Define subarray<T,1> to stop the recursion in operator[]:

template<typename T> class subarray<T,1> {

  public:
    enum { rank = 1 };

    typedef int                                      difference_type;  // difference type for indices
    typedef int                                      size_type;        // type of indices
    typedef radetail::Iterator<T>                    iterator;         // iterator type
    typedef radetail::Iterator<const T>              const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,1>::type         parray_t;         // conforming shorthand for T*
    typedef typename PointerArray<T,1>::noconst_type noconst_parray_t; // conforming shorthand for T*

    int                  extent(int i)        const;                   // retrieve array size in dimension i
    const int*           shape()              const;                   // retrieve array sizes in all dimensions
    int                  size()               const;                   // retrieve the total number of elements
    T*                   data()               const;                   // return T* to the internal pointer to the data
    parray_t             ptr_array()          const;                   // return T*const*.. acting similarly to this rarray when using []
    noconst_parray_t     noconst_ptr_array()  const;                   // return T**.. acting similarly to this rarray when using []
    subarray<const T,1>& const_ref()          const;                   // create a reference to this that treats elements as constant
    void                 fill(const T& value);                         // fill with uniform value
    iterator             begin()              const;                   // start of the *content*
    iterator             end()                const;                   // end of the *content*
    const_iterator       cbegin()             const;                   // start of the *content* (const version)
    const_iterator       cend()               const;                   // end of the *content* (const version)
    int*                 index(const T& a, int* index) const;          // if a an element in the array, get the indices of that element
    int*                 index(const iterator& i, int* index);         // if i points at an element in the array, get the indices of that element
    int*                 index(const const_iterator& i, int* ind)const;// if i points at an element in the array, get the indices of that element
    T&                   operator[](int i)    const;                   // element access

  protected:
    parray_t   const   parray_;                                        // start of the pointer array
    const int* const   extent_;                                        // number of elements in each dimension

    subarray(parray_t parray, const int* extent);                      // constructor    
    friend class ra::rarray<T,1>; 
    friend class ra::rarray<T,2>;
    friend class subarray<T,2>;
    friend class ra::rarray<typename Unconst<T>::type,2>;

};  // end of class template definition of subarray<T,1>.

template<typename T,int R>  std::ostream& text_output(std::ostream &o, const ra::rarray<T,R>& r);
template<typename T>        std::ostream& text_output(std::ostream &o, const ra::rarray<T,1>& r);
template<typename T,int R>  std::ostream& text_output(std::ostream &o, const subarray<T,R>& r);
template<typename T>        std::ostream& text_output(std::ostream &o, const subarray<T,1>& r);


// We need to be able to get a reference in a pointer-to-pointer structure given indices.
//
// Deref<T,1>(T*   p, int* i) where i->{n1}        =  p[n1]
// Deref<T,2>(T**  p, int* i) where i->{n1,n2}     =  p[n1][n2]
// Deref<T,3>(T*** p, int* i) where i->{n1,n2,n3}  =  p[n1][n2][n3]
//...
template<typename T, int R>
struct Deref {
    static T& access(typename PointerArray<T,R>::type p, const int* indices);
};
template<typename T>
struct Deref<T,1>  // R=1 is special
{
    static T& access(typename PointerArray<T,1>::type p, const int* indices);
};

// Convert a string to a value, needed for operator>> .
template<typename T>
struct StringToValue {
    static void get(const std::string& input, T& output);
};
template<>
struct StringToValue<std::string> {
    static void get(const std::string& input, std::string& output);
};

// Function prototype of helper routine used by operator>>:
template<typename T, int R> 
void read_and_parse_shape(std::istream & in, int* shape, typename PointerArray<T,R>::type p = 0);

// Template functions to detemine the dimensions of automatic arrays, for use in the EXTENT macro
// To be able to determine the first dimension, these need to get pass the total size in bytes (byte_size) of such an automatic array
template<typename A>                                                              int extent_given_byte_size(A a[], int i, int byte_size); 
template<typename A,int Z>                                                        int extent_given_byte_size(A a[][Z], int i, int byte_size);
template<typename A,int Y,int Z>                                                  int extent_given_byte_size(A a[][Y][Z], int i, int byte_size);
template<typename A,int X,int Y,int Z>                                            int extent_given_byte_size(A a[][X][Y][Z], int i, int byte_size);
template<typename A,int W,int X,int Y,int Z>                                      int extent_given_byte_size(A a[][W][X][Y][Z], int i, int byte_size);
template<typename A,int V,int W,int X,int Y,int Z>                                int extent_given_byte_size(A a[][V][W][X][Y][Z], int i, int byte_size);
template<typename A,int U,int V,int W,int X,int Y,int Z>                          int extent_given_byte_size(A a[][U][V][W][X][Y][Z], int i, int byte_size);
template<typename A,int T,int U,int V,int W,int X,int Y,int Z>                    int extent_given_byte_size(A a[][T][U][V][W][X][Y][Z], int i, int byte_size);
template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z>              int extent_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int i, int byte_size);
template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>        int extent_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int i, int byte_size);
template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>  int extent_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int i, int byte_size);
template<typename A,int R>                                                        int extent_given_byte_size(const ra::rarray<A,R>& a, int i, int byte_size); // use rarray's extent function
// Template functions to convert automatic arrays, for conversion with RARRAY macro
// To be able to determine the first dimension, these need to get pass the total size in bytes (byte_size) of such an automatic array
template<typename A>                                                              ra::rarray<A,1>  make_rarray_given_byte_size(A a[], int byte_size); 
template<typename A,int Z>                                                        ra::rarray<A,2>  make_rarray_given_byte_size(A a[][Z], int byte_size); 
template<typename A,int Y,int Z>                                                  ra::rarray<A,3>  make_rarray_given_byte_size(A a[][Y][Z], int byte_size);
template<typename A,int X,int Y,int Z>                                            ra::rarray<A,4>  make_rarray_given_byte_size(A a[][X][Y][Z], int byte_size);
template<typename A,int W,int X,int Y,int Z>                                      ra::rarray<A,5>  make_rarray_given_byte_size(A a[][W][X][Y][Z], int byte_size);
template<typename A,int V,int W,int X,int Y,int Z>                                ra::rarray<A,6>  make_rarray_given_byte_size(A a[][V][W][X][Y][Z], int byte_size);
template<typename A,int U,int V,int W,int X,int Y,int Z>                          ra::rarray<A,7>  make_rarray_given_byte_size(A a[][U][V][W][X][Y][Z], int byte_size);
template<typename A,int T,int U,int V,int W,int X,int Y,int Z>                    ra::rarray<A,8>  make_rarray_given_byte_size(A a[][T][U][V][W][X][Y][Z], int byte_size);
template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z>              ra::rarray<A,9>  make_rarray_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int byte_size);
template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>        ra::rarray<A,10> make_rarray_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int byte_size);
template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>  ra::rarray<A,11> make_rarray_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int byte_size);
template<typename A,int R>                                                        ra::rarray<A,R> make_rarray_given_byte_size(ra::rarray<A,R> a, int byte_size); // trivial action for rarray

} // end namespace radetail


//------------------------------------------------//
//                                                //
//          I M P L E M E N T A T I O N           //
//                                                //
//------------------------------------------------//


//----------------------------------------------------------------------//

// Helper macros to define methods with the same body without typing
// it twice, the R=1 case being specialized this is necessary to avoid
// a lot of duplication (or even quadruplication) of code. 
// Note that R=1 has to be specialized so brackets work well.  in the
// 'body', non-enclosed commas need to be replaced by COMMA for his to
// work.

#define AR_COMMA ,
#define AR_DUPLICATE_BODY(header1,header2,body)                         \
    header1 body                                                        \
    header2 body
#define AR_QUADRUPLICATE_BODY(header1,header2,header3,header4,body)     \
    header1 body                                                        \
    header2 body                                                        \
    header3 body                                                        \
    header4 body

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R>::rarray(),
template<typename T>                ra::rarray<T AR_COMMA 1>::rarray(),
  : parray_(AR_NULLPTR) AR_COMMA
    extent_(AR_NULLPTR) AR_COMMA
    ismine_(false)      AR_COMMA
    entire_(false)      AR_COMMA
    rcount_(AR_NULLPTR)
{
    // constructor for an undefined array
    AR_PROFILESAY("rarray<T,R>::rarray()");
})

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1)
{
    // constructor for R=2
    AR_PROFILESAY("rarray<T,R>::rarray(int, int)");
    AR_CHECKORSAY(R==2, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1};
    init_data(extent, n0*n1);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2)
{
    // constructor for R=3
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int)");
    AR_CHECKORSAY(R==3, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2};
    init_data(extent, n0*n1*n2);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3)
{
    // constructor for R=4
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int)");
    AR_CHECKORSAY(R==4, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3};
    init_data(extent, n0*n1*n2*n3);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4)
{
    // constructor for R=5
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int)");
    AR_CHECKORSAY(R==5, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4};
    init_data(extent, n0*n1*n2*n3*n4);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5)
{
    // constructor for R=6
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int, int)");
    AR_CHECKORSAY(R==6, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5};
    init_data(extent, n0*n1*n2*n3*n4*n5);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6)
{
    // constructor for R=7
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==7, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7)
{
    // constructor for R=8
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==8, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
    // constructor for R=9
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==9, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7*n8);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    // constructor for R=10
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==10, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7*n8*n9);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10)
{
    // constructor for R=11
    AR_PROFILESAY("rarray<T,R>::rarray(int, int, int, int, int, int, int, int, int, int. int)");
    AR_CHECKORSAY(R==11, "wrong rank in constructor");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0 and n10!=0, "zero shape in constructor not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10};
    init_data(extent, n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10);
}

template<typename T,int R>
ra::rarray<T,R>::rarray(const int* extent) 
{
    // constructor for any R (the only way for R>11)
    AR_PROFILESAY("rarray<T,R>::rarray(const int*)");
    int volume = 1;
    for (int i=0; i<R; i++)
        volume *= extent[i];
    AR_CHECKORSAY(volume!=0,"zero shape in constructor not allowed");
    init_data(extent, volume);
}

template<typename T>
ra::rarray<T,1>::rarray(int n0)
{
    // constructor for rank==1
    AR_PROFILESAY("rarray<T,1>::rarray(int)");
    init_data(&n0, n0);
}

template<typename T>
ra::rarray<T,1>::rarray(const int* extent) 
{
    // constructor from an array of 1 extent: here just for uniformity
    AR_PROFILESAY("rarray<T,1>::rarray(const int*)");
    init_data(extent, extent[0]);
}

// from existing buffers:

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1) 
{
    // constructor for R=2
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int)");
    AR_CHECKORSAY(R==2, "wrong rank in constructor");
    const int extent[R] = {n0,n1};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2) 
{
    // constructor for R=3
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int)");
    AR_CHECKORSAY(R==3, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3) 
{
    // constructor for R=4
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int)");
    AR_CHECKORSAY(R==4, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4) 
{
    // constructor for R=5
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int)");
    AR_CHECKORSAY(R==5, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5) 
{
    // constructor for R=6
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==6, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6) 
{
    // constructor for R=7
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==7, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7)
{
    // constructor for R=8
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==8, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
    // constructor for R=9
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==9, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    // constructor for R=10
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==10, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10)
{
    // constructor for R=11
    AR_PROFILESAY("rarray<T,R>::rarray(T*, int, int, int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==11, "wrong rank in constructor");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
ra::rarray<T,R>::rarray(T* buffer, const int* extent)
{
    // constructor for any I (the only way for R>11)
    AR_PROFILESAY("rarray<T,R>::rarray(T*, const int*)");
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T>
ra::rarray<T,1>::rarray(T* buffer, int n0)
{
    // constructor for rank==1
    AR_PROFILESAY("rarray<T,1>::rarray(T*, int)");
    const int extent[1] = {n0};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T>
ra::rarray<T,1>::rarray(T* buffer, const int* extent) 
{
    // constructor for rank==1 from an array of 1 extent: here for reasons of conformity
    AR_PROFILESAY("rarray<T,1>::rarray(T*, const int*)");
    init_parray(buffer, extent);
    ismine_ = false;
}

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> radetail::subarray<T AR_COMMA R>::subarray(parray_t parray, const int* extent),
template<typename T>                radetail::subarray<T AR_COMMA 1>::subarray(parray_t parray, const int* extent),
  : parray_(parray) AR_COMMA
    extent_(extent) 
{
    // constructor of subarray; private, to be called from operator[] of rarrays
    AR_PROFILESAY("subarray<T,R>::subarray(parray_t, const int*)");
})


AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R>::~rarray(),
template<typename T>                ra::rarray<T AR_COMMA 1>::~rarray(),
{
    // rarray<T,R> destructor (note: subarray has none)
    AR_PROFILESAY("rarray<T,R>::~rarray()");
    clear();
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> bool ra::rarray<T AR_COMMA R>::is_clear() const,
template<typename T>                bool ra::rarray<T AR_COMMA 1>::is_clear() const,
{
    // check if undefined
    AR_PROFILESAY("bool rarray<T,R>::is_clear()");
    return parray_ == AR_NULLPTR;
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R>::rarray(const rarray<T AR_COMMA R> &a),
template<typename T>                ra::rarray<T AR_COMMA 1>::rarray(const rarray<T AR_COMMA 1> &a),
{
    // copy constructor
    AR_PROFILESAY("rarray<T,R>::rarray(const rarray<T,R>&)");
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_, a.entire_, a.rcount_);
    ismine_ = a.ismine_;
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R>::rarray(const radetail::subarray<T AR_COMMA R> &a),
template<typename T>                ra::rarray<T AR_COMMA 1>::rarray(const radetail::subarray<T AR_COMMA 1> &a), 
{
    // copy constructor
    AR_PROFILESAY("rarray<T,R>::rarray(const subarray<T,R>&)");
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_);
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R> ra::rarray<T AR_COMMA R>::copy() const,
template<typename T>                ra::rarray<T AR_COMMA 1> ra::rarray<T AR_COMMA 1>::copy() const, 
{
    // create an independent, deep copy (regular copy is shallow)
    AR_PROFILESAY("rarray<T,R> rarray<T,R>::copy() const");
    if (parray_ != AR_NULLPTR) { 
        // if initialized
        self_t result(extent_);
        T* bufbegin = get_buffer();
        T* bufend = bufbegin+size();
        T* newbuf = result.get_buffer();
        std::copy(bufbegin, bufend, newbuf);
        return result;
    } else {
        // else return undefined coy
        return rarray();
    }
 })

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> int         ra::rarray<T AR_COMMA R>::extent(int i) const,
template<typename T>                int         ra::rarray<T AR_COMMA 1>::extent(int i) const,
template<typename T AR_COMMA int R> int radetail::subarray<T AR_COMMA R>::extent(int i) const,
template<typename T>                int radetail::subarray<T AR_COMMA 1>::extent(int i) const, 
{
    // retrieve array size in dimension i   
    AR_PROFILESAY("int sub/rarray<T,R>::extent(int) const");
    AR_CHECKORSAY(i >=0 and i < rank, "wrong dimension");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return extent_[i];
})

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> const int*         ra::rarray<T AR_COMMA R>::shape() const,
template<typename T>                const int*         ra::rarray<T AR_COMMA 1>::shape() const,
template<typename T AR_COMMA int R> const int* radetail::subarray<T AR_COMMA R>::shape() const,
template<typename T>                const int* radetail::subarray<T AR_COMMA 1>::shape() const,
{
    AR_PROFILESAY("const int* sub/rarray<T,R>::shape() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return extent_;
})

// rarray method to retrieve the total number of elements

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> int         ra::rarray<T AR_COMMA R>::size() const, 
template<typename T>                int         ra::rarray<T AR_COMMA 1>::size() const,
template<typename T AR_COMMA int R> int radetail::subarray<T AR_COMMA R>::size() const,
template<typename T>                int radetail::subarray<T AR_COMMA 1>::size() const,
{ 
    AR_PROFILESAY("int sub/rarray<T,R>::size() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    // Retrieve array sizes in all dimensions. 
    // Needs to be computed, as this information is not stored.
    int result = 1;        
    for (int i=0; i<rank; i++)
        result *= extent_[i];
    return result;
})


AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R>       T* ra::rarray<T AR_COMMA R>::data(),
template<typename T AR_COMMA int R> const T* ra::rarray<T AR_COMMA R>::data() const,
template<typename T>                      T* ra::rarray<T AR_COMMA 1>::data(),
template<typename T>                const T* ra::rarray<T AR_COMMA 1>::data() const,
{
    AR_PROFILESAY("(const) T* rarray<T,R>::data() (const)");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    // return pointer of type T* to the internal data
    return get_buffer();
})

// ...for subarray

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> T* radetail::subarray<T AR_COMMA R>::data() const,
template<typename T>                T* radetail::subarray<T AR_COMMA 1>::data() const,
{
    AR_PROFILESAY("T* subarray<T,1>::data()");
    return ra::rarray<T AR_COMMA rank>::base(parray_);
})

// rarray begin/end methods

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::iterator ra::rarray<T AR_COMMA R>::begin(),
template<typename T>                typename ra::rarray<T AR_COMMA 1>::iterator ra::rarray<T AR_COMMA 1>::begin(),
{
    AR_PROFILESAY("iterator rarray<T,R>::begin()");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return iterator(get_buffer(), size());
 })

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::const_iterator ra::rarray<T AR_COMMA R>::begin() const,
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::const_iterator ra::rarray<T AR_COMMA R>::cbegin() const, 
template<typename T>                typename ra::rarray<T AR_COMMA 1>::const_iterator ra::rarray<T AR_COMMA 1>::begin() const, 
template<typename T>                typename ra::rarray<T AR_COMMA 1>::const_iterator ra::rarray<T AR_COMMA 1>::cbegin() const, 
{
    AR_PROFILESAY("const_iterator rarray<T,R>::begin() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return const_iterator(get_buffer(), size());
 })

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::iterator ra::rarray<T AR_COMMA R>::end(),
template<typename T>                typename ra::rarray<T AR_COMMA 1>::iterator ra::rarray<T AR_COMMA 1>::end(),
{
    AR_PROFILESAY("iterator rarray<T,R>::end()");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return iterator(get_buffer()+size(), 0);
})

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::const_iterator ra::rarray<T AR_COMMA R>::end() const, 
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::const_iterator ra::rarray<T AR_COMMA R>::cend() const, 
template<typename T>                typename ra::rarray<T AR_COMMA 1>::const_iterator ra::rarray<T AR_COMMA 1>::end() const, 
template<typename T>                typename ra::rarray<T AR_COMMA 1>::const_iterator ra::rarray<T AR_COMMA 1>::cend() const, 
{
    AR_PROFILESAY("const_iterator rarray<T,R>::(c)end() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return const_iterator(get_buffer()+size(), 0);
})

// ...for subarray

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> typename radetail::subarray<T AR_COMMA R>::iterator radetail::subarray<T AR_COMMA R>::begin() const,
template<typename T>                typename radetail::subarray<T AR_COMMA 1>::iterator radetail::subarray<T AR_COMMA 1>::begin() const,
{
    AR_PROFILESAY("T* subarray<T,R>::begin()");
    return iterator(ra::rarray<T AR_COMMA rank>::base(parray_), size());
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> typename radetail::subarray<T AR_COMMA R>::const_iterator radetail::subarray<T AR_COMMA R>::cbegin() const,
template<typename T>                typename radetail::subarray<T AR_COMMA 1>::const_iterator radetail::subarray<T AR_COMMA 1>::cbegin() const,
{
    AR_PROFILESAY("T* subarray<T,R>::cbegin()");
    return const_iterator(ra::rarray<T AR_COMMA rank>::base(parray_), size());
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> typename radetail::subarray<T AR_COMMA R>::iterator radetail::subarray<T AR_COMMA R>::end() const,
template<typename T>                typename radetail::subarray<T AR_COMMA 1>::iterator radetail::subarray<T AR_COMMA 1>::end() const,
{
    AR_PROFILESAY("T* subarray<T,R>::end()");
    return iterator(ra::rarray<T AR_COMMA rank>::base(parray_) + size(), 0);
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> typename radetail::subarray<T AR_COMMA R>::const_iterator radetail::subarray<T AR_COMMA R>::cend() const,
template<typename T>                typename radetail::subarray<T AR_COMMA 1>::const_iterator radetail::subarray<T AR_COMMA 1>::cend() const,
{
    AR_PROFILESAY("T* subarray<T,1>::cend()");
    return const_iterator(ra::rarray<T AR_COMMA rank>::base(parray_) + size(), 0);
})

// retrieve indices of an element
AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> int* ra::rarray<T AR_COMMA R>::index(const iterator&i, int* ind),
template<typename T AR_COMMA int R> int* ra::rarray<T AR_COMMA R>::index(const const_iterator&i, int* ind) const,
template<typename T>                int* ra::rarray<T AR_COMMA 1>::index(const iterator&i, int* ind),
template<typename T>                int* ra::rarray<T AR_COMMA 1>::index(const const_iterator&i, int* ind) const,
{
    AR_PROFILESAY("int* rarray<T,R>::index((const_)iterator&,int*) (const)");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    AR_CHECKORSAY(ind!=AR_NULLPTR, "invalid index buffer");
    // return pointer of type T* to the internal data
    int linearindex = &(*i) - get_buffer();
    int j = rank;
    while (j-->0) {
        ind[j] = linearindex % extent[j];
        linearindex /= extent[j];
    }
    return ind;
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> int* ra::rarray<T AR_COMMA R>::index(const T& a, int* ind) const,
template<typename T>                int* ra::rarray<T AR_COMMA 1>::index(const T& a, int* ind) const,
{
    AR_PROFILESAY("int* rarray<T,R>::index((const_)iterator&,int*) (const)");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    AR_CHECKORSAY(ind!=AR_NULLPTR, "invalid index buffer");
    // return pointer of type T* to the internal data
    int linearindex = &a - get_buffer();
    int j = rank;
    while (j-->0) {
        ind[j] = linearindex % extent[j];
        linearindex /= extent[j];
    }
    return ind;
})
// to do sub arrays

// rarray method to return T*const*.. acting similarly to this rarray
// when using []:

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> typename ra::rarray<T AR_COMMA R>::parray_t                 ra::rarray<T AR_COMMA R>::ptr_array() const,
template<typename T>                typename ra::rarray<T AR_COMMA 1>::parray_t                 ra::rarray<T AR_COMMA 1>::ptr_array() const, 
template<typename T AR_COMMA int R> typename radetail::subarray<T AR_COMMA R>::parray_t radetail::subarray<T AR_COMMA R>::ptr_array() const,
template<typename T>                typename radetail::subarray<T AR_COMMA 1>::parray_t radetail::subarray<T AR_COMMA 1>::ptr_array() const,
{
    AR_PROFILESAY("sub/rarray<T,R>::parray_t sub/rarray<T,R>::ptr_array() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined array");
    return parray_;
})

// rarray method to return  T**.. acting similarly to this rarray when using []

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> typename         ra::rarray<T AR_COMMA R>::noconst_parray_t         ra::rarray<T AR_COMMA R>::noconst_ptr_array() const, 
template<typename T>                typename         ra::rarray<T AR_COMMA 1>::noconst_parray_t         ra::rarray<T AR_COMMA 1>::noconst_ptr_array() const, 
template<typename T AR_COMMA int R> typename radetail::subarray<T AR_COMMA R>::noconst_parray_t radetail::subarray<T AR_COMMA R>::noconst_ptr_array() const, 
template<typename T>                typename radetail::subarray<T AR_COMMA 1>::noconst_parray_t radetail::subarray<T AR_COMMA 1>::noconst_ptr_array() const, 
{
    AR_PROFILESAY("rarray<T,R>::noconst_parray_t sub/rarray<T,R>::noconst_ptr_array() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return const_cast<noconst_parray_t>(parray_);
})

// rarray method to create a reference to this that treats elements as constant

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<const T AR_COMMA R>& ra::rarray<T AR_COMMA R>::const_ref() const,
template<typename T>                ra::rarray<const T AR_COMMA 1>& ra::rarray<T AR_COMMA 1>::const_ref() const,
{
    AR_PROFILESAY("rarray<const T,R>& rarray<T,R>::const_ref() const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return (rarray<const T AR_COMMA rank>&)(*this);
 })

// ...for subarray
AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> radetail::subarray<const T AR_COMMA R>& radetail::subarray<T AR_COMMA R>::const_ref() const,
template<typename T>                radetail::subarray<const T AR_COMMA 1>& radetail::subarray<T AR_COMMA 1>::const_ref() const,
{
    AR_PROFILESAY("subarray<const T,R>& subarray<T,R>::const_ref() const");
    return (subarray<const T,rank>&)(*this);
 })

AR_QUADRUPLICATE_BODY(
template<typename T AR_COMMA int R> void         ra::rarray<T AR_COMMA R>::fill(const T& value),
template<typename T>                void         ra::rarray<T AR_COMMA 1>::fill(const T& value),
template<typename T AR_COMMA int R> void radetail::subarray<T AR_COMMA R>::fill(const T& value),
template<typename T>                void radetail::subarray<T AR_COMMA 1>::fill(const T& value),
{
    AR_PROFILESAY("void sub/rarray<T,R>::fill(const T&)");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    for (iterator i=begin(); i!=end(); i++)
        *i = value;
})



#ifndef AR_SKIPINTERMEDIATE
template<typename T,int R> radetail::subarray<const T,R-1> ra::rarray<T,R>::operator[](int i) const
{
    AR_PROFILESAY("subarray<const T,R-1> rarray<T,R>::operator[](int) const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    AR_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return radetail::subarray<const T,R-1>(parray_[i], extent_+1);
}
template<typename T,int R> radetail::subarray<T,R-1> ra::rarray<T,R>::operator[](int i)
{
    AR_PROFILESAY("subarray<T,R-1> rarray<T,R>::operator[](int)");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    AR_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return radetail::subarray<T,R-1>(parray_[i], extent_+1);
}
#else
template<typename T,int R> ra::rarray<T,R>::operator typename radetail::PointerArray<const T,R>::type () const 
{
    AR_PROFILESAY("rarray<T,R>::operator typename PointerArray<const T,R>::parray_t () const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return parray_;
}
template<typename T,int R> ra::rarray<T,R>::operator typename radetail::PointerArray<T,R>::type ()
{
    AR_PROFILESAY("rarray<T,R>::operator typename PointerArray<T,R>::type ()");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return parray_;
}
#endif

#ifndef AR_SKIPINTERMEDIATE
template<typename T> const T& ra::rarray<T,1>::operator[](int i) const 
{
    AR_PROFILESAY("const T& rarray<T,1>::operator[](int) const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    AR_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}
template<typename T> T& ra::rarray<T,1>::operator[](int i)
{
    AR_PROFILESAY("T& rarray<T,1>::operator[](int)");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    AR_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}
#else
template<typename T> ra::rarray<T,1>::operator typename radetail::PointerArray<const T,1>::type () const 
{
    AR_PROFILESAY("rarray<T,1>::operator PointerArray<const T,1>::type () const");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return parray_;
}
template<typename T> ra::rarray<T,1>::operator typename radetail::PointerArray<T,1>::type ()
{
    AR_PROFILESAY("rarray<T,1>::operator typename PointerArray<T,1>::type ()");
    AR_CHECKORSAY(parray_!=AR_NULLPTR, "attempt at using undefined rarray");
    return parray_;
}
#endif

template<typename T,int R> radetail::subarray<T,R-1> radetail::subarray<T,R>::operator[](int i) const
{
    // subarray methods for element access
    AR_PROFILESAY("subarray<T,R-1> subarray<T,R>::operator[](int)");
    AR_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");        
    return subarray<T,R-1>(parray_[i], extent_+1);
}

template<typename T> T& radetail::subarray<T,1>::operator[](int i) const
{
    // subarray methods for element access, rank==1 version
    AR_PROFILESAY("T& subarray<T,1>::operator[](int)");
    AR_CHECKORSAY(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R>& ra::rarray<T AR_COMMA R>::operator=(const ra::rarray<T AR_COMMA R> &a),
template<typename T>                ra::rarray<T AR_COMMA 1>& ra::rarray<T AR_COMMA 1>::operator=(const ra::rarray<T AR_COMMA 1> &a),
{
    // rarray assignment operator from other rarray
    AR_PROFILESAY("rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R>&)");
    if (&a != this) {
        clear();
        extent_ = const_cast<int*>(a.extent_);
        init_shallow(a.parray_, a.extent_, a.entire_, a.rcount_);
        ismine_ = a.ismine_;
    }
    return *this;
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> ra::rarray<T AR_COMMA R>& ra::rarray<T AR_COMMA R>::operator=(const radetail::subarray<T AR_COMMA R> &a),
template<typename T>                ra::rarray<T AR_COMMA 1>& ra::rarray<T AR_COMMA 1>::operator=(const radetail::subarray<T AR_COMMA 1> &a),
{
    // rarray assignment operator from subarray
    AR_PROFILESAY("rarray<T,R>& rarray<T,R>::operator=(const subarray<T,R>&)");
    clear();
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_);
    return *this;
})

AR_DUPLICATE_BODY(
template<typename T AR_COMMA int R> T* ra::rarray<T AR_COMMA R>::get_buffer() const,
template<typename T>                T* ra::rarray<T AR_COMMA 1>::get_buffer() const,
{ 
    // get start of current contiguous buffer
    AR_PROFILESAY("T* rarray<T,R>::get_buffer()");
    return base(parray_); 
 })

template<typename T,int R>
void ra::rarray<T,R>::init_shallow(parray_t parray, const int*  extent, bool entire, int* rcount)
{
    // shallow init function : reuses buffer and parray
    AR_PROFILESAY("void rarray<T,R>::init_shallow(parray_t, const int*, bool, int*)");
    AR_CHECKORSAY(      extent != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(      parray != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(base(parray) != AR_NULLPTR, "null pointer");
    rcount_ = rcount;
    parray_ = parray;
    entire_ = entire;
    for (int i=0;i<R;i++)
        extent_[i] = extent[i];
    if (entire_)
        (*rcount_)++;
}

template<typename T,int R>
void ra::rarray<T,R>::init_shallow(parray_t parray, const int* extent)
{
    // shallow init function for subarray: reuses buffer and parray
    AR_PROFILESAY("void rarray<T,R>::init_shallow(parray_t, const int*)");
    AR_CHECKORSAY(      extent != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(      parray != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(base(parray) != AR_NULLPTR, "null pointer");
    parray_ = parray;
    rcount_ = AR_NULLPTR;
    entire_ = false;
    for (int i=0;i<R;i++)
        extent_[i] = extent[i];
}

template<typename T,int R>
void ra::rarray<T,R>::init_parray(T* buffer, const int* extent)
{
    // init functions that reuses a buffer but not a parray
    AR_PROFILESAY("void rarray<T,R>::init_parray(T*, const int*)");
    AR_CHECKORSAY(buffer != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(extent != AR_NULLPTR, "null pointer");
    parray_t parray = new_except_base(buffer, extent);
    extent_ = new int[R];
    init_shallow(parray, extent, true, new int(0));
}

template<typename T,int R>
void ra::rarray<T,R>::init_data(const int* extent, int extenttot)
{
    // init functions that need new buffer and parray
    AR_PROFILESAY("void rarray<T,R>::init_data(const int*, int)");
    AR_CHECKORSAY( extent != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY( extenttot >= 0, "negative number of elements");
    T* buffer = new T[extenttot];
    init_parray(buffer, extent);
    ismine_ = true;
}

template<typename T>
void ra::rarray<T,1>::init_shallow(parray_t    parray,
                                   const int*  extent,
                                   bool        entire,
                                   int*        rcount)
{
    // shallow init function : reuses buffer and parray
    AR_PROFILESAY("void rarray<T,1>::init_shallow(parray_t, const int*, bool, int*)");
    AR_CHECKORSAY(      extent != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(      parray != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(base(parray) != AR_NULLPTR, "null pointer");
    rcount_ = rcount;
    parray_ = parray;
    entire_ = entire;
    for (int i=0;i<1;i++)
        extent_[i] = extent[i];
    if (entire_ and rcount != AR_NULLPTR)
        (*rcount_)++;
}

template<typename T>
void ra::rarray<T,1>::init_shallow(parray_t parray, const int* extent)
{
    // shallow init function : reuses parray
    AR_PROFILESAY("void rarray<T,1>::init_shallow(parray_t, const int*)");
    AR_CHECKORSAY(      extent != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(      parray != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(base(parray) != AR_NULLPTR, "null pointer");
    parray_ = parray;
    rcount_ = AR_NULLPTR;
    entire_ = false;
    for (int i=0;i<1;i++)
        extent_[i] = extent[i];
}

template<typename T>
void ra::rarray<T,1>::init_parray(T* buffer, const int* extent)
{
    // shallow init function for subarray: reuses buffer and parray
    AR_PROFILESAY("void rarray<T,1>::init_parray(T*, const int*)");
    AR_CHECKORSAY(buffer != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(extent != AR_NULLPTR, "null pointer");
    extent_ = new int[1];
    parray_t parray = new_except_base(buffer, extent);
    init_shallow(parray, extent, true, new int(0));
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(const int* extent)
{
    // common method to reshape an array (takes an c-array argument)
    AR_PROFILESAY("void rarray<T,R>::reshape(const int* extent)");
    AR_CHECKORSAY(parray_ != AR_NULLPTR and entire_, "reshape not allowed on subarray");
    int tot1 = 1, tot2 = 1;
    for (int i=0;i<R;i++) {
        tot1 *= extent_[i];
        tot2 *= extent[i];
    }
    AR_CHECKORSAY(tot2 <= tot1, "reshaping beyond underlying memory buffer");
    if (parray_ != AR_NULLPTR and entire_) { // cannot reshape subarrays
        if (*rcount_==1) {
            T* buffer = get_buffer();
            delete[] parray_;
            for (int i=0;i<R;i++)
                extent_[i] = extent[i];
            parray_ = new_except_base(buffer, extent_);
        } else { // if this has other references to it, create a new view
            T* buffer = get_buffer();    // get buffer address
            ismine_ = false;             // protect buffer from being deleted
            clear();                     // deallocate everything else
            init_parray(buffer, extent); // reallocate pointer arrays
        }
    }
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1) 
{
    // reshape method only for R=2
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int)");
    AR_CHECKORSAY(R==2, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0, "zero shape not allowed");
    // reuse common method
    const int extent[R] = {n0,n1};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2)
{
    // reshape method only for R=3
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int)");
    AR_CHECKORSAY(R==3, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3) 
{
    // reshape method only for R=4
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int)");
    AR_CHECKORSAY(R==4, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4)
{
    // reshape method only for R=5
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int)");
    AR_CHECKORSAY(R==5, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5)
{
    // reshape method only for R=6
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int, int)");
    AR_CHECKORSAY(R==6, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6)
{
    // reshape method only for R=7
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==7, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7)
{
    // reshape method only for R=8
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==8, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
    // reshape method only for R=9
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==9, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    // reshape method only for R=10
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==10, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10)
{
    // reshape method only for R=11
    AR_PROFILESAY("void rarray<T,R>::reshape(int, int, int, int, int, int, int, int, int, int, int)");
    AR_CHECKORSAY(R==11, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0 and n6!=0 and n7!=0 and n8!=0 and n9!=0 and n10!=0, "zero shape not allowed");
    const int extent[R] = {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10};
    reshape(extent);
}

template<typename T>
void ra::rarray<T,1>::init_data(const int* extent, int extenttot)
{
    AR_PROFILESAY("void rarray<T,1>::init_data(const int*, int)");
    AR_CHECKORSAY(extent != AR_NULLPTR, "null pointer");
    AR_CHECKORSAY(extenttot >= 0, "negative number of elements");
    T* buffer = new T[extenttot];
    init_parray(buffer, extent);
    ismine_ = true;
}

template<typename T>
void ra::rarray<T,1>::reshape(const int* extent)
{
    AR_PROFILESAY("void rarray<T,1>::reshape(const int* extent)");
    AR_CHECKORSAY(parray_ != AR_NULLPTR and entire_, "reshape not allowed on subarray");
    AR_CHECKORSAY(*extent <= *extent_, "reshaping beyond underlying memory buffer");
    if (parray_ != AR_NULLPTR and entire_) {
        if (*rcount_==1) {
            *extent_ = *extent;
        } else {
            T* buffer = get_buffer();    // get buffer address        
            ismine_ = false;             // protect buffer from being deleted
            clear();                      // deallocate everything else
            init_parray(buffer, extent); // reallocate pointer arrays
        }
    }
}

template<typename T>
void ra::rarray<T,1>::reshape(int n0)
{
    // constructor for R=1
    AR_PROFILESAY("void rarray<T,1>::reshape(int)");
    AR_CHECKORSAY(rank==1, "wrong rank in reshape");
    AR_CHECKORSAY(n0!=0, "zero shape not allowed");
    const int extent[1] = {n0};
    reshape(extent);
}

template<typename T,int R>
void ra::rarray<T,R>::clear() 
{
    //  rarray private cleanup routine
    AR_PROFILESAY("void rarray<T,R>::clear()");
    if (parray_ != AR_NULLPTR and entire_) {
        (*rcount_)--;
        if (*rcount_==0) {
            if (ismine_) 
                delete[] get_buffer(); 
            delete[] parray_;
            delete[] extent_;
            delete rcount_;
        }
    }
    rcount_ = AR_NULLPTR;
    parray_ = AR_NULLPTR;
}

template<typename T>
void ra::rarray<T,1>::clear() 
{
    AR_PROFILESAY("void rarray<T,1>::clear()");
    if (parray_ != AR_NULLPTR and entire_) {
        (*rcount_)--;
        if (*rcount_==0) {
            if (ismine_)
                delete[] get_buffer();
            delete[] extent_;
            delete rcount_;
        }
    }
    rcount_ = AR_NULLPTR;
    parray_ = AR_NULLPTR;
}


template<typename T,int R>
typename ra::rarray<T,R>::parray_t 
ra::rarray<T,R>::new_except_base(T* buffer, const int* extent) 
{
    // rarray private method to allocate the chain of pointers, except the base
    AR_PROFILESAY("parray_t rarray<T,R>::new_except_base(T*, const int*)");
    if (R>1) {
        typedef typename radetail::Unconst<T>::type* bufstart_t;       
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

template<typename T>
typename ra::rarray<T,1>::parray_t 
ra::rarray<T,1>::new_except_base(T* buffer, const int* extent) 
{
    AR_PROFILESAY("parray_t rarray<T,1>::new_except_base(T*, const int*)");
    return reinterpret_cast<parray_t>(buffer);
}



template<typename T,int R>
T* ra::rarray<T,R>::base(parray_t parray) 
{
    // rarray private method to find base of a chain of pointers
    AR_PROFILESAY("T* rarray<T,R>::base(parray_t)");
    if (parray != AR_NULLPTR) {
        const char* result1 = reinterpret_cast<const char*>(parray);
        char* result2 = const_cast<char*>(result1);
        char** result = reinterpret_cast<char**>(result2);    
        for (int i=0; i < R-1; i++)
            result = reinterpret_cast<char**>(*result);
        return reinterpret_cast<T*>(result);
    } else
        return AR_NULLPTR;
}

template<typename T>
T* ra::rarray<T,1>::base(parray_t parray)
{
    // rarray private method to find base of a chain of pointers: different for rank==1?
    AR_PROFILESAY("T* rarray<T,1>::base(parray_t)");
    const char*  result1 = reinterpret_cast<const char*>(parray);
          char*  result2 = const_cast<char*>(result1);
          char** result  = reinterpret_cast<char**>(result2);
    return reinterpret_cast<T*>(result);
}


template<typename T>
T& radetail::Iterator<T>::operator*()
{
    // Iterator dereference
    AR_PROFILESAY("T& Iterator<T>::operator*()");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_ < pointer_max_plus_one_, "iterator out of bounds");
    return *pointer_;
}           

template<typename T>
radetail::Iterator<T>& radetail::Iterator<T>::operator++()
{
    // Iterator pre-incrememt
    AR_PROFILESAY("Iterator<T> Iterator<T>::operator++()");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_ < pointer_max_plus_one_, "iterator out of bounds");
    ++pointer_;
    return *this;
}

template<typename T>
radetail::Iterator<T> radetail::Iterator<T>::operator++(int)
{
    // Iterator post-increment
    AR_PROFILESAY("Iterator<T> Iterator<T>::operator++(int)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_ < pointer_max_plus_one_, "iterator out of bounds");
    radetail::Iterator<T> saved = *this;
    ++pointer_;
    return saved;
}

template<typename T>
bool radetail::Iterator<T>::operator==(const Iterator<T>& other)
{
    // Iterator comparison: equal to
    AR_PROFILESAY("bool Iterator<T>::operator==(const Iterator<T>&)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    AR_CHECKORSAY(other.pointer_ != AR_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ == other.pointer_;
}

template<typename T>
bool radetail::Iterator<T>::operator!=(const Iterator<T>& other)
{
    // Iterator comparison: not equal to
    AR_PROFILESAY("bool Iterator<T>::operator!=(const Iterator<T>&)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    AR_CHECKORSAY(other.pointer_ != AR_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ != other.pointer_;
}

template<typename T>
bool radetail::Iterator<T>::operator<(const Iterator<T>& other)
{
    // Iterator comparison: less than
    AR_PROFILESAY("bool Iterator<T>::operator<(const Iterator<T>&)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    AR_CHECKORSAY(other.pointer_ != AR_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ < other.pointer_;
}

template<typename T>
bool radetail::Iterator<T>::operator>(const Iterator<T>& other)
{
    // Iterator comparison: greater than
    AR_PROFILESAY("bool Iterator<T>::operator>(const Iterator<T>&)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    AR_CHECKORSAY(other.pointer_ != AR_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ > other.pointer_;
}

template<typename T>
bool radetail::Iterator<T>::operator<=(const Iterator<T>& other)
{
    // Iterator comparison: less than or equal to
    AR_PROFILESAY("bool Iterator<T>::operator<=(const Iterator<T>&)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    AR_CHECKORSAY(other.pointer_ != AR_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ <= other.pointer_;
}

template<typename T>
bool radetail::Iterator<T>::operator>=(const Iterator<T>& other)
{
    // Iterator comparison: greater than or equal to
    AR_PROFILESAY("bool Iterator<T>::operator>=(const Iterator<T>&)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
    AR_CHECKORSAY(other.pointer_ != AR_NULLPTR and other.pointer_ >= other.pointer_min_ and other.pointer_max_plus_one_ >= other.pointer_min_, "invalid iterator 2");
    return pointer_ >= other.pointer_;
}

template<typename T>
radetail::Iterator<T>::Iterator(T* ptr, int size)
  : pointer_(ptr),
    pointer_min_(ptr),
    pointer_max_plus_one_(ptr+size)
{
    AR_PROFILESAY("Iterator<T>::Iterator(T*,int)");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and size >=0, "invalid construction of iterator");
    AR_CHECKORSAY(pointer_ != AR_NULLPTR and pointer_ >= pointer_min_ and pointer_max_plus_one_ >= pointer_min_, "invalid iterator 1");
}


template<typename A>
int radetail::extent_given_byte_size(A a[], int i, int byte_size) 
{
    // find shape of automatic arrays, given the size of the array
    AR_PROFILESAY("int extent_given_byte_size(A[],int,int)");
    AR_CHECKORSAY(i>=0 and i<1, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A);
        default: return 1;
    }
}

template<typename A,int Z>
int radetail::extent_given_byte_size(A a[][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<2, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/Z;
        case 1:  return Z;
        default: return 1;
    }
}

template<typename A,int Y,int Z>
int radetail::extent_given_byte_size(A a[][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<3, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/Z/Y;
        case 1:  return Y;
        case 2:  return Z;
        default: return 1;
    }
}

template<typename A,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<4, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/X/Z/Y;
        case 1:  return X;
        case 2:  return Y;
        case 3:  return Z;
        default: return 1;
    }
}

template<typename A,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<5, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A)/W/X/Z/Y;
        case 1:  return W;
        case 2:  return X;
        case 3:  return Y;
        case 4:  return Z;
        default: return 1;
    }
}

template<typename A,int V,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][V][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][V][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<6, "wrong dimension");
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

template<typename A,int U,int V,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][U][V][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][U][V][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<7, "wrong dimension");
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

template<typename A,int T,int U,int V,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][T][U][V][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<8, "wrong dimension");
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


template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][S][T][U][V][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<9, "wrong dimension");
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

template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][R][S][T][U][V][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<10, "wrong dimension");
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


template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>
int radetail::extent_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(A[][Q][R][S][T][U][V][W][X][Y][Z],int,int)");
    AR_CHECKORSAY(i>=0 and i<11, "wrong dimension");
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

template<typename A,int R>
int radetail::extent_given_byte_size(const ra::rarray<A,R>& a, int i, int byte_size) 
{
    AR_PROFILESAY("int extent_given_byte_size(const rarray<A,R>&,int,int)");
    return a.extent(i);
}

// convert to rarray

template<typename A>
ra::rarray<A,1> radetail::make_rarray_given_byte_size(A a[], int byte_size) 
{
    AR_PROFILESAY("rarray<A,1> make_rarray_given_byte_size(A[],int)");
    const int z = byte_size/sizeof(A);
    return ra::rarray<A,1>(a,z);
}

template<typename A,int Z>
ra::rarray<A,2> radetail::make_rarray_given_byte_size(A a[][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,2> make_rarray_given_byte_size(A[][Z],int)");
    const int y = byte_size/sizeof(A)/Z;
    return ra::rarray<A,2>(*a,y,Z);
}

template<typename A,int Y,int Z>
ra::rarray<A,3> radetail::make_rarray_given_byte_size(A a[][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,3> make_rarray_given_byte_size(A[][Y][Z],int)");
    const int x = byte_size/sizeof(A)/Z/Y;
    return ra::rarray<A,3>(**a,x,Y,Z);
}

template<typename A,int X,int Y,int Z>
ra::rarray<A,4> radetail::make_rarray_given_byte_size(A a[][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,4> make_rarray_given_byte_size(A[][X][Y][Z],int)");
    const int w = byte_size/sizeof(A)/X/Z/Y;
    return ra::rarray<A,4>(***a,w,X,Y,Z);
}

template<typename A,int W,int X,int Y,int Z>
ra::rarray<A,5> radetail::make_rarray_given_byte_size(A a[][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,5> make_rarray_given_byte_size(A[][W][X][Y][Z],int)");
    const int v = byte_size/sizeof(A)/W/X/Z/Y;
    return ra::rarray<A,5>(****a,v,W,X,Y,Z);
}

template<typename A,int V,int W,int X,int Y,int Z>
ra::rarray<A,6> radetail::make_rarray_given_byte_size(A a[][V][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,6> make_rarray_given_byte_size(A[][V][W][X][Y][Z],int)");
    const int u = byte_size/sizeof(A)/V/W/X/Z/Y;
    return ra::rarray<A,6>(*****a,u,V,W,X,Y,Z);
}

template<typename A,int U,int V,int W,int X,int Y,int Z>
ra::rarray<A,7> radetail::make_rarray_given_byte_size(A a[][U][V][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,7> make_rarray_given_byte_size(A[][U][V][W][X][Y][Z],int)");
    const int t = byte_size/sizeof(A)/U/V/W/X/Z/Y;
    return ra::rarray<A,7>(******a,t,U,V,W,X,Y,Z);
}

template<typename A,int T,int U,int V,int W,int X,int Y,int Z>
ra::rarray<A,8> radetail::make_rarray_given_byte_size(A a[][T][U][V][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,8> make_rarray_given_byte_size(A[][T][U][V][W][X][Y][Z],int)");
    const int s = byte_size/sizeof(A)/T/U/V/W/X/Z/Y;
    return ra::rarray<A,8>(*******a,s,T,U,V,W,X,Y,Z);
}

template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z>
ra::rarray<A,9> radetail::make_rarray_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,9> make_rarray_given_byte_size(A[][Q][R][S][T][U][V][W][X][Y][Z],int)");
    const int r = byte_size/sizeof(A)/S/T/U/V/W/X/Z/Y;
    return ra::rarray<A,9>(********a,r,S,T,U,V,W,X,Y,Z);
}

template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>
ra::rarray<A,10> radetail::make_rarray_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,10> make_rarray_given_byte_size(A[][R][S][T][U][V][W][X][Y][Z],int)");
    const int q = byte_size/sizeof(A)/R/S/T/U/V/W/X/Z/Y;
    return ra::rarray<A,10>(*********a,q,R,S,T,U,V,W,X,Y,Z);
}

template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z>
ra::rarray<A,11> radetail::make_rarray_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    AR_PROFILESAY("rarray<A,11> make_rarray_given_byte_size(A[][Q][R][S][T][U][V][W][X][Y][Z],int)");
    const int p = byte_size/sizeof(A)/Q/R/S/T/U/V/W/X/Z/Y;
    return ra::rarray<A,11>(**********a,p,Q,R,S,T,U,V,W,X,Y,Z);
}

template<typename T,int R>
ra::rarray<T,R> radetail::make_rarray_given_byte_size(ra::rarray<T,R> a, int byte_size) 
{
    AR_PROFILESAY("rarray<T,R> make_rarray_given_byte_size(rarray<T,R>,int)");
    return a;
}

template<typename T,int R>
std::ostream& operator<<(std::ostream &o, const ra::rarray<T,R>& r)
{
    AR_PROFILESAY("std::ostream& operator<<(std::ostream&,const rarray<T,R>&)");
    return radetail::text_output(o,r);
}

template<typename T,int R>
std::ostream& operator<<(std::ostream &o, const radetail::subarray<T,R>& r)
{
    AR_PROFILESAY("std::ostream& operator<<(std::ostream&,const rarray<T,R>&)");
    return radetail::text_output(o,r);
}

template<typename T,int R>
std::ostream& radetail::text_output(std::ostream &o, const ra::rarray<T,R>& r)
{
    AR_PROFILESAY("std::ostream& operator<<(std::ostream&,const rarray<T,R>&)");
    if (not r.is_clear()) {
        o << '{';
        for (int i=0; i<r.extent(0); i++)  {
            if (i>0)
                o << ',';
            #ifndef AR_SKIPINTERMEDIATE
            o << r[i];
            #else
            T* subdata = AR_NULLPTR;
            if (r[i] != AR_NULLPTR) {
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

template<typename T>
std::ostream& radetail::text_output(std::ostream &o, const ra::rarray<T,1>& r)
{
    AR_PROFILESAY("std::ostream& operator<<(std::ostream&,const rarray<T,1>&)");
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

template<typename T,int R>
std::ostream& radetail::text_output(std::ostream &o, const radetail::subarray<T,R>& r)
{
    AR_PROFILESAY("std::ostream& operator<<(std::ostream&,const subarray<T,R>&)");
    o << '{' << r[0];
    for (int i=1; i<r.extent(0); i++) 
        o << ',' << r[i];
    o << '}';
    return o;
}

template<typename T>
std::ostream& radetail::text_output(std::ostream &o, const radetail::subarray<T,1>& r)
{
    AR_PROFILESAY("std::ostream& operator<<(std::ostream&,const subarray<T,1>&)");
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

// helper routines to convert a string to any data type

template<typename T>
void radetail::StringToValue<T>::get(const std::string& input, T& output) 
{
    AR_PROFILESAY("void StringToValue::get(const std::string&,T&)");
    std::stringstream str(input); // use streaming operators by default
    str >> output; // won't work with strings as they get truncated at first whitespace
}

void radetail::StringToValue<std::string>::get(const std::string& input, std::string& output)
{
    AR_PROFILESAY("void StringToValue::get(const std::string&,std::string&)");
    output = input;
}

template<typename T, int R>
T& radetail::Deref<T,R>::access(typename PointerArray<T,R>::type p, const int* indices) 
{
    AR_PROFILESAY("Deref<T,R>::access(PointerArray<T,R>::type,const int*)");
    return Deref<T,R-1>::access(p[indices[0]-1], indices+1);
}

template<typename T>
T& radetail::Deref<T,1>::access(typename PointerArray<T,1>::type p, const int* indices) 
{
    AR_PROFILESAY("Deref<T,1>::access(PointerArray<T,1>::type,const int*)");
    return p[indices[0]-1];
}

template<typename T, int R> 
void radetail::read_and_parse_shape(std::istream &                             in, 
                                    int*                                       shape, 
                                    typename radetail::PointerArray<T,R>::type p)
{
    AR_PROFILESAY("void read_and_parse_shape(std::istream&,int*,PointerArray<T,R>::type)");
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

template<typename T,int R>
std::istream& operator>>(std::istream &in, ra::rarray<T,R>& r)
{
    AR_PROFILESAY("std::istream& operator>>(std::istream&,rarray<T,R>&)");
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
        radetail::read_and_parse_shape<T,R>(in, extent, 0);
        // allocate array
        r = ra::rarray<T,R>(extent);
        // fill array
        radetail::read_and_parse_shape<T,R>(in, 0, r.ptr_array());
        return in;
    }
    catch (std::istream::failure& e) {
        in.seekg(init_file_ptr, in.beg);// upon failure, undo characters read in
        throw e;                        // and pass on the error
    }
}

// Get rid of the macros
#undef AR_PROFILESAY
#undef AR_CHECKORSAY
#undef AR_DUPLICATE_BODY
#undef AR_QUADRUPLICATE_BODY
#undef AR_COMMA

// Global namespace stuff
// (also in global namespace: operator<< and operator>> for rarray and subarray)

#define EXTENT(A,I) radetail::extent_given_byte_size(A,I,sizeof(A))
#define RARRAY(A)   radetail::make_rarray_given_byte_size(A,sizeof(A))
using ra::rarray; // for now.

#endif
