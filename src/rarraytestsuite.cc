//
// tests.cc
//
// testsuite for rarray.h
//
#include "rarray.h"
#include <iostream>
#include <array>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <string>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

using std::string;
using std::cerr;
char FP[2][5] = {"FAIL","PASS"};
string strip(const char* s) 
{
    int len=strlen(s);
    while (s[0]=='(' and s[len-1]==')') {
        ++s;
        len -= 2;
    }
    return string(s,len);
}
#define ALLCLEAR 0
#define CHECK(x) {if(!(x)){cerr<<__LINE__<<'\n';return 1;}}
#define PASSORRETURN(x) {int e=x;cerr<<strip(#x)<<": "<<FP[e==0]<<'\n';if(e)return e;}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T,int R> 
const T* getconstdata(const rarray<T,R>& a)
{
    return a.data();
}

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
int testconstructors7dim() 
{
    int dim[7] = {7,10,13,2,4,5,21};
    rarray<T,1> z1(7);
    rarray<T,1> a1;
    a1 = z1;
    rarray<T,1> b1(dim);
    rarray<T,1> c1(b1);
    rarray<T,2> z2(7,10);
    rarray<T,2> a2;
    a2 = z2;
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


    a1.free(); //optional here, as a1 will go out of scope
    b7.free();

    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
int testconstructors7dimbuf()
{    
    int dim[7] = {7,10,13,2,4,5,21};
    T* buf = new T[7*10*13*2*4*5*21];
    {
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
    }
    delete[] buf;
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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
    rarray<T,2> b(a[2]);
    rarray<T,1> c(b[2]);
    const rarray<T,2> d(a[2]);
    const rarray<T,1> e(b[2]);
    const T* tan = getconstdata(rarray<T,2>(a[1]));
    T* tac = a[1].data();
#endif
    CHECK(tan==tac);
#ifndef SKIPINTERMEDIATE
    CHECK(a[1].extent(0)==21);
    CHECK(a[1].extent(1)==13);
    CHECK(a[1].extents()[1]==13);
    CHECK(a[1][6].extent(0)==13);
    CHECK(a[1][6].extents()[0]==13);
    CHECK(a[1].size()==21*13);
    CHECK(a[1][6].size()==13);
    T* p1 = a[3][2].data();
    T* p2 = a[3].data();
#endif
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void print1d_1(float* a, int n, std::ostream &out) 
{
    for (int i=0;i<n;i++) 
        out << a[i] << ' ';
    out << std::endl;
}

//////////////////////////////////////////////////////////////////////

void print1d_2(const float* a, int n, std::ostream &out) 
{
    for (int i=0;i<n;i++) 
        out << a[i] << ' ';
    out << std::endl;
}

//////////////////////////////////////////////////////////////////////

void print1d_3(const rarray<float,1> &a, std::ostream &out) 
{
    for (int i=0;i<a.extent(0);i++) 
        out << a[i] << ' ';
    out << std::endl;
}

//////////////////////////////////////////////////////////////////////

void print1d_4(const rarray<const float,1>& a, std::ostream &out)
{
    for (int i=0;i<a.extent(0);i++) 
        out << a[i] << ' ';
    out << std::endl;
}

//////////////////////////////////////////////////////////////////////

int test1dconversions()
{
    const int n=9;
    rarray<float,1> a(n);
    for (int i=0;i<n;i++)
        a[i] = i+1;
    const rarray<float,1>& c=a;
    std::stringstream s1,s2,s3,s4,s5,s6,s7;
    //print1d_1(c.ptr(), c.extent(0), std::cout);
    print1d_1(c.ptr(), c.extent(0), s1);
    CHECK(s1.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.cptr(), c.extent(0), s2);
    CHECK(s2.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_1(a.data(), c.extent(0), s3);
    CHECK(s3.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.data(), c.extent(0), s4);
    CHECK(s4.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_3(c, s5);
    CHECK(s5.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(a.cref(), s6);
    CHECK(s6.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(c.cref(), s7);
    CHECK(s7.str()=="1 2 3 4 5 6 7 8 9 \n");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// print2d_1 takes a double-pointer matrix, whose elements and row
// pointers could be changed. Dangerous.
// - Not const-correct, but common in non-const (often C) libraries.  
// - Will require a call to ptr_no_const
void print2d_1(float**a, int n, int m, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_2 takes a matrix with const elements, but whose row
// pointers could in principle be changed. Dangerous, but not
// uncommon!
// - Not const-correct.
// - Requires a ptr_no_mid_const of a shapeal 2d array
void print2d_2(const float**a, int n, int m, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_3 takes a matrix, which is a pointer to a set of pointers. The
// row pointers are constant, but the elements would be changable.
// - Not (logically) const-correct.
// - A non-const shapeal 2d array can be passed right in.
void print2d_3(float *const* a, int n, int m, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_4 takes a constant matrix, as a set of pointers to rows. Both
// the row pointers and the elements are const, and can't be changed.
// - Const-correct.
// - A const shapeal 2d array can be passed right in.
void print2d_4(const float*const*a, int n, int m, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_5 wants the matrix as a contiguous memory block.
// because of const, print2d_5 cannot change the elements of a. 
// Dangerous, and very common.
// - Not (logically) const-correct
// - Requires a const-cast.
void print2d_5(float *a, int n, int m, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) 
            cout << a[i*m+j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_6 wants the constant matrix as a contiguous memory block.
// because of const, print2d_6 cannot change the elements of a. 
// - Const-correct
// - A const shapeal 2d array can be passed right in.
void print2d_6(const float *a, int n, int m, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) 
            cout << a[i*m+j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_7 takes the wrapper 2d class, which already contains its dimenstions
// because of const, print2d_7 cannot change the elements of a.
// - Const-correct.
// - A non-const shapeal 2d array can, of course, be passed right in.
void print2d_7(const rarray<float,2> &a, std::ostream& cout)
{
    for (int i=0;i<a.extent(0);i++) {
        for (int j=0;j<a.extent(1);j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_8 takes the wrapper 2d class, which already contains its dimenstions
// because of const, print2d_7 cannot change the elements of a.
// - Const-correct.
// - A non-const shapeal 2d array can, of course, be passed right in.
void print2d_8(const rarray<const float,2> &a, std::ostream& cout)
{
  for (int i=0;i<a.extent(0);i++) {
    for (int j=0;j<a.extent(1);j++) 
      cout << a[i][j] << ' ';
    cout << '\n';
  }
  cout << '\n';
}

//////////////////////////////////////////////////////////////////////

int test2dconversions()
{
    const int n = 9;
    const int m = 5;
    rarray<float,2> a(n,m);
    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        a[i][j]=(i+1)*10+j+1;
#ifndef SKIPINTERMEDIATE
    rarray<float,1> a1 = a[1];
    a1=a[1]; // not really testing runtime
#endif
    const rarray<float,2>& c=a; // note the const
    std::stringstream s1,s2,s3,s4,s5,s6,s7,s8;
 // print2d_1(c, a.extent(0), a.extent(1), s1); won't work, one needs:
    print2d_1(c.cptr(), c.extent(0), c.extent(1), s1);
    CHECK(s1.str()==
          "11 12 13 14 15 \n"
          "21 22 23 24 25 \n"
          "31 32 33 34 35 \n" 
          "41 42 43 44 45 \n"
          "51 52 53 54 55 \n"
          "61 62 63 64 65 \n"
          "71 72 73 74 75 \n"
          "81 82 83 84 85 \n"
          "91 92 93 94 95 \n\n");
 // print2d_2(c, c.extent(0), c.extent(1), s2); // won't work, one needs:
    print2d_2(c.cref().cptr(), c.extent(0), c.extent(1), s2);
    CHECK(s2.str()==s1.str());
    print2d_3(c.ptr(), c.extent(0), c.extent(1), s3);
    CHECK(s3.str()==s1.str());
    print2d_4(c.ptr(), c.extent(0), c.extent(1), s4);
    CHECK(s4.str()==s1.str());
    print2d_5(a.data(), c.extent(0), c.extent(1), s5);
    CHECK(s5.str()==s1.str());
    print2d_6(c.data(), c.extent(0), c.extent(1), s6);
    CHECK(s6.str()==s1.str());
    print2d_7(c, s7);
    CHECK(s7.str()==s1.str());
    print2d_8(c.cref(), s8);
    CHECK(s8.str()==s1.str());
    return ALLCLEAR;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// Six different signatures of a print function for a tensor: 

// print_1 takes a double-pointer tensor, whose elements and row
// pointers could be changed. Dangerous.
// - Not const-correct, but common in non-const (often C) libraries.  
// - Will require a call to no_const
void print3d_1(float***a, int n, int m, int l, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            for (int k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

//////////////////////////////////////////////////////////////////////

// print3d_2 takes a tensor whose elements are constant, but whose row
// pointers could in principle be changed. Dangerous, but common!
// - Not const-correct.
// - Requires a mid_const_cast of a shapeal 3d array
void print3d_2(const float***a, int n, int m, int l, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++)  {
            for (int k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

//////////////////////////////////////////////////////////////////////

// print3d_3 takes a tensor, which is a pointer to a set of pointers. The
// row pointers are constant, but the elements would be changable.
// - Not (logically) const-correct.
// - A non-const shaped 3d array can be passed right in.
void print3d_3(float *const*const* a, int n, int m, int l, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++)  {
            for (int k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

//////////////////////////////////////////////////////////////////////

// print3d_4 takes a constant tensor, as a set of pointers to rows. Both
// the row pointers and the elements are const, and can't be changed.
// - Const-correct.
// - A const shaped 3d array can be passed right in.
void print3d_4(const float*const*const*a, int n, int m, int l, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            for (int k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

//////////////////////////////////////////////////////////////////////

// print3d_5 wants the tensor as a contiguous memory block.
// because of absence of const, print3d_5 could change the elements of a. 
// Dangerous, and very common.
// - Not (logically) const-correct
// - Requires a const-cast.
void print3d_5(float *a, int n, int m, int l, std::ostream& cout)
{    
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            for (int k=0;k<l;k++) 
                cout << a[(i*m+j)*l+k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

//////////////////////////////////////////////////////////////////////

// print3d_6 wants the constant tensor as a contiguous memory block.
// because of const, print3d_6 cannot change the elements of a. 
// - Const-correct
// - A const shapeal 3d array can be passed right in.
void print3d_6(const float *a, int n, int m, int l, std::ostream& cout)
{
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            for (int k=0;k<l;k++) 
                cout << a[(i*m+j)*l+k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

//////////////////////////////////////////////////////////////////////

// print3d_7 takes the wrapper 3d class, which already contains its dimensions
// because of const, print3d_7 cannot change the elements of a.
// - Const-correct.
// - A non-const shaped 3d array can, of course, be passed right in.
void print3d_7(const rarray<float,3> &a, std::ostream& cout)
{
    for (int i=0;i<a.extent(0);i++) {
        for (int j=0;j<a.extent(1);j++) {
            for (int k=0;k<a.extent(2);k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

int test3dconversions() 
{
    const int n = 9;
    const int m = 5;
    const int l = 2;
    rarray<float,3> a(n,m,l);
    std::stringstream s1,s2,s3,s4,s5,s6,s7;

    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        for (int k=0;k<l;k++)
          a[i][j][k]=((i+1)*10+j+1)*10+k+1;
    
    const rarray<float,3>& c=a; // note the const: not enough

    const rarray<float,3>* pa = &a;
  //print3d_1(c, c.extent(0), c.extent(1), c.extent(2)); //won't work, one needs:
    print3d_1(c.cptr(), c.extent(0), c.extent(1), c.extent(2), s1); 
    CHECK(s1.str()==
     "111 112       \t121 122       \t131 132       \t141 142       \t151 152       \t\n"
     "211 212       \t221 222       \t231 232       \t241 242       \t251 252       \t\n"
     "311 312       \t321 322       \t331 332       \t341 342       \t351 352       \t\n"
     "411 412       \t421 422       \t431 432       \t441 442       \t451 452       \t\n"
     "511 512       \t521 522       \t531 532       \t541 542       \t551 552       \t\n"
     "611 612       \t621 622       \t631 632       \t641 642       \t651 652       \t\n"
     "711 712       \t721 722       \t731 732       \t741 742       \t751 752       \t\n"
     "811 812       \t821 822       \t831 832       \t841 842       \t851 852       \t\n"
     "911 912       \t921 922       \t931 932       \t941 942       \t951 952       \t\n\n");
 // print3d_2(c, c.extent(0), c.extent(1), c.extent(2)); won't work, one needs:
    print3d_2(c.cref().cptr(), c.extent(0), c.extent(1), c.extent(2), s2); 
    CHECK(s2.str()==s1.str());    
    print3d_3(c.ptr(), c.extent(0), c.extent(1), c.extent(2), s3); 
    CHECK(s3.str()==s1.str());
    print3d_4(c.ptr(), c.extent(0), c.extent(1), c.extent(2), s4);
    CHECK(s4.str()==s1.str());
 // print3d_5(c, c.extent(0), c.extent(1), c.extent(2)); won't work, one needs
    print3d_5(a.data(), c.extent(0), c.extent(1), c.extent(2), s5);
    CHECK(s5.str()==s1.str());
    print3d_6(c.data(), c.extent(0), c.extent(1), c.extent(2), s6);
    CHECK(s6.str()==s1.str());
    print3d_7(c, s7);
    CHECK(s7.str()==s1.str());
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int testassignment()
{
    const int n=10;
    const int m=100;
    const int p=3;
    int l = 0;
    rarray<float,3> a(n,m,p);
    rarray<float,3> b(0,0,0);
    for (int i=0;i<n;i++)
        for (int j=0;j<m;j++)
            for (int k=0;k<p;k++)
                a[i][j][k] = float(l++);
    b = a;
    CHECK(b.data()==a.data());
    //CHECK(b.ptr()==a.ptr()); // not yet, b has its own ptr.
    CHECK(b.extent(0)==a.extent(0));
    CHECK(b.extent(1)==a.extent(1));
    CHECK(b.extent(2)==a.extent(2));
#ifndef SKIPINTERMEDIATE
    rarray<float,2> e(0,0);
    e = a[2];
    CHECK(e.data()==a[2].data());
    CHECK(e.extent(0)==a.extent(1));
    CHECK(e.extent(1)==a.extent(2));    
#endif
    rarray<float,1> c(2048), d(0);
    d = c;
    CHECK(d.data()==c.data());
    CHECK(d.extent(0)==c.extent(0));
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef SKIPINTERMEDIATE
int testconstintermediatefunction(const rarray<float,3>& a, const float* data1check)
{
    const float* a1=a[1].data();
    CHECK(a1==data1check); 
    CHECK(a[1].ptr());
    CHECK(a[1].cptr());
    CHECK(a[1].cref().ptr());
    CHECK(a[1][2].ptr());
    CHECK(a[1][2].cptr());
    CHECK(a[1][2].cref().ptr());
    return ALLCLEAR;
}
#endif

//////////////////////////////////////////////////////////////////////

int testconstintermediate()
{
#ifdef SKIPINTERMEDIATE
    return ALLCLEAR;
#else
    rarray<float,3> a(7,8,9);
    int l=0;
    for (int i=0;i<7;i++)
        for (int j=0;j<8;j++)
            for (int k=0;k<9;k++)
                a[i][j][k] = float(l++);
    return testconstintermediatefunction(a,a[1].data());
#endif
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////

std::array<compound,3> operator+(const std::array<compound,3> &a,
                                 const std::array<compound,3> &b)
{
    std::array<compound,3> result = {a[0]+b[0],a[1]+b[1],a[2]+b[2]};
    return result;
}

//////////////////////////////////////////////////////////////////////

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

    PASSORRETURN(test1dconversions());
    PASSORRETURN(test2dconversions());
    PASSORRETURN(test3dconversions());

    PASSORRETURN(testconstintermediate()); 
    PASSORRETURN(testassignment());

    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////

