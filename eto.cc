#include <iostream>

// Redefine force inline stuff

#if not defined(INLINEF)
# if defined(__INTEL_COMPILER)
#   define INLINEF  __forceinline
# elif defined(__GNUC__)
#   define INLINEF inline __attribute__((always_inline)) 
# else
#   define INLINEF inline
# endif
#endif

/////////////////////////////////////////////
// FORWARD-DEFINE ALL POSSIBLE EXPRESSIONS //
/////////////////////////////////////////////

enum ExOp { 
    XprOp,                                      // create expression out of an array
    CnvOp,                                      // convert expression or array with elements of one type, to another type 
    RepOp,                                      // repeat a scalar value into an array expression of given shape
    AddOp, SubOp, MulOp, DivOp, ModOp, NegOp,   // arithmetic: + - * / % -(unary)
    EqOp,  NeqOp, LeOp,  GrOp, LeqOp,  GeqOp,   // comparison: == != < > <= >=
    AndOp, OrOp,  NotOp,                        // logical: && || !
    IfElseOp                                    // take an array of bools, for true element give back one expression, for false, the other
}; 

// Each operator creates a subexpression of the Expr<...>, which we forward-define first
template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3> class Expr;

// Here's the basic type to be used in expressions. To be replaced with rarrays in due course
template<typename T, int R>
class Vec 
{
  public:
    T element_[R];
    int shape_[1];
    Vec() {} 
    Vec(T a, T b, T c) {
        element_[0] = a;
        element_[1] = b;
        element_[2] = c;
    }
    const int* shape() const { return shape_; }
    // Need constructor and assignment for expressions
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF explicit   Vec (const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF Vec& operator= (const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF Vec& operator+=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF Vec& operator-=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF Vec& operator*=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF Vec& operator/=(const Expr<T,R,AOP,A1,A2,A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> INLINEF Vec& operator%=(const Expr<T,R,AOP,A1,A2,A3>& e);
};

////////////////////////////////////////////////////////////
// START ELEMENT-WISE EXPRESSIONS (to become rarrayex.h?) //
////////////////////////////////////////////////////////////

// assignment from expressions

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>::Vec(const Expr<T,R,AOP,A1,A2,A3>& e) 
{
    shape_[0] = e.shape()[0];
    for (int i=0; i<R; i++) {
        T thing = e.eval(i);
        element_[i] = thing;
    }
}
 
template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>& Vec<T,R>::operator=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    // TO DO IN RARRAY APPLICATION:
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
        element_[i] = e.eval(i);
    return *this;
}

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>& Vec<T,R>::operator+=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    // TO DO IN RARRAY APPLICATION:
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
        element_[i] += e.eval(i);
    return *this;
}

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>& Vec<T,R>::operator-=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    // TO DO IN RARRAY APPLICATION:
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
        element_[i] -= e.eval(i);
    return *this;
}

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>& Vec<T,R>::operator*=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    // TO DO IN RARRAY APPLICATION:
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
        element_[i] *= e.eval(i);
    return *this;
}

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>& Vec<T,R>::operator/=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    // TO DO IN RARRAY APPLICATION:
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
        element_[i] /= e.eval(i);
    return *this;
}

template<typename T, int R>
template<ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Vec<T,R>& Vec<T,R>::operator%=(const Expr<T,R,AOP,A1,A2,A3> &e) 
{
    // TO DO IN RARRAY APPLICATION:
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
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
#define EXPR3BOOL Expr<bool,R,COP,C1,C2,C3>

/////////////////////////////////////////////////////////////////////////////////////////
// LOGISTIC HELPER FUNCTIONS (TO TURN ARRAYS INTO EXPRESSIONS, OR DO TYPE CONVERSIONS) //
/////////////////////////////////////////////////////////////////////////////////////////

// Parent class

class EShape
{
  public:
    INLINEF const int* shape() const {
        return shape_;
    }
  protected:
    INLINEF EShape(const int* shape) : shape_(shape) {}
    const int* shape_;
};

template<typename A>
class Eptr : public EShape
{
  public:
    Eptr(const A& a) : EShape(a.shape()), a_(&a) {}
  protected:
    const A* a_;
};

template<typename A, typename B>
class Eptrpair : public EShape
{
  public:
    Eptrpair(const A& a, const B& b) : EShape(a.shape()), a_(&a), b_(&b) {}
  protected:
    const A* a_;
    const B* b_;
};

template<typename A, typename B, typename C>
class Eptrtriple : public EShape
{
  public:
    Eptrtriple(const A& a, const B& b, const C& c) : EShape(a.shape()), a_(&a), b_(&b), c_(&c) {}
  protected:
    const A* a_;
    const B* b_;
    const C* c_;
};

// Basic expression, a wrapper around the array

template<typename T, int R> 
INLINEF EXPR0 express(const Vec<T,R>& a)
{ 
    // create a basic expression out of an array
    return EXPR0(a);
}

template<typename T, int R> 
class EXPR0: public Eptr<Vec<T,R> >
{
  public:
    INLINEF T eval(int i) const { 
        return this->a_->element_[i];
    }
    INLINEF Expr(const Vec<T,R>& a) : Eptr<Vec<T,R> >(a) {}
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
Expr<TO,R,CnvOp,EXPR0,void,void> convert(const Vec<T,R>& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<TO,R,CnvOp,EXPR0,void,void>(express(a));
}

template<typename T, int R, typename A> 
class Expr<T,R,CnvOp,A,void,void> : public Eptr<A>
{ 
  public:
    INLINEF T eval(int i) const { 
        return (T)(this->a_->eval(i));
    }
    INLINEF Expr(const A &a) : Eptr<A>(a) {}
};

////////////////////////////////////////////////////////////////////////////

// Pretend to be an array of given shape same as a, but with all
// element equal to x (without creating such an object).

template<typename T, int R, typename TLIKE> 
INLINEF ECNST repeatlike(const Vec<T,R>& a, const TLIKE& x)
{ 
    // create a basic expression out of an array
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename TLIKE, ExOp AOP, typename A1, typename A2, typename A3> 
INLINEF ECNST repeatlike(const EXPR1& a, const TLIKE& x)
{ 
    // create a basic expression out of an array
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename TLIKE> 
class ECNST : public EShape
{
  public:
    INLINEF T eval(int i) const { 
        return *x_; 
    }
    INLINEF Expr(const Vec<T,R>& a, const T& x) : EShape(a.shape_), x_(&x) {}
    template<ExOp AOP, typename A1, typename A2, typename A3>
    INLINEF Expr(const EXPR1& a, const T& x) : EShape(a.shape()), x_(&x) {}
  private:
    const T* x_;
};

////////////////////////////
// ARITHMETIC EXPRESSIONS //
////////////////////////////

// Addition of array elements

template<typename T, int R> 
INLINEF Expr<T,R,AddOp,EXPR0,EXPR0,void> operator+(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,AddOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,AddOp,EXPR1,EXPR0,void> operator+(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,AddOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,AddOp,EXPR0,EXPR1,void> operator+(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,AddOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<T,R,AddOp,EXPR1,EXPR2,void> operator+(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,AddOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,AddOp,A,B,void> : public Eptrpair<A,B>
{
  public:
    INLINEF T eval(int i) const { 
        return this->a_->eval(i) + this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a, b) {}
};

////////////////////////////////////////////////////////////////////////////

// Subtraction of array elements

template<typename T, int R> 
INLINEF Expr<T,R,SubOp,EXPR0,EXPR0,void> operator-(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,SubOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,SubOp,EXPR1,EXPR0,void> operator-(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,SubOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,SubOp,EXPR0,EXPR1,void> operator-(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,SubOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<T,R,SubOp,EXPR1,EXPR2,void> operator-(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,SubOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,SubOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF T eval(int i) const { 
        return this->a_->eval(i) - this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Negate elements of an array

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
Expr<T,R,NegOp,EXPR1,void,void> operator-(const EXPR1& a)
{
    return Expr<T,R,NegOp,EXPR1,void,void>(a);
}

template<typename T, int R>
Expr<T,R,NegOp,EXPR0,void,void> operator-(const Vec<T,R>& a)
{
    return Expr<T,R,NegOp,EXPR0,void,void>(express(a));
}

template<typename T, int R, typename A> 
class Expr<T,R,NegOp,A,void,void> : public Eptr<A>
{ 
  public:
    INLINEF T eval(int i) const { 
        return -this->a_->eval(i);
    }
    INLINEF Expr(const A &a) : Eptr<A>(a) {}
};

////////////////////////////////////////////////////////////////////////////

// Multiplication of array elements

template<typename T, int R> 
INLINEF Expr<T,R,MulOp,EXPR0,EXPR0,void> operator*(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,MulOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,MulOp,EXPR1,EXPR0,void> operator*(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,MulOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,MulOp,EXPR0,EXPR1,void> operator*(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,MulOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<T,R,MulOp,EXPR1,EXPR2,void> operator*(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,MulOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,MulOp,EXPR0,ECNST,void> operator*(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,MulOp,EXPR0,ECNST,void>(express(a), repeatlike(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,MulOp,ECNST,EXPR0,void> operator*(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,MulOp,ECNST,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
INLINEF Expr<T,R,MulOp,EXPR1,ECNST,void> operator*(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,MulOp,EXPR1,ECNST,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
INLINEF Expr<T,R,MulOp,ECNST,EXPR1,void> operator*(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,MulOp,ECNST,EXPR1,void>(repeatlike(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,MulOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF T eval(int i) const { 
        return this->a_->eval(i) * this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Division of array elements

template<typename T, int R> 
INLINEF Expr<T,R,DivOp,EXPR0,EXPR0,void> operator/(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,DivOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,DivOp,EXPR1,EXPR0,void> operator/(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,DivOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,DivOp,EXPR0,EXPR1,void> operator/(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,DivOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<T,R,DivOp,EXPR1,EXPR2,void> operator/(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,DivOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,DivOp,EXPR0,ECNST,void> operator/(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,DivOp,EXPR0,ECNST,void>(express(a), repeatlike(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,DivOp,ECNST,EXPR0,void> operator/(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,DivOp,ECNST,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
INLINEF Expr<T,R,DivOp,EXPR1,ECNST,void> operator/(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,DivOp,EXPR1,ECNST,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
INLINEF Expr<T,R,DivOp,ECNST,EXPR1,void> operator/(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,DivOp,ECNST,EXPR1,void>(repeatlike(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,DivOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF T eval(int i) const { 
        return this->a_->eval(i) / this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Modulus of array elements

template<typename T, int R> 
INLINEF Expr<T,R,ModOp,EXPR0,EXPR0,void> operator%(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,ModOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,ModOp,EXPR1,EXPR0,void> operator%(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,ModOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,ModOp,EXPR0,EXPR1,void> operator%(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,ModOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<T,R,ModOp,EXPR1,EXPR2,void> operator%(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,ModOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ModOp,EXPR0,ECNST,void> operator%(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,ModOp,EXPR0,ECNST,void>(express(a), repeatlike(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ModOp,ECNST,EXPR0,void> operator%(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ModOp,ECNST,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
INLINEF Expr<T,R,ModOp,EXPR1,ECNST,void> operator%(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,ModOp,EXPR1,ECNST,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, typename TLIKE>
INLINEF Expr<T,R,ModOp,ECNST,EXPR1,void> operator%(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ModOp,ECNST,EXPR1,void>(repeatlike(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,ModOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF T eval(int i) const { 
        return this->a_->eval(i) % this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////
// COMPARISON EXPRESSIONS //
////////////////////////////

// Check equality of array elements

template<typename T, int R> 
INLINEF Expr<bool,R,EqOp,EXPR0,EXPR0,void> operator==(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,EqOp,EXPR1,EXPR0,void> operator==(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,EqOp,EXPR0,EXPR1,void> operator==(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,EqOp,EXPR1,EXPR2,void> operator==(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EqOp,EXPR0,ECNSTSTRICT,void> operator==(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,EqOp,ECNSTSTRICT,EXPR0,void> operator==(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,EqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,EqOp,EXPR1,ECNSTSTRICT,void> operator==(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,EqOp,ECNSTSTRICT,EXPR1,void> operator==(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,EqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,EqOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) == this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Check inequality of array elements

template<typename T, int R> 
INLINEF Expr<bool,R,NeqOp,EXPR0,EXPR0,void> operator!=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,NeqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,NeqOp,EXPR1,EXPR0,void> operator!=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,NeqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,NeqOp,EXPR0,EXPR1,void> operator!=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,NeqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,NeqOp,EXPR1,EXPR2,void> operator!=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,NeqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,NeqOp,EXPR0,ECNSTSTRICT,void> operator!=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,NeqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR0,void> operator!=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,NeqOp,EXPR1,ECNSTSTRICT,void> operator!=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,NeqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR1,void> operator!=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,NeqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,NeqOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) != this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Check lesser-than of array elements

template<typename T, int R> 
INLINEF Expr<bool,R,LeOp,EXPR0,EXPR0,void> operator<(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,LeOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeOp,EXPR1,EXPR0,void> operator<(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,LeOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeOp,EXPR0,EXPR1,void> operator<(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,LeOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,LeOp,EXPR1,EXPR2,void> operator<(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,LeOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,LeOp,EXPR0,ECNSTSTRICT,void> operator<(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,LeOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,LeOp,ECNSTSTRICT,EXPR0,void> operator<(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,LeOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeOp,EXPR1,ECNSTSTRICT,void> operator<(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,LeOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeOp,ECNSTSTRICT,EXPR1,void> operator<(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,LeOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,LeOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) < this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Check greaterness of array elements

template<typename T, int R> 
INLINEF Expr<bool,R,GrOp,EXPR0,EXPR0,void> operator>(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,GrOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GrOp,EXPR1,EXPR0,void> operator>(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,GrOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GrOp,EXPR0,EXPR1,void> operator>(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,GrOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,GrOp,EXPR1,EXPR2,void> operator>(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,GrOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,GrOp,EXPR0,ECNSTSTRICT,void> operator>(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,GrOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,GrOp,ECNSTSTRICT,EXPR0,void> operator>(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,GrOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GrOp,EXPR1,ECNSTSTRICT,void> operator>(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,GrOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GrOp,ECNSTSTRICT,EXPR1,void> operator>(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,GrOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,GrOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) > this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Check lesser-than or equality of array elements

template<typename T, int R> 
INLINEF Expr<bool,R,LeqOp,EXPR0,EXPR0,void> operator<=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,LeqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeqOp,EXPR1,EXPR0,void> operator<=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,LeqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeqOp,EXPR0,EXPR1,void> operator<=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,LeqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,LeqOp,EXPR1,EXPR2,void> operator<=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,LeqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,LeqOp,EXPR0,ECNSTSTRICT,void> operator<=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,LeqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR0,void> operator<=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeqOp,EXPR1,ECNSTSTRICT,void> operator<=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,LeqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR1,void> operator<=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,LeqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,LeqOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) <= this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Check greater or equality of array elements

template<typename T, int R> 
INLINEF Expr<bool,R,GeqOp,EXPR0,EXPR0,void> operator>=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,GeqOp,EXPR0,EXPR0,void>(express(a), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GeqOp,EXPR1,EXPR0,void> operator>=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,GeqOp,EXPR1,EXPR0,void>(a, express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GeqOp,EXPR0,EXPR1,void> operator>=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,GeqOp,EXPR0,EXPR1,void>(express(a), b);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,GeqOp,EXPR1,EXPR2,void> operator>=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,GeqOp,EXPR1,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,GeqOp,EXPR0,ECNSTSTRICT,void> operator>=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,GeqOp,EXPR0,ECNSTSTRICT,void>(express(a), repeatlike(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR0,void> operator>=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR0,void>(repeatlike(b,x), express(b));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GeqOp,EXPR1,ECNSTSTRICT,void> operator>=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,GeqOp,EXPR1,ECNSTSTRICT,void>(a, repeatlike(a,x));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR1,void> operator>=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,GeqOp,ECNSTSTRICT,EXPR1,void>(repeatlike(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,GeqOp,A,B,void>  : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) > this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

///////////////////////
// LOGICAL OPERATORS //
///////////////////////

// Logical AND

template<int R>
INLINEF Expr<bool,R,AndOp,EXPR0BOOL,EXPR0BOOL,void> operator&&(const Vec<bool,R>& a, const Vec<bool,R>& b)
{
    return Expr<bool,R,AndOp,EXPR0BOOL,EXPR0BOOL,void>(express(a), express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,AndOp,EXPR1BOOL,EXPR0BOOL,void> operator&&(const EXPR1BOOL &a, const Vec<bool,R> &b)
{
    return Expr<bool,R,AndOp,EXPR1BOOL,EXPR0BOOL,void>(a, express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,AndOp,EXPR0BOOL,EXPR1BOOL,void> operator&&(const Vec<bool,R> &a, const EXPR1BOOL &b)
{
    return Expr<bool,R,AndOp,EXPR0BOOL,EXPR1BOOL,void>(express(a), b);
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,AndOp,EXPR1BOOL,EXPR2BOOL,void> operator&&(const EXPR1BOOL &a, const EXPR2BOOL &b)
{
    return Expr<bool,R,AndOp,EXPR1BOOL,EXPR2BOOL,void>(a, b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,AndOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) && this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// Logical OR

template<int R>
INLINEF Expr<bool,R,OrOp,EXPR0BOOL,EXPR0BOOL,void> operator||(const Vec<bool,R>& a, const Vec<bool,R>& b)
{
    return Expr<bool,R,OrOp,EXPR0BOOL,EXPR0BOOL,void>(express(a), express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,OrOp,EXPR1BOOL,EXPR0BOOL,void> operator||(const EXPR1BOOL &a, const Vec<bool,R> &b)
{
    return Expr<bool,R,OrOp,EXPR1BOOL,EXPR0BOOL,void>(a, express(b));
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,OrOp,EXPR0BOOL,EXPR1BOOL,void> operator||(const Vec<bool,R> &a, const EXPR1BOOL &b)
{
    return Expr<bool,R,OrOp,EXPR0BOOL,EXPR1BOOL,void>(express(a), b);
}

template<int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3> 
INLINEF Expr<bool,R,OrOp,EXPR1BOOL,EXPR2BOOL,void> operator||(const EXPR1BOOL &a, const EXPR2BOOL &b)
{
    return Expr<bool,R,OrOp,EXPR1BOOL,EXPR2BOOL,void>(a, b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,OrOp,A,B,void> : public Eptrpair<A,B>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return this->a_->eval(i) || this->b_->eval(i); 
    }
    INLINEF Expr(const A &a, const B &b) : Eptrpair<A,B>(a,b) {}
};

////////////////////////////////////////////////////////////////////////////

// NOT the elements of a boolean array

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<bool,R,NotOp,EXPR1BOOL,void,void> operator!(const EXPR1BOOL& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<bool,R,NotOp,EXPR1BOOL,void,void>(a);
}

template<int R>
INLINEF Expr<bool,R,NotOp,EXPR0BOOL,void,void> operator!(const Vec<bool,R>& a)
{
    // create an intermediate expression object that will convert upon evaluation.
    return Expr<bool,R,NotOp,EXPR0BOOL,void,void>(express(a));
}

template<int R, typename A> 
class Expr<bool,R,NotOp,A,void,void> : public Eptr<A>
{ 
  public:
    INLINEF bool eval(int i) const { 
        return ! (this->a_->eval(i));
    }
    INLINEF Expr(const A &a) : Eptr<A>(a) {}
};

////////////////
// REDUCTIONS //
////////////////

// summing

template<typename T, int R>
INLINEF T sum(const Vec<T,R>& a)
{
    T y = a.element_[0];
    for (int i=1;i<R;i++)
        y += a.element_[i];
    return y;

}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF T sum(const EXPR1& a)
{
    T y = a.eval(0);
    for (int i=1; i<R; i++)
        y += a.eval(i);
    return y;
}

// multiplying

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF T product(const EXPR1& a)
{
    T y = a.eval(0);
    for (int i=1; i<R; i++)
        y *= a.eval(i);
    return y;
}

template<typename T, int R>
INLINEF T product(const Vec<T,R>& a)
{
    T y = a.element_[0];
    for (int i=1; i<R; i++)
        y *= a.element_[i];
    return y;
}

// are all boolean elements true? (&& reduction)

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF bool all(const EXPR1BOOL& a)
{
    for (int i=0;i<R;i++)
        if (not a.eval(i))
            return false;
    return true;
}

template<int R>
INLINEF bool all(const Vec<bool,R>& a)
{
    for (int i=0;i<R;i++)
        if (not a.element_[i])
            return false;
    return true;
}

// are any boolean elements true? (|| reduction)

template<int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF bool any(const EXPR1BOOL& a)
{
    for (int i=0;i<R;i++)
        if (a.eval(i))
            return true;
    return false;
}

template<int R>
INLINEF bool any(const Vec<bool,R>& a)
{
    for (int i=0;i<R;i++)
        if (a.element_[i])
            return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////

// If-then-else construct

template<typename T,int R>
INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR0> ifelse(const Vec<bool,R>& a, const Vec<T,R>& b, const Vec<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR0>(express(a), express(b), express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR0> ifelse(const EXPR1BOOL& a, const Vec<T,R>& b, const Vec<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR0>(a, express(b), express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR0> ifelse(const Vec<bool,R>& a, const EXPR1& b, const Vec<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR0>(express(a), b, express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3>
INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR1> ifelse(const Vec<bool,R>& a, const Vec<T,R>& b, const EXPR1& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR0,EXPR1>(express(a), express(b), c);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3>
INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR0> ifelse(const EXPR1BOOL& a, const EXPR2& b, const Vec<T,R>& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR0>(a, b, express(c));
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3>
INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR2> ifelse(const EXPR1BOOL& a, const Vec<T,R>& b, const EXPR2& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR0,EXPR2>(a, express(b), c);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3>
INLINEF Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR2> ifelse(const Vec<bool,R>& a, const EXPR1& b, const EXPR2& c)
{
    return Expr<T,R,IfElseOp,EXPR0BOOL,EXPR1,EXPR2>(express(a), b, c);
}

template<typename T, int R, ExOp AOP, typename A1, typename A2, typename A3, ExOp BOP, typename B1, typename B2, typename B3, ExOp COP, typename C1, typename C2, typename C3>
INLINEF Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR3> ifelse(const EXPR1BOOL& a, const EXPR2& b, const EXPR3& c)
{
    return Expr<T,R,IfElseOp,EXPR1BOOL,EXPR2,EXPR3>(a, b, c);
}

template<typename T, int R, typename A, typename B, typename C> 
class Expr<T,R,IfElseOp,A,B,C> : public Eptrtriple<A,B,C>
{ 
  public:
    INLINEF T eval(int i) const { 
        if (this->a_->eval(i))
            return this->b_->eval(i);
        else
            return this->c_->eval(i);
    }
    INLINEF Expr(const A &a, const B &b, const C &c) : Eptrtriple<A,B,C>(a,b,c) {}
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Test

int main() 
{
    Vec<double,3> va(1,1,1);
    Vec<int,3>    vb(4,4,2);
    Vec<double,3> vc(2*(va + va - va) + 2/convert<double>(4.6*vb));
    Vec<double,3> vd;
    vd = 1/convert<double>(vb);
    std::cout << vc.element_[0] << ' '<< vc.element_[1] << ' '<< vc.element_[2] << '\n';
    std::cout << vd.element_[0] << ' '<< vd.element_[1] << ' '<< vd.element_[2] << '\n';
    Vec<int,3> ve;
    ve = vb%2;
    std::cout << ve.element_[0] << ' '<< ve.element_[1] << ' '<< ve.element_[2] << '\n';
    Vec<bool,3> z (vb<3);
    Vec<double,3> y;
    std::cout << "va = " << va.element_[0] << ' '<< va.element_[1] << ' '<< va.element_[2] << '\n';
    std::cout << "vd = " << vd.element_[0] << ' '<< vd.element_[1] << ' '<< vd.element_[2] << '\n';
    std::cout << "vb = " << vb.element_[0] << ' '<< vb.element_[1] << ' '<< vb.element_[2] << '\n';
    std::cout << "z  = " <<  z.element_[0] << ' '<<  z.element_[1] << ' '<<  z.element_[2] << '\n';
    y = ifelse(!z,vd,va/2);
    std::cout << y.element_[0] << ' '<< y.element_[1] << ' '<< y.element_[2] << '\n';
    bool b = true;
    std::cout << b << '\n';
    b=all((-vb)<-1);
    std::cout << b << '\n';
    std::cout << sum(-vb) << '\n';
    return 0;
}
