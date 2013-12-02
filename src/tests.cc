#include "rarray.h"
#include <iostream>
#include <array>
#include <cassert>

using std::cerr;
char FP[2][5] = {"FAIL","PASS"};

#define ALLCLEAR 0
#define CHECK(x) {if(!(x)){std::cerr<<__LINE__<<'\n';return 1;}}
#define PASSORRETURN(x) {int e=x;cerr<<#x<<": "<<FP[e==0]<<'\n';if(e)return e;}

template<typename T,int R> 
const T* getconstdata(const rarray<T,R>& a)
{
    return a.data();
}

template<typename T> 
int testconstructors() 
{
    // Exercises the following constructors of the rarray<T,3> class
    //   rarray(int,int,int)
    //   rarray(const int*)
    //   rarray(const rarray<T,3>&)
    // as well as the destructor
    //   ~rarray()
    // And the following methods
    //   T* data()
    //   int extent(int)
    int dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    CHECK(a.data());
    CHECK(a.extent(0) == dim[0]);
    CHECK(a.extent(1) == dim[1]);
    CHECK(a.extent(2) == dim[2]);
    CHECK(b.data());
    CHECK(b.extent(0) == dim[0]);
    CHECK(b.extent(1) == dim[1]);
    CHECK(b.extent(2) == dim[2]);
    CHECK(c.data());
    CHECK(c.extent(0) == dim[0]);
    CHECK(c.extent(1) == dim[1]);
    CHECK(c.extent(2) == dim[2]);
    CHECK(c.data()==b.data());
    return ALLCLEAR;
}

template<typename T> 
int testaccessors(T value1, T value2) 
{
    // Exercises the following methods of the rarray<T,{1,2,3}> class
    //   rarray_intermediate operator[](int i);
    // Uses previously checked
    //   rarray(int,int,int)
    //   rarray(const int*)
    //   rarray(const rarray<T,3>&)
    //   ~rarray()
    //   T* data()
    //   int extent(int)
    assert(value1!=value2);  // required for the test to work
    int dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    for (int i=0; i<a.extent(0); i++) {
        for (int j=0; j<a.extent(1); j++) {
            for (int k=0; k<a.extent(2); k++) {
                a[i][j][k] = value1;
                b[i][j][k] = value2;
            }
        }
    }
    for (int i=0; i<a.extent(0); i++)
        for (int j=0; j<a.extent(1); j++)
            for (int k=0; k<a.extent(2); k++)
                CHECK(a[i][j][k] == value1);
    
    for (int i=0; i<b.extent(0); i++) {
        for (int j=0; j<b.extent(1); j++) {
            for (int k=0; k<b.extent(2); k++) {
                CHECK(b[i][j][k] == value2);   
            }
        }
    }
    b[6][1][0] = value1;
    CHECK(c[6][1][0] == value1)
    return ALLCLEAR;
}

template<typename T> 
int testsliceconstructor() 
{
    // Exercises the following methods of the rarray<T,3> class
    //   rarray_intermediate operator[](int i);
    //   const T* data() const;
    // Exercises the following constructor of the rarray<T,2> class
    //   rarray(const rarray_intermediate&)
    // Uses previously checked
    //   rarray(int,int,int)
    //   T* data();
    rarray<T,3> a(7,21,13);
    const T* tan = getconstdata(rarray<T,2>(a[1]));
    T* tac = a[1].data();
    CHECK(tan==tac);
    return ALLCLEAR;
}

template<typename T> 
int testcopy(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,3> copy() const;
    rarray<T,3> b(100,40,3);

    T value3 = value1;
    for (int i=0; i<b.extent(0); i++) {
        for (int j=0; j<b.extent(1); j++) {
            for (int k=0; k<b.extent(2); k++) {
                b[i][j][k] = value3;
                value3 = value3+value2;
            }
        }
    }

    rarray<T,3> d(b.copy());

    CHECK(d.data()!=b.data());
    CHECK(d.extent(0)==b.extent(0));
    CHECK(d.extent(1)==b.extent(1));
    CHECK(d.extent(2)==b.extent(2));
    for (int i=0; i<b.extent(0); i++) {
        for (int j=0; j<b.extent(1); j++) {
            for (int k=0; k<b.extent(2); k++) {
                CHECK(b[i][j][k]==d[i][j][k]);
            }
        }
    }

    return ALLCLEAR;
}

class compound 
{
  public:
    compound(): x(0), y(0) {}
    compound(int x, int y): x(x), y(y) {}
    int GetX() { return x; }
    int GetY() { return y; }
    bool operator==(const compound&other) const {
        return x==other.x and y==other.y;
    }
    bool operator!=(const compound&other) const {
        return x!=other.x or y!=other.y;
    }
    compound operator+(const compound& other) const {
        return compound(x+other.x,y+other.y);
    }
  private:
    int x;
    int y;
};

std::array<compound,3> operator+(const std::array<compound,3> &a,
                                 const std::array<compound,3> &b)
{
    std::array<compound,3> result = {a[0]+b[0],a[1]+b[1],a[2]+b[2]};
    return result;
}

int main() 
{
    double d1 = -2.2, d2 = 7.1;
    compound c1(1,2), c2(-7,13);
    std::array<compound,3> a1 = {compound(1,2),compound(3,4),compound(5,6)};
    std::array<compound,3> a2 = {compound(-1,-2),compound(3,-4),compound(5,-6)};

    PASSORRETURN(testconstructors<double>());
    PASSORRETURN(testconstructors<compound>());
    PASSORRETURN((testconstructors<std::array<compound,3> >()));

    PASSORRETURN(testaccessors<double>(d1,d2));
    PASSORRETURN(testaccessors<compound>(c1,c2));
    PASSORRETURN((testaccessors<std::array<compound,3> >(a1,a2)));

    PASSORRETURN(testsliceconstructor<double>());
    PASSORRETURN(testsliceconstructor<compound>());
    PASSORRETURN((testsliceconstructor<std::array<compound,3> >()));

    PASSORRETURN(testcopy<double>(d1,d2));
    PASSORRETURN(testcopy<compound>(c1,c2));
    PASSORRETURN((testcopy<std::array<compound,3> >(a1,a2)));

    return ALLCLEAR;
}
