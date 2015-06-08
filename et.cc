const int N=10;
template<typename A, typename O, typename B> struct Expr;
class PlusOp;
struct X {
    int element[N];
    template<typename A, typename O, typename B>    
    X& operator=(const Expr<A,O,B> e);
};
Expr<X,PlusOp,X> operator+(const X& a, const X& b);
template<typename A, typename O, typename B>
Expr<Expr<A,O,B>,PlusOp,X> operator+(const Expr<A,O,B>& a, const X& b);
template<typename A, typename O, typename B>
Expr<X,PlusOp,Expr<A,O,B> > operator+(const X& a, const Expr<A,O,B>& b);
template<typename A1, typename O1, typename B1, typename A2, typename O2, typename B2>
Expr<Expr<A1,O1,B1>,PlusOp,Expr<A2,O2,B2> > operator+(const Expr<A1,O1,B1>& a, const Expr<A2,O2,B2>& b);
template<typename A, typename B>
struct Expr<A,PlusOp,B>
{};
int main()
{
    X a,b,c,d;
    for (int i=0;i<N;i++) {
        a.element[i] = 1.1;
        b.element[i] = 2.2;
        c.element[i] = 3.3;
        d.element[i] = 4.4;
    }
    a = b + c + d;
    return int(a.element[N-1]);
}
