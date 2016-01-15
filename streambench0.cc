#include "rarray.h"
#include "rarrayex0.h"
#include "rarrayio.h"

int main()
{
    const int        n = 2;
    double           s = 21;
    double           adata[n];
    double           bdata[n] = {2.0,2.0};
    double           cdata[n] = {4.0, 10.0};
    rarray<double,1> a(adata, n);
    rarray<double,1> b(bdata, n);
    rarray<double,1> c(cdata, n);
    std::cerr << "b=" << b << std::endl;
    std::cerr << "c=" << c << std::endl;
    std::cerr << "s=" << s << std::endl;
    // triad
    std::cerr << "a <- b + s*c" << std::endl;
    double s2data[n] = {s,s};
    rarray<double,1> s2 = RARRAY(s2data);
    a = b + Expr<double, 1, RepOp, void, void, void>(c,21.0)*c;
    a = b + repeatlike(c,21.0)*c;
    std::cerr << a << std::endl;
    a = b + 21.0*c;
    std::cerr << a << std::endl;
}
