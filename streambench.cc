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

    d = a + b + c;

    std::cerr << "d <- a+b+c" << std::endl;

    std::cerr << d << std::endl;

    return 0;
}
