//
// tests.cc - testsuite for rarray.h
//
// Copyright (c) 2013-2014  Ramses van Zon
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
#include "rarray.h"
#include <iostream>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <string>
#include <sstream>

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

    PASSORRETURN(testconstructors7dim<double>());
    PASSORRETURN(testconstructors7dim<compound>());
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



    return ALLCLEAR;
}

//////////////////////////////////////////////////////////////////////

