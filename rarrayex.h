//
// rarrayex.h - Array expressions for runtime arrays defined in
//              rarray.h.
//
// Copyright (c) 2015  Ramses van Zon
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

#ifndef RARRAYEXH
#define RARRAYEXH

#ifndef RARRAY_H
#include "rarray.h"
#endif

// Redefine force inline stuff
#include "rarraymacros.h"

/////////////////////////////////////////////
// FORWARD-DEFINE ALL POSSIBLE EXPRESSIONS //
/////////////////////////////////////////////

enum { 
    XprOp,                                      // create expression out of an array
    CnvOp,                                      // convert expression or array with elements of one type, to another type 
    RepOp,                                      // repeat a scalar value into an array expression of given shape
    AddOp, SubOp, MulOp, DivOp, ModOp, NegOp,   // arithmetic: + - * / % -(unary)
    EqOp,  NeqOp, LeOp,  GrOp, LeqOp,  GeqOp,   // comparison: == != < > <= >=
    AndOp, OrOp,  NotOp,                        // logical: && || !
    IfElseOp                                    // take an array of bools, for true element give back one expression, for false, the other
}; 

////////////////////////////////////
// START ELEMENT-WISE EXPRESSIONS //
////////////////////////////////////

namespace ra {
    typedef const int* Shape;
}

// compute number of elements

template<int R>
RA_INLINEF int element_count(const ra::Shape& shape)
{
    int ec = 1;
    for (int i = 0; i < R; i++)
        ec *= shape[i];
    return ec;
}

// assignment from expressions

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>::rarray(const Expr<T,R,AOP,A1,A2,A3>& e) 
{
    ra::Shape shape = e.shape();
    const int size = element_count<R>(shape);
    init_data(shape, size);
    T* const  element = get_buffer();
    for (int i=0; i<size; i++) 
        element[i] = e.eval(i);
}
 
template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>::rarray(const Expr<T,1,AOP,A1,A2,A3>& e) 
{
    ra::Shape shape = e.shape();
    const int size = element_count<1>(shape);
    init_data(shape, size);
    T* const  element = get_buffer();
    for (int i=0; i<size; i++) 
        element[i] = e.eval(i);
}
 
template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>& ra::rarray<T,R>::operator=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    for (int i=0; i<R; i++) {
        RA_CHECKORSAY(e.shape()[i] == shape()[i], "incompatible shapes in operator=");
    }
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] = e.eval(i);
    return *this;
}

template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>& ra::rarray<T,1>::operator=(const Expr<T,1,AOP,A1,A2,A3> &e) 
{
    RA_CHECKORSAY(e.shape()[0] == shape()[0], "incompatible 1d shapes in operator=");
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
         element_[i] = e.eval(i);
    return *this;
}

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>& ra::rarray<T,R>::operator+=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    for (int i=0; i<R; i++) {
        RA_CHECKORSAY(e.shape()[i] == shape()[i], "incompatible shapes in operator+=");
    }
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] += e.eval(i);
    return *this;
}

template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>& ra::rarray<T,1>::operator+=(const Expr<T,1,AOP,A1,A2,A3> &e) 
{
    RA_CHECKORSAY(e.shape()[0] == shape()[0], "incompatible 1d shapes in operator+=");
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] += e.eval(i);
    return *this;
}

// -=

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>& ra::rarray<T,R>::operator-=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    for (int i=0; i<R; i++) {
        RA_CHECKORSAY(e.shape()[i] == shape()[i], "incompatible shapes in operator-=");
    }
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] -= e.eval(i);
    return *this;
}

template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>& ra::rarray<T,1>::operator-=(const Expr<T,1,AOP,A1,A2,A3> &e) 
{
    RA_CHECKORSAY(e.shape()[0] == shape()[0], "incompatible 1d shapes in operator-=");
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] -= e.eval(i);
    return *this;
}

// *=
template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>& ra::rarray<T,R>::operator*=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    for (int i=0; i<R; i++) {
        RA_CHECKORSAY(e.shape()[i] == shape()[i], "incompatible shapes in operator*=");
    }
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] *= e.eval(i);
    return *this;
}

template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>& ra::rarray<T,1>::operator*=(const Expr<T,1,AOP,A1,A2,A3> &e) 
{
    RA_CHECKORSAY(e.shape()[0] == shape()[0], "incompatible 1d shapes in operator*=");
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] *= e.eval(i);
    return *this;
}

// /=
template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>& ra::rarray<T,R>::operator/=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    for (int i=0; i<R; i++) {
        RA_CHECKORSAY(e.shape()[i] == shape()[i], "incompatible shapes in operator/=");
    }
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] /= e.eval(i);
    return *this;
}

template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>& ra::rarray<T,1>::operator/=(const Expr<T,1,AOP,A1,A2,A3> &e) 
{
    RA_CHECKORSAY(e.shape()[0] == shape()[0], "incompatible 1d shapes in operator/=");
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] /= e.eval(i);
    return *this;
}

// %=
template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,R>& ra::rarray<T,R>::operator%=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    for (int i=0; i<R; i++) {
        RA_CHECKORSAY(e.shape()[i] == shape()[i], "incompatible shapes in operator%=");
    }
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] %= e.eval(i);
    return *this;
}

template<typename T>
template<ExOp AOP, typename A1, typename A2, typename A3>
inline ra::rarray<T,1>& ra::rarray<T,1>::operator%=(const Expr<T,1,AOP,A1,A2,A3> &e) 
{
    RA_CHECKORSAY(e.shape()[0] == shape()[0], "incompatible 1d shapes in operator%=");
    T* const  element_ = get_buffer();
    const int size_    = size();
    for (int i=0; i<size_; i++)
        element_[i] %= e.eval(i);
    return *this;
}

////////////////////////////////
// USEFUL ABBREVIATING MACROS //
////////////////////////////////

#define EXPR0 Expr<T,R,XprOp,void,void,void>
#define EXPR1 Expr<T,R,AOP,A1,A2,A3>
#define EXPR2 Expr<T,R,BOP,B1,B2,B3>
#define EXPR3 Expr<T,R,COP,C1,C2,C3>
#define ECNST Expr<T,R,RepOp,void,TLIKE,void>
#define ECNSTSTRICT Expr<T,R,RepOp,void,T,void>
#define EXPR0BOOL Expr<bool,R,XprOp,void,void,void>
#define EXPR1BOOL Expr<bool,R,AOP,A1,A2,A3>
#define EXPR2BOOL Expr<bool,R,BOP,B1,B2,B3>

/////////////////////////////////////////////////////////////////////////////////////////
// LOGISTIC HELPER FUNCTIONS (TO TURN ARRAYS INTO EXPRESSIONS, OR DO TYPE CONVERSIONS) //
/////////////////////////////////////////////////////////////////////////////////////////

// Basic expression, a wrapper around the array

template<typename T, int R> 
RA_INLINEF EXPR0 express(const ra::rarray<T,R>& a)
{ 
    // create a basic expression out of an array
    return EXPR0(a);
}

template<typename T, int R> 
class EXPR0
{
  public:
    RA_INLINEF T eval(int i) const {
        return data_[i];
    }
    RA_INLINEF Expr(const ra::rarray<T,R>& a)
      : shape_(a.shape()), data_(a.data()), a_(&a)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape        shape_;
    const T*               data_;
    const ra::rarray<T,R>* a_;
};

////////////////////////////////////////////////////////////////////////////

// Conversion of the elements of arrays

template<typename TO, typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
Expr<TO,R,CnvOp,EXPR1,void,void> convert(const EXPR1& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<TO,R,CnvOp,EXPR1,void,void>(a);
}

template<typename TO, typename T, int R>
Expr<TO,R,CnvOp,EXPR0,void,void> convert(const ra::rarray<T,R>& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<TO,R,CnvOp,EXPR0,void,void>(express(a));
}

template<typename T, int R, typename A> 
class Expr<T,R,CnvOp,A,void,void>
{ 
  public:
    RA_INLINEF T eval(int i) const {
        return (T)(a_->eval(i));
    }
    RA_INLINEF Expr(const A &a)
      : shape_(a.shape()), a_(&a)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_; 
};

////////////////////////////////////////////////////////////////////////////

// Pretend to be an array of given shape same as a, but with all
// element equal to x (without creating such an object).

template<typename T, int R, typename TLIKE> 
RA_INLINEF ECNST repeatlike(const ra::rarray<T,R>& a, const TLIKE& x)
{ 
    // create a basic expression out of a constant, with the shape of
    // array a
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename TLIKE, ExOp AOP, typename A1, typename A2, typename A3> 
RA_INLINEF ECNST repeatlike(const EXPR1& a, const TLIKE& x)
{ 
    // create a basic expression out of a constant, with the shape of
    // expression a
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename TLIKE> 
class ECNST 
{
  public:
    RA_INLINEF T eval(int i) const { 
        return x_; 
    }
    RA_INLINEF Expr(const ra::rarray<T,R>& a, const T& x)
      : shape_(a.shape()), x_(x)
    {}
    template<ExOp AOP, typename A1, typename A2, typename A3>
    RA_INLINEF Expr(const EXPR1& a, const T& x)
      : shape_(a.shape()), x_(x)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const T         x_; //*?
};

////////////////////////////
// ARITHMETIC EXPRESSIONS //
////////////////////////////

// Addition of array elements

template<typename T, int R> 
RA_INLINEF Expr<T,R,AddOp,EXPR0,EXPR0,void> operator+(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,AddOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,AddOp,EXPR1,EXPR0,void> operator+(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,AddOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,AddOp,EXPR0,EXPR1,void> operator+(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,AddOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<T,R,AddOp,EXPR1,EXPR2,void> operator+(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,AddOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,AddOp,A,B,void>
{
  public:
    RA_INLINEF T eval(int i) const { 
        return a_->eval(i) + b_->eval(i); 
    }
    RA_INLINEF Expr(const A&a, const B&b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Subtraction of array elements

template<typename T, int R> 
RA_INLINEF Expr<T,R,SubOp,EXPR0,EXPR0,void> operator-(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,SubOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,SubOp,EXPR1,EXPR0,void> operator-(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,SubOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,SubOp,EXPR0,EXPR1,void> operator-(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,SubOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<T,R,SubOp,EXPR1,EXPR2,void> operator-(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,SubOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,SubOp,A,B,void> 
{ 
  public:
    RA_INLINEF T eval(int i) const { 
        return a_->eval(i) - b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Negate elements of an array

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
Expr<T,R,NegOp,EXPR1,void,void> operator-(const EXPR1& a)
{
    return Expr<T,R,NegOp,EXPR1,void,void>(a);
}

template<typename T, int R>
Expr<T,R,NegOp,EXPR0,void,void> operator-(const ra::rarray<T,R>& a)
{
    auto e = express(a);
    auto me = Expr<T,R,NegOp,EXPR0,void,void>(e);
    return me;
}

template<typename T, int R, typename A> 
class Expr<T,R,NegOp,A,void,void> 
{ 
  public:
    RA_INLINEF T eval(int i) const {
        return -(a_->eval(i));
    }
    Expr(const A& a)
      : shape_(a.shape()), a_(&a)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
};

////////////////////////////////////////////////////////////////////////////

// Multiplication of array elements

template<typename T, int R> 
RA_INLINEF Expr<T,R,MulOp,EXPR0,EXPR0,void> operator*(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,MulOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,MulOp,EXPR1,EXPR0,void> operator*(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,MulOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,MulOp,EXPR0,EXPR1,void> operator*(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,MulOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<T,R,MulOp,EXPR1,EXPR2,void> operator*(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,MulOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
RA_INLINEF Expr<T,R,MulOp,EXPR0,ECNST,void> operator*(const ra::rarray<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,MulOp,EXPR0,ECNST,void>(express(a), repeatlike(a,x));
}

template<typename T, int R, typename TLIKE>
RA_INLINEF Expr<T,R,MulOp,ECNST,EXPR0,void> operator*(const TLIKE& x, const ra::rarray<T,R> &b)
{
    return Expr<T,R,MulOp,ECNST,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
RA_INLINEF Expr<T,R,MulOp,EXPR1,ECNST,void> operator*(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,MulOp,EXPR1,ECNST,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
RA_INLINEF Expr<T,R,MulOp,ECNST,EXPR1,void> operator*(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,MulOp,ECNST,EXPR1,void>(repeatlike(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,MulOp,A,B,void> 
{ 
  public:
    RA_INLINEF T eval(int i) const { 
        return a_->eval(i) * b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Division of array elements

template<typename T, int R> 
RA_INLINEF Expr<T,R,DivOp,EXPR0,EXPR0,void> operator/(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,DivOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,DivOp,EXPR1,EXPR0,void> operator/(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,DivOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,DivOp,EXPR0,EXPR1,void> operator/(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,DivOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<T,R,DivOp,EXPR1,EXPR2,void> operator/(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,DivOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
RA_INLINEF Expr<T,R,DivOp,EXPR0,ECNST,void> operator/(const ra::rarray<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,DivOp,EXPR0,ECNST,void>(express(a), repeatlike(a,x));
}

template<typename T, int R, typename TLIKE>
RA_INLINEF Expr<T,R,DivOp,ECNST,EXPR0,void> operator/(const TLIKE& x, const ra::rarray<T,R> &b)
{
    return Expr<T,R,DivOp,ECNST,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
RA_INLINEF Expr<T,R,DivOp,EXPR1,ECNST,void> operator/(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,DivOp,EXPR1,ECNST,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
RA_INLINEF Expr<T,R,DivOp,ECNST,EXPR1,void> operator/(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,DivOp,ECNST,EXPR1,void>(repeatlike(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,DivOp,A,B,void> 
{ 
  public:
    RA_INLINEF T eval(int i) const { 
        return a_->eval(i) / b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Modulus of array elements

template<typename T, int R> 
RA_INLINEF Expr<T,R,ModOp,EXPR0,EXPR0,void> operator%(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,ModOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,ModOp,EXPR1,EXPR0,void> operator%(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<T,R,ModOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,ModOp,EXPR0,EXPR1,void> operator%(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,ModOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<T,R,ModOp,EXPR1,EXPR2,void> operator%(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,ModOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
RA_INLINEF Expr<T,R,ModOp,EXPR0,ECNST,void> operator%(const ra::rarray<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,ModOp,EXPR0,ECNST,void>(express(a), repeatlike(a,x));
}

template<typename T, int R, typename TLIKE>
RA_INLINEF Expr<T,R,ModOp,ECNST,EXPR0,void> operator%(const TLIKE& x, const ra::rarray<T,R> &b)
{
    return Expr<T,R,ModOp,ECNST,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
RA_INLINEF Expr<T,R,ModOp,EXPR1,ECNST,void> operator%(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,ModOp,EXPR1,ECNST,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
RA_INLINEF Expr<T,R,ModOp,ECNST,EXPR1,void> operator%(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ModOp,ECNST,EXPR1,void>(repeatlike(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,ModOp,A,B,void> 
{ 
  public:
    RA_INLINEF T eval(int i) const { 
        return a_->eval(i) % b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////
// COMPARISON EXPRESSIONS //
////////////////////////////

// Check equality of array elements

template<typename T, int R> 
RA_INLINEF Expr<bool,R,EqOp,EXPR0,EXPR0,void> operator==(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,EqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,EqOp,EXPR1,EXPR0,void> operator==(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,EqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,EqOp,EXPR0,EXPR1,void> operator==(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,EqOp,EXPR1,EXPR2,void> operator==(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,EqOp,EXPR0,ECNSTSTRICT,void> operator==(const ra::rarray<T,R> &a, const T& x)
{
    return Expr<bool,R,EqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,EqOp,ECNSTSTRICT,EXPR0,void> operator==(const T& x, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,EqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,EqOp,EXPR1,ECNSTSTRICT,void> operator==(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,EqOp,ECNSTSTRICT,EXPR1,void> operator==(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,EqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,EqOp,A,B,void> 
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) == b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b) 
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;    
};

////////////////////////////////////////////////////////////////////////////

// Check inequality of array elements

template<typename T, int R> 
RA_INLINEF Expr<bool,R,NeqOp,EXPR0,EXPR0,void> operator!=(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,NeqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,NeqOp,EXPR1,EXPR0,void> operator!=(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,NeqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,NeqOp,EXPR0,EXPR1,void> operator!=(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,NeqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,NeqOp,EXPR1,EXPR2,void> operator!=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,NeqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,NeqOp,EXPR0,ECNSTSTRICT,void> operator!=(const ra::rarray<T,R> &a, const T& x)
{
    return Expr<bool,R,NeqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR0,void> operator!=(const T& x, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,NeqOp,EXPR1,ECNSTSTRICT,void> operator!=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,NeqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR1,void> operator!=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,NeqOp,A,B,void>
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) != b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Check lesser-than of array elements

template<typename T, int R> 
RA_INLINEF Expr<bool,R,LeOp,EXPR0,EXPR0,void> operator<(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,LeOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeOp,EXPR1,EXPR0,void> operator<(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,LeOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeOp,EXPR0,EXPR1,void> operator<(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,LeOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,LeOp,EXPR1,EXPR2,void> operator<(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,LeOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,LeOp,EXPR0,ECNSTSTRICT,void> operator<(const ra::rarray<T,R> &a, const T& x)
{
    return Expr<bool,R,LeOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,LeOp,ECNSTSTRICT,EXPR0,void> operator<(const T& x, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,LeOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeOp,EXPR1,ECNSTSTRICT,void> operator<(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,LeOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeOp,ECNSTSTRICT,EXPR1,void> operator<(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,LeOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,LeOp,A,B,void>
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) < b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Check greaterness of array elements

template<typename T, int R> 
RA_INLINEF Expr<bool,R,GrOp,EXPR0,EXPR0,void> operator>(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,GrOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GrOp,EXPR1,EXPR0,void> operator>(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,GrOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GrOp,EXPR0,EXPR1,void> operator>(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,GrOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,GrOp,EXPR1,EXPR2,void> operator>(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,GrOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,GrOp,EXPR0,ECNSTSTRICT,void> operator>(const ra::rarray<T,R> &a, const T& x)
{
    return Expr<bool,R,GrOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,GrOp,ECNSTSTRICT,EXPR0,void> operator>(const T& x, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,GrOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GrOp,EXPR1,ECNSTSTRICT,void> operator>(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,GrOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GrOp,ECNSTSTRICT,EXPR1,void> operator>(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,GrOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,GrOp,A,B,void>
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) > b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Check lesser-than or equality of array elements

template<typename T, int R> 
RA_INLINEF Expr<bool,R,LeqOp,EXPR0,EXPR0,void> operator<=(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,LeqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeqOp,EXPR1,EXPR0,void> operator<=(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,LeqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeqOp,EXPR0,EXPR1,void> operator<=(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,LeqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,LeqOp,EXPR1,EXPR2,void> operator<=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,LeqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,LeqOp,EXPR0,ECNSTSTRICT,void> operator<=(const ra::rarray<T,R> &a, const T& x)
{
    return Expr<bool,R,LeqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR0,void> operator<=(const T& x, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeqOp,EXPR1,ECNSTSTRICT,void> operator<=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,LeqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR1,void> operator<=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,LeqOp,A,B,void> 
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) <= b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Check greater or equality of array elements

template<typename T, int R> 
RA_INLINEF Expr<bool,R,GeqOp,EXPR0,EXPR0,void> operator>=(const ra::rarray<T,R> &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,GeqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GeqOp,EXPR1,EXPR0,void> operator>=(const EXPR1 &a, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,GeqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GeqOp,EXPR0,EXPR1,void> operator>=(const ra::rarray<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,GeqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,GeqOp,EXPR1,EXPR2,void> operator>=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,GeqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,GeqOp,EXPR0,ECNSTSTRICT,void> operator>=(const ra::rarray<T,R> &a, const T& x)
{
    return Expr<bool,R,GeqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
RA_INLINEF Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR0,void> operator>=(const T& x, const ra::rarray<T,R> &b)
{
    return Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GeqOp,EXPR1,ECNSTSTRICT,void> operator>=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,GeqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR1,void> operator>=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,GeqOp,A,B,void> 
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) > b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

///////////////////////
// LOGICAL OPERATORS //
///////////////////////

// Logical AND

template<int R>
RA_INLINEF Expr<bool,R,AndOp,EXPR0BOOL,EXPR0BOOL,void> operator&&(const ra::rarray<bool,R>& a, const ra::rarray<bool,R>& b)
{
    return Expr<bool,R,AndOp,EXPR0BOOL,EXPR0BOOL,void>(express(a), express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,AndOp,EXPR1BOOL,EXPR0BOOL,void> operator&&(const EXPR1BOOL &a, const ra::rarray<bool,R> &b)
{
    return Expr<bool,R,AndOp,EXPR1BOOL,EXPR0BOOL,void>(a, express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,AndOp,EXPR0BOOL,EXPR1BOOL,void> operator&&(const ra::rarray<bool,R> &a, const EXPR1BOOL &b)
{
    return Expr<bool,R,AndOp,EXPR0BOOL,EXPR1BOOL,void>(express(a), b);
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,AndOp,EXPR1BOOL,EXPR2BOOL,void> operator&&(const EXPR1BOOL &a, const EXPR2BOOL &b)
{
    return Expr<bool,R,AndOp,EXPR1BOOL,EXPR2BOOL,void>(a, b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,AndOp,A,B,void> 
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) && b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// Logical OR

template<int R>
RA_INLINEF Expr<bool,R,OrOp,EXPR0BOOL,EXPR0BOOL,void> operator||(const ra::rarray<bool,R>& a, const ra::rarray<bool,R>& b)
{
    return Expr<bool,R,OrOp,EXPR0BOOL,EXPR0BOOL,void>(express(a), express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,OrOp,EXPR1BOOL,EXPR0BOOL,void> operator||(const EXPR1BOOL &a, const ra::rarray<bool,R> &b)
{
    return Expr<bool,R,OrOp,EXPR1BOOL,EXPR0BOOL,void>(a, express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,OrOp,EXPR0BOOL,EXPR1BOOL,void> operator||(const ra::rarray<bool,R> &a, const EXPR1BOOL &b)
{
    return Expr<bool,R,OrOp,EXPR0BOOL,EXPR1BOOL,void>(express(a), b);
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
RA_INLINEF Expr<bool,R,OrOp,EXPR1BOOL,EXPR2BOOL,void> operator||(const EXPR1BOOL &a, const EXPR2BOOL &b)
{
    return Expr<bool,R,OrOp,EXPR1BOOL,EXPR2BOOL,void>(a, b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,OrOp,A,B,void> 
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return a_->eval(i) || b_->eval(i); 
    }
    RA_INLINEF Expr(const A &a, const B &b)
      : shape_(a.shape()), a_(&a), b_(&b)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
};

////////////////////////////////////////////////////////////////////////////

// NOT the elements of a boolean array

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<bool,R,NotOp,EXPR1BOOL,void,void> operator!(const EXPR1BOOL& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<bool,R,NotOp,EXPR1BOOL,void,void>(a);
}

template<int R>
RA_INLINEF Expr<bool,R,NotOp,EXPR0BOOL,void,void> operator!(const ra::rarray<bool,R>& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<bool,R,NotOp,EXPR0BOOL,void,void>(express(a));
}

template<int R, typename A> 
class Expr<bool,R,NotOp,A,void,void>
{ 
  public:
    RA_INLINEF bool eval(int i) const { 
        return ! (a_->eval(i));
    }
    RA_INLINEF Expr(const A &a)
      : shape_(a.shape()), a_(&a)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
};

////////////////
// REDUCTIONS //
////////////////

// summing

template<typename T, int R>
RA_INLINEF T sum(const ra::rarray<T,R>& a)
{
    const T* const element = a.data();
    const int size  = a.size();
    T y = element[0];
    for (int i=0; i<size; i++)        
        y += a.element[i];
    return y;
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF T sum(const EXPR1& a)
{
    T y = a.eval(0);
    const int size  = element_count<R>(a.shape());
    for (int i=1; i<size; i++)
        y += a.eval(i);
    return y;
}

// multiplying

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF T product(const EXPR1& a)
{
    T y = a.eval(0);
    for (int i=1; i<R; i++)
        y *= a.eval(i);
    return y;
}

template<typename T, int R>
RA_INLINEF T product(const ra::rarray<T,R>& a)
{
    T y = a.element_[0];
    for (int i=1; i<R; i++)
        y *= a.element_[i];
    return y;
}

// are all boolean elements true? (&& reduction)

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF bool all(const EXPR1BOOL& a)
{
    for (int i=0;i<R;i++) {
        if (a.eval(i) == false)
            return false;
    }
    return true;
}

template<int R>
RA_INLINEF bool all(const ra::rarray<bool,R>& a)
{
    for (int i=0;i<R;i++)
        if (not a.element_[i])
            return false;
    return true;
}

// are any boolean elements true? (|| reduction)

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF bool any(const EXPR1BOOL& a)
{
    for (int i=0;i<R;i++)
        if (a.eval(i))
            return true;
    return false;
}

template<int R>
RA_INLINEF bool any(const ra::rarray<bool,R>& a)
{
    for (int i=0;i<R;i++)
        if (a.element_[i])
            return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////

// If-then-else construct

template<typename T,int R>
RA_INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR0> ifelse(const ra::rarray<bool,R>& a, const ra::rarray<T,R>& b, const ra::rarray<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR0>(express(a), express(b), express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR0> ifelse(const EXPR1BOOL& a, const ra::rarray<T,R>& b, const ra::rarray<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR0>(a, express(b), express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR0> ifelse(const ra::rarray<bool,R>& a, const EXPR1& b, const ra::rarray<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR0>(express(a), b, express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR1> ifelse(const ra::rarray<bool,R>& a, const ra::rarray<T,R>& b, const EXPR1& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR1>(express(a), express(b), c);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR0> ifelse(const EXPR1BOOL& a, const EXPR2& b, const ra::rarray<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR0>(a, b, express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR2> ifelse(const EXPR1BOOL& a, const ra::rarray<T,R>& b, const EXPR2& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR2>(a, express(b), c);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR2> ifelse(const ra::rarray<bool,R>& a, const EXPR1& b, const EXPR2& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR2>(express(a), b, c);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3, ExOp COP, typename C1, typename C2, typename C3>
RA_INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR3> ifelse(const EXPR1BOOL& a, const EXPR2& b, const EXPR3& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR3>(a, b, c);
}

template<typename T, int R, typename A, typename B, typename C> 
class Expr<T,R,IfElseOp,A,B,C>
{ 
  public:
    RA_INLINEF T eval(int i) const { 
        if (a_->eval(i))
            return b_->eval(i);
        else
            return c_->eval(i);
    }
    RA_INLINEF Expr(const A &a, const B &b, const C &c)
      : shape_(a.shape()), a_(&a), b_(&b), c_(&c)
    {}
    RA_INLINEF ra::Shape shape() const {
        return shape_;
    }
  private:
    const ra::Shape shape_;
    const A*        a_;
    const B*        b_;
    const C*        c_;
};

// Remove force inline macros etc.
#include "rarraydelmacros.h"
#undef EXPR0 
#undef EXPR1 
#undef EXPR2 
#undef EXPR3 
#undef ECNST 
#undef ECNSTSTRICT
#undef EXPR0BOOL
#undef EXPR1BOOL
#undef EXPR2BOOL

#endif
