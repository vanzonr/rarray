//
// rarraytestsuite.cc - testsuite for rarray.h
//
// Copyright (c) 2013-2015  Ramses van Zon
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "rarray"
#include "rarrayio"
#include <iostream>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <string>
#include <sstream>

#if __cplusplus <= 199711L
#define nullptr 0 
#endif

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
#define PASSORRETURN(x) {int e=x;cerr<<strip(#x)<<": "; cerr <<FP[e==0]<<'\n';if(e)return e;}

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
    const int* asize = a.shape();
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
    b.clear();
    CHECK(b.is_clear());
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
int testconstructors_with_functions() 
{
    int dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    const int* asize = shapeof(a);
    CHECK(dataof(a));
    CHECK(countof(a)==7*21*13);
    CHECK(asize);
    CHECK(asize[0] == dim[0]);
    CHECK(asize[1] == dim[1]);
    CHECK(asize[2] == dim[2]);
    CHECK(extentof(a,0) == dim[0]);
    CHECK(extentof(a,1) == dim[1]);
    CHECK(extentof(a,2) == dim[2]);
    CHECK(shapeof(a)[0] == dim[0]);
    CHECK(shapeof(a)[1] == dim[1]);
    CHECK(shapeof(a)[2] == dim[2]);
#ifndef RA_SKIPINTERMEDIATE
    CHECK(shapeof(a[0])[0] == dim[1]);
    CHECK(shapeof(a[0][1])[0] == dim[2]);
#endif
    CHECK(dataof(b));
    CHECK(countof(b)==7*21*13);
    CHECK(extentof(b,0) == dim[0]);
    CHECK(extentof(b,1) == dim[1]);
    CHECK(extentof(b,2) == dim[2]);
    CHECK(dataof(c));
    CHECK(countof(c)==7*21*13);
    CHECK(extentof(c,0) == dim[0]);
    CHECK(extentof(c,1) == dim[1]);
    CHECK(extentof(c,2) == dim[2]);
    CHECK(dataof(c)==dataof(c));
#ifndef RA_SKIPINTERMEDIATE
    CHECK(countof(b[2])==21*13);
    CHECK(countof(b[2][10])==13);
    CHECK(extentof(c[2],0) == dim[1]);
    CHECK(extentof(c[2],1) == dim[2]);
#endif
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//test --->
template<typename T> 
int testconstructors7dimtest() 
{
    int dim[5] = {7,10,13,2,4};
    rarray<T,5> a5(7,10,13,2,4);
    rarray<T,5> b5(dim);
    rarray<T,5> c5(b5);
    return ALLCLEAR;
}

template<typename T> 
int testconstructors7dimtest2() 
{
    // minimize
    int dim[7] = {7,10,13,2,4,5,21};
    rarray<T,1> z1(7);
    rarray<T,1> a1;
    //    a1 = z1;
    rarray<T,1> b1(dim);
    rarray<T,1> c1(b1);
    rarray<T,2> z2(7,10);
    rarray<T,2> a2;
    //a2 = z2;
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

    a1.clear(); //optional here, as a1 will go out of scope
    b7.clear();

    return ALLCLEAR;
}

//<--- test

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
    CHECK(a1.shape());
    CHECK(a1.shape()[0] == dim[0]);

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

    a1.clear(); //optional here, as a1 will go out of scope
    b7.clear();

    CHECK(a1.is_clear());
    CHECK(b7.is_clear());
    CHECK(c7.is_clear() == false);
    return ALLCLEAR;
}

template<typename T> 
int testconstructors12dim() 
{
    int dim[12] = {2,3,4,3,2,3,4,3,2,3,4,3};
    rarray<T,7> z7(2,3,4,3,2,3,4);
    rarray<T,7> a7;
    a7 = z7;
    rarray<T,7> b7(dim);
    rarray<T,7> c7(b7);
    rarray<T,8> z8(2,3,4,3,2,3,4,3);
    rarray<T,8> a8;
    a8 = z8;
    rarray<T,8> b8(dim);
    rarray<T,8> c8(b8);
    rarray<T,9> a9(2,3,4,3,2,3,4,3,2);
    rarray<T,9> b9(dim);
    rarray<T,9> c9(b9);
    rarray<T,10> a10(2,3,4,3,2,3,4,3,2,3);
    rarray<T,10> b10(dim);
    rarray<T,10> c10(b10);
    rarray<T,11> a11(2,3,4,3,2,3,4,3,2,3,4);
    rarray<T,11> b11(dim);
    rarray<T,11> c11(b11);
    rarray<T,12> b12(dim);
    rarray<T,12> c12(b12);

    CHECK(a7.data());
    CHECK(b7.data());
    CHECK(c7.data()==b7.data());
    CHECK(a8.data());
    CHECK(b8.data());
    CHECK(c8.data()==b8.data());
    CHECK(a9.data());
    CHECK(b9.data());
    CHECK(c9.data()==b9.data());
    CHECK(a10.data());
    CHECK(b10.data());
    CHECK(c10.data()==b10.data());
    CHECK(a11.data());
    CHECK(b11.data());
    CHECK(c11.data()==b11.data());
    CHECK(b12.data());
    CHECK(c12.data()==b12.data());

    CHECK( b7.size()==2*3*4*3*2*3*4);
    CHECK( b8.size()==2*3*4*3*2*3*4*3);
    CHECK( b9.size()==2*3*4*3*2*3*4*3*2);
    CHECK(b10.size()==2*3*4*3*2*3*4*3*2*3);
    CHECK(b11.size()==2*3*4*3*2*3*4*3*2*3*4);
    CHECK(b12.size()==2*3*4*3*2*3*4*3*2*3*4*3);

    CHECK(a7.extent(0) == dim[0]);
    CHECK(a7.extent(1) == dim[1]);
    CHECK(a7.extent(2) == dim[2]);
    CHECK(a7.extent(3) == dim[3]);
    CHECK(a7.extent(4) == dim[4]);
    CHECK(a7.extent(5) == dim[5]);
    CHECK(a7.extent(6) == dim[6]);
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

    CHECK(a8.extent(0) == dim[0]);
    CHECK(a8.extent(1) == dim[1]);
    CHECK(a8.extent(2) == dim[2]);
    CHECK(a8.extent(3) == dim[3]);
    CHECK(a8.extent(4) == dim[4]);
    CHECK(a8.extent(5) == dim[5]);
    CHECK(a8.extent(6) == dim[6]);
    CHECK(a8.extent(7) == dim[7]);
    CHECK(b8.extent(0) == dim[0]);
    CHECK(b8.extent(1) == dim[1]);
    CHECK(b8.extent(2) == dim[2]);
    CHECK(b8.extent(3) == dim[3]);
    CHECK(b8.extent(4) == dim[4]);
    CHECK(b8.extent(5) == dim[5]);
    CHECK(b8.extent(6) == dim[6]);
    CHECK(b8.extent(7) == dim[7]);
    CHECK(c8.extent(0) == dim[0]);
    CHECK(c8.extent(1) == dim[1]);
    CHECK(c8.extent(2) == dim[2]);
    CHECK(c8.extent(3) == dim[3]);
    CHECK(c8.extent(4) == dim[4]);
    CHECK(c8.extent(5) == dim[5]);
    CHECK(c8.extent(6) == dim[6]);
    CHECK(c8.extent(7) == dim[7]);

    CHECK(a9.extent(0) == dim[0]);
    CHECK(a9.extent(1) == dim[1]);
    CHECK(a9.extent(2) == dim[2]);
    CHECK(a9.extent(3) == dim[3]);
    CHECK(a9.extent(4) == dim[4]);
    CHECK(a9.extent(5) == dim[5]);
    CHECK(a9.extent(6) == dim[6]);
    CHECK(a9.extent(7) == dim[7]);
    CHECK(a9.extent(8) == dim[8]);
    CHECK(b9.extent(0) == dim[0]);
    CHECK(b9.extent(1) == dim[1]);
    CHECK(b9.extent(2) == dim[2]);
    CHECK(b9.extent(3) == dim[3]);
    CHECK(b9.extent(4) == dim[4]);
    CHECK(b9.extent(5) == dim[5]);
    CHECK(b9.extent(6) == dim[6]);
    CHECK(b9.extent(7) == dim[7]);
    CHECK(b9.extent(8) == dim[8]);
    CHECK(c9.extent(0) == dim[0]);
    CHECK(c9.extent(1) == dim[1]);
    CHECK(c9.extent(2) == dim[2]);
    CHECK(c9.extent(3) == dim[3]);
    CHECK(c9.extent(4) == dim[4]);
    CHECK(c9.extent(5) == dim[5]);
    CHECK(c9.extent(6) == dim[6]);
    CHECK(c9.extent(7) == dim[7]);
    CHECK(c9.extent(8) == dim[8]);

    CHECK(a10.extent(0) == dim[0]);
    CHECK(a10.extent(1) == dim[1]);
    CHECK(a10.extent(2) == dim[2]);
    CHECK(a10.extent(3) == dim[3]);
    CHECK(a10.extent(4) == dim[4]);
    CHECK(a10.extent(5) == dim[5]);
    CHECK(a10.extent(6) == dim[6]);
    CHECK(a10.extent(7) == dim[7]);
    CHECK(a10.extent(8) == dim[8]);
    CHECK(a10.extent(9) == dim[9]);
    CHECK(b10.extent(0) == dim[0]);
    CHECK(b10.extent(1) == dim[1]);
    CHECK(b10.extent(2) == dim[2]);
    CHECK(b10.extent(3) == dim[3]);
    CHECK(b10.extent(4) == dim[4]);
    CHECK(b10.extent(5) == dim[5]);
    CHECK(b10.extent(6) == dim[6]);
    CHECK(b10.extent(7) == dim[7]);
    CHECK(b10.extent(8) == dim[8]);
    CHECK(b10.extent(9) == dim[9]);
    CHECK(c10.extent(0) == dim[0]);
    CHECK(c10.extent(1) == dim[1]);
    CHECK(c10.extent(2) == dim[2]);
    CHECK(c10.extent(3) == dim[3]);
    CHECK(c10.extent(4) == dim[4]);
    CHECK(c10.extent(5) == dim[5]);
    CHECK(c10.extent(6) == dim[6]);
    CHECK(c10.extent(7) == dim[7]);
    CHECK(c10.extent(8) == dim[8]);
    CHECK(c10.extent(9) == dim[9]);

    CHECK(a11.extent( 0) == dim[ 0]);
    CHECK(a11.extent( 1) == dim[ 1]);
    CHECK(a11.extent( 2) == dim[ 2]);
    CHECK(a11.extent( 3) == dim[ 3]);
    CHECK(a11.extent( 4) == dim[ 4]);
    CHECK(a11.extent( 5) == dim[ 5]);
    CHECK(a11.extent( 6) == dim[ 6]);
    CHECK(a11.extent( 7) == dim[ 7]);
    CHECK(a11.extent( 8) == dim[ 8]);
    CHECK(a11.extent( 9) == dim[ 9]);
    CHECK(a11.extent(10) == dim[10]);
    CHECK(b11.extent( 0) == dim[ 0]);
    CHECK(b11.extent( 1) == dim[ 1]);
    CHECK(b11.extent( 2) == dim[ 2]);
    CHECK(b11.extent( 3) == dim[ 3]);
    CHECK(b11.extent( 4) == dim[ 4]);
    CHECK(b11.extent( 5) == dim[ 5]);
    CHECK(b11.extent( 6) == dim[ 6]);
    CHECK(b11.extent( 7) == dim[ 7]);
    CHECK(b11.extent( 8) == dim[ 8]);
    CHECK(b11.extent( 9) == dim[ 9]);
    CHECK(b11.extent(10) == dim[10]);
    CHECK(c11.extent( 0) == dim[ 0]);
    CHECK(c11.extent( 1) == dim[ 1]);
    CHECK(c11.extent( 2) == dim[ 2]);
    CHECK(c11.extent( 3) == dim[ 3]);
    CHECK(c11.extent( 4) == dim[ 4]);
    CHECK(c11.extent( 5) == dim[ 5]);
    CHECK(c11.extent( 6) == dim[ 6]);
    CHECK(c11.extent( 7) == dim[ 7]);
    CHECK(c11.extent( 8) == dim[ 8]);
    CHECK(c11.extent( 9) == dim[ 9]);
    CHECK(c11.extent(10) == dim[10]);

    CHECK(b12.extent( 0) == dim[ 0]);
    CHECK(b12.extent( 1) == dim[ 1]);
    CHECK(b12.extent( 2) == dim[ 2]);
    CHECK(b12.extent( 3) == dim[ 3]);
    CHECK(b12.extent( 4) == dim[ 4]);
    CHECK(b12.extent( 5) == dim[ 5]);
    CHECK(b12.extent( 6) == dim[ 6]);
    CHECK(b12.extent( 7) == dim[ 7]);
    CHECK(b12.extent( 8) == dim[ 8]);
    CHECK(b12.extent( 9) == dim[ 9]);
    CHECK(b12.extent(10) == dim[10]);
    CHECK(b12.extent(11) == dim[11]);
    CHECK(c12.extent( 0) == dim[ 0]);
    CHECK(c12.extent( 1) == dim[ 1]);
    CHECK(c12.extent( 2) == dim[ 2]);
    CHECK(c12.extent( 3) == dim[ 3]);
    CHECK(c12.extent( 4) == dim[ 4]);
    CHECK(c12.extent( 5) == dim[ 5]);
    CHECK(c12.extent( 6) == dim[ 6]);
    CHECK(c12.extent( 7) == dim[ 7]);
    CHECK(c12.extent( 8) == dim[ 8]);
    CHECK(c12.extent( 9) == dim[ 9]);
    CHECK(c12.extent(10) == dim[10]);
    CHECK(c12.extent(11) == dim[11]);

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

template<typename T> 
int testconstructors12dimbuf()
{    
    int dim[12] = {2,3,4,3,2,3,4,3,2,3,4,3};
    T* buf = new T[2*3*4*3*2*3*4*3*2*3*4*3];
    {
        rarray<T,7>  a7 (buf, 2,3,4,3,2,3,4);
        rarray<T,7>  b7 (buf, dim);
        rarray<T,8>  a8 (buf, 2,3,4,3,2,3,4,3);
        rarray<T,8>  b8 (buf, dim);
        rarray<T,9>  a9 (buf, 2,3,4,3,2,3,4,3,2);
        rarray<T,9>  b9 (buf, dim);
        rarray<T,10> a10(buf, 2,3,4,3,2,3,4,3,2,3);
        rarray<T,10> b10(buf, dim);
        rarray<T,11> a11(buf, 2,3,4,3,2,3,4,3,2,3,4);
        rarray<T,11> b11(buf, dim);
        rarray<T,12> b12(buf, dim);
        
        CHECK(a7.data());
        CHECK(b7.data());
        CHECK(a8.data());
        CHECK(b8.data());
        CHECK(a9.data());
        CHECK(b9.data());
        CHECK(a10.data());
        CHECK(b10.data());
        CHECK(a11.data());
        CHECK(b11.data());
        CHECK(b12.data());
        
        CHECK(a7.extent(0) == dim[0]);
        CHECK(a7.extent(1) == dim[1]);
        CHECK(a7.extent(2) == dim[2]);
        CHECK(a7.extent(3) == dim[3]);
        CHECK(a7.extent(4) == dim[4]);
        CHECK(a7.extent(5) == dim[5]);
        CHECK(a7.extent(6) == dim[6]);
        CHECK(b7.extent(0) == dim[0]);
        CHECK(b7.extent(1) == dim[1]);
        CHECK(b7.extent(2) == dim[2]);
        CHECK(b7.extent(3) == dim[3]);
        CHECK(b7.extent(4) == dim[4]);
        CHECK(b7.extent(5) == dim[5]);
        CHECK(b7.extent(6) == dim[6]);

        CHECK(a8.extent(0) == dim[0]);
        CHECK(a8.extent(1) == dim[1]);
        CHECK(a8.extent(2) == dim[2]);
        CHECK(a8.extent(3) == dim[3]);
        CHECK(a8.extent(4) == dim[4]);
        CHECK(a8.extent(5) == dim[5]);
        CHECK(a8.extent(6) == dim[6]);
        CHECK(a8.extent(7) == dim[7]);
        CHECK(b8.extent(0) == dim[0]);
        CHECK(b8.extent(1) == dim[1]);
        CHECK(b8.extent(2) == dim[2]);
        CHECK(b8.extent(3) == dim[3]);
        CHECK(b8.extent(4) == dim[4]);
        CHECK(b8.extent(5) == dim[5]);
        CHECK(b8.extent(6) == dim[6]);
        CHECK(b8.extent(7) == dim[7]);

        CHECK(a9.extent(0) == dim[0]);
        CHECK(a9.extent(1) == dim[1]);
        CHECK(a9.extent(2) == dim[2]);
        CHECK(a9.extent(3) == dim[3]);
        CHECK(a9.extent(4) == dim[4]);
        CHECK(a9.extent(5) == dim[5]);
        CHECK(a9.extent(6) == dim[6]);
        CHECK(a9.extent(7) == dim[7]);
        CHECK(a9.extent(8) == dim[8]);
        CHECK(b9.extent(0) == dim[0]);
        CHECK(b9.extent(1) == dim[1]);
        CHECK(b9.extent(2) == dim[2]);
        CHECK(b9.extent(3) == dim[3]);
        CHECK(b9.extent(4) == dim[4]);
        CHECK(b9.extent(5) == dim[5]);
        CHECK(b9.extent(6) == dim[6]);
        CHECK(b9.extent(7) == dim[7]);
        CHECK(b9.extent(8) == dim[8]);

        CHECK(a10.extent(0) == dim[0]);
        CHECK(a10.extent(1) == dim[1]);
        CHECK(a10.extent(2) == dim[2]);
        CHECK(a10.extent(3) == dim[3]);
        CHECK(a10.extent(4) == dim[4]);
        CHECK(a10.extent(5) == dim[5]);
        CHECK(a10.extent(6) == dim[6]);
        CHECK(a10.extent(7) == dim[7]);
        CHECK(a10.extent(8) == dim[8]);
        CHECK(a10.extent(9) == dim[9]);
        CHECK(b10.extent(0) == dim[0]);
        CHECK(b10.extent(1) == dim[1]);
        CHECK(b10.extent(2) == dim[2]);
        CHECK(b10.extent(3) == dim[3]);
        CHECK(b10.extent(4) == dim[4]);
        CHECK(b10.extent(5) == dim[5]);
        CHECK(b10.extent(6) == dim[6]);
        CHECK(b10.extent(7) == dim[7]);
        CHECK(b10.extent(8) == dim[8]);
        CHECK(b10.extent(9) == dim[9]);

        CHECK(a11.extent( 0) == dim[ 0]);
        CHECK(a11.extent( 1) == dim[ 1]);
        CHECK(a11.extent( 2) == dim[ 2]);
        CHECK(a11.extent( 3) == dim[ 3]);
        CHECK(a11.extent( 4) == dim[ 4]);
        CHECK(a11.extent( 5) == dim[ 5]);
        CHECK(a11.extent( 6) == dim[ 6]);
        CHECK(a11.extent( 7) == dim[ 7]);
        CHECK(a11.extent( 8) == dim[ 8]);
        CHECK(a11.extent( 9) == dim[ 9]);
        CHECK(a11.extent(10) == dim[10]);
        CHECK(b11.extent( 0) == dim[ 0]);
        CHECK(b11.extent( 1) == dim[ 1]);
        CHECK(b11.extent( 2) == dim[ 2]);
        CHECK(b11.extent( 3) == dim[ 3]);
        CHECK(b11.extent( 4) == dim[ 4]);
        CHECK(b11.extent( 5) == dim[ 5]);
        CHECK(b11.extent( 6) == dim[ 6]);
        CHECK(b11.extent( 7) == dim[ 7]);
        CHECK(b11.extent( 8) == dim[ 8]);
        CHECK(b11.extent( 9) == dim[ 9]);
        CHECK(b11.extent(10) == dim[10]);

        CHECK(b12.extent( 0) == dim[ 0]);
        CHECK(b12.extent( 1) == dim[ 1]);
        CHECK(b12.extent( 2) == dim[ 2]);
        CHECK(b12.extent( 3) == dim[ 3]);
        CHECK(b12.extent( 4) == dim[ 4]);
        CHECK(b12.extent( 5) == dim[ 5]);
        CHECK(b12.extent( 6) == dim[ 6]);
        CHECK(b12.extent( 7) == dim[ 7]);
        CHECK(b12.extent( 8) == dim[ 8]);
        CHECK(b12.extent( 9) == dim[ 9]);
        CHECK(b12.extent(10) == dim[10]);
        CHECK(b12.extent(11) == dim[11]);

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
#ifdef RA_SKIPINTERMEDIATE
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
#ifndef RA_SKIPINTERMEDIATE
    CHECK(a[1].extent(0)==21);
    CHECK(a[1].extent(1)==13);
    CHECK(a[1].shape()[1]==13);
    CHECK(a[1][6].extent(0)==13);
    CHECK(a[1][6].shape()[0]==13);
    CHECK(a[1].size()==21*13);
    CHECK(a[1][6].size()==13);
    T* p1 = a[3][2].data();
    T* p2 = a[3].data();
    CHECK(p1!=nullptr);
    CHECK(p2!=nullptr);
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

template<typename T> 
int testcopy_with_functions(T value1, T value2) 
{
    rarray<T,3> b(100,40,3);
    const int l = extentof(b,0);
    const int m = extentof(b,1);
    const int n = extentof(b,2);
    T value3 = value1;
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
                b[i][j][k] = value3;
                value3 = value3+value2;
            }
        }
    }

    rarray<T,3> d(copy(b));

    CHECK(dataof(d)!=dataof(b));
    CHECK(extentof(d,0)==extentof(b,0));
    CHECK(extentof(d,1)==extentof(b,1));
    CHECK(extentof(d,2)==extentof(b,2));
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
    //print1d_1(c.ptr_array(), c.extent(0), std::cout);
    print1d_1(c.ptr_array(), c.extent(0), s1);
    CHECK(s1.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.noconst_ptr_array(), c.extent(0), s2);
    CHECK(s2.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_1(a.data(), c.extent(0), s3);
    CHECK(s3.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.data(), c.extent(0), s4);
    CHECK(s4.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_3(c, s5);
    CHECK(s5.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(a.const_ref(), s6);
    CHECK(s6.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(c.const_ref(), s7);
    CHECK(s7.str()=="1 2 3 4 5 6 7 8 9 \n");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// print2d_1 takes a double-pointer matrix, whose elements and row
// pointers could be changed. Dangerous.
// - Not const-correct, but common in non-const (often C) libraries.  
// - Will require a call to ptr_array_no_const
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
// - Requires a ptr_array_no_mid_const of a shapeal 2d array
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
#ifndef RA_SKIPINTERMEDIATE
    rarray<float,1> a1 = a[1];
    a1=a[1]; // not really testing runtime
#endif
    const rarray<float,2>& c=a; // note the const
    std::stringstream s1,s2,s3,s4,s5,s6,s7,s8;
 // print2d_1(c, a.extent(0), a.extent(1), s1); won't work, one needs:
    print2d_1(c.noconst_ptr_array(), c.extent(0), c.extent(1), s1);
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
    print2d_2(c.const_ref().noconst_ptr_array(), c.extent(0), c.extent(1), s2);
    CHECK(s2.str()==s1.str());
    print2d_3(c.ptr_array(), c.extent(0), c.extent(1), s3);
    CHECK(s3.str()==s1.str());
    print2d_4(c.ptr_array(), c.extent(0), c.extent(1), s4);
    CHECK(s4.str()==s1.str());
    print2d_5(a.data(), c.extent(0), c.extent(1), s5);
    CHECK(s5.str()==s1.str());
    print2d_6(c.data(), c.extent(0), c.extent(1), s6);
    CHECK(s6.str()==s1.str());
    print2d_7(c, s7);
    CHECK(s7.str()==s1.str());
    print2d_8(c.const_ref(), s8);
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

    //  const rarray<float,3>* pa = &a;
  //print3d_1(c, c.extent(0), c.extent(1), c.extent(2)); //won't work, one needs:
    print3d_1(c.noconst_ptr_array(), c.extent(0), c.extent(1), c.extent(2), s1); 
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
    print3d_2(c.const_ref().noconst_ptr_array(), c.extent(0), c.extent(1), c.extent(2), s2); 
    CHECK(s2.str()==s1.str());    
    print3d_3(c.ptr_array(), c.extent(0), c.extent(1), c.extent(2), s3); 
    CHECK(s3.str()==s1.str());
    print3d_4(c.ptr_array(), c.extent(0), c.extent(1), c.extent(2), s4);
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

int testconversions() {
    CHECK(test1dconversions()==ALLCLEAR);
    CHECK(test2dconversions()==ALLCLEAR);
    CHECK(test3dconversions()==ALLCLEAR);
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
    rarray<float,3> b;
    for (int i=0;i<n;i++)
        for (int j=0;j<m;j++)
            for (int k=0;k<p;k++)
                a[i][j][k] = float(l++);
    b = a;
    CHECK(b.data()==a.data());
    //CHECK(b.ptr_array()==a.ptr_array()); // not yet, b has its own ptr_array.
    CHECK(b.extent(0)==a.extent(0));
    CHECK(b.extent(1)==a.extent(1));
    CHECK(b.extent(2)==a.extent(2));
#ifndef RA_SKIPINTERMEDIATE
    rarray<float,2> e;
    e = a[2];
    CHECK(e.data()==a[2].data());
    CHECK(e.extent(0)==a.extent(1));
    CHECK(e.extent(1)==a.extent(2));    
#endif
    rarray<float,1> c(2048);
    rarray<float,1> d;
    d = c;
    CHECK(d.data()==c.data());
    CHECK(d.extent(0)==c.extent(0));
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef RA_SKIPINTERMEDIATE
int testconstintermediatefunction(const rarray<float,3>& a, const float* data1check)
{
    const float* a1=a[1].data();
    CHECK(a1==data1check); 
    CHECK(a[1].ptr_array());
    CHECK(a[1].noconst_ptr_array());
    CHECK(a[1].const_ref().ptr_array());
    CHECK(a[1][2].ptr_array());
    CHECK(a[1][2].noconst_ptr_array());
    CHECK(a[1][2].const_ref().ptr_array());
    return ALLCLEAR;
}
#endif

//////////////////////////////////////////////////////////////////////

int testconstintermediate()
{
#ifdef RA_SKIPINTERMEDIATE
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

void fill_1d_rarray(rarray<float,1> a, float value)
{
    for (int i=0; i < a.size(); i++)
        a[i] = value;
}

int testintermediateconversion()
{
    rarray<float,2> a(10,10);
    a[2][7]=14;
#ifndef RA_SKIPINTERMEDIATE
    fill_1d_rarray(a[2], 13);
    CHECK(a[2][7]==13);
#endif
    return ALLCLEAR;
}

int testreshape() 
{
    int dim[7] = {7,10,13,2,4,5,21};
    int dimr[7] = {21,5,4,2,13,10,7};
    int dim12[12] = {2,3,4,3,2,3,4,3,2,3,4,3};
    int dimr12[12] = {4,3,2,3,4,3,2,3,2,3,4,3};//first 7 in reversed order
    rarray<float,1> a(dim), a2(a.data(),a.shape());
    rarray<float,2> b(dim), b2(b);
    rarray<float,3> c(dim), c2(c);
    rarray<float,4> d(dim);
    rarray<float,5> e(dim);
    rarray<float,6> f(dim);
    rarray<float,7> g(dim);
    rarray<float,8> h(dim12);
    rarray<float,9> i(dim12);
    rarray<float,10> j(dim12);
    rarray<float,11> k(dim12);
    rarray<float,12> l(dim12);

    rarray<float,1> novela(a);
    rarray<float,1> novela2(a2);
    novela[3] = 4;
    novela.reshape(4);
    a.reshape(dim);
    a.reshape(*dim);
    CHECK(novela.extent(0)==4);
    CHECK(novela[3]==4);
    CHECK(novela2.extent(0)==7);
    novela2.reshape(4);
    CHECK(novela2.extent(0)==4);
    CHECK(novela2[3]==4);
    b[5][6] = 5;
    rarray<float,2> novelb(b);
    rarray<float,2> novelb2(novelb);
    novelb.reshape(10,7);
    b.reshape(dim);
    CHECK(novelb.extent(0)==10);
    CHECK(novelb.extent(1)==7);
    CHECK(novelb2.extent(0)==7);
    CHECK(novelb2.extent(1)==10);
    CHECK(novelb[8][0] == 5);
    c[4][8][3] = 6;
    rarray<float,3> novelc(c);
    novelc.reshape(10,7,13);
    CHECK(novelc.extent(0)==10);
    CHECK(novelc.extent(1)==7);
    CHECK(novelc.extent(2)==13);
    CHECK(novelc[6][6][3] == 6);
    rarray<float,4> noveld(d);
    rarray<float,5> novele(e);
    rarray<float,6> novelf(f);
    rarray<float,7> novelg(g);
    rarray<float,7> novelg2(g);
    rarray<float,8> novelh(h);
    rarray<float,9> noveli(i);
    rarray<float,10> novelj(j);
    rarray<float,11> novelk(k);
    rarray<float,12> novell(l);
    noveld.reshape(2,2,2,2);                // TODO: check
    novele.reshape(13,7,10,2,4);            // TODO: check
    novelf.reshape(5,6,1,13,10,7);          // TODO: check
    novelg.reshape(dimr);                   // TODO: check
    novelg.reshape(2,5,6,1,13,10,7);  // TODO: check
    novelh.reshape(4,3,2,3,4,3,2,3);        // TODO: check
    noveli.reshape(4,3,2,3,4,3,2,3,2);      // TODO: check
    novelj.reshape(4,3,2,3,4,3,2,3,2,3);    // TODO: check
    novelk.reshape(4,3,2,3,4,3,2,3,2,3,4);  // TODO: check
    novell.reshape(dimr12);                 // TODO: check
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print1d(const rarray<float,1> &a) 
{
    std::stringstream s;
    const int n = EXTENT(a,0);
    for (int i=0;i<n-1;i++)
        s << a[i] << ' ';
    s << a[n-1];
    return s.str();
}

int test1dautoconversion() 
{
    const int n=9;
    float b[n] = {1,2,3,4,5,6,7,8,9};
    const rarray<float,1> a = RARRAY(b);
    CHECK(EXTENT(a,0)==EXTENT(b,0));
    std::string s = print1d(RARRAY(b));
    CHECK(s=="1 2 3 4 5 6 7 8 9");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print2d(const rarray<float,2> &a) 
{
    std::stringstream s;
    const int n = EXTENT(a,0);
    const int m = EXTENT(a,1);
    for (int i=0;i<n;i++) {
        for (int j=0;j<m-1;j++)
            s << a[i][j] << ' ';
        s << a[i][m-1];
        s << '\n';
    }
    return s.str();
}

int test2dautoconversion() 
{
    const int n=2;
    const int m=7;
    float b[n][m] = {{1,2,3,4,5,6,7},
                     {8,9,8,7,6,5,4}};
    const rarray<float,2> a = RARRAY(b);
    CHECK(EXTENT(a,0)==EXTENT(b,0));
    CHECK(EXTENT(a,1)==EXTENT(b,1));
    std::string s = print2d(RARRAY(b));
    CHECK(s=="1 2 3 4 5 6 7\n8 9 8 7 6 5 4\n");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print3d(const rarray<float,3> &a) 
{
    std::stringstream s;
    const int n = EXTENT(a,0);
    const int m = EXTENT(a,1);
    const int l = EXTENT(a,2);
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            s << '{';
            for (int k=0;k<l-1;k++)
                s << a[i][j][k] << ',';
            s << a[i][j][l-1] << '}';
        }
        s << '\n';
    }
    return s.str();
}

int test3dautoconversion() 
{
    const int n=2;
    const int m=7;
    const int l=3;
    float b[n][m][l] = {{{1,2,3},{2,3,4},{3,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
                        {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,3},{4,3,2}}};
    const rarray<float,3> a = RARRAY(b);
    CHECK(EXTENT(a,0)==EXTENT(b,0));
    CHECK(EXTENT(a,1)==EXTENT(b,1));
    CHECK(EXTENT(a,2)==EXTENT(b,2));
    std::string s = print3d(RARRAY(b));
    CHECK(s=="{1,2,3}{2,3,4}{3,4,5}{4,5,6}{5,6,7}{6,7,8}{7,8,9}\n"
             "{8,7,6}{9,8,7}{8,7,6}{7,6,5}{6,5,4}{5,4,3}{4,3,2}\n");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print4d(const rarray<float,4> &a) 
{
    std::stringstream s;
    const int n = EXTENT(a,0);
    const int m = EXTENT(a,1);
    const int p = EXTENT(a,2);
    const int q = EXTENT(a,3);
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            s << '{';
            for (int k=0;k<p;k++) {
                s << '[';
                for (int l=0;l<q-1;l++) {
                    s << a[i][j][k][l] << ' ';
                }
                s << a[i][j][k][q-1] << ']';
            }
            s << '}';
        }
        s << '\n';
    }
    return s.str();
}

int test4dautoconversion() 
{
    const int p=2;
    const int n=2;
    const int m=7;
    const int l=3;
    float b[p][n][m][l] = {
        {{{1,2,3},{2,3,4},{3,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
         {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,3},{4,3,2}}}
        ,
        {{{1,2,3},{2,3,6},{3,6,5},{6,5,9},{5,9,7},{9,7,8},{7,8,9}},
         {{8,7,9},{9,8,7},{8,7,9},{7,9,5},{9,5,6},{5,6,3},{6,3,2}}}
    };
    const rarray<float,4> a = RARRAY(b);
    CHECK(EXTENT(a,0)==EXTENT(b,0));
    CHECK(EXTENT(a,1)==EXTENT(b,1));
    CHECK(EXTENT(a,2)==EXTENT(b,2));
    CHECK(EXTENT(a,3)==EXTENT(b,3));
    std::string s = print4d(RARRAY(b));
    CHECK(s==
          "{[1 2 3][2 3 4][3 4 5][4 5 6][5 6 7][6 7 8][7 8 9]}{[8 7 6][9 8 7][8 7 6][7 6 5][6 5 4][5 4 3][4 3 2]}\n"
          "{[1 2 3][2 3 6][3 6 5][6 5 9][5 9 7][9 7 8][7 8 9]}{[8 7 9][9 8 7][8 7 9][7 9 5][9 5 6][5 6 3][6 3 2]}\n");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print5d(const rarray<float,5> &a) 
{
    std::stringstream s;
    const int n = EXTENT(a,0);
    const int m = EXTENT(a,1);
    const int p = EXTENT(a,2);
    const int q = EXTENT(a,3);
    const int r = EXTENT(a,4);
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            s << '{';
            for (int k=0;k<p;k++) {
                s << '[';
                for (int l=0;l<q-1;l++) {
                    s << '(';
                    for (int m=0;m<r-1;m++) {
                        s << a[i][j][k][l][m] << ',';
                    }
                    s << a[i][j][k][l][r-1];
                    s << ')';
                }
                s << '(';
                for (int m=0;m<r-1;m++) {
                    s << a[i][j][k][q-1][m] << ',';
                }
                s << a[i][j][k][q-1][r-1] << ')';
                s << ']';
            }
            s << '}';
        }
        s << '\n';
    }
    return s.str();
}

int test5dautoconversion() 
{
    const int p=2;
    const int n=2;
    const int m=7;
    const int l=3;
    const int k=2;
    float b[k][p][n][m][l] = {
        {
            {{{1,2,3},{2,3,4},{3,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
             {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,3},{4,3,2}}}
            ,
            {{{1,2,3},{2,3,6},{3,6,5},{6,5,9},{5,9,7},{9,7,8},{7,8,9}},
             {{8,7,9},{9,8,7},{8,7,9},{7,9,5},{9,5,6},{5,6,3},{6,3,2}}}
        }
        ,
        {
            {{{1,2,7},{2,7,4},{7,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
             {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,7},{4,7,2}}}
            ,
            {{{1,2,7},{2,7,6},{7,6,5},{6,5,9},{5,9,7},{9,7,8},{7,8,9}},
             {{8,7,9},{9,8,7},{8,7,9},{7,9,5},{9,5,6},{5,6,7},{6,7,2}}}
        }
    };
    const rarray<float,5> a = RARRAY(b);
    CHECK(EXTENT(a,0)==EXTENT(b,0));
    CHECK(EXTENT(a,1)==EXTENT(b,1));
    CHECK(EXTENT(a,2)==EXTENT(b,2));
    CHECK(EXTENT(a,3)==EXTENT(b,3));
    CHECK(EXTENT(a,4)==EXTENT(b,4));
    std::string s = print5d(RARRAY(b));
    CHECK(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
          "{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n");
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print6d(const rarray<float,6> &a) 
{
    std::stringstream s;
    const int t = EXTENT(a,0);
    const int n = EXTENT(a,1);
    const int m = EXTENT(a,2);
    const int p = EXTENT(a,3);
    const int q = EXTENT(a,4);
    const int r = EXTENT(a,5);
    for (int b=0; b<t; b++) {
    for (int i=0;i<n;i++) {
        for (int j=0;j<m;j++) {
            s << '{';
            for (int k=0;k<p;k++) {
                s << '[';
                for (int l=0;l<q-1;l++) {
                    s << '(';
                    for (int m=0;m<r-1;m++) {
                        s << a[b][i][j][k][l][m] << ',';
                    }
                    s << a[b][i][j][k][l][r-1];
                    s << ')';
                }
                s << '(';
                for (int m=0;m<r-1;m++) {
                    s << a[b][i][j][k][q-1][m] << ',';
                }
                s << a[b][i][j][k][q-1][r-1] << ')';
                s << ']';
            }
            s << '}';
        }
        s << '\n';
    }
    s << '\n';
    }
    return s.str();
}

int test6dautoconversion() 
{
    const int q=2;
    const int p=2;
    const int n=2;
    const int m=7;
    const int l=3;
    const int k=2;
    float b[q][k][p][n][m][l] = {
        {
        {
            {{{1,2,3},{2,3,4},{3,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
             {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,3},{4,3,2}}}
            ,
            {{{1,2,3},{2,3,6},{3,6,5},{6,5,9},{5,9,7},{9,7,8},{7,8,9}},
             {{8,7,9},{9,8,7},{8,7,9},{7,9,5},{9,5,6},{5,6,3},{6,3,2}}}
        }
        ,
        {
            {{{1,2,7},{2,7,4},{7,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
             {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,7},{4,7,2}}}
            ,
            {{{1,2,7},{2,7,6},{7,6,5},{6,5,9},{5,9,7},{9,7,8},{7,8,9}},
             {{8,7,9},{9,8,7},{8,7,9},{7,9,5},{9,5,6},{5,6,7},{6,7,2}}}
        }
        },
        {
        {
            {{{1,-2,-3},{2,-3,-4},{3,-4,-5},{4,-5,-6},{5,-6,-7},{6,-7,-8},{7,-8,-9}},
             {{8,-7,-6},{9,-8,-7},{8,-7,-6},{7,-6,-5},{6,-5,-4},{5,-4,-3},{4,-3,-2}}}
            ,
            {{{1,-2,-3},{2,-3,-6},{3,-6,-5},{6,-5,-9},{5,-9,-7},{9,-7,-8},{7,-8,-9}},
             {{8,-7,-9},{9,-8,-7},{8,-7,-9},{7,-9,-5},{9,-5,-6},{5,-6,-3},{6,-3,-2}}}
        }
        ,
        {
            {{{1,-2,-7},{2,-7,-4},{7,-4,-5},{4,-5,-6},{5,-6,-7},{6,-7,-8},{7,-8,-9}},
             {{8,-7,-6},{9,-8,-7},{8,-7,-6},{7,-6,-5},{6,-5,-4},{5,-4,-7},{4,-7,-2}}}
            ,
            {{{1,-2,-7},{2,-7,-6},{7,-6,-5},{6,-5,-9},{5,-9,-7},{9,-7,-8},{7,-8,-9}},
             {{8,-7,-9},{9,-8,-7},{8,-7,-9},{7,-9,-5},{9,-5,-6},{5,-6,-7},{6,-7,-2}}}
        }
        }
    };    
    const rarray<float,6> a = RARRAY(b);
    const rarray<float,6> c = RARRAY(a);
    CHECK(EXTENT(a,0)==EXTENT(b,0));
    CHECK(EXTENT(a,1)==EXTENT(b,1));
    CHECK(EXTENT(a,2)==EXTENT(b,2));
    CHECK(EXTENT(a,3)==EXTENT(b,3));
    CHECK(EXTENT(a,4)==EXTENT(b,4));
    CHECK(EXTENT(a,5)==EXTENT(b,5));
    std::string s = print6d(RARRAY(b));
    CHECK(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
"{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n\n"
"{[(1,-2,-3)(2,-3,-4)(3,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-3)(4,-3,-2)]}{[(1,-2,-3)(2,-3,-6)(3,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-3)(6,-3,-2)]}\n"
"{[(1,-2,-7)(2,-7,-4)(7,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-7)(4,-7,-2)]}{[(1,-2,-7)(2,-7,-6)(7,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-7)(6,-7,-2)]}\n\n");
    s = print6d(RARRAY(c));
    CHECK(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
"{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n\n"
"{[(1,-2,-3)(2,-3,-4)(3,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-3)(4,-3,-2)]}{[(1,-2,-3)(2,-3,-6)(3,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-3)(6,-3,-2)]}\n"
"{[(1,-2,-7)(2,-7,-4)(7,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-7)(4,-7,-2)]}{[(1,-2,-7)(2,-7,-6)(7,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-7)(6,-7,-2)]}\n\n");
    return ALLCLEAR;
}

int test6autoconversion() {

    CHECK(test1dautoconversion()==ALLCLEAR);
    CHECK(test2dautoconversion()==ALLCLEAR);
    CHECK(test3dautoconversion()==ALLCLEAR);
    CHECK(test4dautoconversion()==ALLCLEAR);
    CHECK(test5dautoconversion()==ALLCLEAR);
    CHECK(test6dautoconversion()==ALLCLEAR);
    return ALLCLEAR;
}

int test7dautoconversion() {

    int seven[2][2][2][2][2][2][2] = {{{{{{{0}}}}}}}; 
    char expected_output[] =
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}";

    std::stringstream s;
    s << RARRAY(seven);
    CHECK(s.str()==expected_output);
    CHECK(EXTENT(seven,0) == 2);
    CHECK(EXTENT(seven,1) == 2);
    CHECK(EXTENT(seven,2) == 2);
    CHECK(EXTENT(seven,3) == 2);
    CHECK(EXTENT(seven,4) == 2);
    CHECK(EXTENT(seven,5) == 2);
    CHECK(EXTENT(seven,6) == 2);

    return ALLCLEAR;
}

int test8dautoconversion() {

    int eight[2][2][2][2][2][2][2][2] = {{{{{{{{0}}}}}}}}; 
    char expected_output[] =
          "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}";

    std::stringstream s;
    s << RARRAY(eight);
    CHECK(s.str()==expected_output);
    CHECK(EXTENT(eight,0) == 2);
    CHECK(EXTENT(eight,1) == 2);
    CHECK(EXTENT(eight,2) == 2);
    CHECK(EXTENT(eight,3) == 2);
    CHECK(EXTENT(eight,4) == 2);
    CHECK(EXTENT(eight,5) == 2);
    CHECK(EXTENT(eight,6) == 2);
    CHECK(EXTENT(eight,7) == 2);

    return ALLCLEAR;
}

int test9dautoconversion() {

    int nine[2][2][2][2][2][2][2][2][2] = {{{{{{{{{0}}}}}}}}}; 
    char expected_output[] =
         "{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
          "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n}";
    std::stringstream s;
    s << RARRAY(nine);
    CHECK(s.str()==expected_output);
    CHECK(EXTENT(nine,0) == 2);
    CHECK(EXTENT(nine,1) == 2);
    CHECK(EXTENT(nine,2) == 2);
    CHECK(EXTENT(nine,3) == 2);
    CHECK(EXTENT(nine,4) == 2);
    CHECK(EXTENT(nine,5) == 2);
    CHECK(EXTENT(nine,6) == 2);
    CHECK(EXTENT(nine,7) == 2);
    CHECK(EXTENT(nine,8) == 2);

    return ALLCLEAR;
}

int test10dautoconversion() {

    int ten[2][2][2][2][2][2][2][2][2][2] = {{{{{{{{{{0}}}}}}}}}}; 
    char expected_output[] =
        "{\n{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
          "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n},\n"
         "{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
          "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n}\n}";
    std::stringstream s;
    s << RARRAY(ten);
    CHECK(s.str()==expected_output);
    CHECK(EXTENT(ten,0) == 2);
    CHECK(EXTENT(ten,1) == 2);
    CHECK(EXTENT(ten,2) == 2);
    CHECK(EXTENT(ten,3) == 2);
    CHECK(EXTENT(ten,4) == 2);
    CHECK(EXTENT(ten,5) == 2);
    CHECK(EXTENT(ten,6) == 2);
    CHECK(EXTENT(ten,7) == 2);
    CHECK(EXTENT(ten,8) == 2);
    CHECK(EXTENT(ten,9) == 2);

    return ALLCLEAR;
}

int test11dautoconversion() {

    // can't resist: WHEEEEEEEEEEE!
    int eleven[2][2][2][2][2][2][2][2][2][2][2] =
         {{{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}, 
            {{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}, 
           {{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}, 
            {{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}}, 
          {{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}, 
            {{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}, 
           {{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}} 
            ,{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}, 
            {{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}, 
             {{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}, 
              {{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}, 
               {{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}}};
    char expected_output[] = 
        "{\n{\n{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n},\n"
          "{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n}\n},\n"
         "{\n{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n},\n"
          "{\n{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}"
           ",\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n},\n"
           "{\n{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
              "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}\n}\n}\n}\n}";
    std::stringstream s;
    s << RARRAY(eleven);
    CHECK(s.str()==expected_output);
    CHECK(EXTENT(eleven,0) == 2);
    CHECK(EXTENT(eleven,1) == 2);
    CHECK(EXTENT(eleven,2) == 2);
    CHECK(EXTENT(eleven,3) == 2);
    CHECK(EXTENT(eleven,4) == 2);
    CHECK(EXTENT(eleven,5) == 2);
    CHECK(EXTENT(eleven,6) == 2);
    CHECK(EXTENT(eleven,7) == 2);
    CHECK(EXTENT(eleven,8) == 2);
    CHECK(EXTENT(eleven,9) == 2);
    CHECK(EXTENT(eleven,10) == 2);
    return ALLCLEAR;
}


int test711autoconversion() {

    CHECK(test7dautoconversion()==ALLCLEAR);
    CHECK(test8dautoconversion()==ALLCLEAR);
    CHECK(test9dautoconversion()==ALLCLEAR);
    CHECK(test10dautoconversion()==ALLCLEAR);
    CHECK(test11dautoconversion()==ALLCLEAR);
    return ALLCLEAR;
}

int testoutput() {

    double a[5]={1,2,3,4,5};
    double b[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    double c[27]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
    rarray<double,1> q(a,5);
    rarray<double,2> r(b,4,4);
    rarray<double,3> s(c,3,3,3);
    std::stringstream out;
    out << q << r << s;
    CHECK(out.str() == "{1,2,3,4,5}{\n{1,2,3,4},\n{5,6,7,8},\n{9,10,11,12},\n{13,14,15,16}\n}{\n{\n{1,2,3},\n{4,5,6},\n{7,8,9}\n},\n{\n{10,11,12},\n{13,14,15},\n{16,17,18}\n},\n{\n{19,20,21},\n{22,23,24},\n{25,26,27}\n}\n}");

    std::stringstream instr("  \t\n{{{#2:14,5},{2,#3:{}2},{#7:{1,2,3},1}},{{4},{5,5},{6,6}},{{7,7},{8,8},{9,9}}}");
    std::string outstr("{\n{\n{14,5},\n{2,0},\n{0,1}\n},\n{\n{4,0},\n{5,5},\n{6,6}\n},\n{\n{7,7},\n{8,8},\n{9,9}\n}\n}");
    
    rarray<int,3> intarray;

    instr >> intarray;
    intarray[1][0][1] = 0;
    intarray[0][2][0] = 0;

    std::stringstream check;
    check << intarray;
    CHECK(check.str()==outstr);

    rarray<std::string,2> A(2,2);
    A[0][0] = "Hello, world";
    A[0][1] = "I like { and }";
    A[1][0] = "I prefer #";
    A[1][1] = "I'm easy.";
    
    rarray<std::string,2> B;

    std::stringstream sin("{{#12:Hello, world,#14:I like { and }},{#10:I prefer #,I'm easy.}}");
    sin >> B;

    CHECK(A[0][0] == B[0][0]);
    CHECK(A[0][1] == B[0][1]);
    CHECK(A[1][0] == B[1][0]);
    CHECK(A[1][1] == B[1][1]);

    float autoarr[2][2][2][2] = {{{{1,2},{3,4}},{{5,6},{7,8}}},{{{9,10},{11,12}},{{13,14},{15,16}}}};
    const std::string outcheck = "{\n{\n{\n{1,2},\n{3,4}\n},\n{\n{5,6},\n{7,8}\n}\n},\n{\n{\n{9,10},\n{11,12}\n},\n{\n{13,14},\n{15,16}\n}\n}\n}";
    std::stringstream sautoarr;
    sautoarr << RARRAY(autoarr);
    CHECK(sautoarr.str()==outcheck);
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////

int testiterators() {

    double a[5]={1,2,3,4,5};
    double b[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    double c[27]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
    rarray<double,1> q(a,5);
    rarray<double,2> r(b,4,4);
    rarray<double,3> s(c,3,3,3);
    std::stringstream qout;

    for (rarray<double,2>::iterator i=r.begin(); i!=r.end(); ++i)
    {
        *i += 2;
    }
    for (rarray<double,2>::const_iterator i=r.cbegin(); i!=r.cend(); i++)
    {
        qout << *i << ',';
    }

#ifndef RA_SKIPINTERMEDIATE
    for (rarray<double,2>::const_iterator i=r[1].cbegin(); i!=r[1].cend(); i++)
    {
        qout << *i << ',';
    }
#else
    qout << "7,8,9,10,";
#endif

    CHECK(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,");

    const rarray<double,2> rview = r;

    for (rarray<double,2>::const_iterator i=rview.begin(); i!=rview.end(); i++)
    {
        qout << *i << ',';
    }

    CHECK(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,");

    std::stringstream rout;

#if __cplusplus <= 199711L
    for (auto ap = q.begin(); ap != q.end(); ++ap)
    {
        *ap *= 2;
    }
#else
    for (auto& a: q)
    {
        a *= 2;
    }
#endif


    for (rarray<double,2>::const_iterator i=q.cbegin(); i!=q.cend(); i++)
    {
        qout << *i << ',';
    }

    CHECK(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,2,4,6,8,10,");

    const rarray<double,1> qconst = q;

#if __cplusplus <= 199711L
    for (auto bp=qconst.begin(); bp != qconst.end(); ++bp)
    {
        rout << (*bp) << ',';
    }
#else
    for (const auto& b: qconst)
    {
        rout << b << ',';
    }
#endif

    CHECK(rout.str() == "2,4,6,8,10,");

    #ifndef RA_SKIPINTERMEDIATE
    std::stringstream check;
    
#if __cplusplus <= 199711L
    for (auto cp = s[1].begin(); cp != s[1].end(); ++cp)
        *cp *= 2;
    for (auto dp = s[1][2].begin(); dp != s[1][2].end(); ++dp)
        *dp += 10;
    for (auto cp = s[1].begin(); cp != s[1].end(); ++cp)
        check << (*cp) << ',';
#else
    for (auto& c: s[1])
        c *= 2;
    for (auto& d: s[1][2])
        d += 10;
    for (const auto& c: s[1])
        check << c << ',';
#endif

    for (rarray<double,2>::const_iterator i=s[2].cbegin(); i!=s[2].cend(); i++)
    {
        check << *i << ',';
    }

    CHECK(check.str() == "20,22,24,26,28,30,42,44,46,19,20,21,22,23,24,25,26,27,");

    #endif

    auto sb = s.begin();
    auto se = s.end();

    CHECK(not (sb==se));
    CHECK(sb < se);
    CHECK(sb <= se);
    CHECK(se > sb);
    CHECK(se >= sb);
         
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int testfill()
{
    rarray<float,2> a(3,3);
    a.fill(1.23f);
    for (int i=0;i<EXTENT(a,0);i++) {
        for (int j=0;j<EXTENT(a,1);j++) {
            CHECK(a[i][j]==1.23f);
        }
    }
    
    rarray<float,1> b(5);
    b.fill(1.24f);
    for (int i=0;i<EXTENT(a,0);i++) {
        CHECK(b[i]==1.24f);
    }
    
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int testindex()
{

    rarray<float,1> a(6);
    int ind;
    CHECK(*a.index(a[0],&ind)==0);
    CHECK(*a.index(a[1],&ind)==1);
    CHECK(*a.index(a[2],&ind)==2);
    CHECK(*a.index(a[3],&ind)==3);
    CHECK(*a.index(a[4],&ind)==4);
    CHECK(*a.index(a[5],&ind)==5);
    CHECK(a.index(a[0],0)==0);
    CHECK(a.index(a[1],0)==1);
    CHECK(a.index(a[2],0)==2);
    CHECK(a.index(a[3],0)==3);
    CHECK(a.index(a[4],0)==4);
    CHECK(a.index(a[5],0)==5);
    CHECK(INDEX(a,a[0],0)==0);
    CHECK(INDEX(a,a[1],0)==1);
    CHECK(INDEX(a,a[2],0)==2);
    CHECK(INDEX(a,a[3],0)==3);
    CHECK(INDEX(a,a[4],0)==4);
    CHECK(INDEX(a,a[5],0)==5);
    for (auto i=a.begin(); i != a.end(); i++) {
        a.index(i,&ind);
        int ind2=a.index(i,0);
        CHECK(ind==ind2);
        *i = ind+1;
    }
#if __cplusplus <= 199711L
    for (auto element = a.begin(); element != a.end(); ++element)
        *element *= a.index(*element,&ind)[0];
    for (auto element = a.begin(); element != a.end(); ++element)
        *element *= a.index(*element,0);
#else
    for (auto& element: a)
        element *= a.index(element,&ind)[0];
    for (auto& element: a)
        element *= a.index(element,0);
#endif
    CHECK(a[0]==0);
    CHECK(a[1]==2);
    CHECK(a[2]==12)
    CHECK(a[3]==36);
    CHECK(a[4]==80);
    CHECK(a[5]==150);

    rarray<float,3> z(2,3,4);
    CHECK(z.index(z[1][2][3],0)==1);
    CHECK(z.index(z[1][2][3],1)==2);
    CHECK(z.index(z[1][2][3],2)==3);


    rarray<float,3> b(2,2,2);
    int i[3];
    CHECK(b.index(b[0][0][0],i)==i);
    CHECK(b.index(b[0][0][0],i)[0]==0);
    CHECK(b.index(b[0][0][1],i)[0]==0);
    CHECK(b.index(b[0][1][0],i)[0]==0);
    CHECK(b.index(b[0][1][1],i)[0]==0);
    CHECK(b.index(b[1][0][0],i)[0]==1);
    CHECK(b.index(b[1][0][1],i)[0]==1);
    CHECK(b.index(b[1][1][0],i)[0]==1);
    CHECK(b.index(b[1][1][1],i)[0]==1);
    CHECK(b.index(b[0][0][0],i)[1]==0);
    CHECK(b.index(b[0][0][1],i)[1]==0);
    CHECK(b.index(b[0][1][0],i)[1]==1);
    CHECK(b.index(b[0][1][1],i)[1]==1);
    CHECK(b.index(b[1][0][0],i)[1]==0);
    CHECK(b.index(b[1][0][1],i)[1]==0);
    CHECK(b.index(b[1][1][0],i)[1]==1);
    CHECK(b.index(b[1][1][1],i)[1]==1);
    CHECK(b.index(b[0][0][0],i)[2]==0);
    CHECK(b.index(b[0][0][1],i)[2]==1);
    CHECK(b.index(b[0][1][0],i)[2]==0);
    CHECK(b.index(b[0][1][1],i)[2]==1);
    CHECK(b.index(b[1][0][0],i)[2]==0);
    CHECK(b.index(b[1][0][1],i)[2]==1);
    CHECK(b.index(b[1][1][0],i)[2]==0);
    CHECK(b.index(b[1][1][1],i)[2]==1);

    float rbuf[3][3] = { {0,0,0}, 
                         {1,1,1}, 
                         {2,2,2} }; 
    float cbuf[3][3] = { {0,1,2}, 
                         {0,1,2},
                         {0,1,2} }; 
    rarray<float,2> r = RARRAY(rbuf);
    rarray<float,2> c = RARRAY(cbuf);

    for (auto i=r.begin(); i != r.end(); i++) {
        int ind[2];
        r.index(*i,ind);
        CHECK(ind[0]==*i);
    }
    
    for (auto i=c.begin(); i != c.end(); i++) {
        int ind[2];
        c.index(i,ind);
        CHECK(ind[1]==*i);
    }
    
    return ALLCLEAR;
}
//////////////////////////////////////////////////////////////////////

int testcomma_assignment()
{
    rarray<double,1> b(8);
    b.fill(0);
    b = 1,2,3,6,5,4;
    CHECK(b[0]==1);
    CHECK(b[1]==2);
    CHECK(b[2]==3);
    CHECK(b[3]==6);
    CHECK(b[4]==5);
    CHECK(b[5]==4);
    CHECK(b[6]==0);
    CHECK(b[7]==0);

    rarray<double,3> a(3,4,2);

    a =  1,2,    3,6,   5,4,   7,8, 
         9,12,  11,10, 21,22, 23,26, 
        25,24,  27,28, 29,32, 31,30;
    CHECK(a[0][0][0]== 1);
    CHECK(a[0][0][1]== 2);
    CHECK(a[0][1][0]== 3);
    CHECK(a[0][1][1]== 6);
    CHECK(a[0][2][0]== 5);
    CHECK(a[0][2][1]== 4);
    CHECK(a[0][3][0]== 7);
    CHECK(a[0][3][1]== 8);
    CHECK(a[1][0][0]== 9);
    CHECK(a[1][0][1]==12);
    CHECK(a[1][1][0]==11);
    CHECK(a[1][1][1]==10);
    CHECK(a[1][2][0]==21);
    CHECK(a[1][2][1]==22);
    CHECK(a[1][3][0]==23);
    CHECK(a[1][3][1]==26);
    CHECK(a[2][0][0]==25);
    CHECK(a[2][0][1]==24);
    CHECK(a[2][1][0]==27);
    CHECK(a[2][1][1]==28);
    CHECK(a[2][2][0]==29);
    CHECK(a[2][2][1]==32);
    CHECK(a[2][3][0]==31);
    CHECK(a[2][3][1]==30);

#ifndef RA_SKIPINTERMEDIATE

    a[1]       = 100,101,102,103,104,105,106,107;
    a[2][1]    = 200,201;
    a[2][2][0] = 300,301;   // on purpose using built-in comma operator which forgets the 301
    a[2][3][0] = (300,301); // on purpose using built-in comma operator which forgets the 300
    CHECK(a[0][0][0]== 1);
    CHECK(a[0][0][1]== 2);
    CHECK(a[0][1][0]== 3);
    CHECK(a[0][1][1]== 6);
    CHECK(a[0][2][0]== 5);
    CHECK(a[0][2][1]== 4);
    CHECK(a[0][3][0]== 7);
    CHECK(a[0][3][1]== 8);
    CHECK(a[1][0][0]==100);
    CHECK(a[1][0][1]==101);
    CHECK(a[1][1][0]==102);
    CHECK(a[1][1][1]==103);
    CHECK(a[1][2][0]==104);
    CHECK(a[1][2][1]==105);
    CHECK(a[1][3][0]==106);
    CHECK(a[1][3][1]==107);
    CHECK(a[2][0][0]==25);
    CHECK(a[2][0][1]==24);
    CHECK(a[2][1][0]==200);
    CHECK(a[2][1][1]==201);
    CHECK(a[2][2][0]==300);
    CHECK(a[2][2][1]==32);
    CHECK(a[2][3][0]==301);
    CHECK(a[2][3][1]==30);

#endif

    return ALLCLEAR;
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

template<typename T, int R>
struct array {
    T elements_[R];
    T& operator[](const int i) { return elements_[i]; }
    const T& operator[](const int i) const { return elements_[i]; }
    bool operator!=(const struct array& other) {
        for (int i=0;i<R;i++)
            if (elements_[i] != other.elements_[i])
                return true;
        return false;
    }
    bool operator==(const struct array& other) {
        return !operator!=(other);
    }       
};

//////////////////////////////////////////////////////////////////////

array<compound,3> operator+(const array<compound,3> &a,
                            const array<compound,3> &b)
{
    array<compound,3> result = {{a[0]+b[0],a[1]+b[1],a[2]+b[2]}};
    return result;
}


int testrlinear() {
    int a = 1, b = 30;
    auto r = rlinear(a,b);
    int i = a;
    for (auto x: r) {
        CHECK(x==i);
        i++;
    }
    auto r2 = rlinear(0,30,4);
    int check2[] = {0,10,20,30};
    int j = 0;
    for (auto y: r2) {
        CHECK(y == check2[j]);
        j++;
    }
    auto r3 = rlinear(0,30,3,false);
    int check3[] = {0,10,20};
    int k = 0;
    for (auto z: r3) {
        CHECK(z == check3[k]);
        k++;
    }
    auto r4 = rlinear(0.0, 30.0, 4);
    double check4[] = {0.0, 10.0, 20.0, 30.0};
    int l = 0;
    for (auto zz: r3) {
        CHECK(zz == check4[l]);
        l++;
    }
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////

int main() 
{
    double d1 = -2.2, d2 = 7.1;
    compound c1(1,2), c2(-7,13);
    array<compound,3> a1 = {{compound(1,2),compound(3,4),compound(5,6)}};
    array<compound,3> a2 = {{compound(-1,-2),compound(3,-4),compound(5,-6)}};

    PASSORRETURN(testconstructors<double>());
    PASSORRETURN(testconstructors<compound>());
    PASSORRETURN((testconstructors<array<compound,3> >()));

    PASSORRETURN(testconstructors7dimtest<double>());
    PASSORRETURN(testconstructors7dimtest<compound>());
    PASSORRETURN(testconstructors7dimtest2<double>());
    PASSORRETURN(testconstructors7dimtest2<compound>());
    PASSORRETURN((testconstructors7dim<array<compound,3> >()));

    PASSORRETURN(testconstructors7dimbuf<double>());
    PASSORRETURN(testconstructors7dimbuf<compound>());
    PASSORRETURN((testconstructors7dimbuf<array<compound,3> >()));

    PASSORRETURN(testconstructors12dim<double>());
    PASSORRETURN(testconstructors12dim<compound>());
    PASSORRETURN((testconstructors12dim<array<compound,3> >()));

    PASSORRETURN(testconstructors12dimbuf<double>());
    PASSORRETURN(testconstructors12dimbuf<compound>());
    PASSORRETURN((testconstructors12dimbuf<array<compound,3> >()));


    PASSORRETURN(testaccessors<double>(d1,d2));
    PASSORRETURN(testaccessors<compound>(c1,c2));
    PASSORRETURN((testaccessors<array<compound,3> >(a1,a2)));

    PASSORRETURN(testsliceconstructor<double>());
    PASSORRETURN(testsliceconstructor<compound>());
    PASSORRETURN((testsliceconstructor<array<compound,3> >()));

    PASSORRETURN(testcopy<double>(d1,d2));
    PASSORRETURN(testcopy<compound>(c1,c2));
    PASSORRETURN((testcopy<array<compound,3> >(a1,a2)));

    PASSORRETURN(testcopy1d<double>(d1,d2));
    PASSORRETURN(testcopy1d<compound>(c1,c2));
    PASSORRETURN((testcopy1d<array<compound,3> >(a1,a2)));

    PASSORRETURN(testmmm<int>());
    PASSORRETURN(testmmm<double>());

    PASSORRETURN(testconversions());
    PASSORRETURN(testconstintermediate()); 
    PASSORRETURN(testassignment());
    PASSORRETURN(testintermediateconversion());
    PASSORRETURN(testreshape());
    PASSORRETURN(test6autoconversion());
    PASSORRETURN(testoutput());
    PASSORRETURN(testiterators());

    PASSORRETURN(test711autoconversion());

    PASSORRETURN(testfill());

    PASSORRETURN(testindex());

    PASSORRETURN(testcomma_assignment());

    PASSORRETURN(testrlinear());
    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////

