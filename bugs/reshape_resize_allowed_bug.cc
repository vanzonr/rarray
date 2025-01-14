#include <rarray>

int main()
{
    const int oldsize = 2, newsize = 1;
    rvector<double> a(oldsize);
    auto oldbegin = a.begin();
    auto oldend = a.end();
    a.reshape(newsize, ra::RESIZE::ALLOWED);
    auto newbegin = a.begin();
    auto newend = a.end();
    auto reported_size = a.size();
    auto measured_extent = a.end() - a.begin();
    auto test1 = measured_extent == reported_size;
    auto test2 = measured_extent == newsize;
    auto test3 = reported_size == newsize;
    auto test4 = newbegin == oldbegin;
    auto test5 = newend != oldend;
    if (test1)
        std::cerr << "RIGHT: measured_extent == reported_size\n";
    else
        std::cerr << "WRONG: measured_extent != reported_size\n";
    if (test2)
        std::cerr << "RIGHT: measured_extent == newsize\n";
    else
        std::cerr << "WRONG: measured_extent != newsize\n";
    if (test3) 
        std::cerr << "RIGHT: reported_size == newsize\n";
    else
        std::cerr << "WRONG: reported_size != newsize\n";
    if (test4) 
        std::cerr << "RIGHT: newbegin == oldbegin\n";
    else
        std::cerr << "WRONG: newbegin != oldbegin\n";
    if (test5) 
        std::cerr << "RIGHT: newend != oldend\n";
    else
        std::cerr << "WRONG: newend == oldend\n";
    return not (test1 and test2 and test3 and test4 and test5);
}
