#include <iostream>

#if not defined(INLINEF)
# if defined(__INTEL_COMPILER)
#   define INLINEF  __forceinline
# elif defined(__GNUC__)
#   define INLINEF inline __attribute__((always_inline)) 
# else
#   define INLINEF inline
# endif
#endif

enum ExOp { EquateOp,  ConvertOp,  PretendOp, 
            PlusOp,    MinusOp,    MultOp,     DivOp,   ModOp,      NegOp,
            EqOp,      NotEqOp,    LeOp,       GrOp,    LeOrEqOp,   GrOrEqOp,
            AndOp,     OrOp,       NotOp,      IfElseOp };

template<typename T, int R, typename A, ExOp B, typename C, typename CC> class Expr;

template<typename T, int R>
class Vec 
{
  public:
    T a_[R];
    int shape_[1];
    Vec() {} 
    Vec(T a, T b, T c) {
        a_[0] = a;
        a_[1] = b;
        a_[2] = c;
    }
    template<typename A, ExOp B, typename C, typename CC> INLINEF explicit Vec(const Expr<T,R,A,B,C,CC>& e);
    template<typename A, ExOp B, typename C, typename CC> INLINEF Vec& operator=(const Expr<T,R,A,B,C,CC>&e);
};

///////////////////////////////////////////////////////////////////////////////////////////////

// start element-wise expressions rarrayex

///////////////////////////////////////////////////////////////////////////////////////////////

// assignment from expression

template<typename T, int R>
template<typename A, ExOp B, typename C, typename CC>
INLINEF Vec<T,R>::Vec(const Expr<T,R,A,B,C,CC>& e) 
{
    shape_[0] = e.shape()[0];
    for (int i=0; i<R; i++) {
        T thing = e.eval(i);
        a_[i] = thing;
    }
}
 
template<typename T, int R>
template<typename A, ExOp B, typename C, typename CC>
INLINEF Vec<T,R>& Vec<T,R>::operator=(const Expr<T,R,A,B,C,CC> &e) 
{
    // Check shape compatibility
    // Last R-1 dimensions must match.
    // First dimension of e may be less than that of *this.
    // In that case, it wraps around.
    // size = mult(e.shape())
    int size = R;
    int mysize = R;
    if (mysize<size) size = mysize;
    for (int i=0; i<size; i++)
        a_[i] = e.eval(i);
    return *this;
}

// useful abbreviating macros

#define EXPR0 Expr<T,R,void,EquateOp,void,void>
#define EXPR1 Expr<T,R,A,B,C,CC>
#define EXPR2 Expr<T,R,D,E,F,FF>
#define EXPR3 Expr<T,R,G,H,I,II>
#define ECNST Expr<T,R,void,PretendOp,TLIKE,void>
#define ECNSTSTRICT Expr<T,R,void,PretendOp,T,void>
#define EXPR0BOOL Expr<bool,R,void,EquateOp,void,void>
#define EXPR1BOOL Expr<bool,R,A,B,C,CC>
#define EXPR2BOOL Expr<bool,R,D,E,F,FF>
#define EXPR3BOOL Expr<bool,R,G,H,I,II>

// Basic expression, a wrapper around the array

template<typename T, int R> 
INLINEF EXPR0 equate(const Vec<T,R>& a)
{ 
    // create a basic expression out of an array
    return EXPR0(a);
}

template<typename T, int R> 
class EXPR0 
{
  public:
    INLINEF T eval(int i) const { 
        return a_.a_[i]; 
    }
    INLINEF const int* shape() const {
        return shape_;
    }
  private:
    const Vec<T,R>& a_;
    const int* shape_;
    INLINEF Expr(const Vec<T,R>& a)
    : a_(a), shape_(a.shape_) 
    {}
    friend Expr equate<T,R>(const Vec<T,R>& a);
};

////////////////////////////////////////////////////////////////////////////

// Conversion of the elements of arrays

template<typename TO, typename T, int R, typename A, ExOp B, typename C, typename CC>
Expr<TO,R,EXPR1,ConvertOp,void,void> convert(const EXPR1& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return a;//Expr<TO,R,EXPR1,ConvertOp,void>(a);
}

template<typename TO, typename T, int R>
Expr<TO,R,EXPR0,ConvertOp,void,void> convert(const Vec<T,R>& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return equate(a);//Expr<TO,R,EXPR0,ConvertOp,void>(equate(a));
}

template<typename T, int R, typename A> 
class Expr<T,R,A,ConvertOp,void,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return (T)(a_.eval(i));
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a)
    : a_(a), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const int* shape_;    
};

////////////////////////////////////////////////////////////////////////////

// Pretend to be an array of given shape same as a, but with all
// element equal to x (without creating such an object).

template<typename T, int R, typename TLIKE> 
INLINEF ECNST pretend(const Vec<T,R>& a, const TLIKE& x)
{ 
    // create a basic expression out of an array
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename TLIKE, typename A, ExOp B, typename C, typename CC> 
INLINEF ECNST pretend(const EXPR1& a, const TLIKE& x)
{ 
    // create a basic expression out of an array
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename TLIKE> 
class ECNST
{
  public:
    INLINEF T eval(int i) const { 
        return x_; 
    }
    INLINEF const int* shape() const {
        return shape_;
    }
  private:
    const T x_;
    const int* shape_;
    INLINEF Expr(const Vec<T,R>& a, const T& x)
    : x_(x), shape_(a.shape_) 
    {}
    template<typename A, ExOp B, typename C, typename CC>
    INLINEF Expr(const Expr<T,R,A,B,C,CC>& a, const T& x)
    : x_(x), shape_(a.shape()) 
    {}
    friend Expr pretend<T,R,TLIKE>(const Vec<T,R>& a, const TLIKE& x);
    template<typename TT, int RR, typename TTT, typename A, ExOp B, typename C, typename CC> 
    friend Expr<TT,RR,void,PretendOp,TTT,void> pretend(const Expr<TT,RR,A,B,C,CC>& a, const TTT& x);
};

////////////////////////////////////////////////////////////////////////////

// Addition of array elements

// create a PlusOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,PlusOp,EXPR0,void> operator+(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,PlusOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR1,PlusOp,EXPR0,void> operator+(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,PlusOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR0,PlusOp,EXPR1,void> operator+(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,PlusOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<T,R,EXPR1,PlusOp,EXPR2,void> operator+(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,PlusOp,EXPR2,void>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,PlusOp,B,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return a_.eval(i) + b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Subtraction of array elements

// create a MinusOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,MinusOp,EXPR0,void> operator-(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,MinusOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR1,MinusOp,EXPR0,void> operator-(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,MinusOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR0,MinusOp,EXPR1,void> operator-(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,MinusOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<T,R,EXPR1,MinusOp,EXPR2,void> operator-(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,MinusOp,EXPR2,void>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,MinusOp,B,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return a_.eval(i) - b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Negate elements of an array

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
Expr<T,R,EXPR1,NegOp,void,void> operator-(const EXPR1& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return a;//Expr<T,R,EXPR1,NegOp,void>(a);
}

template<typename T, int R>
Expr<T,R,EXPR0,NegOp,void,void> operator-(const Vec<T,R>& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return equate(a);//Expr<T,R,EXPR0,NegOp,void>(equate(a));
}

template<typename T, int R, typename A> 
class Expr<T,R,A,NegOp,void,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return -a_.eval(i);
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a)
    : a_(a), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const int* shape_;    
};

////////////////////////////////////////////////////////////////////////////

// Multiplication of array elements

// create a MultOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,MultOp,EXPR0,void> operator*(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,MultOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR1,MultOp,EXPR0,void> operator*(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,MultOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
    INLINEF Expr<T,R,EXPR0,MultOp,EXPR1,void> operator*(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,MultOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<T,R,EXPR1,MultOp,EXPR2,void> operator*(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,MultOp,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,EXPR0,MultOp,ECNST,void> operator*(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,EXPR0,MultOp,ECNST,void>(equate(a), pretend(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ECNST,MultOp,EXPR0,void> operator*(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,MultOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename TLIKE>
INLINEF Expr<T,R,EXPR1,MultOp,ECNST,void> operator*(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,EXPR1,MultOp,ECNST,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename TLIKE>
    INLINEF Expr<T,R,ECNST,MultOp,EXPR1,void> operator*(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,MultOp,EXPR1,void>(pretend(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,MultOp,B,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return a_.eval(i) * b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Division of array elements

// create a DivOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,DivOp,EXPR0,void> operator/(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,DivOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR1,DivOp,EXPR0,void> operator/(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,DivOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR0,DivOp,EXPR1,void> operator/(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,DivOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<T,R,EXPR1,DivOp,EXPR2,void> operator/(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,DivOp,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,EXPR0,DivOp,ECNST,void> operator/(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,EXPR0,DivOp,ECNST,void>(equate(a), pretend(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ECNST,DivOp,EXPR0,void> operator/(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,DivOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename TLIKE>
INLINEF Expr<T,R,EXPR1,DivOp,ECNST,void> operator/(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,EXPR1,DivOp,ECNST,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename TLIKE>
INLINEF Expr<T,R,ECNST,DivOp,EXPR1,void> operator/(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,DivOp,EXPR1,void>(pretend(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,DivOp,B,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return a_.eval(i) / b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Modulus of array elements

// create a ModOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,ModOp,EXPR0, void> operator%(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,ModOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR1,ModOp,EXPR0, void> operator%(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,ModOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR0,ModOp,EXPR1, void> operator%(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,ModOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<T,R,EXPR1,ModOp,EXPR2, void> operator%(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,ModOp,EXPR2,void>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,EXPR0,ModOp,ECNST, void> operator%(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,EXPR0,ModOp,ECNST,void>(equate(a), pretend(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ECNST,ModOp,EXPR0, void> operator%(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,ModOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename TLIKE>
INLINEF Expr<T,R,EXPR1,ModOp,ECNST, void> operator%(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,EXPR1,ModOp,ECNST,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename TLIKE>
INLINEF Expr<T,R,ECNST,ModOp,EXPR1, void> operator%(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,ModOp,EXPR1,void>(pretend(b,x), b);
}

template<typename T, int R, typename A, typename B> 
    class Expr<T,R,A,ModOp,B,void> 
{ 
  public:
    INLINEF T eval(int i) const { 
        return a_.eval(i) % b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Check equality of array elements

// create a EqOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,EqOp,EXPR0, void> operator==(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,EqOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,EqOp,EXPR0, void> operator==(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,EqOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0,EqOp,EXPR1, void> operator==(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,EqOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1,EqOp,EXPR2, void> operator==(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,EqOp,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,EqOp,ECNSTSTRICT, void> operator==(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,EqOp,ECNSTSTRICT,void>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,EqOp,EXPR0, void> operator==(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,EqOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,EqOp,ECNSTSTRICT,void> operator==(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,EqOp,ECNSTSTRICT,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,ECNSTSTRICT,EqOp,EXPR1,void> operator==(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,EqOp,EXPR1,void>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,EqOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) == b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Check inequality of array elements

// create a NotEqOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,NotEqOp,EXPR0, void> operator!=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,NotEqOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,NotEqOp,EXPR0, void> operator!=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,NotEqOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0,NotEqOp,EXPR1, void> operator!=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,NotEqOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1,NotEqOp,EXPR2, void> operator!=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,NotEqOp,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,NotEqOp,ECNSTSTRICT, void> operator!=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,NotEqOp,ECNSTSTRICT,void>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR0, void> operator!=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,NotEqOp,ECNSTSTRICT, void> operator!=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,NotEqOp,ECNSTSTRICT,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR1, void> operator!=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR1,void>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,NotEqOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) != b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Check lesser-than of array elements

// create a LeOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,LeOp,EXPR0, void> operator<(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,LeOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,LeOp,EXPR0, void> operator<(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,LeOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0,LeOp,EXPR1, void> operator<(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,LeOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1,LeOp,EXPR2, void> operator<(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,LeOp,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,LeOp,ECNSTSTRICT, void> operator<(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,LeOp,ECNSTSTRICT,void>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,LeOp,EXPR0, void> operator<(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,LeOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,LeOp,ECNSTSTRICT, void> operator<(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,LeOp,ECNSTSTRICT,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,ECNSTSTRICT,LeOp,EXPR1, void> operator<(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,LeOp,EXPR1,void>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,LeOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) < b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Check greaterness of array elements

// create a GrOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,GrOp,EXPR0, void> operator>(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,GrOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,GrOp,EXPR0, void> operator>(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,GrOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0,GrOp,EXPR1, void> operator>(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,GrOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1,GrOp,EXPR2, void> operator>(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,GrOp,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,GrOp,ECNSTSTRICT, void> operator>(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,GrOp,ECNSTSTRICT,void>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,GrOp,EXPR0, void> operator>(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,GrOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,GrOp,ECNSTSTRICT, void> operator>(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,GrOp,ECNSTSTRICT,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,ECNSTSTRICT,GrOp,EXPR1, void> operator>(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,GrOp,EXPR1,void>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,GrOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) > b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Check lesser-than or equality of array elements

// create a LeOrEqOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,LeOrEqOp,EXPR0, void> operator<=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,LeOrEqOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,LeOrEqOp,EXPR0, void> operator<=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,LeOrEqOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0,LeOrEqOp,EXPR1, void> operator<=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,LeOrEqOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1,LeOrEqOp,EXPR2, void> operator<=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,LeOrEqOp,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,LeOrEqOp,ECNSTSTRICT, void> operator<=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,LeOrEqOp,ECNSTSTRICT,void>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,LeOrEqOp,EXPR0, void> operator<=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,LeOrEqOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,LeOrEqOp,ECNSTSTRICT, void> operator<=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,LeOrEqOp,ECNSTSTRICT,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,ECNSTSTRICT,LeOrEqOp,EXPR1, void> operator<=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,LeOrEqOp,EXPR1,void>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,LeOrEqOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) <= b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Check greater or equality of array elements

// create a GrOrEqOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,GrOrEqOp,EXPR0, void> operator>=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,GrOrEqOp,EXPR0,void>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,GrOrEqOp,EXPR0, void> operator>=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,GrOrEqOp,EXPR0,void>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0,GrOrEqOp,EXPR1, void> operator>=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,GrOrEqOp,EXPR1,void>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1,GrOrEqOp,EXPR2, void> operator>=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,GrOrEqOp,EXPR2,void>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,GrOrEqOp,ECNSTSTRICT, void> operator>=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,GrOrEqOp,ECNSTSTRICT,void>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,GrOrEqOp,EXPR0, void> operator>=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,GrOrEqOp,EXPR0,void>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1,GrOrEqOp,ECNSTSTRICT, void> operator>=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,GrOrEqOp,ECNSTSTRICT,void>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,ECNSTSTRICT,GrOrEqOp,EXPR1, void> operator>=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,GrOrEqOp,EXPR1,void>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,GrOrEqOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) > b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Logical Operators 

////////////////////////////////////////////////////////////////////////////

// Logical AND

template<int R>
INLINEF Expr<bool,R,EXPR0BOOL,AndOp,EXPR0BOOL, void> operator&&(const Vec<bool,R>& a, const Vec<bool,R>& b)
{
    return Expr<bool,R,EXPR0BOOL,AndOp,EXPR0BOOL,void>(equate(a), equate(b));
}

template<int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1BOOL,AndOp,EXPR0BOOL, void> operator&&(const EXPR1BOOL &a, const Vec<bool,R> &b)
{
    return Expr<bool,R,EXPR1BOOL,AndOp,EXPR0BOOL,void>(a, equate(b));
}

template<int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0BOOL,AndOp,EXPR1BOOL, void> operator&&(const Vec<bool,R> &a, const EXPR1BOOL &b)
{
    return Expr<bool,R,EXPR0BOOL,AndOp,EXPR1BOOL,void>(equate(a), b);
}

template<int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1BOOL,AndOp,EXPR2BOOL, void> operator&&(const EXPR1BOOL &a, const EXPR2BOOL &b)
{
    return Expr<bool,R,EXPR1BOOL,AndOp,EXPR2BOOL,void>(a, b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,AndOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) && b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Logical OR

template<int R>
INLINEF Expr<bool,R,EXPR0BOOL,OrOp,EXPR0BOOL, void> operator||(const Vec<bool,R>& a, const Vec<bool,R>& b)
{
    return Expr<bool,R,EXPR0BOOL,OrOp,EXPR0BOOL,void>(equate(a), equate(b));
}

template<int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1BOOL,OrOp,EXPR0BOOL, void> operator||(const EXPR1BOOL &a, const Vec<bool,R> &b)
{
    return Expr<bool,R,EXPR1BOOL,OrOp,EXPR0BOOL,void>(a, equate(b));
}

template<int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR0BOOL,OrOp,EXPR1BOOL, void> operator||(const Vec<bool,R> &a, const EXPR1BOOL &b)
{
    return Expr<bool,R,EXPR0BOOL,OrOp,EXPR1BOOL,void>(equate(a), b);
}

template<int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF> 
INLINEF Expr<bool,R,EXPR1BOOL,OrOp,EXPR2BOOL, void> operator||(const EXPR1BOOL &a, const EXPR2BOOL &b)
{
    return Expr<bool,R,EXPR1BOOL,OrOp,EXPR2BOOL,void>(a, b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,OrOp,B,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return a_.eval(i) || b_.eval(i); 
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b)
    : a_(a), b_(b), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// Not the elements of a boolean array

template<int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<bool,R,EXPR1BOOL,NotOp,void,void> operator!(const EXPR1BOOL& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return a;//Expr<T,R,EXPR1BOOL,NotOp,void,void>(a);
}

template<int R>
INLINEF Expr<bool,R,EXPR0BOOL,NotOp,void, void> operator!(const Vec<bool,R>& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return equate(a);//Expr<T,R,EXPR0BOOL,NotOp,void,void>(equate(a));
}

template<int R, typename A> 
class Expr<bool,R,A,NotOp,void,void> 
{ 
  public:
    INLINEF bool eval(int i) const { 
        return ! (a_.eval(i));
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a)
    : a_(a), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const int* shape_;
};

////////////////////////////////////////////////////////////////////////////

// reductions

// summing

template<typename T, int R>
INLINEF T sum(const Vec<T,R>& a)
{
    T y = a.a_[0];
    for (int i=1;i<R;i++)
        y += a.a_[i];
    return y;
}

template<typename T, int R, typename A, enum ExOp B, typename C, typename CC>
INLINEF T sum(const Expr<T,R,A,B,C,CC>& a)
{
    T y = a.eval(0);
    for (int i=1; i<R; i++)
        y += a.eval(i);
    return y;
}

// multiplying

template<typename T, int R, typename A, enum ExOp B, typename C, typename CC>
INLINEF T product(const Expr<T,R,A,B,C,CC>& a)
{
    T y = a.eval(0);
    for (int i=1; i<R; i++)
        y *= a.eval(i);
    return y;
}

template<typename T, int R>
INLINEF T product(const Vec<T,R>& a)
{
    T y = a.a_[0];
    for (int i=1; i<R; i++)
        y *= a.a_[i];
    return y;
}

// are all boolean elements true? (&& reduction)

template<int R, typename A, enum ExOp B, typename C, typename CC>
INLINEF bool all(const Expr<bool,R,A,B,C,CC>&a)
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
        if (not a.a_[i])
            return false;
    return true;
}

// are any boolean elements true? (|| reduction)

template<int R, typename A, enum ExOp B, typename C, typename CC>
INLINEF bool any(const Expr<bool,R,A,B,C,CC>&a)
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
        if (a.a_[i])
            return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////

// If-then-else construct

template<typename T,int R>
INLINEF Expr<T,R,EXPR0BOOL,IfElseOp,EXPR0,EXPR0> ifelse(const Vec<bool,R>& a, const Vec<T,R>& b, const Vec<T,R>& c)
{
    return Expr<T,R,EXPR0BOOL,IfElseOp,EXPR0,EXPR0>(equate(a), equate(b), equate(c));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR1BOOL,IfElseOp,EXPR0,EXPR0> ifelse(const EXPR1BOOL& a, const Vec<T,R>& b, const Vec<T,R>& c)
{
    return Expr<T,R,EXPR1BOOL,IfElseOp,EXPR0,EXPR0>(a, equate(b), equate(c));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
INLINEF Expr<T,R,EXPR0BOOL,IfElseOp,EXPR1,EXPR0> ifelse(const Vec<bool,R>& a, const EXPR1& b, const Vec<T,R>& c)
{
    return Expr<T,R,EXPR0BOOL,IfElseOp,EXPR1,EXPR0>(equate(a), b, equate(c));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC>
    INLINEF Expr<T,R,EXPR0BOOL,IfElseOp,EXPR0,EXPR1> ifelse(const Vec<bool,R>& a, const Vec<T,R>& b, const EXPR1& c)
{
    return Expr<T,R,EXPR0BOOL,IfElseOp,EXPR0,EXPR1>(equate(a), equate(b), c);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF>
INLINEF Expr<T,R,EXPR1BOOL,IfElseOp,EXPR2,EXPR0> ifelse(const EXPR1BOOL& a, const EXPR2& b, const Vec<T,R>& c)
{
    return Expr<T,R,EXPR1BOOL,IfElseOp,EXPR2,EXPR0>(a, b, equate(c));
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF>
INLINEF Expr<T,R,EXPR1BOOL,IfElseOp,EXPR0,EXPR2> ifelse(const EXPR1BOOL& a, const Vec<T,R>& b, const EXPR2& c)
{
    return Expr<T,R,EXPR1BOOL,IfElseOp,EXPR0,EXPR2>(a, equate(b), c);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF>
INLINEF Expr<T,R,EXPR0BOOL,IfElseOp,EXPR1,EXPR2> ifelse(const Vec<bool,R>& a, const EXPR1& b, const EXPR2& c)
{
    return Expr<T,R,EXPR0BOOL,IfElseOp,EXPR1,EXPR2>(equate(a), b, c);
}

template<typename T, int R, typename A, ExOp B, typename C, typename CC, typename D, ExOp E, typename F, typename FF, typename G, ExOp H, typename I, typename II>
INLINEF Expr<T,R,EXPR1BOOL,IfElseOp,EXPR2,EXPR3> ifelse(const EXPR1BOOL& a, const EXPR2& b, const EXPR3& c)
{
    return Expr<T,R,EXPR1BOOL,IfElseOp,EXPR2,EXPR3>(a, b, c);
}

template<typename T, int R, typename A, typename B, typename C> 
class Expr<T,R,A,IfElseOp,B,C> 
{ 
  public:
    INLINEF T eval(int i) const { 
        if (a_.eval(i))
            return b_.eval(i);
        else
            return c_.eval(i);
    };
    INLINEF const int* shape() const {
        return shape_;
    }
    INLINEF Expr(const A &a, const B &b, const C &c)
    : a_(a), b_(b), c_(c), shape_(a.shape()) 
    {}
  private:
    const A a_; 
    const B b_;
    const C c_;
    const int* shape_;
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
    std::cout << vc.a_[0] << ' '<< vc.a_[1] << ' '<< vc.a_[2] << '\n';
    std::cout << vd.a_[0] << ' '<< vd.a_[1] << ' '<< vd.a_[2] << '\n';
    Vec<int,3> ve;
    ve = vb%2;
    std::cout << ve.a_[0] << ' '<< ve.a_[1] << ' '<< ve.a_[2] << '\n';
    Vec<bool,3> z (vb<3);
    Vec<double,3> y;
    std::cout << "va = " << va.a_[0] << ' '<< va.a_[1] << ' '<< va.a_[2] << '\n';
    std::cout << "vd = " << vd.a_[0] << ' '<< vd.a_[1] << ' '<< vd.a_[2] << '\n';
    std::cout << "vb = " << vb.a_[0] << ' '<< vb.a_[1] << ' '<< vb.a_[2] << '\n';
    std::cout << "z  = " <<  z.a_[0] << ' '<<  z.a_[1] << ' '<<  z.a_[2] << '\n';
    y = ifelse(!z,vd,va/2);
    std::cout << y.a_[0] << ' '<< y.a_[1] << ' '<< y.a_[2] << '\n';
    bool b = true;
    std::cout << b << '\n';
    b=all((-vb)<-1);
    std::cout << b << '\n';
    std::cout << sum(-vb) << '\n';
    return 0;
}
