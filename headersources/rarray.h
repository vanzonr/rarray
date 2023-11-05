//
// rarray - Runtime arrays: template classes for pointer-based,
//          runtime, reference counted, multi-dimensional
//          arrays.  Documentation in rarraydoc.pdf
//
// Copyright (c) 2013-2023  Ramses van Zon
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

#include "versionheader.h"
#include "rarraymacros.h"
#include "shared_buffer.h"
#include "shared_shape.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

// Forward definitions of ra::rarray<T,R> and ra::CommaOp
namespace ra {
typedef ssize_t size_type;
enum class RESIZE { NO, ALLOWED };
template<typename T,int R> class rarray;
template<typename T> class CommaOp;
// // Definitions of streaming operator; implementation is in rarrayio
template<typename T,int R> RA_INLINE_ std::istream& operator>>(std::istream &i, rarray<T,R>& r);
template<typename T,int R> RA_INLINE_ std::ostream& operator<<(std::ostream &o, const rarray<T,R>& r);
// Definitions for (repeated) bracket access
template<typename T,int R,typename P> class _Bracket;
template<typename T,int R,typename P> class _ConstBracket;
// TODO: Each operator will create a subexpression of the Expr<...>, which we forward-define first
// Forward definitions to support array expressions //
// What type enumerates possible operators?
//typedef int ExOp;
#define ExOp class
template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3> class Expr;
} // end namespace ra (forward declarations)
   
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
    // constructors creating its own buffer for various R values
    template<int R_=R,class=typename std::enable_if<R_==1>::type>
    RA_INLINE_ explicit rarray(size_type n0);                                                                                                                                     // R=1
    template<int R_=R,class=typename std::enable_if<R_==2>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1);                                                                                                                                // R=2
    template<int R_=R,class=typename std::enable_if<R_==3>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2);                                                                                                                  // R=3
    template<int R_=R,class=typename std::enable_if<R_==4>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3);                                                                                                    // R=4
    template<int R_=R,class=typename std::enable_if<R_==5>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4);                                                                                      // R=5
    template<int R_=R,class=typename std::enable_if<R_==6>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5);                                                                        // R=6
    template<int R_=R,class=typename std::enable_if<R_==7>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6);                                                          // R=7
    template<int R_=R,class=typename std::enable_if<R_==8>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7);                                            // R=8
    template<int R_=R,class=typename std::enable_if<R_==9>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8);                              // R=9 
    template<int R_=R,class=typename std::enable_if<R_==10>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9);                // R=10
    template<int R_=R,class=typename std::enable_if<R_==11>::type>
    RA_INLINE_ rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, size_type n10); // R=11
    explicit RA_INLINE_ rarray(const size_type* anextent);                                                                                                                                 // any R
    template<int R_=R,class=typename std::enable_if<R_==1>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0);                                                                                           // constructor from an existing buffer for R=1
    template<int R_=R,class=typename std::enable_if<R_==2>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1);                                                                                                                     // R=2
    template<int R_=R,class=typename std::enable_if<R_==3>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2);                                                                                                       // R=3
    template<int R_=R,class=typename std::enable_if<R_==4>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3);                                                                                         // R=4
    template<int R_=R,class=typename std::enable_if<R_==5>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4);                                                                           // R=5
    template<int R_=R,class=typename std::enable_if<R_==6>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5);                                                             // R=6
    template<int R_=R,class=typename std::enable_if<R_==7>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6);                                               // R=7 
    template<int R_=R,class=typename std::enable_if<R_==8>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7);                                 // R=8
    template<int R_=R,class=typename std::enable_if<R_==9>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8);                   // R=9
    template<int R_=R,class=typename std::enable_if<R_==10>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9);     // R=10
    template<int R_=R,class=typename std::enable_if<R_==11>::type>
    RA_INLINE_ rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, size_type n10);// R=11
    RA_INLINE_ rarray(T* buffer, const size_type* anextent);                                                                                                                      // any R
    // construct from automatic array
    template<std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==1>::type>
    RA_INLINE_ explicit rarray(T (&a)[Z]);
    template<std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==2>::type>
    RA_INLINE_ explicit rarray(T (&a)[Y][Z]); 
    template<std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==3>::type>
    RA_INLINE_ explicit rarray(T (&a)[X][Y][Z]);
    template<std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==4>::type>
    RA_INLINE_ explicit rarray(T (&a)[W][X][Y][Z]);
    template<std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==5>::type>
    RA_INLINE_ explicit rarray(T (&a)[V][W][X][Y][Z]);
    template<std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==6>::type>
    RA_INLINE_ explicit rarray(T (&a)[U][V][W][X][Y][Z]);
    template<std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==7>::type>
    RA_INLINE_ explicit rarray(T (&a)[T_][U][V][W][X][Y][Z]);
    template<std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==8>::type>
    RA_INLINE_ explicit rarray(T (&a)[S][T_][U][V][W][X][Y][Z]); 
    template<std::size_t R_,std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==9>::type>
    RA_INLINE_ explicit rarray(T (&a)[R_][S][T_][U][V][W][X][Y][Z]);
    template<std::size_t Q,std::size_t R_,std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==10>::type>
    RA_INLINE_ explicit rarray(T (&a)[Q][R_][S][T_][U][V][W][X][Y][Z]); 
    template<std::size_t P,std::size_t Q,std::size_t R_,std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,
             int R__=R,typename=typename std::enable_if<R__==11>::type>
    RA_INLINE_ explicit rarray(T (&a)[P][Q][R_][S][T_][U][V][W][X][Y][Z]); 
    RA_INLINEF rarray(const rarray<T,R> &a) noexcept;                             // copy constructor
    RA_INLINE_ rarray<T,R>& operator=(const rarray<T,R> &a) noexcept;             // array assignment operator
    rarray(rarray<T,R>&& x) noexcept;                                             // move constructor
    rarray<T,R>& operator=(rarray<T,R>&& x) noexcept;                             // move assignment operator
    RA_INLINE_ CommaOp<T> operator=(const T& e) RA_NOEXCEPT(std::is_nothrow_copy_constructible<T>());  // Comma separated element assignment
    RA_INLINEF ~rarray();                                                         // destructor
    constexpr int rank() const noexcept { return R; } 
    // Need constructor and assignment for expressions
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF explicit   rarray (const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator= (const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator+=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator-=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator*=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator/=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_INLINEF rarray& operator%=(const Expr<T,R,AOP,A1,A2,A3>& e);
    //
    RA_INLINEF void clear() noexcept;                                              // clean up routine, make undefined
    template<int R_=R,class=typename std::enable_if<R_==1>::type>
    RA_INLINE_ void reshape(size_type n0, RESIZE resize_allowed=RESIZE::NO);                                                                            // reshape shallow copy keeping the underlying data for R=1
    template<int R_=R,class=typename std::enable_if<R_==2>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, RESIZE resize_allowed=RESIZE::NO);                                                                                                                   // R=2
    template<int R_=R,class=typename std::enable_if<R_==3>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, RESIZE resize_allowed=RESIZE::NO);                                                                                                     // R=3
    template<int R_=R,class=typename std::enable_if<R_==4>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, RESIZE resize_allowed=RESIZE::NO);                                                                                       // R=4
    template<int R_=R,class=typename std::enable_if<R_==5>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, RESIZE resize_allowed=RESIZE::NO);                                                                         // R=5
    template<int R_=R,class=typename std::enable_if<R_==6>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, RESIZE resize_allowed=RESIZE::NO);                                                           // R=6
    template<int R_=R,class=typename std::enable_if<R_==7>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, RESIZE resize_allowed=RESIZE::NO);                                             // R=7
    template<int R_=R,class=typename std::enable_if<R_==8>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, RESIZE resize_allowed=RESIZE::NO);                               // R=8
    template<int R_=R,class=typename std::enable_if<R_==9>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, RESIZE resize_allowed=RESIZE::NO);                 // R=9
    template<int R_=R,class=typename std::enable_if<R_==10>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, RESIZE resize_allowed=RESIZE::NO);   // R=10
    template<int R_=R,class=typename std::enable_if<R_==11>::type>
    RA_INLINE_ void reshape(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4, size_type n5, size_type n6, size_type n7, size_type n8, size_type n9, size_type n10, RESIZE resize_allowed=RESIZE::NO); // R=11
    RA_INLINE_ void reshape(const size_type* extent, RESIZE resize_allowed=RESIZE::NO);                                                                                                                      // any R
    //
    RA_INLINE_ bool                empty()           const noexcept;              // check if undefined
    RA_INLINE_ rarray<T,R>         copy()               const;                       // return a copy
    RA_INLINE_ size_type           extent(int i)        const;                       // retrieve array size in dimension i
    RA_INLINEF const size_type*    shape()              const noexcept;              // retrieve array sizes in all dimensions
    RA_INLINE_ size_type           size()               const noexcept;              // retrieve the total number of elements  
    RA_INLINE_ T*                  data()               noexcept;                    // return a T* to the internal data
    RA_INLINE_ const T*            data()               const noexcept;              // return a T* to the internal data
    RA_INLINE_ parray_t            ptr_array()          const noexcept;              // return a T*const*.. acting similarly to this rarray when using []:
    RA_INLINE_ noconst_parray_t    noconst_ptr_array()  const noexcept;              // return a T**.. acting similarly to this rarray when using []:    
    RA_INLINE_ const rarray<const T,R>&  const_ref()    const noexcept;              // create a reference to this that treats elements as constant:
    RA_INLINE_ void                fill(const T& value);                             // fill with uniform value
    //
    RA_INLINE_ iterator            begin()              noexcept;                    // start of the content
    RA_INLINE_ const_iterator      begin()              const noexcept;              // start of the content, when *this is constant
    RA_INLINE_ const_iterator      cbegin()             const noexcept;              // start of the content, when *this can be constant and you need to be explicit
    RA_INLINE_ iterator            end()                noexcept;                    // end of the content
    RA_INLINE_ const_iterator      end()                const noexcept;              // end of the content, when *this is constant
    RA_INLINE_ const_iterator      cend()               const noexcept;              // end of the content, when *this is constant and you need to be explicit about that
    RA_INLINE_ size_type           index(const T& a, int i) const;                   // if a an element in the array, get index i of that element
    RA_INLINE_ size_type           index(const iterator& iter, int i) const;         // if i points at an element in the array, get index i of that element
    RA_INLINE_ std::array<size_type,R> index(const T& a) const;           // if a an element in the array, get the indices of that element
    RA_INLINE_ std::array<size_type,R> index(const iterator& i) const;// if i points at an element in the array, get the indices of that element
    // old form of index functions:
    RA_INLINE_ size_type*          index(const T& a, size_type* ind) const;           // if a an element in the array, get the indices of that element
    RA_INLINE_ size_type*          index(const iterator& i, size_type* ind) const;          // if i points at an element in the array, get the indices of that element
    // access elements r
    template<class U=rarray<T,R-1>>
    RA_INLINEF typename std::enable_if<R!=1,U>::type at(size_type i);
    template<class U=T&>
    RA_INLINEF typename std::enable_if<R==1,U>::type at(size_type i);
    template<class U=const rarray<T,R-1>>
    RA_INLINEF typename std::enable_if<R!=1,U>::type at(size_type i) const;
    template<class U=const T&>
    RA_INLINEF typename std::enable_if<R==1,U>::type at(size_type i) const;
    // for square bracket access:
    template<class U=T> typename std::enable_if<R==1,
    U&
    >::type
    operator[](ssize_t index) {
        RA_CHECKORSAY(index >= 0 and index < extent(0), "index out of range of array");
        return buffer_.begin()[index];
    }
    template<class U=T> typename std::enable_if<R!=1,
    typename ra::_Bracket<U,R-1,rarray>
    >::type
    operator[](ssize_t index) {
        return { *this, index, this->shape() };
    }
    template<class U=T> typename std::enable_if<R==1,
    const U&
    >::type
    operator[](ssize_t index) const {
        RA_CHECKORSAY(index >= 0 and index < extent(0), "index out of range of array");
        return buffer_.cbegin()[index];
    }
    template<class U=T> typename std::enable_if<R!=1,
    typename ra::_ConstBracket<U,R-1,rarray>
    >::type
    operator[](ssize_t index) const {
        return { *this, index, this->shape() };
    }
    // allow c++23 multidimentional subscripts
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_INLINEF typename std::enable_if<R==sizeof...(Ts)+1,T&>::type
    operator[](ssize_t index, Ts... args) {
        return operator[](index)[args...];
    }
    template<typename... Ts>
    RA_INLINEF typename std::enable_if<R==sizeof...(Ts)+1,const T&>::type
    operator[](ssize_t index, Ts... args) const {
        return operator[](index)[args...];
    }
    #endif
    // TODO: for expressions
    RA_INLINEF const T& leval(size_type i) const;
  private:
    // allow _*Bracket to access _at
    friend class _Bracket<T,R-1,rarray<T,R>>;
    friend class _ConstBracket<T,R-1,rarray<T,R>>;
    RA_INLINEF typename PointerArray<T,R-1>::type _at(ssize_t index);
    RA_INLINEF typename PointerArray<T,R-1>::type _at(ssize_t index) const;
    // allow rarray<T,R+1>::at(..) to create a proper rarray<T,R>
    // referencing data in current array
    friend class rarray<T,R+1>;    
    rarray(shared_buffer<T>&& abuffer, shared_shape<T,R>&& ashape) :
        buffer_(std::forward<shared_buffer<T>>(abuffer)),
        shape_(std::forward<shared_shape<T,R>>(ashape))
    {}
    // need the following for rarray<T,R+1>::at(..) const
    rarray(const shared_buffer<T>&& abuffer, shared_shape<T,R>&& ashape) :
        buffer_(std::forward<shared_buffer<T>>(const_cast<shared_buffer<T>&& >(abuffer))),
        shape_(std::forward<shared_shape<T,R>>(ashape))
    {}
    shared_buffer<T>  buffer_;
    shared_shape<T,R> shape_;
        
}; // end definition rarray<T,R>
    
// Class to facilitate assignment from a comma separated list
template<typename T>
class CommaOp {
  public:
    RA_INLINEF CommaOp& operator,(const T& e);                         // puts the next number into the array.
  private:
    RA_INLINEF CommaOp(T* ptr, T* last) RA_NOEXCEPT(true); 
    T *ptr_;                                                           // points to next element to be filled
    T * const last_;                                                   // points to last element
    template<typename,int> friend class rarray;
    template<typename,int> friend class subrarray;
};

// Classes for repeated bracket access to elements    
template<typename T,int R,typename P>
class _Bracket {
  private:
    P&             parent_; // what array/array expre is being accessed
    ssize_t        index_;  // at what index
    const ssize_t* shape_;  // what is the shape of the result
  public:
    // implement square brackets to go to the next level:
    RA_INLINEF _Bracket<T,R-1,_Bracket> operator[](ssize_t nextindex) noexcept(noboundscheck);
    // implement implicit conversion to whatever parent.at() gives:
    RA_INLINEF operator decltype(parent_.at(index_)) ();
    _Bracket(const _Bracket&) = delete;
    _Bracket(const _Bracket&&) = delete;
    _Bracket& operator=(const _Bracket&) = delete;                              
    _Bracket& operator=(const _Bracket&&) = delete;      
  private:
    RA_INLINEF _Bracket(P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck);
    RA_INLINEF auto _at(ssize_t nextindex) noexcept(noboundscheck) -> decltype(parent_._at(index_)[nextindex]);
    template<typename T2,int R2> friend class ra::rarray; // allow descending
    template<typename T2,int R2,typename P2> friend class ra::_Bracket; // allow descending   
  public:
    RA_INLINEF auto at(ssize_t nextindex) -> decltype(parent_.at(index_).at(nextindex));
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_INLINEF typename std::enable_if<R==sizeof...(Ts)+1,T&>::type
    operator[](ssize_t nextindex, Ts... args) {
        return operator[](nextindex)[args...];
    }
    #endif
};

template<typename T,typename P>
class _Bracket<T,1,P> {
  private:
    P&             parent_; // what array/array expre is being accessed
    ssize_t        index_;  // at what index
    const ssize_t* shape_;  // what is the shape of the result
  public:
    // implement square brackets to go to the next level:
    RA_INLINEF T& operator[](ssize_t nextindex) noexcept(noboundscheck);
    // implement implicit conversion to whatever parent.at() gives:
    RA_INLINEF operator decltype(parent_.at(index_)) ();
    _Bracket(const _Bracket&) = delete;
    _Bracket(const _Bracket&&) = delete;
    _Bracket& operator=(const _Bracket&) = delete;                              
    _Bracket& operator=(const _Bracket&&) = delete;      
  private:
    RA_INLINEF _Bracket(P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck);
    RA_INLINEF auto _at(ssize_t nextindex) noexcept(noboundscheck) -> decltype(parent_._at(index_)[nextindex]);
    template<typename T2,int R2> friend class ra::rarray; // allow descending

    template<typename T2,int R2,typename P2> friend class ra::_Bracket; // allow descending   
  public:
    RA_INLINEF auto at(ssize_t nextindex) -> decltype(parent_.at(index_).at(nextindex));
};

template<typename T,int R,typename P>
class _ConstBracket {
  private:
    const P&       parent_;
    ssize_t        index_;
  public:
    RA_INLINEF _ConstBracket<T,R-1,_ConstBracket> operator[](ssize_t nextindex) const noexcept(noboundscheck);
    RA_INLINEF operator decltype(parent_.at(index_)) ();
    _ConstBracket(const _ConstBracket&) = delete;
    _ConstBracket(const _ConstBracket&&) = delete;
    _ConstBracket& operator=(const _ConstBracket&) = delete;                              
    _ConstBracket& operator=(const _ConstBracket&&) = delete;
  private:
    const ssize_t* shape_;
    RA_INLINEF _ConstBracket(const P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck);
    RA_INLINEF auto _at(ssize_t nextindex) const noexcept(noboundscheck) -> decltype(parent_._at(index_)[nextindex]);
    template<typename T2,int R2> friend class ra::rarray; // allow descending
    template<typename T2,int R2,typename P2> friend class ra::_ConstBracket;
  public:
    RA_INLINEF auto at(ssize_t nextindex) const -> decltype(parent_.at(index_).at(nextindex));
    // allow c++23 multidimentional subscripts
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_INLINEF typename std::enable_if<R==sizeof...(Ts)+1,const T&>::type
    operator[](ssize_t nextindex, Ts... args) {
        return operator[](nextindex)[args...];
    }
    #endif
};

template<typename T,typename P>
class _ConstBracket<T,1,P> {
  private:
    const P&       parent_;
    ssize_t        index_;
  public:
    RA_INLINEF const T& operator[](ssize_t nextindex) const noexcept(noboundscheck);
    RA_INLINEF operator decltype(parent_.at(index_)) ();
    _ConstBracket(const _ConstBracket&) = delete;
    _ConstBracket(const _ConstBracket&&) = delete;
    _ConstBracket& operator=(const _ConstBracket&) = delete;                              
    _ConstBracket& operator=(const _ConstBracket&&) = delete;
  private:
    const ssize_t* shape_;
    RA_INLINEF _ConstBracket(const P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck);
    RA_INLINEF auto _at(ssize_t nextindex) const noexcept(noboundscheck) -> decltype(parent_._at(index_)[nextindex]);
    template<typename T2,int R2> friend class ra::rarray; // allow descending
    template<typename T2,int R2,typename P2> friend class ra::_ConstBracket;
  public:
    RA_INLINEF auto at(ssize_t nextindex) const -> decltype(parent_.at(index_).at(nextindex));
};

template<typename S>
rarray<S,1> linspace(S x1, S x2, int n=0, bool end_incl=true) //// TODO: Choose better integer type for n
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
        const_iterator(): i_(0), di_(1), b_(0) {}
        const_iterator(T i, T di, T b): i_(i), di_(di), b_(b) {}
        bool operator!=(const const_iterator& other) const {
            return i_ != other.i_;
        }
        bool operator==(const const_iterator& other) const {
            return i_ == other.i_ && di_ == other.di_ && b_ == other.b_;
        }
        const_iterator& operator++() {
            i_+=di_;
            if (di_>0 && i_ >= b_)
               i_ = b_;
            if (di_<0 && i_ <= b_)
               i_ = b_;
            return *this;
        }
        const_iterator operator++(int) {
            const const_iterator temp = *this;
            this->operator++();
            return temp;
        }
        const T& operator*() const {
            return i_;
        }
        T i_, di_, b_;
    };
    T a_, b_, d_;
  public:
    Xrange(T a, T b, T d)
        : a_(a), b_(a + static_cast<T>(static_cast<T>(std::ceil(static_cast<double>(b-a)/static_cast<double>(d)))*d)), d_(d) 
    {}
    const_iterator begin() const {
        return const_iterator(a_, d_, b_);
    }
    const const_iterator end() const {
        return const_iterator(b_, d_, b_);
    }
    size_t size() const {
        return static_cast<size_t>((b_-a_)/d_);
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
    
template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(ra::size_type n0)
  : buffer_(n0),
    shape_({n0}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1)
  : buffer_(n0*n1),
    shape_({n0,n1}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2)
  : buffer_(n0*n1*n2),
    shape_({n0,n1,n2}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3)
  : buffer_(n0*n1*n2*n3),
    shape_({n0,n1,n2,n3}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4)
  : buffer_(n0*n1*n2*n3*n4),
    shape_({n0,n1,n2,n3,n4}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5)
  : buffer_(n0*n1*n2*n3*n4*n5),
    shape_({n0,n1,n2,n3,n4,n5}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6)
  : buffer_(n0*n1*n2*n3*n4*n5*n6),
    shape_({n0,n1,n2,n3,n4,n5,n6}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[Z])
  : buffer_(Z,  a),
    shape_({Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[Y][Z]) 
   : buffer_(Y*Z,  *a),
     shape_({Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[X][Y][Z]) 
   : buffer_(X*Y*Z,  **a),
     shape_({X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t W, std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[W][X][Y][Z]) 
   : buffer_(W*X*Y*Z,  ***a),
     shape_({W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[V][W][X][Y][Z]) 
   : buffer_(V*W*X*Y*Z,  ****a),
     shape_({V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[U][V][W][X][Y][Z]) 
   : buffer_(U*V*W*X*Y*Z,  *****a),
     shape_({U,V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[T_][U][V][W][X][Y][Z]) 
   : buffer_(T_*U*V*W*X*Y*Z,  ******a),
     shape_({T_,U,V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[S][T_][U][V][W][X][Y][Z]) 
   : buffer_(S*T_*U*V*W*X*Y*Z,  *******a),
     shape_({S,T_,U,V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t R_,std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[R_][S][T_][U][V][W][X][Y][Z]) 
   : buffer_(R_*S*T_*U*V*W*X*Y*Z,  ********a),
     shape_({R_,S,T_,U,V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t Q,std::size_t R_,std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[Q][R_][S][T_][U][V][W][X][Y][Z]) 
   : buffer_(Q*R_*S*T_*U*V*W*X*Y*Z,  *********a),
     shape_({Q,R_,S,T_,U,V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<std::size_t P,std::size_t Q,std::size_t R_,std::size_t S,std::size_t T_,std::size_t U,std::size_t V,std::size_t W,std::size_t X,std::size_t Y,std::size_t Z,int R__,typename> RA_INLINE_ 
ra::rarray<T,R>::rarray(T (&a)[P][Q][R_][S][T_][U][V][W][X][Y][Z]) 
   : buffer_(P*Q*R_*S*T_*U*V*W*X*Y*Z,  **********a),
     shape_({P,Q,R_,S,T_,U,V,W,X,Y,Z}, buffer_.begin())
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0)
  : buffer_(n0,  buffer),
    shape_({n0}, buffer)
{}

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1)
  : buffer_(n0*n1,  buffer),
    shape_({n0,n1}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2)
  : buffer_(n0*n1*n2,  buffer),
    shape_({n0,n1,n2}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3)
  : buffer_(n0*n1*n2*n3,  buffer),
    shape_({n0,n1,n2,n3}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4)
  : buffer_(n0*n1*n2*n3*n4,  buffer),
    shape_({n0,n1,n2,n3,n4}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5)
  : buffer_(n0*n1*n2*n3*n4*n5,  buffer),
    shape_({n0,n1,n2,n3,n4,n5}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6)
  : buffer_(n0*n1*n2*n3*n4*n5*n6,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}, buffer)
{}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10)
  : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10,  buffer),
    shape_({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10}, buffer)
{}

namespace ra {
RA_INLINE_ size_type mul(const size_type * x, std::size_t n) noexcept {
    size_type result = 1;
    for (std::size_t i=0;i<n;i++)
        result *= x[i];
    return result;
}
}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(const size_type* anextent)
  : buffer_(mul(anextent,R)),
    shape_(reinterpret_cast<const std::array<size_type,R>&>(*anextent), buffer_.begin())
{}

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::rarray(T* buffer, const size_type* anextent)
  : buffer_(mul(anextent,R), buffer),
    shape_(reinterpret_cast<const std::array<size_type,R>&>(*anextent), buffer)
{}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>::rarray(const rarray<T,R> &a) noexcept
  : buffer_(a.buffer_),
    shape_(a.shape_)
{
    // copy constructor
}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>::rarray(rarray<T,R>&& x) noexcept
  : buffer_(std::move(x.buffer_)),
    shape_(std::move(x.shape_))
{
    // move constructor
}

//

template<typename T, int R> RA_INLINE_ 
void ra::rarray<T,R>::clear() noexcept
{
    shape_ = shared_shape<T,R>();
    buffer_ = shared_buffer<T>();
}

//

template<typename T, int R>
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0, RESIZE resize_allowed)
{
    if (size() == n0
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0)) {
        
        shape_ = shared_shape<T,R>({n0}, buffer_.begin());

    } else {

        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0, size_type n1, RESIZE resize_allowed)
{
    if (size() == n0*n1
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1)) {
        
        shape_ = shared_shape<T,R>({n0,n1}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0, size_type n1, size_type n2, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2}, buffer_.begin());
        
    } else {
            
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3*n4
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3*n4*n5
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3*n4*n5*n6
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7,n8}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9, RESIZE resize_allowed)
{
    if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8*n9
        or (resize_allowed == RESIZE::ALLOWED and size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8*n9)) {
        
        shape_ = shared_shape<T,R>({n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}, buffer_.begin());
        
    } else {
        
        throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        
    }
}

template<typename T, int R> 
template<int R_,typename>
RA_INLINE_ 
void ra::rarray<T,R>::reshape(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10, RESIZE resize_allowed)
{
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_ 
ra::rarray<T,R>::~rarray()
{}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, int R> RA_INLINE_
ra::CommaOp<T> ra::rarray<T,R>::operator=(const T& e) RA_NOEXCEPT(std::is_nothrow_copy_constructible<T>())
{
    // Comma separated element assignment: puts the first one in and prepares for more
    RA_CHECKORSAY(not empty(), "assignment to unsized array");
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
ra::CommaOp<T>::CommaOp(T* ptr, T* last) RA_NOEXCEPT(true)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_
bool ra::rarray<T,R>::empty() const noexcept
{
    // check if empty
    return buffer_.cbegin() == nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R> RA_INLINE_
ra::rarray<T,R> ra::rarray<T,R>::copy() const {
    // return a copy
    rarray<T,R> clone;
    clone.buffer_ = buffer_.copy();
    // clone.shape_ = ra::shared_shape<T,R>(shape_.extent(), clone.buffer_.begin());
    clone.shape_ = shape_.copy();
    clone.shape_.relocate(clone.buffer_.begin());
    return clone;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, int R>
template<class U> // = rarray<T,R-1>
RA_INLINEF
typename std::enable_if<R!=1,U>::type 
ra::rarray<T,R>::at(size_type i)
{
    // subarray access with bounds checking
    if (i < 0 or i >= extent(0))
        throw std::out_of_range("rarray<T,R>::at");
    size_type stride = size()/extent(0);
    return ra::rarray<T,R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
}

template<typename T, int R>
template<class U> // = const rarray<T,R-1>
RA_INLINEF
typename std::enable_if<R!=1,U>::type 
ra::rarray<T,R>::at(size_type i) const
{
    // const subarray access with bounds checking
    if (i < 0 or i >= extent(0))
        throw std::out_of_range("rarray<T,R>::at");
    size_type stride = size()/extent(0);
    return ra::rarray<T,R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
}

template<typename T, int R>
template<class U> // = T&
RA_INLINEF
typename std::enable_if<R==1,U>::type 
ra::rarray<T,R>::at(size_type i)
{
    // subarray access with bounds checking
    if (i < 0 or i >= extent(0))
        throw std::out_of_range("rarray<T,R>::at");
    return shape_.ptrs()[i];
}

template<typename T, int R>
template<class U> // = const T&
RA_INLINEF
typename std::enable_if<R==1,U>::type 
ra::rarray<T,R>::at(size_type i) const
{
    // const subarray access with bounds checking
    if (i < 0 or i >= extent(0))
        throw std::out_of_range("rarray<T,R>::at");
    return shape_.ptrs()[i];
}

template<typename T, int R> RA_INLINEF
typename ra::PointerArray<T,R-1>::type ra::rarray<T,R>::_at(ssize_t index)
{
    return shape_.ptrs()[index];
}

template<typename T, int R> RA_INLINEF
typename ra::PointerArray<T,R-1>::type ra::rarray<T,R>::_at(ssize_t index) const
{
    return shape_.ptrs()[index];
}


template<typename T, int R> RA_INLINEF
const ra::size_type* ra::rarray<T,R>::shape() const noexcept
{
    // retrieve array sizes in all dimensions
    return &(shape_.extent()[0]); 
}

template<typename T, int R> RA_INLINE_
typename ra::rarray<T,R>::parray_t ra::rarray<T,R>::ptr_array() const noexcept
{
    return shape_.ptrs();
}

template<typename T, int R> RA_INLINE_
typename ra::rarray<T,R>::noconst_parray_t ra::rarray<T,R>::noconst_ptr_array() const noexcept
{
    return const_cast<noconst_parray_t>(shape_.ptrs());
}

template<typename T, int R> RA_INLINE_
const typename ra::rarray<const T,R>& ra::rarray<T,R>::const_ref() const noexcept
{
    return reinterpret_cast<const rarray<const T,R>&>(*this);
}

template<typename T, int R> RA_INLINEF
ra::rarray<T,R>& ra::rarray<T,R>::operator=(const rarray<T,R> &a) noexcept
{
    buffer_ = a.buffer_;
    shape_ = a.shape_;
    return *this;
}
template<typename T, int R> RA_INLINEF
ra::rarray<T,R>& ra::rarray<T,R>::operator=(rarray<T,R> &&a) noexcept
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
ra::size_type ra::rarray<T,R>::size() const noexcept {
    // retrieve the total number of elements
    return shape_.size(); 
}

template<typename T, int R> RA_INLINEF
T* ra::rarray<T,R>::data() noexcept
{
    // return a T* to the internal data
    return buffer_.begin(); 
}

template<typename T, int R> RA_INLINEF
const T* ra::rarray<T,R>::data() const noexcept
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
typename ra::rarray<T,R>::iterator ra::rarray<T,R>::begin() noexcept
{
    // start of the content
    return buffer_.begin();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::begin() const noexcept
{
    // start of the content, when *this is constant
    return buffer_.begin();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::cbegin() const noexcept
{
    // start of the content, when *this can be constant and you need to be explicit
    return buffer_.cbegin();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::iterator ra::rarray<T,R>::end() noexcept
{
    // end of the content
    return buffer_.end();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::end() const noexcept
{
    // end of the content, when *this is constant
    return buffer_.end();
}
template<typename T, int R> RA_INLINEF
typename ra::rarray<T,R>::const_iterator ra::rarray<T,R>::cend() const noexcept
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

template<typename T, int R, typename P> RA_INLINEF 
ra::_Bracket<T,R-1,ra::_Bracket<T,R,P>> ra::_Bracket<T,R,P>::operator[](ssize_t nextindex) noexcept(noboundscheck)
{
    // implement square brackets to go to the next level
    return { *this, nextindex, shape_ + 1 };
}
template<typename T, int R, typename P> RA_INLINEF
ra::_Bracket<T,R,P>::operator decltype(parent_.at(index_)) ()
{
    // implement implicit conversion to whatever parent.at() gives
    return parent_.at(index_);
}
template<typename T, int R, typename P> RA_INLINEF
ra::_Bracket<T,R,P>::_Bracket(P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck)
 : parent_(parent),
   index_(index),
   shape_(shape)
{
    RA_CHECKORSAY(index >=0 and index_ < shape_[0], "index out of range of array");
}
template<typename T, int R, typename P> RA_INLINEF
auto ra::_Bracket<T,R,P>::_at(ssize_t nextindex) noexcept(noboundscheck) -> decltype(parent_._at(index_)[nextindex])
{
    // used for access with square brackets
    return parent_._at(index_)[nextindex];
}
template<typename T, int R, typename P> RA_INLINEF
auto ra::_Bracket<T,R,P>::at(ssize_t nextindex) -> decltype(parent_.at(index_).at(nextindex))
{
    // used for conversion to whatever parent.at() gives
    return parent_.at(index_).at(nextindex);
}

//////

template<typename T, typename P> RA_INLINEF 
T& ra::_Bracket<T,1,P>::operator[](ssize_t nextindex) noexcept(noboundscheck)
{
    // implement square brackets to go to the next level
    RA_CHECKORSAY(nextindex >=0 and nextindex < shape_[1], "index out of range of array");
    return parent_._at(index_)[nextindex];
}
template<typename T, typename P> RA_INLINEF
ra::_Bracket<T,1,P>::operator decltype(parent_.at(index_)) ()
{
    // implement implicit conversion to whatever parent.at() gives
    return parent_.at(index_);
}
template<typename T, typename P> RA_INLINEF
ra::_Bracket<T,1,P>::_Bracket(P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck)
 : parent_(parent),
   index_(index),
   shape_(shape)
{
    RA_CHECKORSAY(index >=0 and index_ < shape_[0], "index out of range of array");
}
template<typename T, typename P> RA_INLINEF
auto ra::_Bracket<T,1,P>::at(ssize_t nextindex) -> decltype(parent_.at(index_).at(nextindex))
{
    // used for conversion to whatever parent.at() gives
    return parent_.at(index_).at(nextindex);
}

////

template<typename T,int R,typename P> RA_INLINEF
ra::_ConstBracket<T,R-1,ra::_ConstBracket<T,R,P>> ra::_ConstBracket<T,R,P>::operator[](ssize_t nextindex) const noexcept(noboundscheck)
{
    return { *this, nextindex, shape_ + 1 };
}
template<typename T, int R, typename P> RA_INLINEF
ra::_ConstBracket<T,R,P>::operator decltype(parent_.at(index_)) ()
{
    // implement implicit conversion to whatever parent.at() gives
    return parent_.at(index_);
}
template<typename T,int R,typename P> RA_INLINEF
ra::_ConstBracket<T,R,P>::_ConstBracket(const P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck)
: parent_(parent),
  index_(index),
  shape_(shape)
{
    RA_CHECKORSAY(index >=0 and index_ < shape_[0], "index out of range of array");
}
template<typename T,int R,typename P> RA_INLINEF
auto ra::_ConstBracket<T,R,P>::_at(ssize_t nextindex) const noexcept(noboundscheck) -> decltype(parent_._at(index_)[nextindex]) 
{
    return parent_._at(index_)[nextindex];
}
template<typename T, int R, typename P> RA_INLINEF
auto ra::_ConstBracket<T,R,P>::at(ssize_t nextindex) const -> decltype(parent_.at(index_).at(nextindex))
{
    // used for conversion to whatever parent.at() gives
    return parent_.at(index_).at(nextindex);
}

////

template<typename T,typename P> RA_INLINEF
const T& ra::_ConstBracket<T,1,P>::operator[](ssize_t nextindex) const noexcept(noboundscheck)
{
    RA_CHECKORSAY(nextindex >=0 and nextindex < shape_[1], "index out of range of array");
    return parent_._at(index_)[nextindex];
}
template<typename T, typename P> RA_INLINEF
ra::_ConstBracket<T,1,P>::operator decltype(parent_.at(index_)) ()
{
    // implement implicit conversion to whatever parent.at() gives
    return parent_.at(index_);
}
template<typename T,typename P> RA_INLINEF
ra::_ConstBracket<T,1,P>::_ConstBracket(const P& parent, ssize_t index, const ssize_t* shape) noexcept(noboundscheck)
: parent_(parent),
  index_(index),
  shape_(shape)
{
    RA_CHECKORSAY(index >=0 and index_ < shape_[0], "index out of range of array");
}
template<typename T, typename P> RA_INLINEF
auto ra::_ConstBracket<T,1,P>::at(ssize_t nextindex) const -> decltype(parent_.at(index_).at(nextindex))
{
    // used for conversion to whatever parent.at() gives
    return parent_.at(index_).at(nextindex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ra {
template<typename A> 
size_type extent(const A &a, int i)
{
    switch (i) {
    case 0: return std::extent<A,0>();
    case 1: return std::extent<A,1>();
    case 2: return std::extent<A,2>();
    case 3: return std::extent<A,3>();
    case 4: return std::extent<A,4>();
    case 5: return std::extent<A,5>();
    case 6: return std::extent<A,6>();
    case 7: return std::extent<A,7>();
    case 8: return std::extent<A,8>();
    case 9: return std::extent<A,9>();
    case 10: return std::extent<A,10>();
    default: throw std::out_of_range("ra::extent"); return 0;
    }
}

template<typename T, int R> 
size_type extent(const rarray<T,R> &a, int i)
{
    return a.extent(i);
}
} // end namespace ra

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Include I/O always (from version 2.3 onwards)
#include "rarrayio.h"

// Get rid of the macros
#include "rarraydelmacros.h"

// Global namespace stuff

#define EXTENT(A,I)  ra::extent(A,I)
#define RARRAY(A)    rarray<typename std::remove_all_extents<decltype(A)>::type,std::rank<decltype(A)>::value>(A)
#define INDEX(A,X,I) RARRAY(A).index(X,I)

// make RARRAY and INDEX work for rarrays as well as automatic arrays:
namespace std {
    template<class T, int R >
    struct remove_all_extents<ra::rarray<T,R>> {
        typedef T type;
    };
    template<class T, int R >
    struct rank<ra::rarray<T,R>> {
        static const size_t value = R;
    };
}

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
