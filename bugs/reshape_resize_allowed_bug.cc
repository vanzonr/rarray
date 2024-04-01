#include <rarray>

int main()
{
    const int oldsize = 2, newsize = 1;
    rvector<double> a(oldsize);
    a.reshape(newsize, ra::RESIZE::ALLOWED);
    return (a.end() - a.begin() != a.size())
        || (a.end() - a.begin() != newsize)
        || (a.size() != newsize);
}
