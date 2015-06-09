#include "rarray.h"
#include "rarrayex.h"
#include "rarrayio.h"

int main()
{
    const int        n = 2;
    rarray<double,1> a(n), b(n), c(n), d(n);
    double           s = 1.5;

    a.fill(1.0);
    b.fill(2.0);
    c.fill(4.0);
    c[1] = 10;
    std::cerr << "a=" << a << std::endl;
    std::cerr << "b=" << b << std::endl;
    std::cerr << "c=" << c << std::endl;
    std::cerr << "s=" << s << std::endl;

    // copy
    std::cerr << "a <- b" << std::endl;
    a = b.copy();
    std::cerr << "a=" << a << std::endl;

    // scale
    std::cerr << "a <- s*c" << std::endl;
    a = s*c;
    std::cerr << "a=" << a << std::endl;

    // sum
    std::cerr << "s <- sum(a[i])" << std::endl;
    s = sum(a);
    std::cerr << "s=" << s << std::endl;

    // triad
    std::cerr << "a <- b + s*c" << std::endl;
    a = b + s*c;
    std::cerr << a << std::endl;

    return 0;
}
