#if not defined(INLINE)
# if defined(__INTEL_COMPILER)
#   define INLINE  __forceinline
# elif defined(__GNUC__)
#   define INLINE inline __attribute__((always_inline)) 
# else
#   define INLINE inline
# endif
#endif

#define INLINEF INLINE
#define INLINE_ inline

template<typename T, int R, class A, int B, class C> class Expr;

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
    template<class A, int B, class C> 
    INLINEF Vec(const Expr<T,R,A,B,C>& e);
    template<class A, int B, class C> 
    INLINEF Vec& operator=(const Expr<T,R,A,B,C>&e);
};

// element-wise expressions rarrayex

class Base;
enum { EquateOp=1, ConvertOp=2, PretendOp=3,
       PlusOp=4, MinusOp=5, MultOp=6, DivOp=7, ModulusOp=8, 
       EqualOp, NotEqualOp, LessOp, GreaterOp, LessOrEqualOp, GreaterOrEqualOp,
       AndOp, OrOp, NotOp,
       IfThenElseOp };

//rarray<T,R> c(a.shape());
//c = if_else(a<b,a,b);
//if (all(a==b)) ...
//if (any(a==b)) ...
//if (none(a==b)) ...

// assignment from expression

template<typename T, int R>
template<class A, int B, class C>
INLINEF Vec<T,R>::Vec(const Expr<T,R,A,B,C>& e) 
{
    shape_[0] = e.shape()[0];
    for (int i=0; i<R; i++)
        a_[i] = e.eval(i);
}
 
template<typename T, int R>
template<class A, int B, class C>
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

#define EXPR0 Expr<T,R,Base,EquateOp,Base>
#define EXPR1 Expr<T,R,A,B,C>
#define EXPR2 Expr<T,R,D,E,F>
#define ECNST Expr<T,R,Base,PretendOp,CONVERT>

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

template<typename TO, typename T, int R, class A, int B, class C>
Expr<TO,R,EXPR1,ConvertOp,Base> convert(const EXPR1& a)
{
    // create an intermediate expression object that will convert upon
    // evaluation.
    return EXPR1(a);
}

template<typename T, int R, class A> 
class Expr<T,R,A,ConvertOp,Base> 
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

template<typename T, int R, typename CONVERT> 
INLINEF ECNST pretend(const Vec<T,R>& a, const CONVERT& x)
{ 
    // create a basic expression out of an array
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename CONVERT, typename A, int B, typename C> 
INLINEF ECNST pretend(const EXPR1& a, const CONVERT& x)
{ 
    // create a basic expression out of an array
    return ECNST(a, (const T&)(x));
}

template<typename T, int R, typename CONVERT> 
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
    friend Expr pretend<T,R,CONVERT>(const Vec<T,R>& a, const CONVERT& x);
    template<typename TT, int RR, typename CC, typename A, int B, typename C> 
    friend Expr<TT,RR,Base,PretendOp,CC> pretend(const Expr<TT,RR,A,B,C>& a, const CC& x);
};

////////////////////////////////////////////////////////////////////////////

// Addition of array elements

// create a PlusOp expression from parts
template<typename T, int R> 
INLINEF Expr<T,R,EXPR0,PlusOp,EXPR0> operator+(const Vec<T,R> &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR0,PlusOp,EXPR0>(equate(a), equate(b));
}

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR1,PlusOp,EXPR0> operator+(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,PlusOp,EXPR0>(a, equate(b));
}

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR0,PlusOp,EXPR1> operator+(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,PlusOp,EXPR1>(equate(a), b);
}

template<typename T, int R, class A, int B, class C, class D, int E, class F> 
INLINEF Expr<T,R,EXPR1,PlusOp,EXPR2> operator+(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,PlusOp,EXPR2>(a, b);
}

template<typename T, int R, class A, class B> 
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

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR1,MinusOp,EXPR0> operator-(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,MinusOp,EXPR0>(a, equate(b));
}

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR0,MinusOp,EXPR1> operator-(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,MinusOp,EXPR1>(equate(a), b);
}

template<typename T, int R, class A, int B, class C, class D, int E, class F> 
INLINEF Expr<T,R,EXPR1,MinusOp,EXPR2> operator-(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,MinusOp,EXPR2>(a, b);
}

template<typename T, int R, class A, class B> 
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

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR1,MultOp,EXPR0> operator*(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,MultOp,EXPR0>(a, equate(b));
}

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR0,MultOp,EXPR1> operator*(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,MultOp,EXPR1>(equate(a), b);
}

template<typename T, int R, class A, int B, class C, class D, int E, class F> 
INLINEF Expr<T,R,EXPR1,MultOp,EXPR2> operator*(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,MultOp,EXPR2>(a, b);
}

template<typename T, int R, typename CONVERT>
INLINEF Expr<T,R,EXPR0,MultOp,ECNST> operator*(const Vec<T,R> &a, const CONVERT& x)
{
    return Expr<T,R,EXPR0,MultOp,ECNST>(equate(a), pretend(a,x));
}

template<typename T, int R, typename CONVERT>
INLINEF Expr<T,R,ECNST,MultOp,EXPR0> operator*(const CONVERT& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,MultOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, class A, int B, class C, typename CONVERT>
INLINEF Expr<T,R,EXPR1,MultOp,ECNST> operator*(const EXPR1 &a, const CONVERT& x)
{
    return Expr<T,R,EXPR1,MultOp,ECNST>(a, pretend(a,x));
}

template<typename T, int R, class A, int B, class C, typename CONVERT>
INLINEF Expr<T,R,ECNST,MultOp,EXPR1> operator*(const CONVERT &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,MultOp,EXPR1>(pretend(b,x), b);
}

template<typename T, int R, class A, class B> 
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

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR1,DivOp,EXPR0> operator/(const EXPR1 &a, const Vec<T,R> &b)
{
    return Expr<T,R,EXPR1,DivOp,EXPR0>(a, equate(b));
}

template<typename T, int R, class A, int B, class C>
INLINEF Expr<T,R,EXPR0,DivOp,EXPR1> operator/(const Vec<T,R> &a, const EXPR1 &b)
{
    return Expr<T,R,EXPR0,DivOp,EXPR1>(equate(a), b);
}

template<typename T, int R, class A, int B, class C, class D, int E, class F> 
INLINEF Expr<T,R,EXPR1,DivOp,EXPR2> operator/(const EXPR1 &a, const EXPR2 &b)
{
    return Expr<T,R,EXPR1,DivOp,EXPR2>(a, b);
}

template<typename T, int R, typename CONVERT>
INLINEF Expr<T,R,EXPR0,DivOp,ECNST> operator/(const Vec<T,R> &a, const CONVERT& x)
{
    return Expr<T,R,EXPR0,DivOp,ECNST>(equate(a), pretend(a,x));
}

template<typename T, int R, typename CONVERT>
INLINEF Expr<T,R,ECNST,DivOp,EXPR0> operator/(const CONVERT& x, const Vec<T,R> &b)
{
    return Expr<T,R,ECNST,DivOp,EXPR0>(pretend(b,x), equate(b));
}

template<typename T, int R, class A, int B, class C, typename CONVERT>
INLINEF Expr<T,R,EXPR1,DivOp,ECNST> operator/(const EXPR1 &a, const CONVERT& x)
{
    return Expr<T,R,EXPR1,DivOp,ECNST>(a, pretend(a,x));
}

template<typename T, int R, class A, int B, class C, typename CONVERT>
INLINEF Expr<T,R,ECNST,DivOp,EXPR1> operator/(const CONVERT &x, const EXPR1 &b)
{
    return Expr<T,R,ECNST,DivOp,EXPR1>(pretend(b,x), b);
}

template<typename T, int R, class A, class B> 
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

// Test

int main() 
{
    Vec<double,3> va(1,1,1);
    Vec<int,3> vb(1,1,2);
    Vec<double,3> vc(2*(va + va - va) + convert<double>(vb + vb)/2);
    return vc.a_[2];
}
