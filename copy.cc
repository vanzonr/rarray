#include <rarray>
#include <rarrayio>

#define BOOST_CHECK(x) if (!(x)) { std::cout << "Check " << #x << " failed." << std::endl; }

void testcopy(double value1, double value2) 
{
    typedef double T;
    rarray<T,1> b(100);
    const int n=b.extent(0);
    T value3 = value1;
    for (int i=0; i<n; i++) {
        b[i] = value3;
        value3 = value3+value2;
    }
    rarray<T,1> d(b.copy());
    BOOST_CHECK(d.data()!=b.data());
    BOOST_CHECK(d.extent(0)==b.extent(0));
    for (int i=0; i<n; i++) {
        BOOST_CHECK(b[i]==d[i]);
    }
}

int main() 
{
    // rarray<double,2> a(2,2), b , d;

    // a = 7, 6, 5, 4;

    // d = a;
    
    // std::cout << "a=" << a << std::endl;

    // b = a;
    // std::cout << "b=" << b << std::endl;

    // rarray<double,2> c(3,3);
    // c.fill(1.0);

    // a.reshape(1,2);

    // std::cout << "a=" << a << std::endl;

    // std::cout << "b=" << b << std::endl;

    testcopy(7.0,13.2);
}
