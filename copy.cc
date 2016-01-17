#include <rarray>
#include <rarrayio>

int main() 
{
    rarray<double,2> a(2,2), b;

    a = 7, 6, 5, 4;

    std::cout << "a=" << a << std::endl;

    b = a ;
    std::cout << "b=" << b << std::endl;

    rarray<double,2> c(3,3);
    c.fill(1.0);

    a.reshape_force(1,2);

    std::cout << "a=" << a << std::endl;

    if (b.is_valid()) {
        std::cout << "b=" << b << std::endl;
    } else {
        std::cout << "b no longer valid" << std::endl;
    }
}
