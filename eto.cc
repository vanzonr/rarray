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
            PlusOp,    MinusOp,    MultOp,     DivOp,   ModOp, 
            EqOp,      NotEqOp,    LeOp,       GrOp,    LeOrEqOp,   GrOrEqOp,
            AndOp,     OrOp,       NotOp,      
            IfElseOp };

template<typename T, int R, typename A, ExOp B, typename C> class Expr;

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
    template<typename A, ExOp B, typename C> INLINEF explicit Vec(const Expr<T,R,A,B,C>& e);
    template<typename A, ExOp B, typename C> INLINEF Vec& operator=(const Expr<T,R,A,B,C>&e);
};

// element-wise expressions rarrayex


//rarray<T,R> c(a.shape());
//c = if_else(a<b,a,b);
//if (all(a==b)) ...
//if (any(a==b)) ...
//if (none(a==b)) ...

// assignment from expression

template<typename T, int R>
template<typename A, ExOp B, typename C>
INLINEF Vec<T,R>::Vec(const Expr<T,R,A,B,C>& e) 
{
    shape_[0] = e.shape()[0];
    for (int i=0; i<R; i++) {
        T thing = e.eval(i);
        a_[i] = thing;
    }
}
 
template<typename T, int R>
template<typename A, ExOp B, typename C>
INLINEF Vec<T,R>& Vec<T,R>::operator=(const Expr<T,R,A,B,C> &e) 
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

#define EXPR0 Expr<T,R,void,EquateOp,void>
#define EXPR1 Expr<T,R,A,B,C>
#define EXPR2 Expr<T,R,D,E,F>
#define ECNST Expr<T,R,void,PretendOp,TLIKE>
#define ECNSTSTRICT Expr<T,R,void,PretendOp,T>

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

template<typename TO, typename T, int R, typename A, ExOp B, typename C>
Expr<TO,R,EXPR1,ConvertOp,void> convert(const EXPR1& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return Expr<TO,R,EXPR1,ConvertOp,void>(a);
}

template<typename TO, typename T, int R>
Expr<TO,R,EXPR0,ConvertOp,void> convert(const Vec<T,R>& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return Expr<TO,R,EXPR0,ConvertOp,void>(equate(a));
}

template<typename T, int R, typename A> 
class Expr<T,R,A,ConvertOp,void> 
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

template<typename T, int R, typename TLIKE, typename A, ExOp B, typename C> 
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
    template<typename A, ExOp B, typename C>
    INLINEF Expr(const Expr<T,R,A,B,C>& a, const T& x)
    : x_(x), shape_(a.shape()) 
    {}
    friend Expr pretend<T,R,TLIKE>(const Vec<T,R>& a, const TLIKE& x);
    template<typename TT, int RR, typename CC, typename A, ExOp B, typename C> 
    friend Expr<TT,RR,void,PretendOp,CC> pretend(const Expr<TT,RR,A,B,C>& a, const CC& x);
};

////////////////////////////////////////////////////////////////////////////

// Addition of array elements

// create a PlusOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,PlusOp,EXPR0> operator+(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,PlusOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR1,PlusOp,EXPR0> operator+(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,PlusOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR0,PlusOp,EXPR1> operator+(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,PlusOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<T,R,EXPR1,PlusOp,EXPR2> operator+(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,PlusOp,EXPR2>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,PlusOp,B> 
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
INLINEF Expr<T,R,EXPR0,MinusOp,EXPR0> operator-(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,MinusOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR1,MinusOp,EXPR0> operator-(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,MinusOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR0,MinusOp,EXPR1> operator-(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,MinusOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<T,R,EXPR1,MinusOp,EXPR2> operator-(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,MinusOp,EXPR2>(a, b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,MinusOp,B> 
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

// Multiplication of array elements

// create a MultOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,MultOp,EXPR0> operator*(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,MultOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR1,MultOp,EXPR0> operator*(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,MultOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR0,MultOp,EXPR1> operator*(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,MultOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<T,R,EXPR1,MultOp,EXPR2> operator*(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,MultOp,EXPR2>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,EXPR0,MultOp,ECNST> operator*(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,EXPR0,MultOp,ECNST>(equate(a), pretend(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ECNST,MultOp,EXPR0> operator*(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,MultOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename TLIKE>
INLINEF Expr<T,R,EXPR1,MultOp,ECNST> operator*(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,EXPR1,MultOp,ECNST>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename TLIKE>
INLINEF Expr<T,R,ECNST,MultOp,EXPR1> operator*(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,MultOp,EXPR1>(pretend(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,MultOp,B> 
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
INLINEF Expr<T,R,EXPR0,DivOp,EXPR0> operator/(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,DivOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR1,DivOp,EXPR0> operator/(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,DivOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR0,DivOp,EXPR1> operator/(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,DivOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<T,R,EXPR1,DivOp,EXPR2> operator/(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,DivOp,EXPR2>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,EXPR0,DivOp,ECNST> operator/(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,EXPR0,DivOp,ECNST>(equate(a), pretend(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ECNST,DivOp,EXPR0> operator/(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,DivOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename TLIKE>
INLINEF Expr<T,R,EXPR1,DivOp,ECNST> operator/(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,EXPR1,DivOp,ECNST>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename TLIKE>
INLINEF Expr<T,R,ECNST,DivOp,EXPR1> operator/(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,DivOp,EXPR1>(pretend(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,DivOp,B> 
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
INLINEF Expr<T,R,EXPR0,ModOp,EXPR0> operator%(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,ModOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR1,ModOp,EXPR0> operator%(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,ModOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<T,R,EXPR0,ModOp,EXPR1> operator%(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,ModOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<T,R,EXPR1,ModOp,EXPR2> operator%(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,ModOp,EXPR2>(a, b);
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,EXPR0,ModOp,ECNST> operator%(const Vec<T,R> &a, const TLIKE& x)
{
    return Expr<T,R,EXPR0,ModOp,ECNST>(equate(a), pretend(a,x));
}

template<typename T, int R, typename TLIKE>
INLINEF Expr<T,R,ECNST,ModOp,EXPR0> operator%(const TLIKE& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,ModOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C, typename TLIKE>
INLINEF Expr<T,R,EXPR1,ModOp,ECNST> operator%(const EXPR1 &a, const TLIKE& x)
{
    return Expr<T,R,EXPR1,ModOp,ECNST>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C, typename TLIKE>
INLINEF Expr<T,R,ECNST,ModOp,EXPR1> operator%(const TLIKE &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,ModOp,EXPR1>(pretend(b,x), b);
}

template<typename T, int R, typename A, typename B> 
class Expr<T,R,A,ModOp,B> 
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
INLINEF Expr<bool,R,EXPR0,EqOp,EXPR0> operator==(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,EqOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,EqOp,EXPR0> operator==(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,EqOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR0,EqOp,EXPR1> operator==(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,EqOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<bool,R,EXPR1,EqOp,EXPR2> operator==(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,EqOp,EXPR2>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,EqOp,ECNSTSTRICT> operator==(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,EqOp,ECNSTSTRICT>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,EqOp,EXPR0> operator==(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,EqOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,EqOp,ECNSTSTRICT> operator==(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,EqOp,ECNSTSTRICT>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,ECNSTSTRICT,EqOp,EXPR1> operator==(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,EqOp,EXPR1>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,EqOp,B> 
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
INLINEF Expr<bool,R,EXPR0,NotEqOp,EXPR0> operator!=(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,NotEqOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,NotEqOp,EXPR0> operator!=(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,NotEqOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR0,NotEqOp,EXPR1> operator!=(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,NotEqOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<bool,R,EXPR1,NotEqOp,EXPR2> operator!=(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,NotEqOp,EXPR2>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,NotEqOp,ECNSTSTRICT> operator!=(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,NotEqOp,ECNSTSTRICT>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR0> operator!=(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,NotEqOp,ECNSTSTRICT> operator!=(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,NotEqOp,ECNSTSTRICT>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR1> operator!=(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,NotEqOp,EXPR1>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,NotEqOp,B> 
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
INLINEF Expr<bool,R,EXPR0,LeOp,EXPR0> operator<(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,LeOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,LeOp,EXPR0> operator<(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,LeOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR0,LeOp,EXPR1> operator<(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,LeOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<bool,R,EXPR1,LeOp,EXPR2> operator<(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,LeOp,EXPR2>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,LeOp,ECNSTSTRICT> operator<(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,LeOp,ECNSTSTRICT>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,LeOp,EXPR0> operator<(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,LeOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,LeOp,ECNSTSTRICT> operator<(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,LeOp,ECNSTSTRICT>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,ECNSTSTRICT,LeOp,EXPR1> operator<(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,LeOp,EXPR1>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,LeOp,B> 
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

// Check inequality of array elements

// create a GrOp expression from parts
template<typename T, int R> 
INLINEF Expr<bool,R,EXPR0,GrOp,EXPR0> operator>(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR0,GrOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,GrOp,EXPR0> operator>(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<bool,R,EXPR1,GrOp,EXPR0>(a, equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR0,GrOp,EXPR1> operator>(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<bool,R,EXPR0,GrOp,EXPR1>(equate(a), b);
}

template<typename T, int R, typename A, ExOp B, typename C, typename D, ExOp E, typename F> 
INLINEF Expr<bool,R,EXPR1,GrOp,EXPR2> operator>(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<bool,R,EXPR1,GrOp,EXPR2>(a, b);
}

template<typename T, int R>
INLINEF Expr<bool,R,EXPR0,GrOp,ECNSTSTRICT> operator>(const Vec<T,R> &a, const T& x)
{
    return Expr<bool,R,EXPR0,GrOp,ECNSTSTRICT>(equate(a), pretend(a,x));
}

template<typename T, int R>
INLINEF Expr<bool,R,ECNSTSTRICT,GrOp,EXPR0> operator>(const T& x, const Vec<T,R> &b)
{
    return Expr<bool,R,ECNSTSTRICT,GrOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,EXPR1,GrOp,ECNSTSTRICT> operator>(const EXPR1 &a, const T& x)
{
    return Expr<bool,R,EXPR1,GrOp,ECNSTSTRICT>(a, pretend(a,x));
}

template<typename T, int R, typename A, ExOp B, typename C>
INLINEF Expr<bool,R,ECNSTSTRICT,GrOp,EXPR1> operator>(const T& x, const EXPR1 &b)
{
    return Expr<bool,R,ECNSTSTRICT,GrOp,EXPR1>(pretend(b,x), b);
}

template<int R, typename A, typename B> 
class Expr<bool,R,A,GrOp,B> 
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

// reductions (why not also sum, prod, norm, norm1, and norm2)?

template<int R, typename A, enum ExOp B, typename C>
bool all(const Expr<bool,R,A,B,C>&a)
{
    for (int i=0;i<R;i++)
        if (not a.eval(i))
            return false;
    return true;
}

template<int R, typename A, enum ExOp B, typename C>
bool any(const Expr<bool,R,A,B,C>&a)
{
    for (int i=0;i<R;i++)
        if (a.eval(i))
            return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

// Test

int main() 
{
    Vec<double,3> va(1,1,1);
    Vec<int,3>    vb(1,1,2);
    Vec<double,3> vc(2*(va + va - va) + 2/convert<double>(4.6*vb));
    Vec<double,3> vd;
    vd = 1/convert<double>(vb);
    std::cout << vc.a_[0] << ' '<< vc.a_[1] << ' '<< vc.a_[2] << '\n';
    std::cout << vd.a_[0] << ' '<< vd.a_[1] << ' '<< vd.a_[2] << '\n';
    Vec<int,3> ve;
    ve = vb%2;
    std::cout << ve.a_[0] << ' '<< ve.a_[1] << ' '<< ve.a_[2] << '\n';
    bool b=any(vb==1);
    std::cout << b << '\n';
    b=all(vb>1);
    std::cout << b << '\n';
    return 0;
}
