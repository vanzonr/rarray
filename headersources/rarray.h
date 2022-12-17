//
// rarray - Runtime arrays: template classes for pointer-based,
//          runtime, reference counted, multi-dimensional
//          arrays.  Documentation in rarraydoc.pdf
//
// Copyright (c) 2013-2022  Ramses van Zon
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

#if __cplusplus >= 201103L

#include <stdexcept>
#include <cstdlib>
#include <utility>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <list>

#include "versionheader.h"
#include "rarraymacros.h"
#include "shared_buffer.h"
#include "shared_shape.h"

namespace ra {
typedef ssize_t size_type;
enum class RESIZE { NO, ALLOWED };
}

// Forward definitions of ra::rarray<T,R> and ra::CommaOp

namespace ra { template<typename T,int R> class rarray;    }
namespace ra { template<typename T>       class CommaOp;   }

// Forward definitions to support array expressions //

// What type enumerates possible operators?
//typedef int ExOp;
#define ExOp class

// Definitions of streaming operator; implementation is in rarrayio
namespace ra {
template<typename T,int R> RA_INLINE_ std::istream& operator>>(std::istream &i, ra::rarray<T,R>& r);
template<typename T,int R> RA_INLINE_ std::ostream& operator<<(std::ostream &o, const ra::rarray<T,R>& r);
}
// Each operator creates a subexpression of the Expr<...>, which we forward-define first
namespace ra { 
template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3> class Expr;
}
   
namespace ra {

//------------------------------------------------//
//                                                //
//               I N T E R F A C E                //
//                                                //
//------------------------------------------------//
    
template<typename T,int R> 
class rarray {
    
  public:
    typedef T                                         value_type;       
    typedef ssize_t                                   difference_type;  // difference type for indices
    typedef ssize_t                                   size_type;        // type of indices
    typedef T*                                        iterator;         // iterator type
    typedef const T*                                  const_iterator;   // iterator type for constant access
    typedef typename PointerArray<T,R>::type          parray_t;         // shorthand for T*const*const*...
    typedef typename PointerArray<T,R>::noconst_type  noconst_parray_t; // shorthand for T***...
    RA_INLINE_ rarray();                                                                                                                  // constructor leaving rarray undefined 
    RA_INLINE_ explicit rarray(size_type n0);                                                                                             // constructor creating its own buffer for R=1
    RA_INLINE_ rarray(size_type n0, size_type n1);                                                                                        // constructor creating its own buffer for R=2
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2);                                                                                                                  // R=3
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3);                                                                                                    // R=4
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4);                                                                                      // R=5
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5);                                                                        // R=6
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6);                                                          // R=7
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7);                                            // R=8
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8);                              // R=9 
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9);                // R=10
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, size_type n10); // R=11
    RA_INLINE_ rarray(const size_type* extent);                                                                                                                                   // R>11
    RA_INLINE_ rarray(T* buffer, size_type n0);                                                                                           // constructor from an existing buffer for R=1
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1);                                                                                                                     // R=2
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2);                                                                                                       // R=3
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3);                                                                                         // R=4
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4);                                                                           // R=5
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5);                                                             // R=6
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6);                                               // R=7 
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7);                                 // R=8
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8);                   // R=9
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9);     // R=10
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, size_type n10);// R=11
    RA_INLINE_ rarray(T* buffer, const size_type* extent);                                                                                                                        // R>11
    RA_INLINEF rarray(const rarray<T,R> &a);                                      // copy constructor
    RA_INLINE_ rarray<T,R>& operator=(const rarray<T,R> &a);                      // array assignment operator
    rarray(rarray<T,R>&& x);                                                      // move constructor
    rarray<T,R>& operator=(rarray<T,R>&& x);                                      // move assignment operator
    RA_INLINE_ CommaOp<T> operator=(const T& e);                                  // Comma separated element assignment
    RA_INLINEF ~rarray();                                                         // destructor
    constexpr int rank() { return R; } 
    // Need constructor and assignment for expressions
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF explicit   rarray (const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator= (const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator+=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator-=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator*=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator/=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator%=(const Expr<T,R,AOP,A1,A2,A3>& e);
    //
    RA_INLINEF void clear();                                                      // clean up routine, make undefined
    RA_INLINE_ void reshape(size_type n0, RESIZE resize_allowed=RESIZE::NO);                                                                            // reshape shallow copy keeping the underlying data for R=1
    RA_INLINE_ void reshape(size_type n0, size_type n1, RESIZE resize_allowed=RESIZE::NO);                                                                                                                   // R=2
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, RESIZE resize_allowed=RESIZE::NO);                                                                                                     // R=3
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, RESIZE resize_allowed=RESIZE::NO);                                                                                       // R=4
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, RESIZE resize_allowed=RESIZE::NO);                                                                         // R=5
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, RESIZE resize_allowed=RESIZE::NO);                                                           // R=6
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, RESIZE resize_allowed=RESIZE::NO);                                             // R=7
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, RESIZE resize_allowed=RESIZE::NO);                               // R=8
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, RESIZE resize_allowed=RESIZE::NO);                 // R=9
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, RESIZE resize_allowed=RESIZE::NO);   // R=10
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, size_type n10, RESIZE resize_allowed=RESIZE::NO); // R=11
    RA_INLINE_ void reshape(const size_type* extent, RESIZE resize_allowed=RESIZE::NO);                                                                                                                      // R>11
    //
    RA_INLINE_ bool                is_clear()           const;                       // check if undefined
    RA_INLINE_ rarray<T,R>         copy()               const;                       // return a copy
    RA_INLINE_ size_type           extent(int i)        const;                       // retrieve array size in dimension i
    RA_INLINE_ const size_type*    shape()              const;                       // retrieve array sizes in all dimensions
    RA_INLINE_ size_type           size()               const;                       // retrieve the total number of elements  
    RA_INLINE_ T*                  data();                                           // return a T* to the internal data
    RA_INLINE_ const T*            data()               const;                       // return a T* to the internal data
    RA_INLINE_ parray_t            ptr_array()          const;                       // return a T*const*.. acting similarly to this rarray when using []:
    RA_INLINE_ noconst_parray_t    noconst_ptr_array()  const;                       // return a T**.. acting similarly to this rarray when using []:    
    RA_INLINE_ const rarray<const T,R>&  const_ref()          const;                       // create a reference to this that treats elements as constant:
    RA_INLINE_ void                fill(const T& value);                             // fill with uniform value
    //
    RA_INLINE_ iterator            begin();                                          // start of the content
    RA_INLINE_ const_iterator      begin()              const;                       // start of the content, when *this is constant
    RA_INLINE_ const_iterator      cbegin()             const;                       // start of the content, when *this can be constant and you need to be explicit
    RA_INLINE_ iterator            end();                                            // end of the content
    RA_INLINE_ const_iterator      end()                const;                       // end of the content, when *this is constant
    RA_INLINE_ const_iterator      cend()               const;                       // end of the content, when *this is constant and you need to be explicit about that
    RA_INLINE_ size_type           index(const T& a, int i) const;                   // if a an element in the array, get index i of that element
    RA_INLINE_ size_type           index(const iterator& iter, int i) const;         // if i points at an element in the array, get index i of that element
    RA_INLINE_ std::array<size_type,R> index(const T& a) const;           // if a an element in the array, get the indices of that element
    RA_INLINE_ std::array<size_type,R> index(const iterator& i) const;// if i points at an element in the array, get the indices of that element
    // old form of index functions:
    RA_INLINE_ size_type*          index(const T& a, size_type* ind) const;           // if a an element in the array, get the indices of that element
    RA_INLINE_ size_type*          index(const iterator& i, size_type* ind) const;          // if i points at an element in the array, get the indices of that element
    // access elements r
    RA_INLINEF rarray<T,R-1> at(size_type i);
    RA_INLINEF const rarray<T,R-1> at(size_type i) const;
    RA_INLINEF operator typename PointerArray<T,R>::type ();  // makes a[..][..] work, as well as automatic conversion
    RA_INLINEF operator typename PointerArray<const T,R>::type () const; 
    // for expressions
    RA_INLINEF const T& leval(size_type i) const;
  private:
    // allow rarray<T,R+1>::at(..) to create a proper rarray<T,R>
    // referencing data in current array
    friend class rarray<T,R+1>;
    rarray(shared_buffer<T>&& buffer, shared_shape<T,R>&& shape) :
        buffer_(std::forward<shared_buffer<T>>(buffer)),
        shape_(std::forward<shared_shape<T,R>>(shape))
    {}
    // need the following for rarray<T,R+1>::at(..) const
    rarray(const shared_buffer<T>&& buffer, shared_shape<T,R>&& shape) :
        buffer_(std::forward<shared_buffer<T>>(const_cast<shared_buffer<T>&& >(buffer))),
        shape_(std::forward<shared_shape<T,R>>(shape))
    {}
  private:
    shared_buffer<T>  buffer_;
    shared_shape<T,R> shape_;
        
}; // end definition rarray<T,R>

// stop "at(...)" recursion at R=0, a single scalar value
template<typename T> 
class rarray<T,0> {
  private:
    // for rarray<T,1>::at
    friend class rarray<T,1>;
    rarray(shared_buffer<T>&& buffer, const shared_shape<T,0>& shape) :
        buffer_(std::forward<shared_buffer<T>>(buffer))
    {}
    rarray(const shared_buffer<T>&& buffer, const shared_shape<T,0>& shape) :
        buffer_(std::forward<shared_buffer<T>>(const_cast<shared_buffer<T>&&>(buffer)))
    {}
    shared_buffer<T>  buffer_;
  public:
    ~rarray() {}
    RA_INLINEF operator T& () { return buffer_[0]; }
    RA_INLINEF operator const T& () const { return buffer_[0]; }
    RA_INLINE_ T& operator=(const T& e) { return buffer_[0] = e; }
    // for expressions
    RA_INLINEF const T& leval(size_type i) const;
}; // end definition rarray<T,0>
    
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

template<typename S>
rarray<S,1> linspace(S x1, S x2, int n=0, bool end_incl=true)
{
    if (n==0) {
        if (x2>x1)
            n = static_cast<int>(x2 - x1 + end_incl);
        else
            n = static_cast<int>(x1 - x2 + end_incl);
    }
    rarray<S,1> x(n);
    for (int i = 0; i < n; i++)
        x[i] = x1 + static_cast<S>(((x2-x1)*static_cast<long long int>(i))/(n-end_incl));
    if (end_incl)
        x[n-1] = x2;
    return x;
}

// T should be an arithemetic type, i.e, an integer, float or double. 
template<class T>
class Xrange {
  private:
    struct const_iterator {
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef std::input_iterator_tag iterator_category;
        const_iterator(T i, T di, T b): i_(i), di_(di), b_(b) {}
        bool operator!=(const const_iterator& other) const {
            return i_ != other.i_;
        }
        const const_iterator& operator++() {
            i_+=di_;
            if (di_>0 && i_ >= b_)
               i_ = b_;
            if (di_<0 && i_ <= b_)
               i_ = b_;
            return *this;
        }
        const T& operator*() {
            return i_;
        }
        T i_, di_, b_;
    };
    T a_, b_, d_;
  public:
    Xrange(T a, T b, T d)
        : a_(a), b_(a + static_cast<T>(static_cast<size_t>(std::ceil((b-a)/static_cast<double>(d)))*d)), d_(d) 
    {}
    const_iterator begin() const {
        return const_iterator(a_, d_, b_);
    }
    const_iterator end() const {
        return const_iterator(b_, d_, b_);
    }
};

template<class T>
Xrange<T> xrange(T end) {
    return Xrange<T>(static_cast<T>(0), end, static_cast<T>(1));
}

template<class S, class T>
Xrange<T> xrange(S begin, T end) {
    return Xrange<T>(static_cast<T>(begin), end, static_cast<T>(1));
}

template<class S, class T, class U>
Xrange<T> xrange(S begin, T end, U step) {
    return Xrange<T>(static_cast<T>(begin), end, static_cast<T>(step));
}
   
} // end namespace ra

//------------------------------------------------//
//                                                //
//          I M P L E M E N T A T I O N           //
//                                                //
//------------------------------------------------//

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray()
  : buffer_(),
    shape_()
{}
    
template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(ra::size_type n0)
  : buffer_(n0),
    shape_({n0}, buffer_.begin())
{
    static_assert( R==1, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1)
  : buffer_(n0*n1),
    shape_({n0,n1}, buffer_.begin())
{
    static_assert( R==2, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2)
  : buffer_(n0*n1*n2),
    shape_({n0,n1,n2}, buffer_.begin())
{
    static_assert( R==3, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3)
  : buffer_(n0*n1*n2*n3),
    shape_({n0,n1,n2,n3}, buffer_.begin())
{
    static_assert( R==4, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4)
  : buffer_(n0*n1*n2*n3*n4),
    shape_({n0,n1,n2,n3,n4}, buffer_.begin())
{
    static_assert( R==5, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5)
  : buffer_(n0*n1*n2*n3*n4*n5),
    shape_({n0,n1,n2,n3,n4,n5}, buffer_.begin())
{
    static_assert( R==6, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6)
  : buffer_(n0*n1*n2*n3*n4*n5*n6),
    shape_({n0,n1,n2,n3,n4,n5,n6}, buffer_.begin())
{
    static_assert( R==7, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7}, buffer_.begin())
{
    static_assert( R==8, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8}, buffer_.begin())
{
    static_assert( R==9, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}, buffer_.begin())
{
    static_assert( R==10, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10}, buffer_.begin())
{
    static_assert( R==11, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0)
  : buffer_(n0,  buffer),
    shape_({n0}, buffer)
{
    static_assert( R==1, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1)
  : buffer_(n0*n1,  buffer),
    shape_({n0,n1}, buffer)
{
    static_assert( R==2, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2)
  : buffer_(n0*n1*n2,  buffer),
    shape_({n0,n1,n2}, buffer)
{
    static_assert( R==3, "Incorrect number of dimensions in rarray constructor.");
}


template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3)
  : buffer_(n0*n1*n2*n3,  buffer),
    shape_({n0,n1,n2,n3}, buffer)
{
    static_assert( R==4, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4)
  : buffer_(n0*n1*n2*n3*n4,  buffer),
    shape_({n0,n1,n2,n3,n4}, buffer)
{
    static_assert( R==5, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5)
  : buffer_(n0*n1*n2*n3*n4*n5,  buffer),
    shape_({n0,n1,n2,n3,n4,n5}, buffer)
{
    static_assert( R==6, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6)
  : buffer_(n0*n1*n2*n3*n4*n5*n6,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6}, buffer)
{
    static_assert( R==7, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7}, buffer)
{
    static_assert( R==8, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8}, buffer)
{
    static_assert( R==9, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}, buffer)
{
    static_assert( R==10, "Incorrect number of dimensions in rarray constructor.");
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10}, buffer)
{
    static_assert( R==11, "Incorrect number of dimensions in rarray constructor.");
}

namespace ra {
RA_INLINE_ size_type mul(const size_type * x, int n) {
    size_type result = 1;
    for (int i=0;i<n;i++)
        result *= x[i];
    return result;
}
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(const size_type* extent)
  : buffer_(mul(extent,R)),
    shape_((const std::array<size_type,R>&)(*extent), buffer_.begin())
{}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, const size_type* extent)
  : buffer_(mul(extent,R), buffer),
    shape_((const std::array<size_type,R>&)(*extent), buffer)
{}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>::rarray(const rarray<T,R> &a)
  : buffer_(a.buffer_),
    shape_(a.shape_)
{
    // copy constructor
}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>::rarray(rarray<T,R>&& x)
  : buffer_(std::move(x.buffer_)),
    shape_(std::move(x.shape_))
{
    // move constructor
}

//

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::clear()
{
    shape_ = shared_shape<T,R>();
    buffer_ = shared_buffer<T>();
}

//

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0, RESIZE resize_allowed)
{
    static_assert( R==1, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0)) {
        
        shape_ = shared_shape<T,R>({n0}, buffer_.begin());

    } else {

        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0, size_type n1, RESIZE resize_allowed)
{
    static_assert( R==2, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0*n1
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1)) {
        
        shape_ = shared_shape<T,R>({n0,n1}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0, size_type n1, size_type n2, RESIZE resize_allowed)
{
    static_assert( R==3, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0*n1*n2
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2}, buffer_.begin());
        
    } else {
            
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}


template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3, RESIZE resize_allowed)
{
    static_assert( R==4, "Incorrect number of dimensions in rarray::reshape method.");

    if (size() == n0*n1*n2*n3
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4, RESIZE resize_allowed)
{
    static_assert( R==5, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0*n1*n2*n3*n4
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5, RESIZE resize_allowed)
{
    static_assert( R==6, "Incorrect number of dimensions in rarray::reshape method.");

    if (size() == n0*n1*n2*n3*n4*n5
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6, RESIZE resize_allowed)
{
    static_assert( R==7, "Incorrect number of dimensions in rarray::reshape method.");
        
    if (size() == n0*n1*n2*n3*n4*n5*n6
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7, RESIZE resize_allowed)
{
    static_assert( R==8, "Incorrect number of dimensions in rarray::reshape method.");
        
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8, RESIZE resize_allowed)
{
    static_assert( R==9, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7,n8}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9, RESIZE resize_allowed)
{
    static_assert( R==10, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8*n9
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8*n9)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10, RESIZE resize_allowed)
{
    static_assert( R==11, "Incorrect number of dimensions in rarray::reshape method.");
    
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10}, buffer_.begin());

    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));

    }
}

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::reshape(const size_type* newshape, RESIZE resize_allowed)
{
    if (size() == mul(newshape,R)
        or (resize_allowed == RESIZE::ALLOWED and size() >= mul(newshape,R))) {
        
        shape_ = shared_shape<T,R>((const std::array<size_type,R>&)(*newshape), buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::~rarray()
{}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, int R> RA_INLINE_
ra::CommaOp<T> ra::rarray<T,R>::operator=(const T& e)
{
    // Comma separated element assignment: puts the first one in and prepares for more
    RA_CHECKORSAY(parray_!=nullptr, "assignment to unsized array");
    RA_CHECKORSAY(size()>0,"assignment with more elements than in array");
    T* first = &(buffer_[0]);
    if (size() > 0)
        *first = e;
    else
        return ra::CommaOp<T>(nullptr, nullptr);
    ra::CommaOp<T> co(first+1, first+size()-1); 
    return co;
}

// comma operator constructor
template<typename T> RA_INLINE_
ra::CommaOp<T>::CommaOp(T* ptr, T* last)
: ptr_(ptr), last_(last)
{ 
    // fill remainder of array with zeros
    RA_CHECKORSAY(ptr_!=nullptr and last_!=nullptr, "invalid comma operator");
}

// comma operator; set next element
template<typename T> RA_INLINE_
ra::CommaOp<T>& ra::CommaOp<T>::operator,(const T& e)
{ 
    // puts the next number into the array.
    RA_CHECKORSAY(ptr_!=nullptr and last_!=nullptr, "invalid comma operator");
    RA_CHECKORSAY(ptr_<=last_, "assignment with more elements than in array");
    if (ptr_ and ptr_ <= last_)
        *ptr_++ = e;
    return *this; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_
bool ra::rarray<T,R>::is_clear() const
{
    // check if empty
    return buffer_.cbegin() == nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_
ra::rarray<T,R> ra::rarray<T,R>::copy() const {
    // return a copy
    rarray<T,R> clone;
    clone.buffer_ = buffer_.copy();
    // clone.shape_ = ra::shared_shape<T,R>(shape_.extent(), clone.buffer_.begin());
    clone.shape_ = shape_.copy();
    clone.shape_.relocate(clone.buffer_.begin()); // something is wrong with relocate for some types
    return clone;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINEF
ra::rarray<T,R-1> ra::rarray<T,R>::at(size_type i)
{
    // subarray access with bounds checking
    if (i < 0 or i >= extent(0))
        throw std::out_of_range("rarray<T,R>::at");
    size_type stride = size()/extent(0);
    return ra::rarray<T,R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
}

template<typename T, int R> RA_INLINEF
const ra::rarray<T,R-1> ra::rarray<T,R>::at(size_type i) const
{
    // const subarray access with bounds checking
    if (i < 0 or i >= extent(0))
        throw std::out_of_range("rarray<T,R>::at");
    size_type stride = size()/extent(0);
    return ra::rarray<T,R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>::operator typename PointerArray<T,R>::type () 
{
    return shape_.ptrs(); // makes a[..][..] = ... work.
}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>::operator typename PointerArray<const T,R>::type () const 
{
    return shape_.ptrs(); // makes a[..][..] work.
}

template<typename T, int R> RA_INLINE_
const ra::size_type* ra::rarray<T,R>::shape() const
{
    // retrieve array sizes in all dimensions
    return &(shape_.extent()[0]);
}

template<typename T, int R> RA_INLINE_
typename ra::rarray<T,R>::parray_t ra::rarray<T,R>::ptr_array() const 
{
    return shape_.ptrs();
}

template<typename T, int R> RA_INLINE_
typename ra::rarray<T,R>::noconst_parray_t ra::rarray<T,R>::noconst_ptr_array() const 
{
    return const_cast<noconst_parray_t>(shape_.ptrs());
}

template<typename T, int R> RA_INLINE_
const typename ra::rarray<const T,R>& ra::rarray<T,R>::const_ref() const 
{
    return reinterpret_cast<const rarray<const T,R>&>(*this);
}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>& ra::rarray<T,R>::operator=(const rarray<T,R> &a) 
{
    buffer_ = a.buffer_;
    shape_ = a.shape_;
    return *this;
}
template<typename T, int R> RA_INLINEF
ra::rarray<T,R>& ra::rarray<T,R>::operator=(rarray<T,R> &&a) 
{
    buffer_ = std::move(a.buffer_);
    shape_ = std::move(a.shape_);
    return *this;
}

template<typename T, int R> RA_INLINEF
void ra::rarray<T,R>::fill(const T& value) {
    // fill with uniform value
    buffer_.assign(value);
}

template<typename T, int R> RA_INLINEF
ra::size_type ra::rarray<T,R>::size() const {
    // retrieve the total number of elements
    return shape_.size(); 
}

template<typename T, int R> RA_INLINEF
T* ra::rarray<T,R>::data()
{
    // return a T* to the internal data
    return buffer_.begin(); 
}

template<typename T, int R> RA_INLINEF
const T* ra::rarray<T,R>::data() const
{
    // return a T* to the internal data
    return buffer_.begin(); 
}

template<typename T, int R> RA_INLINEF
ra::size_type ra::rarray<T,R>::extent(int i) const
{
    // retrieve array size in dimension i
    return shape_.extent(i);
}

template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::iterator ra::rarray<T,R>::begin()
{
    // start of the content
    return buffer_.begin();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::begin() const
{
    // start of the content, when *this is constant
    return buffer_.begin();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::cbegin() const
{
    // start of the content, when *this can be constant and you need to be explicit
    return buffer_.cbegin();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::iterator ra::rarray<T,R>::end()
{
    // end of the content
    return buffer_.end();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::end() const
{
    // end of the content, when *this is constant
    return buffer_.end();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::cend() const
{
    // end of the content, when *this is constant and you need to be explicit about that
    return buffer_.cend();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// retrieve indices of an element

template<typename T,int R> RA_INLINE_
ra::size_type* ra::rarray<T,R>::index(const iterator&i, size_type* ind) const
{
    // retrieve indexes within *this of the element pointed to by i
    // Puts them in preexisting array and returns pointer
    return index(*i, ind);
}

template<typename T,int R> RA_INLINE_
ra::size_type* ra::rarray<T,R>::index(const T& a, size_type* ind) const
{
    // retrieve indexes within *this of the element a. puts them in preexisting array and returns pointer
    ptrdiff_t linearindex = &a - &(buffer_[0]);
    RA_CHECKORSAY(linearindex >= 0 and linearindex < size(), "element not in array");
    int j = R;
    const size_type* extent_ = shape();
    while (j-->0) {
        ind[j] = linearindex % extent_[j];
        linearindex /= extent_[j];
    }
    return ind;
}

template<typename T,int R> RA_INLINE_
std::array<ra::size_type,R> ra::rarray<T,R>::index(const iterator&i) const
{
    // retrieve indexes within *this of the element pointed to by i
    // Puts them in preexisting array and returns pointer
    return index(*i);
}

template<typename T,int R> RA_INLINE_
std::array<ra::size_type,R> ra::rarray<T,R>::index(const T& a) const
{
    // retrieve indexes within *this of the element a. puts them in preexisting array and returns pointer
    std::array<size_type,R> ind;
    ptrdiff_t linearindex = &a - &(buffer_[0]);
    RA_CHECKORSAY(linearindex >=0 and linearindex < size(), "element not in array");
    int j = R;
    const size_type* extent_ = shape();
    while (j-->0) {
        ind[j] = linearindex % extent_[j];
        linearindex /= extent_[j];
    }
    return ind;
}

template<typename T,int R> RA_INLINE_
ra::size_type ra::rarray<T,R>::index(const iterator&iter, int i) const
{
    // retrieve index in dimension i within *this of the element pointed to by i
    return index(*iter, i);
}

template<typename T,int R> RA_INLINE_
ra::size_type ra::rarray<T,R>::index(const T& a, int i) const
{
    // retrieve index in dimension i within *this of the element a
    ptrdiff_t linearindex = &a - &(buffer_[0]);
    if (linearindex < 0 or linearindex >= size())
        throw ("element not in array");
    const size_type* extent_ = shape();
    for (int j = R-1; j > i; j--) 
        linearindex /= extent_[j];
    return linearindex % extent_[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename A> RA_INLINE_ 
int ra::extent_given_byte_size(A a[], int i, int byte_size) 
{
    // find shape of automatic arrays, given the size of the array
    RA_CHECKORSAY(i>=0 and i<1, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(A);
        default: return 1;
    }
}

template<typename A,int Z> RA_INLINE_ 
int ra::extent_given_byte_size(A a[][Z], int i, int byte_size) 
{
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename A,int R> RA_INLINE_ 
int ra::extent_given_byte_size(const ra::rarray<A,R>& a, int i, int byte_size) 
{
    return a.extent(i);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// convert to rarray

template<typename A> RA_INLINE_ 
ra::rarray<A,1> ra::make_rarray_given_byte_size(A a[], int byte_size) 
{
    const int z = byte_size/sizeof(A);
    return ra::rarray<A,1>(a,z);
}

template<typename A,int Z> RA_INLINE_ 
ra::rarray<A,2> ra::make_rarray_given_byte_size(A a[][Z], int byte_size) 
{
    const int y = byte_size/sizeof(A)/Z;
    return ra::rarray<A,2>(*a,y,Z);
}

template<typename A,int Y,int Z> RA_INLINE_ 
ra::rarray<A,3> ra::make_rarray_given_byte_size(A a[][Y][Z], int byte_size) 
{
    const int x = byte_size/sizeof(A)/Z/Y;
    return ra::rarray<A,3>(**a,x,Y,Z);
}

template<typename A,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,4> ra::make_rarray_given_byte_size(A a[][X][Y][Z], int byte_size) 
{
    const int w = byte_size/sizeof(A)/X/Z/Y;
    return ra::rarray<A,4>(***a,w,X,Y,Z);
}

template<typename A,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,5> ra::make_rarray_given_byte_size(A a[][W][X][Y][Z], int byte_size) 
{
    const int v = byte_size/sizeof(A)/W/X/Z/Y;
    return ra::rarray<A,5>(****a,v,W,X,Y,Z);
}

template<typename A,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,6> ra::make_rarray_given_byte_size(A a[][V][W][X][Y][Z], int byte_size) 
{
    const int u = byte_size/sizeof(A)/V/W/X/Z/Y;
    return ra::rarray<A,6>(*****a,u,V,W,X,Y,Z);
}

template<typename A,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,7> ra::make_rarray_given_byte_size(A a[][U][V][W][X][Y][Z], int byte_size) 
{
    const int t = byte_size/sizeof(A)/U/V/W/X/Z/Y;
    return ra::rarray<A,7>(******a,t,U,V,W,X,Y,Z);
}

template<typename A,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,8> ra::make_rarray_given_byte_size(A a[][T][U][V][W][X][Y][Z], int byte_size) 
{
    const int s = byte_size/sizeof(A)/T/U/V/W/X/Z/Y;
    return ra::rarray<A,8>(*******a,s,T,U,V,W,X,Y,Z);
}

template<typename A,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,9> ra::make_rarray_given_byte_size(A a[][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    const int r = byte_size/sizeof(A)/S/T/U/V/W/X/Z/Y;
    return ra::rarray<A,9>(********a,r,S,T,U,V,W,X,Y,Z);
}

template<typename A,int R,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,10> ra::make_rarray_given_byte_size(A a[][R][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    const int q = byte_size/sizeof(A)/R/S/T/U/V/W/X/Z/Y;
    return ra::rarray<A,10>(*********a,q,R,S,T,U,V,W,X,Y,Z);
}

template<typename A,int Q,int R,int S,int T,int U,int V,int W,int X,int Y,int Z> RA_INLINE_ 
ra::rarray<A,11> ra::make_rarray_given_byte_size(A a[][Q][R][S][T][U][V][W][X][Y][Z], int byte_size) 
{
    const int p = byte_size/sizeof(A)/Q/R/S/T/U/V/W/X/Z/Y;
    return ra::rarray<A,11>(**********a,p,Q,R,S,T,U,V,W,X,Y,Z);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> RA_INLINE_ 
ra::rarray<T,R> ra::make_rarray_given_byte_size(ra::rarray<T,R> a, int byte_size) 
{
    return a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Include I/O always (from version 2.3 onwards)
#include "rarrayio.h"

// Get rid of the macros
#include "rarraydelmacros.h"

// Global namespace stuff
    
#define EXTENT(A,I)  ra::extent_given_byte_size(A,I,sizeof(A))
#define RARRAY(A)    ra::make_rarray_given_byte_size(A,sizeof(A))
#define INDEX(A,X,I) RARRAY(A).index(X,I)
#define as_rarray(A) ra::make_rarray_given_byte_size(A,sizeof(A)) 

// for now:
using ra::rarray;
using ra::linspace;
using ra::xrange; 

// add rvector, rmatrix and rtensor shortcut types
template<typename T> using rvector = rarray<T,1>;
template<typename T> using rmatrix = rarray<T,2>;
template<typename T> using rtensor = rarray<T,3>;

#else
#error "This file requires C++11 or newer support."
#endif

#endif
