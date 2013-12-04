#include "rarray.h"
#include <iostream>
#include <array>
#include <cassert>
#include <iomanip>

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
    const int* asize = a.extents();
    CHECK(a.data());
    CHECK(a.size()==7*21*13);
    CHECK(asize);
    CHECK(asize[0] == dim[0]);
    CHECK(asize[1] == dim[1]);
    CHECK(asize[2] == dim[2]);
    CHECK(a.extent(0) == dim[0]);
    CHECK(a.extent(1) == dim[1]);
    CHECK(a.extent(2) == dim[2]);
    CHECK(b.data());
    CHECK(b.size()==7*21*13);
    CHECK(b.extent(0) == dim[0]);
    CHECK(b.extent(1) == dim[1]);
    CHECK(b.extent(2) == dim[2]);
    CHECK(c.data());
    CHECK(c.size()==7*21*13);
    CHECK(c.extent(0) == dim[0]);
    CHECK(c.extent(1) == dim[1]);
    CHECK(c.extent(2) == dim[2]);
    CHECK(c.data()==b.data());
    return ALLCLEAR;
}

template<typename T> 
int testconstructors7dim() 
{
    int dim[7] = {7,10,13,2,4,5,21};
    rarray<T,1> a1(7);
    rarray<T,1> b1(dim);
    rarray<T,1> c1(b1);
    rarray<T,2> a2(7,10);
    rarray<T,2> b2(dim);
    rarray<T,2> c2(b2);
    rarray<T,3> a3(7,10,13);
    rarray<T,3> b3(dim);
    rarray<T,3> c3(b3);
    rarray<T,4> a4(7,10,13,2);
    rarray<T,4> b4(dim);
    rarray<T,4> c4(b4);
    rarray<T,5> a5(7,10,13,2,4);
    rarray<T,5> b5(dim);
    rarray<T,5> c5(b5);
    rarray<T,6> a6(7,10,13,2,4,5);
    rarray<T,6> b6(dim);
    rarray<T,6> c6(b6);
    rarray<T,7> b7(dim);
    rarray<T,7> c7(b7);

    CHECK(a1.data());
    CHECK(b1.data());
    CHECK(c1.data()==b1.data());
    CHECK(a2.data());
    CHECK(b2.data());
    CHECK(c2.data()==b2.data());
    CHECK(a3.data());
    CHECK(b3.data());
    CHECK(c3.data()==b3.data());
    CHECK(a4.data());
    CHECK(b4.data());
    CHECK(c4.data()==b4.data());
    CHECK(a5.data());
    CHECK(b5.data());
    CHECK(c5.data()==b5.data());
    CHECK(a6.data());
    CHECK(b6.data());
    CHECK(c6.data()==b6.data());
    CHECK(b7.data());
    CHECK(c7.data()==b7.data());

    CHECK(b1.size()==7);
    CHECK(b2.size()==7*10);
    CHECK(b3.size()==7*10*13);
    CHECK(b4.size()==7*10*13*2);
    CHECK(b5.size()==7*10*13*2*4);
    CHECK(b6.size()==7*10*13*2*4*5);
    CHECK(b7.size()==7*10*13*2*4*5*21);

    CHECK(a1.extent(0) == dim[0]);
    CHECK(b1.extent(0) == dim[0]);
    CHECK(c1.extent(0) == dim[0]);
    CHECK(a1.extents());
    CHECK(a1.extents()[0] == dim[0]);

    CHECK(a2.extent(0) == dim[0]);
    CHECK(a2.extent(1) == dim[1]);
    CHECK(b2.extent(0) == dim[0]);
    CHECK(b2.extent(1) == dim[1]);
    CHECK(c2.extent(0) == dim[0]);
    CHECK(c2.extent(1) == dim[1]);

    CHECK(a3.extent(0) == dim[0]);
    CHECK(a3.extent(1) == dim[1]);
    CHECK(a3.extent(2) == dim[2]);
    CHECK(b3.extent(0) == dim[0]);
    CHECK(b3.extent(1) == dim[1]);
    CHECK(b3.extent(2) == dim[2]);
    CHECK(c3.extent(0) == dim[0]);
    CHECK(c3.extent(1) == dim[1]);
    CHECK(c3.extent(2) == dim[2]);

    CHECK(a4.extent(0) == dim[0]);
    CHECK(a4.extent(1) == dim[1]);
    CHECK(a4.extent(2) == dim[2]);
    CHECK(a4.extent(3) == dim[3]);
    CHECK(b4.extent(0) == dim[0]);
    CHECK(b4.extent(1) == dim[1]);
    CHECK(b4.extent(2) == dim[2]);
    CHECK(b4.extent(3) == dim[3]);
    CHECK(c4.extent(0) == dim[0]);
    CHECK(c4.extent(1) == dim[1]);
    CHECK(c4.extent(2) == dim[2]);
    CHECK(c4.extent(3) == dim[3]);

    CHECK(a5.extent(0) == dim[0]);
    CHECK(a5.extent(1) == dim[1]);
    CHECK(a5.extent(2) == dim[2]);
    CHECK(a5.extent(3) == dim[3]);
    CHECK(a5.extent(4) == dim[4]);
    CHECK(b5.extent(0) == dim[0]);
    CHECK(b5.extent(1) == dim[1]);
    CHECK(b5.extent(2) == dim[2]);
    CHECK(b5.extent(3) == dim[3]);
    CHECK(b5.extent(4) == dim[4]);
    CHECK(c5.extent(0) == dim[0]);
    CHECK(c5.extent(1) == dim[1]);
    CHECK(c5.extent(2) == dim[2]);
    CHECK(c5.extent(3) == dim[3]);
    CHECK(c5.extent(4) == dim[4]);

    CHECK(a6.extent(0) == dim[0]);
    CHECK(a6.extent(1) == dim[1]);
    CHECK(a6.extent(2) == dim[2]);
    CHECK(a6.extent(3) == dim[3]);
    CHECK(a6.extent(4) == dim[4]);
    CHECK(a6.extent(5) == dim[5]);
    CHECK(b6.extent(0) == dim[0]);
    CHECK(b6.extent(1) == dim[1]);
    CHECK(b6.extent(2) == dim[2]);
    CHECK(b6.extent(3) == dim[3]);
    CHECK(b6.extent(4) == dim[4]);
    CHECK(b6.extent(5) == dim[5]);
    CHECK(c6.extent(0) == dim[0]);
    CHECK(c6.extent(1) == dim[1]);
    CHECK(c6.extent(2) == dim[2]);
    CHECK(c6.extent(3) == dim[3]);
    CHECK(c6.extent(4) == dim[4]);
    CHECK(c6.extent(5) == dim[5]);

    CHECK(b7.extent(0) == dim[0]);
    CHECK(b7.extent(1) == dim[1]);
    CHECK(b7.extent(2) == dim[2]);
    CHECK(b7.extent(3) == dim[3]);
    CHECK(b7.extent(4) == dim[4]);
    CHECK(b7.extent(5) == dim[5]);
    CHECK(b7.extent(6) == dim[6]);
    CHECK(c7.extent(0) == dim[0]);
    CHECK(c7.extent(1) == dim[1]);
    CHECK(c7.extent(2) == dim[2]);
    CHECK(c7.extent(3) == dim[3]);
    CHECK(c7.extent(4) == dim[4]);
    CHECK(c7.extent(5) == dim[5]);
    CHECK(c7.extent(6) == dim[6]);

    return ALLCLEAR;
}

template<typename T> 
int testconstructors7dimbuf()
{    
    int dim[7] = {7,10,13,2,4,5,21};
    T* buf = new T[7*10*13*2*4*5*21];
    rarray<T,1> a1(buf, 7);
    rarray<T,1> b1(buf, dim);
    rarray<T,2> a2(buf, 7,10);
    rarray<T,2> b2(buf, dim);
    rarray<T,3> a3(buf, 7,10,13);
    rarray<T,3> b3(buf, dim);
    rarray<T,4> a4(buf, 7,10,13,2);
    rarray<T,4> b4(buf, dim);
    rarray<T,5> a5(buf, 7,10,13,2,4);
    rarray<T,5> b5(buf, dim);
    rarray<T,6> a6(buf, 7,10,13,2,4,5);
    rarray<T,6> b6(buf, dim);
    rarray<T,7> b7(buf, dim);

    CHECK(a1.data());
    CHECK(b1.data());
    CHECK(a2.data());
    CHECK(b2.data());
    CHECK(a3.data());
    CHECK(b3.data());
    CHECK(a4.data());
    CHECK(b4.data());
    CHECK(a5.data());
    CHECK(b5.data());
    CHECK(a6.data());
    CHECK(b6.data());
    CHECK(b7.data());

    CHECK(a1.extent(0) == dim[0]);
    CHECK(b1.extent(0) == dim[0]);

    CHECK(a2.extent(0) == dim[0]);
    CHECK(a2.extent(1) == dim[1]);
    CHECK(b2.extent(0) == dim[0]);
    CHECK(b2.extent(1) == dim[1]);

    CHECK(a3.extent(0) == dim[0]);
    CHECK(a3.extent(1) == dim[1]);
    CHECK(a3.extent(2) == dim[2]);
    CHECK(b3.extent(0) == dim[0]);
    CHECK(b3.extent(1) == dim[1]);
    CHECK(b3.extent(2) == dim[2]);

    CHECK(a4.extent(0) == dim[0]);
    CHECK(a4.extent(1) == dim[1]);
    CHECK(a4.extent(2) == dim[2]);
    CHECK(a4.extent(3) == dim[3]);
    CHECK(b4.extent(0) == dim[0]);
    CHECK(b4.extent(1) == dim[1]);
    CHECK(b4.extent(2) == dim[2]);
    CHECK(b4.extent(3) == dim[3]);

    CHECK(a5.extent(0) == dim[0]);
    CHECK(a5.extent(1) == dim[1]);
    CHECK(a5.extent(2) == dim[2]);
    CHECK(a5.extent(3) == dim[3]);
    CHECK(a5.extent(4) == dim[4]);
    CHECK(b5.extent(0) == dim[0]);
    CHECK(b5.extent(1) == dim[1]);
    CHECK(b5.extent(2) == dim[2]);
    CHECK(b5.extent(3) == dim[3]);
    CHECK(b5.extent(4) == dim[4]);

    CHECK(a6.extent(0) == dim[0]);
    CHECK(a6.extent(1) == dim[1]);
    CHECK(a6.extent(2) == dim[2]);
    CHECK(a6.extent(3) == dim[3]);
    CHECK(a6.extent(4) == dim[4]);
    CHECK(a6.extent(5) == dim[5]);
    CHECK(b6.extent(0) == dim[0]);
    CHECK(b6.extent(1) == dim[1]);
    CHECK(b6.extent(2) == dim[2]);
    CHECK(b6.extent(3) == dim[3]);
    CHECK(b6.extent(4) == dim[4]);
    CHECK(b6.extent(5) == dim[5]);

    CHECK(b7.extent(0) == dim[0]);
    CHECK(b7.extent(1) == dim[1]);
    CHECK(b7.extent(2) == dim[2]);
    CHECK(b7.extent(3) == dim[3]);
    CHECK(b7.extent(4) == dim[4]);
    CHECK(b7.extent(5) == dim[5]);
    CHECK(b7.extent(6) == dim[6]);

    delete[] buf; // test has memory leak-later
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
    const int l=a.extent(0);
    const int m=a.extent(1);
    const int n=a.extent(2);
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
                a[i][j][k] = value1;
                b[i][j][k] = value2;
            }
        }
    }
    for (int i=0; i<l; i++)
        for (int j=0; j<m; j++)
            for (int k=0; k<n; k++)
                CHECK(a[i][j][k] == value1);
    
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
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
#ifdef SKIPINTERMEDIATE
    const T* tan=getconstdata(rarray<T,2>(&a[1][0][0],a.extent(1),a.extent(2)));
    T* tac = &a[1][0][0];
#else
    const T* tan = getconstdata(rarray<T,2>(a[1]));
    T* tac = a[1].data();
#endif
    CHECK(tan==tac);
    return ALLCLEAR;
}

template<typename T> 
int testcopy(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,3> copy() const;
    rarray<T,3> b(100,40,3);
    const int l=b.extent(0);
    const int m=b.extent(1);
    const int n=b.extent(2);
    T value3 = value1;
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
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
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
                CHECK(b[i][j][k]==d[i][j][k]);
            }
        }
    }

    return ALLCLEAR;
}

template<typename T> 
int testcopy1d(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,1> copy() const;
    rarray<T,1> b(100);
    const int n=b.extent(0);
    T value3 = value1;
    for (int i=0; i<n; i++) {
        b[i] = value3;
        value3 = value3+value2;
    }
    rarray<T,1> d(b.copy());
    CHECK(d.data()!=b.data());
    CHECK(d.extent(0)==b.extent(0));
    for (int i=0; i<n; i++) {
        CHECK(b[i]==d[i]);
    }
    return ALLCLEAR;
}

// matrix matrix mutiple A=B*C
template<class T>
void mmm(rarray<T,2> &A, const rarray<T,2>& B, const rarray<T,2>& C)
{
    assert(A.extent(0)==B.extent(0));
    assert(B.extent(1)==C.extent(0));
    assert(C.extent(1)==A.extent(1));
    const int l=B.extent(0);
    const int m=C.extent(1);
    const int n=B.extent(1);
    for (int i=0;i<l;i++) {
        for (int j=0;j<m;j++) {
            A[i][j] = 0;
            for (int k=0;k<n;k++) {
                A[i][j] += B[i][k]*C[k][j];
            }
        }
    }
}

template<typename T>
void print(std::ostream& o, const rarray<T,2>& m)
{
    const int r=m.extent(0);
    const int c=m.extent(1);
    for (int i=0;i<r;i++) {
        for (int j=0;j<c;j++) {
            o << std::setw(15) << m[i][j] << ' ';
        }
        o << '\n';
    }
}

template<typename T>
int testmmm() {
    T bdata[3*3] = { 1,  2,  3,
                     4,  5,  6,
                     7,  8,  9};
    T cdata[3*3] = { 1, -1,  2,
                     3, -1,  2,
                    -1,  4, -1};
    T adata[3*3] = { 4,  9,  3,
                    13, 15, 12,
                    22, 21, 21};
    rarray<T,2> b(bdata,3,3);
    rarray<T,2> c(cdata,3,3);
    rarray<T,2> a(3,3);
    mmm(a,b,c);
    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            CHECK(a[i][j]==adata[i*3+j]);
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

    PASSORRETURN(testconstructors7dim<double>());
    PASSORRETURN(testconstructors7dim<compound>());
    PASSORRETURN((testconstructors7dim<std::array<compound,3> >()));

    PASSORRETURN(testconstructors7dimbuf<double>());
    PASSORRETURN(testconstructors7dimbuf<compound>());
    PASSORRETURN((testconstructors7dimbuf<std::array<compound,3> >()));

    PASSORRETURN(testaccessors<double>(d1,d2));
    PASSORRETURN(testaccessors<compound>(c1,c2));
    PASSORRETURN((testaccessors<std::array<compound,3> >(a1,a2)));

    PASSORRETURN(testsliceconstructor<double>());
    PASSORRETURN(testsliceconstructor<compound>());
    PASSORRETURN((testsliceconstructor<std::array<compound,3> >()));

    PASSORRETURN(testcopy<double>(d1,d2));
    PASSORRETURN(testcopy<compound>(c1,c2));
    PASSORRETURN((testcopy<std::array<compound,3> >(a1,a2)));

    PASSORRETURN(testcopy1d<double>(d1,d2));
    PASSORRETURN(testcopy1d<compound>(c1,c2));
    PASSORRETURN((testcopy1d<std::array<compound,3> >(a1,a2)));

    PASSORRETURN(testmmm<int>());
    PASSORRETURN(testmmm<double>());

    return ALLCLEAR;
}
