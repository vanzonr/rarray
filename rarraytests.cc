//
// rarraytests.cc - testsuite for rarray, using Boost.Test
//
// Copyright (c) 2015-2016  Ramses van Zon
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
#define BOOST_TEST_MODULE RarrayTests
#include "boost/test/included/unit_test.hpp"
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

// Because it is a test, use the headers in the local directory preferentially
#include "rarray"
#include "rarrayio"
#include <iostream>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <string>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

class Compound 
{
  public:
    Compound(): x(0), y(0) {}
    Compound(int x, int y): x(x), y(y) {}
    int GetX() { return x; }
    int GetY() { return y; }
    bool operator==(const Compound&other) const {
        return x==other.x and y==other.y;
    }
    bool operator!=(const Compound&other) const {
        return x!=other.x or y!=other.y;
    }
    Compound operator+(const Compound& other) const {
        return Compound(x+other.x,y+other.y);
    }
  private:
    int x;
    int y;
};

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

array<Compound,3> operator+(const array<Compound,3> &a,
                            const array<Compound,3> &b)
{
    array<Compound,3> result = {{a[0]+b[0],a[1]+b[1],a[2]+b[2]}};
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////

template<typename T,int R> 
const T* getconstdata(const rarray<T,R>& a)
{
    return a.data();
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::mpl::list<double,Compound,array<Compound,3> > testconstructors_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(testconstructors, T, testconstructors_types)
{
    // Exercises the constructors of the rarray class
    int dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    const int* asize = a.shape();
    BOOST_CHECK(a.data());
    BOOST_CHECK(a.size()==7*21*13);
    BOOST_CHECK(asize);
    BOOST_CHECK(asize[0] == dim[0]);
    BOOST_CHECK(asize[1] == dim[1]);
    BOOST_CHECK(asize[2] == dim[2]);
    BOOST_CHECK(a.extent(0) == dim[0]);
    BOOST_CHECK(a.extent(1) == dim[1]);
    BOOST_CHECK(a.extent(2) == dim[2]);
    BOOST_CHECK(b.data());
    BOOST_CHECK(b.size()==7*21*13);
    BOOST_CHECK(b.extent(0) == dim[0]);
    BOOST_CHECK(b.extent(1) == dim[1]);
    BOOST_CHECK(b.extent(2) == dim[2]);
    BOOST_CHECK(c.data());
    BOOST_CHECK(c.size()==7*21*13);
    BOOST_CHECK(c.extent(0) == dim[0]);
    BOOST_CHECK(c.extent(1) == dim[1]);
    BOOST_CHECK(c.extent(2) == dim[2]);
    BOOST_CHECK(c.data()==b.data());
    b.clear();
    BOOST_CHECK(b.is_clear());
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE_TEMPLATE(testconstructors7dim, T, testconstructors_types)
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
    BOOST_CHECK(a1.data());
    BOOST_CHECK(b1.data());
    BOOST_CHECK(c1.data()==b1.data());
    BOOST_CHECK(a2.data());
    BOOST_CHECK(b2.data());
    BOOST_CHECK(c2.data()==b2.data());
    BOOST_CHECK(a3.data());
    BOOST_CHECK(b3.data());
    BOOST_CHECK(c3.data()==b3.data());
    BOOST_CHECK(a4.data());
    BOOST_CHECK(b4.data());
    BOOST_CHECK(c4.data()==b4.data());
    BOOST_CHECK(a5.data());
    BOOST_CHECK(b5.data());
    BOOST_CHECK(c5.data()==b5.data());
    BOOST_CHECK(a6.data());
    BOOST_CHECK(b6.data());
    BOOST_CHECK(c6.data()==b6.data());
    BOOST_CHECK(b7.data());
    BOOST_CHECK(c7.data()==b7.data());
    BOOST_CHECK(b1.size()==7);
    BOOST_CHECK(b2.size()==7*10);
    BOOST_CHECK(b3.size()==7*10*13);
    BOOST_CHECK(b4.size()==7*10*13*2);
    BOOST_CHECK(b5.size()==7*10*13*2*4);
    BOOST_CHECK(b6.size()==7*10*13*2*4*5);
    BOOST_CHECK(b7.size()==7*10*13*2*4*5*21);
    BOOST_CHECK(a1.extent(0) == dim[0]);
    BOOST_CHECK(b1.extent(0) == dim[0]);
    BOOST_CHECK(c1.extent(0) == dim[0]);
    BOOST_CHECK(a1.shape());
    BOOST_CHECK(a1.shape()[0] == dim[0]);
    BOOST_CHECK(a2.extent(0) == dim[0]);
    BOOST_CHECK(a2.extent(1) == dim[1]);
    BOOST_CHECK(b2.extent(0) == dim[0]);
    BOOST_CHECK(b2.extent(1) == dim[1]);
    BOOST_CHECK(c2.extent(0) == dim[0]);
    BOOST_CHECK(c2.extent(1) == dim[1]);
    BOOST_CHECK(a3.extent(0) == dim[0]);
    BOOST_CHECK(a3.extent(1) == dim[1]);
    BOOST_CHECK(a3.extent(2) == dim[2]);
    BOOST_CHECK(b3.extent(0) == dim[0]);
    BOOST_CHECK(b3.extent(1) == dim[1]);
    BOOST_CHECK(b3.extent(2) == dim[2]);
    BOOST_CHECK(c3.extent(0) == dim[0]);
    BOOST_CHECK(c3.extent(1) == dim[1]);
    BOOST_CHECK(c3.extent(2) == dim[2]);
    BOOST_CHECK(a4.extent(0) == dim[0]);
    BOOST_CHECK(a4.extent(1) == dim[1]);
    BOOST_CHECK(a4.extent(2) == dim[2]);
    BOOST_CHECK(a4.extent(3) == dim[3]);
    BOOST_CHECK(b4.extent(0) == dim[0]);
    BOOST_CHECK(b4.extent(1) == dim[1]);
    BOOST_CHECK(b4.extent(2) == dim[2]);
    BOOST_CHECK(b4.extent(3) == dim[3]);
    BOOST_CHECK(c4.extent(0) == dim[0]);
    BOOST_CHECK(c4.extent(1) == dim[1]);
    BOOST_CHECK(c4.extent(2) == dim[2]);
    BOOST_CHECK(c4.extent(3) == dim[3]);
    BOOST_CHECK(a5.extent(0) == dim[0]);
    BOOST_CHECK(a5.extent(1) == dim[1]);
    BOOST_CHECK(a5.extent(2) == dim[2]);
    BOOST_CHECK(a5.extent(3) == dim[3]);
    BOOST_CHECK(a5.extent(4) == dim[4]);
    BOOST_CHECK(b5.extent(0) == dim[0]);
    BOOST_CHECK(b5.extent(1) == dim[1]);
    BOOST_CHECK(b5.extent(2) == dim[2]);
    BOOST_CHECK(b5.extent(3) == dim[3]);
    BOOST_CHECK(b5.extent(4) == dim[4]);
    BOOST_CHECK(c5.extent(0) == dim[0]);
    BOOST_CHECK(c5.extent(1) == dim[1]);
    BOOST_CHECK(c5.extent(2) == dim[2]);
    BOOST_CHECK(c5.extent(3) == dim[3]);
    BOOST_CHECK(c5.extent(4) == dim[4]);
    BOOST_CHECK(a6.extent(0) == dim[0]);
    BOOST_CHECK(a6.extent(1) == dim[1]);
    BOOST_CHECK(a6.extent(2) == dim[2]);
    BOOST_CHECK(a6.extent(3) == dim[3]);
    BOOST_CHECK(a6.extent(4) == dim[4]);
    BOOST_CHECK(a6.extent(5) == dim[5]);
    BOOST_CHECK(b6.extent(0) == dim[0]);
    BOOST_CHECK(b6.extent(1) == dim[1]);
    BOOST_CHECK(b6.extent(2) == dim[2]);
    BOOST_CHECK(b6.extent(3) == dim[3]);
    BOOST_CHECK(b6.extent(4) == dim[4]);
    BOOST_CHECK(b6.extent(5) == dim[5]);
    BOOST_CHECK(c6.extent(0) == dim[0]);
    BOOST_CHECK(c6.extent(1) == dim[1]);
    BOOST_CHECK(c6.extent(2) == dim[2]);
    BOOST_CHECK(c6.extent(3) == dim[3]);
    BOOST_CHECK(c6.extent(4) == dim[4]);
    BOOST_CHECK(c6.extent(5) == dim[5]);
    BOOST_CHECK(b7.extent(0) == dim[0]);
    BOOST_CHECK(b7.extent(1) == dim[1]);
    BOOST_CHECK(b7.extent(2) == dim[2]);
    BOOST_CHECK(b7.extent(3) == dim[3]);
    BOOST_CHECK(b7.extent(4) == dim[4]);
    BOOST_CHECK(b7.extent(5) == dim[5]);
    BOOST_CHECK(b7.extent(6) == dim[6]);
    BOOST_CHECK(c7.extent(0) == dim[0]);
    BOOST_CHECK(c7.extent(1) == dim[1]);
    BOOST_CHECK(c7.extent(2) == dim[2]);
    BOOST_CHECK(c7.extent(3) == dim[3]);
    BOOST_CHECK(c7.extent(4) == dim[4]);
    BOOST_CHECK(c7.extent(5) == dim[5]);
    BOOST_CHECK(c7.extent(6) == dim[6]);
    a1.clear(); //optional here, as a1 will go out of scope
    b7.clear();
    BOOST_CHECK(a1.is_clear());
    BOOST_CHECK(b7.is_clear());
    BOOST_CHECK(c7.is_clear() == false);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE_TEMPLATE(testconstructors12dim, T, testconstructors_types)
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
    BOOST_CHECK(a7.data());
    BOOST_CHECK(b7.data());
    BOOST_CHECK(c7.data()==b7.data());
    BOOST_CHECK(a8.data());
    BOOST_CHECK(b8.data());
    BOOST_CHECK(c8.data()==b8.data());
    BOOST_CHECK(a9.data());
    BOOST_CHECK(b9.data());
    BOOST_CHECK(c9.data()==b9.data());
    BOOST_CHECK(a10.data());
    BOOST_CHECK(b10.data());
    BOOST_CHECK(c10.data()==b10.data());
    BOOST_CHECK(a11.data());
    BOOST_CHECK(b11.data());
    BOOST_CHECK(c11.data()==b11.data());
    BOOST_CHECK(b12.data());
    BOOST_CHECK(c12.data()==b12.data());
    BOOST_CHECK( b7.size()==2*3*4*3*2*3*4);
    BOOST_CHECK( b8.size()==2*3*4*3*2*3*4*3);
    BOOST_CHECK( b9.size()==2*3*4*3*2*3*4*3*2);
    BOOST_CHECK(b10.size()==2*3*4*3*2*3*4*3*2*3);
    BOOST_CHECK(b11.size()==2*3*4*3*2*3*4*3*2*3*4);
    BOOST_CHECK(b12.size()==2*3*4*3*2*3*4*3*2*3*4*3);
    BOOST_CHECK(a7.extent(0) == dim[0]);
    BOOST_CHECK(a7.extent(1) == dim[1]);
    BOOST_CHECK(a7.extent(2) == dim[2]);
    BOOST_CHECK(a7.extent(3) == dim[3]);
    BOOST_CHECK(a7.extent(4) == dim[4]);
    BOOST_CHECK(a7.extent(5) == dim[5]);
    BOOST_CHECK(a7.extent(6) == dim[6]);
    BOOST_CHECK(b7.extent(0) == dim[0]);
    BOOST_CHECK(b7.extent(1) == dim[1]);
    BOOST_CHECK(b7.extent(2) == dim[2]);
    BOOST_CHECK(b7.extent(3) == dim[3]);
    BOOST_CHECK(b7.extent(4) == dim[4]);
    BOOST_CHECK(b7.extent(5) == dim[5]);
    BOOST_CHECK(b7.extent(6) == dim[6]);
    BOOST_CHECK(c7.extent(0) == dim[0]);
    BOOST_CHECK(c7.extent(1) == dim[1]);
    BOOST_CHECK(c7.extent(2) == dim[2]);
    BOOST_CHECK(c7.extent(3) == dim[3]);
    BOOST_CHECK(c7.extent(4) == dim[4]);
    BOOST_CHECK(c7.extent(5) == dim[5]);
    BOOST_CHECK(c7.extent(6) == dim[6]);
    BOOST_CHECK(a8.extent(0) == dim[0]);
    BOOST_CHECK(a8.extent(1) == dim[1]);
    BOOST_CHECK(a8.extent(2) == dim[2]);
    BOOST_CHECK(a8.extent(3) == dim[3]);
    BOOST_CHECK(a8.extent(4) == dim[4]);
    BOOST_CHECK(a8.extent(5) == dim[5]);
    BOOST_CHECK(a8.extent(6) == dim[6]);
    BOOST_CHECK(a8.extent(7) == dim[7]);
    BOOST_CHECK(b8.extent(0) == dim[0]);
    BOOST_CHECK(b8.extent(1) == dim[1]);
    BOOST_CHECK(b8.extent(2) == dim[2]);
    BOOST_CHECK(b8.extent(3) == dim[3]);
    BOOST_CHECK(b8.extent(4) == dim[4]);
    BOOST_CHECK(b8.extent(5) == dim[5]);
    BOOST_CHECK(b8.extent(6) == dim[6]);
    BOOST_CHECK(b8.extent(7) == dim[7]);
    BOOST_CHECK(c8.extent(0) == dim[0]);
    BOOST_CHECK(c8.extent(1) == dim[1]);
    BOOST_CHECK(c8.extent(2) == dim[2]);
    BOOST_CHECK(c8.extent(3) == dim[3]);
    BOOST_CHECK(c8.extent(4) == dim[4]);
    BOOST_CHECK(c8.extent(5) == dim[5]);
    BOOST_CHECK(c8.extent(6) == dim[6]);
    BOOST_CHECK(c8.extent(7) == dim[7]);
    BOOST_CHECK(a9.extent(0) == dim[0]);
    BOOST_CHECK(a9.extent(1) == dim[1]);
    BOOST_CHECK(a9.extent(2) == dim[2]);
    BOOST_CHECK(a9.extent(3) == dim[3]);
    BOOST_CHECK(a9.extent(4) == dim[4]);
    BOOST_CHECK(a9.extent(5) == dim[5]);
    BOOST_CHECK(a9.extent(6) == dim[6]);
    BOOST_CHECK(a9.extent(7) == dim[7]);
    BOOST_CHECK(a9.extent(8) == dim[8]);
    BOOST_CHECK(b9.extent(0) == dim[0]);
    BOOST_CHECK(b9.extent(1) == dim[1]);
    BOOST_CHECK(b9.extent(2) == dim[2]);
    BOOST_CHECK(b9.extent(3) == dim[3]);
    BOOST_CHECK(b9.extent(4) == dim[4]);
    BOOST_CHECK(b9.extent(5) == dim[5]);
    BOOST_CHECK(b9.extent(6) == dim[6]);
    BOOST_CHECK(b9.extent(7) == dim[7]);
    BOOST_CHECK(b9.extent(8) == dim[8]);
    BOOST_CHECK(c9.extent(0) == dim[0]);
    BOOST_CHECK(c9.extent(1) == dim[1]);
    BOOST_CHECK(c9.extent(2) == dim[2]);
    BOOST_CHECK(c9.extent(3) == dim[3]);
    BOOST_CHECK(c9.extent(4) == dim[4]);
    BOOST_CHECK(c9.extent(5) == dim[5]);
    BOOST_CHECK(c9.extent(6) == dim[6]);
    BOOST_CHECK(c9.extent(7) == dim[7]);
    BOOST_CHECK(c9.extent(8) == dim[8]);
    BOOST_CHECK(a10.extent(0) == dim[0]);
    BOOST_CHECK(a10.extent(1) == dim[1]);
    BOOST_CHECK(a10.extent(2) == dim[2]);
    BOOST_CHECK(a10.extent(3) == dim[3]);
    BOOST_CHECK(a10.extent(4) == dim[4]);
    BOOST_CHECK(a10.extent(5) == dim[5]);
    BOOST_CHECK(a10.extent(6) == dim[6]);
    BOOST_CHECK(a10.extent(7) == dim[7]);
    BOOST_CHECK(a10.extent(8) == dim[8]);
    BOOST_CHECK(a10.extent(9) == dim[9]);
    BOOST_CHECK(b10.extent(0) == dim[0]);
    BOOST_CHECK(b10.extent(1) == dim[1]);
    BOOST_CHECK(b10.extent(2) == dim[2]);
    BOOST_CHECK(b10.extent(3) == dim[3]);
    BOOST_CHECK(b10.extent(4) == dim[4]);
    BOOST_CHECK(b10.extent(5) == dim[5]);
    BOOST_CHECK(b10.extent(6) == dim[6]);
    BOOST_CHECK(b10.extent(7) == dim[7]);
    BOOST_CHECK(b10.extent(8) == dim[8]);
    BOOST_CHECK(b10.extent(9) == dim[9]);
    BOOST_CHECK(c10.extent(0) == dim[0]);
    BOOST_CHECK(c10.extent(1) == dim[1]);
    BOOST_CHECK(c10.extent(2) == dim[2]);
    BOOST_CHECK(c10.extent(3) == dim[3]);
    BOOST_CHECK(c10.extent(4) == dim[4]);
    BOOST_CHECK(c10.extent(5) == dim[5]);
    BOOST_CHECK(c10.extent(6) == dim[6]);
    BOOST_CHECK(c10.extent(7) == dim[7]);
    BOOST_CHECK(c10.extent(8) == dim[8]);
    BOOST_CHECK(c10.extent(9) == dim[9]);
    BOOST_CHECK(a11.extent( 0) == dim[ 0]);
    BOOST_CHECK(a11.extent( 1) == dim[ 1]);
    BOOST_CHECK(a11.extent( 2) == dim[ 2]);
    BOOST_CHECK(a11.extent( 3) == dim[ 3]);
    BOOST_CHECK(a11.extent( 4) == dim[ 4]);
    BOOST_CHECK(a11.extent( 5) == dim[ 5]);
    BOOST_CHECK(a11.extent( 6) == dim[ 6]);
    BOOST_CHECK(a11.extent( 7) == dim[ 7]);
    BOOST_CHECK(a11.extent( 8) == dim[ 8]);
    BOOST_CHECK(a11.extent( 9) == dim[ 9]);
    BOOST_CHECK(a11.extent(10) == dim[10]);
    BOOST_CHECK(b11.extent( 0) == dim[ 0]);
    BOOST_CHECK(b11.extent( 1) == dim[ 1]);
    BOOST_CHECK(b11.extent( 2) == dim[ 2]);
    BOOST_CHECK(b11.extent( 3) == dim[ 3]);
    BOOST_CHECK(b11.extent( 4) == dim[ 4]);
    BOOST_CHECK(b11.extent( 5) == dim[ 5]);
    BOOST_CHECK(b11.extent( 6) == dim[ 6]);
    BOOST_CHECK(b11.extent( 7) == dim[ 7]);
    BOOST_CHECK(b11.extent( 8) == dim[ 8]);
    BOOST_CHECK(b11.extent( 9) == dim[ 9]);
    BOOST_CHECK(b11.extent(10) == dim[10]);
    BOOST_CHECK(c11.extent( 0) == dim[ 0]);
    BOOST_CHECK(c11.extent( 1) == dim[ 1]);
    BOOST_CHECK(c11.extent( 2) == dim[ 2]);
    BOOST_CHECK(c11.extent( 3) == dim[ 3]);
    BOOST_CHECK(c11.extent( 4) == dim[ 4]);
    BOOST_CHECK(c11.extent( 5) == dim[ 5]);
    BOOST_CHECK(c11.extent( 6) == dim[ 6]);
    BOOST_CHECK(c11.extent( 7) == dim[ 7]);
    BOOST_CHECK(c11.extent( 8) == dim[ 8]);
    BOOST_CHECK(c11.extent( 9) == dim[ 9]);
    BOOST_CHECK(c11.extent(10) == dim[10]);
    BOOST_CHECK(b12.extent( 0) == dim[ 0]);
    BOOST_CHECK(b12.extent( 1) == dim[ 1]);
    BOOST_CHECK(b12.extent( 2) == dim[ 2]);
    BOOST_CHECK(b12.extent( 3) == dim[ 3]);
    BOOST_CHECK(b12.extent( 4) == dim[ 4]);
    BOOST_CHECK(b12.extent( 5) == dim[ 5]);
    BOOST_CHECK(b12.extent( 6) == dim[ 6]);
    BOOST_CHECK(b12.extent( 7) == dim[ 7]);
    BOOST_CHECK(b12.extent( 8) == dim[ 8]);
    BOOST_CHECK(b12.extent( 9) == dim[ 9]);
    BOOST_CHECK(b12.extent(10) == dim[10]);
    BOOST_CHECK(b12.extent(11) == dim[11]);
    BOOST_CHECK(c12.extent( 0) == dim[ 0]);
    BOOST_CHECK(c12.extent( 1) == dim[ 1]);
    BOOST_CHECK(c12.extent( 2) == dim[ 2]);
    BOOST_CHECK(c12.extent( 3) == dim[ 3]);
    BOOST_CHECK(c12.extent( 4) == dim[ 4]);
    BOOST_CHECK(c12.extent( 5) == dim[ 5]);
    BOOST_CHECK(c12.extent( 6) == dim[ 6]);
    BOOST_CHECK(c12.extent( 7) == dim[ 7]);
    BOOST_CHECK(c12.extent( 8) == dim[ 8]);
    BOOST_CHECK(c12.extent( 9) == dim[ 9]);
    BOOST_CHECK(c12.extent(10) == dim[10]);
    BOOST_CHECK(c12.extent(11) == dim[11]);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE_TEMPLATE(testconstructors7dimbuf, T, testconstructors_types)
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
        BOOST_CHECK(a1.data());
        BOOST_CHECK(b1.data());
        BOOST_CHECK(a2.data());
        BOOST_CHECK(b2.data());
        BOOST_CHECK(a3.data());
        BOOST_CHECK(b3.data());
        BOOST_CHECK(a4.data());
        BOOST_CHECK(b4.data());
        BOOST_CHECK(a5.data());
        BOOST_CHECK(b5.data());
        BOOST_CHECK(a6.data());
        BOOST_CHECK(b6.data());
        BOOST_CHECK(b7.data());
        BOOST_CHECK(a1.extent(0) == dim[0]);
        BOOST_CHECK(b1.extent(0) == dim[0]);
        BOOST_CHECK(a2.extent(0) == dim[0]);
        BOOST_CHECK(a2.extent(1) == dim[1]);
        BOOST_CHECK(b2.extent(0) == dim[0]);
        BOOST_CHECK(b2.extent(1) == dim[1]);
        BOOST_CHECK(a3.extent(0) == dim[0]);
        BOOST_CHECK(a3.extent(1) == dim[1]);
        BOOST_CHECK(a3.extent(2) == dim[2]);
        BOOST_CHECK(b3.extent(0) == dim[0]);
        BOOST_CHECK(b3.extent(1) == dim[1]);
        BOOST_CHECK(b3.extent(2) == dim[2]);
        BOOST_CHECK(a4.extent(0) == dim[0]);
        BOOST_CHECK(a4.extent(1) == dim[1]);
        BOOST_CHECK(a4.extent(2) == dim[2]);
        BOOST_CHECK(a4.extent(3) == dim[3]);
        BOOST_CHECK(b4.extent(0) == dim[0]);
        BOOST_CHECK(b4.extent(1) == dim[1]);
        BOOST_CHECK(b4.extent(2) == dim[2]);
        BOOST_CHECK(b4.extent(3) == dim[3]);
        BOOST_CHECK(a5.extent(0) == dim[0]);
        BOOST_CHECK(a5.extent(1) == dim[1]);
        BOOST_CHECK(a5.extent(2) == dim[2]);
        BOOST_CHECK(a5.extent(3) == dim[3]);
        BOOST_CHECK(a5.extent(4) == dim[4]);
        BOOST_CHECK(b5.extent(0) == dim[0]);
        BOOST_CHECK(b5.extent(1) == dim[1]);
        BOOST_CHECK(b5.extent(2) == dim[2]);
        BOOST_CHECK(b5.extent(3) == dim[3]);
        BOOST_CHECK(b5.extent(4) == dim[4]);
        BOOST_CHECK(a6.extent(0) == dim[0]);
        BOOST_CHECK(a6.extent(1) == dim[1]);
        BOOST_CHECK(a6.extent(2) == dim[2]);
        BOOST_CHECK(a6.extent(3) == dim[3]);
        BOOST_CHECK(a6.extent(4) == dim[4]);
        BOOST_CHECK(a6.extent(5) == dim[5]);
        BOOST_CHECK(b6.extent(0) == dim[0]);
        BOOST_CHECK(b6.extent(1) == dim[1]);
        BOOST_CHECK(b6.extent(2) == dim[2]);
        BOOST_CHECK(b6.extent(3) == dim[3]);
        BOOST_CHECK(b6.extent(4) == dim[4]);
        BOOST_CHECK(b6.extent(5) == dim[5]);
        BOOST_CHECK(b7.extent(0) == dim[0]);
        BOOST_CHECK(b7.extent(1) == dim[1]);
        BOOST_CHECK(b7.extent(2) == dim[2]);
        BOOST_CHECK(b7.extent(3) == dim[3]);
        BOOST_CHECK(b7.extent(4) == dim[4]);
        BOOST_CHECK(b7.extent(5) == dim[5]);
        BOOST_CHECK(b7.extent(6) == dim[6]);
    }
    delete[] buf;
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE_TEMPLATE(testconstructors12dimbuf, T, testconstructors_types)
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
        BOOST_CHECK(a7.data());
        BOOST_CHECK(b7.data());
        BOOST_CHECK(a8.data());
        BOOST_CHECK(b8.data());
        BOOST_CHECK(a9.data());
        BOOST_CHECK(b9.data());
        BOOST_CHECK(a10.data());
        BOOST_CHECK(b10.data());
        BOOST_CHECK(a11.data());
        BOOST_CHECK(b11.data());
        BOOST_CHECK(b12.data());
        BOOST_CHECK(a7.extent(0) == dim[0]);
        BOOST_CHECK(a7.extent(1) == dim[1]);
        BOOST_CHECK(a7.extent(2) == dim[2]);
        BOOST_CHECK(a7.extent(3) == dim[3]);
        BOOST_CHECK(a7.extent(4) == dim[4]);
        BOOST_CHECK(a7.extent(5) == dim[5]);
        BOOST_CHECK(a7.extent(6) == dim[6]);
        BOOST_CHECK(b7.extent(0) == dim[0]);
        BOOST_CHECK(b7.extent(1) == dim[1]);
        BOOST_CHECK(b7.extent(2) == dim[2]);
        BOOST_CHECK(b7.extent(3) == dim[3]);
        BOOST_CHECK(b7.extent(4) == dim[4]);
        BOOST_CHECK(b7.extent(5) == dim[5]);
        BOOST_CHECK(b7.extent(6) == dim[6]);
        BOOST_CHECK(a8.extent(0) == dim[0]);
        BOOST_CHECK(a8.extent(1) == dim[1]);
        BOOST_CHECK(a8.extent(2) == dim[2]);
        BOOST_CHECK(a8.extent(3) == dim[3]);
        BOOST_CHECK(a8.extent(4) == dim[4]);
        BOOST_CHECK(a8.extent(5) == dim[5]);
        BOOST_CHECK(a8.extent(6) == dim[6]);
        BOOST_CHECK(a8.extent(7) == dim[7]);
        BOOST_CHECK(b8.extent(0) == dim[0]);
        BOOST_CHECK(b8.extent(1) == dim[1]);
        BOOST_CHECK(b8.extent(2) == dim[2]);
        BOOST_CHECK(b8.extent(3) == dim[3]);
        BOOST_CHECK(b8.extent(4) == dim[4]);
        BOOST_CHECK(b8.extent(5) == dim[5]);
        BOOST_CHECK(b8.extent(6) == dim[6]);
        BOOST_CHECK(b8.extent(7) == dim[7]);
        BOOST_CHECK(a9.extent(0) == dim[0]);
        BOOST_CHECK(a9.extent(1) == dim[1]);
        BOOST_CHECK(a9.extent(2) == dim[2]);
        BOOST_CHECK(a9.extent(3) == dim[3]);
        BOOST_CHECK(a9.extent(4) == dim[4]);
        BOOST_CHECK(a9.extent(5) == dim[5]);
        BOOST_CHECK(a9.extent(6) == dim[6]);
        BOOST_CHECK(a9.extent(7) == dim[7]);
        BOOST_CHECK(a9.extent(8) == dim[8]);
        BOOST_CHECK(b9.extent(0) == dim[0]);
        BOOST_CHECK(b9.extent(1) == dim[1]);
        BOOST_CHECK(b9.extent(2) == dim[2]);
        BOOST_CHECK(b9.extent(3) == dim[3]);
        BOOST_CHECK(b9.extent(4) == dim[4]);
        BOOST_CHECK(b9.extent(5) == dim[5]);
        BOOST_CHECK(b9.extent(6) == dim[6]);
        BOOST_CHECK(b9.extent(7) == dim[7]);
        BOOST_CHECK(b9.extent(8) == dim[8]);
        BOOST_CHECK(a10.extent(0) == dim[0]);
        BOOST_CHECK(a10.extent(1) == dim[1]);
        BOOST_CHECK(a10.extent(2) == dim[2]);
        BOOST_CHECK(a10.extent(3) == dim[3]);
        BOOST_CHECK(a10.extent(4) == dim[4]);
        BOOST_CHECK(a10.extent(5) == dim[5]);
        BOOST_CHECK(a10.extent(6) == dim[6]);
        BOOST_CHECK(a10.extent(7) == dim[7]);
        BOOST_CHECK(a10.extent(8) == dim[8]);
        BOOST_CHECK(a10.extent(9) == dim[9]);
        BOOST_CHECK(b10.extent(0) == dim[0]);
        BOOST_CHECK(b10.extent(1) == dim[1]);
        BOOST_CHECK(b10.extent(2) == dim[2]);
        BOOST_CHECK(b10.extent(3) == dim[3]);
        BOOST_CHECK(b10.extent(4) == dim[4]);
        BOOST_CHECK(b10.extent(5) == dim[5]);
        BOOST_CHECK(b10.extent(6) == dim[6]);
        BOOST_CHECK(b10.extent(7) == dim[7]);
        BOOST_CHECK(b10.extent(8) == dim[8]);
        BOOST_CHECK(b10.extent(9) == dim[9]);
        BOOST_CHECK(a11.extent( 0) == dim[ 0]);
        BOOST_CHECK(a11.extent( 1) == dim[ 1]);
        BOOST_CHECK(a11.extent( 2) == dim[ 2]);
        BOOST_CHECK(a11.extent( 3) == dim[ 3]);
        BOOST_CHECK(a11.extent( 4) == dim[ 4]);
        BOOST_CHECK(a11.extent( 5) == dim[ 5]);
        BOOST_CHECK(a11.extent( 6) == dim[ 6]);
        BOOST_CHECK(a11.extent( 7) == dim[ 7]);
        BOOST_CHECK(a11.extent( 8) == dim[ 8]);
        BOOST_CHECK(a11.extent( 9) == dim[ 9]);
        BOOST_CHECK(a11.extent(10) == dim[10]);
        BOOST_CHECK(b11.extent( 0) == dim[ 0]);
        BOOST_CHECK(b11.extent( 1) == dim[ 1]);
        BOOST_CHECK(b11.extent( 2) == dim[ 2]);
        BOOST_CHECK(b11.extent( 3) == dim[ 3]);
        BOOST_CHECK(b11.extent( 4) == dim[ 4]);
        BOOST_CHECK(b11.extent( 5) == dim[ 5]);
        BOOST_CHECK(b11.extent( 6) == dim[ 6]);
        BOOST_CHECK(b11.extent( 7) == dim[ 7]);
        BOOST_CHECK(b11.extent( 8) == dim[ 8]);
        BOOST_CHECK(b11.extent( 9) == dim[ 9]);
        BOOST_CHECK(b11.extent(10) == dim[10]);
        BOOST_CHECK(b12.extent( 0) == dim[ 0]);
        BOOST_CHECK(b12.extent( 1) == dim[ 1]);
        BOOST_CHECK(b12.extent( 2) == dim[ 2]);
        BOOST_CHECK(b12.extent( 3) == dim[ 3]);
        BOOST_CHECK(b12.extent( 4) == dim[ 4]);
        BOOST_CHECK(b12.extent( 5) == dim[ 5]);
        BOOST_CHECK(b12.extent( 6) == dim[ 6]);
        BOOST_CHECK(b12.extent( 7) == dim[ 7]);
        BOOST_CHECK(b12.extent( 8) == dim[ 8]);
        BOOST_CHECK(b12.extent( 9) == dim[ 9]);
        BOOST_CHECK(b12.extent(10) == dim[10]);
        BOOST_CHECK(b12.extent(11) == dim[11]);
    }
    delete[] buf;
}

////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> 
void testaccessors(T value1, T value2) 
{
    // Exercises the following methods of the rarray<T,{1,2,3}> class
    //   subrarray operator[](int i);
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
                BOOST_CHECK(a[i][j][k] == value1);
    
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
                BOOST_CHECK(b[i][j][k] == value2);   
            }
        }
    }
    b[6][1][0] = value1;
    BOOST_CHECK(c[6][1][0] == value1);
}

double d1 = -2.2, d2 = 7.1;
Compound c1(1,2), c2(-7,13);
array<Compound,3> a1 = {{Compound(1,2),Compound(3,4),Compound(5,6)}};
array<Compound,3> a2 = {{Compound(-1,-2),Compound(3,-4),Compound(5,-6)}};

BOOST_AUTO_TEST_CASE(testaccessorsdouble) {
   testaccessors(d1,d2);
}

BOOST_AUTO_TEST_CASE(testaccessorscompound) {
   testaccessors(c1,c2);
}

BOOST_AUTO_TEST_CASE(testaccessorsarray3compounddouble) {
   testaccessors(a1,a2);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE_TEMPLATE(testsliceconstructors, T, testconstructors_types)
{
    // Exercises the following methods of the rarray<T,3> class
    //   subrarray operator[](int i);
    //   const T* data() const;
    //   rarray(const subrarray&)
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
    BOOST_CHECK(tan==tac);
#ifndef RA_SKIPINTERMEDIATE
    BOOST_CHECK(a[1].extent(0)==21);
    BOOST_CHECK(a[1].extent(1)==13);
    BOOST_CHECK(a[1].shape()[1]==13);
    BOOST_CHECK(a[1][6].extent(0)==13);
    BOOST_CHECK(a[1][6].shape()[0]==13);
    BOOST_CHECK(a[1].size()==21*13);
    BOOST_CHECK(a[1][6].size()==13);
    T* p1 = a[3][2].data();
    T* p2 = a[3].data();
    BOOST_CHECK(p1!=0);
    BOOST_CHECK(p2!=0);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> 
void testcopy(T value1, T value2) 
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
    BOOST_CHECK(d.data()!=b.data());
    BOOST_CHECK(d.extent(0)==b.extent(0));
    BOOST_CHECK(d.extent(1)==b.extent(1));
    BOOST_CHECK(d.extent(2)==b.extent(2));
    for (int i=0; i<l; i++) {
        for (int j=0; j<m; j++) {
            for (int k=0; k<n; k++) {
                BOOST_CHECK(b[i][j][k]==d[i][j][k]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(testcopydouble) {
   testcopy(d1,d2);
}

BOOST_AUTO_TEST_CASE(testcopycompound) {
   testcopy(c1,c2);
}

BOOST_AUTO_TEST_CASE(testcopyarray3compounddouble) {
   testcopy(a1,a2);
}

////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> 
void testcopy1d(T value1, T value2) 
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
    BOOST_CHECK(d.data()!=b.data());
    BOOST_CHECK(d.extent(0)==b.extent(0));
    for (int i=0; i<n; i++) {
        BOOST_CHECK(b[i]==d[i]);
    }
}


BOOST_AUTO_TEST_CASE(testcopy1ddouble) {
   testcopy1d(d1,d2);
}

BOOST_AUTO_TEST_CASE(testcopy1dcompound) {
   testcopy1d(c1,c2);
}

BOOST_AUTO_TEST_CASE(testcopy1darray3compounddouble) {
   testcopy1d(a1,a2);
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

typedef boost::mpl::list<int,double> testmmm_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(testmmm, T, testmmm_types)
{
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
            BOOST_CHECK(a[i][j]==adata[i*3+j]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

void print1d_1(float* a, int n, std::ostream &out) 
{
    for (int i=0;i<n;i++) 
        out << a[i] << ' ';
    out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////

void print1d_2(const float* a, int n, std::ostream &out) 
{
    for (int i=0;i<n;i++) 
        out << a[i] << ' ';
    out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////

void print1d_3(const rarray<float,1> &a, std::ostream &out) 
{
    for (int i=0;i<a.extent(0);i++) 
        out << a[i] << ' ';
    out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////

void print1d_4(const rarray<const float,1>& a, std::ostream &out)
{
    for (int i=0;i<a.extent(0);i++) 
        out << a[i] << ' ';
    out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test1dconversions)
{
    const int n=9;
    rarray<float,1> a(n);
    for (int i=0;i<n;i++)
        a[i] = i+1;
    const rarray<float,1>& c=a;
    std::stringstream s1,s2,s3,s4,s5,s6,s7;
    //print1d_1(c.ptr_array(), c.extent(0), std::cout);
    print1d_1(c.ptr_array(), c.extent(0), s1);
    BOOST_CHECK(s1.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.noconst_ptr_array(), c.extent(0), s2);
    BOOST_CHECK(s2.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_1(a.data(), c.extent(0), s3);
    BOOST_CHECK(s3.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.data(), c.extent(0), s4);
    BOOST_CHECK(s4.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_3(c, s5);
    BOOST_CHECK(s5.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(a.const_ref(), s6);
    BOOST_CHECK(s6.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(c.const_ref(), s7);
    BOOST_CHECK(s7.str()=="1 2 3 4 5 6 7 8 9 \n");
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test2dconversions)
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
    BOOST_CHECK(s1.str()==
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
    BOOST_CHECK(s2.str()==s1.str());
    print2d_3(c.ptr_array(), c.extent(0), c.extent(1), s3);
    BOOST_CHECK(s3.str()==s1.str());
    print2d_4(c.ptr_array(), c.extent(0), c.extent(1), s4);
    BOOST_CHECK(s4.str()==s1.str());
    print2d_5(a.data(), c.extent(0), c.extent(1), s5);
    BOOST_CHECK(s5.str()==s1.str());
    print2d_6(c.data(), c.extent(0), c.extent(1), s6);
    BOOST_CHECK(s6.str()==s1.str());
    print2d_7(c, s7);
    BOOST_CHECK(s7.str()==s1.str());
    print2d_8(c.const_ref(), s8);
    BOOST_CHECK(s8.str()==s1.str());
}


////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test3dconversions)
{
    const int n = 9;
    const int m = 5;
    const int l = 2;
    rarray<float,3> a(n,m,l);
    std::stringstream s1,s2,s3,s4,s5,s6,s7;
    // fill a
    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        for (int k=0;k<l;k++)
          a[i][j][k]=((i+1)*10+j+1)*10+k+1;
    const rarray<float,3>& c=a; // note the const: not enough
    // const rarray<float,3>* pa = &a;
    // print3d_1(c, c.extent(0), c.extent(1), c.extent(2)); //won't work, one needs:
    print3d_1(c.noconst_ptr_array(), c.extent(0), c.extent(1), c.extent(2), s1); 
    BOOST_CHECK(s1.str()==
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
    BOOST_CHECK(s2.str()==s1.str());    
    print3d_3(c.ptr_array(), c.extent(0), c.extent(1), c.extent(2), s3); 
    BOOST_CHECK(s3.str()==s1.str());
    print3d_4(c.ptr_array(), c.extent(0), c.extent(1), c.extent(2), s4);
    BOOST_CHECK(s4.str()==s1.str());
 // print3d_5(c, c.extent(0), c.extent(1), c.extent(2)); won't work, one needs
    print3d_5(a.data(), c.extent(0), c.extent(1), c.extent(2), s5);
    BOOST_CHECK(s5.str()==s1.str());
    print3d_6(c.data(), c.extent(0), c.extent(1), c.extent(2), s6);
    BOOST_CHECK(s6.str()==s1.str());
    print3d_7(c, s7);
    BOOST_CHECK(s7.str()==s1.str());
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testassignment)
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
    BOOST_CHECK(b.data()==a.data());
    //BOOST_CHECK(b.ptr_array()==a.ptr_array()); // not yet, b has its own ptr_array.
    BOOST_CHECK(b.extent(0)==a.extent(0));
    BOOST_CHECK(b.extent(1)==a.extent(1));
    BOOST_CHECK(b.extent(2)==a.extent(2));
#ifndef RA_SKIPINTERMEDIATE
    rarray<float,2> e;
    e = a[2];
    BOOST_CHECK(e.data()==a[2].data());
    BOOST_CHECK(e.extent(0)==a.extent(1));
    BOOST_CHECK(e.extent(1)==a.extent(2));    
#endif
    rarray<float,1> c(2048);
    rarray<float,1> d;
    d = c;
    BOOST_CHECK(d.data()==c.data());
    BOOST_CHECK(d.extent(0)==c.extent(0));
}

////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RA_SKIPINTERMEDIATE
void testconstintermediatefunction(const rarray<float,3>& a, const float* data1check)
{
    const float* a1=a[1].data();
    BOOST_CHECK(a1==data1check); 
    BOOST_CHECK(a[1].ptr_array());
    BOOST_CHECK(a[1].noconst_ptr_array());
    BOOST_CHECK(a[1].const_ref().ptr_array());
    BOOST_CHECK(a[1][2].ptr_array());
    BOOST_CHECK(a[1][2].noconst_ptr_array());
    BOOST_CHECK(a[1][2].const_ref().ptr_array());
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testconstintermediate)
{
#ifdef RA_SKIPINTERMEDIATE
    BOOST_CHECK(true);
#else
    rarray<float,3> a(7,8,9);
    int l=0;
    for (int i=0;i<7;i++)
        for (int j=0;j<8;j++)
            for (int k=0;k<9;k++)
                a[i][j][k] = float(l++);
    testconstintermediatefunction(a,a[1].data());
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

void fill_1d_rarray(rarray<float,1> a, float value)
{
    for (int i=0; i < a.size(); i++)
        a[i] = value;
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testintermediateconversion)
{
    rarray<float,2> a(10,10);
    a[2][7]=14;
#ifndef RA_SKIPINTERMEDIATE
    fill_1d_rarray(a[2], 13);
    BOOST_CHECK(a[2][7]==13);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testreshape)
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
    BOOST_CHECK(novela.extent(0)==4);
    BOOST_CHECK(novela[3]==4);
    BOOST_CHECK(novela2.extent(0)==7);
    novela2.reshape(4);
    BOOST_CHECK(novela2.extent(0)==4);
    BOOST_CHECK(novela2[3]==4);
    b[5][6] = 5;
    rarray<float,2> novelb(b);
    rarray<float,2> novelb2(novelb);
    novelb.reshape(10,7);
    b.reshape(dim);
    BOOST_CHECK(novelb.extent(0)==10);
    BOOST_CHECK(novelb.extent(1)==7);
    BOOST_CHECK(novelb2.extent(0)==7);
    BOOST_CHECK(novelb2.extent(1)==10);
    BOOST_CHECK(novelb[8][0] == 5);
    c[4][8][3] = 6;
    rarray<float,3> novelc(c);
    novelc.reshape(10,7,13);
    BOOST_CHECK(novelc.extent(0)==10);
    BOOST_CHECK(novelc.extent(1)==7);
    BOOST_CHECK(novelc.extent(2)==13);
    BOOST_CHECK(novelc[6][6][3] == 6);
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
    novelg.reshape(2,5,6,1,13,10,7);        // TODO: check
    novelh.reshape(4,3,2,3,4,3,2,3);        // TODO: check
    noveli.reshape(4,3,2,3,4,3,2,3,2);      // TODO: check
    novelj.reshape(4,3,2,3,4,3,2,3,2,3);    // TODO: check
    novelk.reshape(4,3,2,3,4,3,2,3,2,3,4);  // TODO: check
    novell.reshape(dimr12);                 // TODO: check
}

////////////////////////////////////////////////////////////////////////////////////////////

std::string print1d(const rarray<float,1> &a) 
{
    std::stringstream s;
    const int n = EXTENT(a,0);
    for (int i=0;i<n-1;i++)
        s << a[i] << ' ';
    s << a[n-1];
    return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test1dautoconversion) 
{
    const int n=9;
    float b[n] = {1,2,3,4,5,6,7,8,9};
    const rarray<float,1> a = RARRAY(b);
    BOOST_CHECK(EXTENT(a,0)==EXTENT(b,0));
    std::string s = print1d(RARRAY(b));
    BOOST_CHECK(s=="1 2 3 4 5 6 7 8 9");
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test2dautoconversion) 
{
    const int n=2;
    const int m=7;
    float b[n][m] = {{1,2,3,4,5,6,7},
                     {8,9,8,7,6,5,4}};
    const rarray<float,2> a = RARRAY(b);
    BOOST_CHECK(EXTENT(a,0)==EXTENT(b,0));
    BOOST_CHECK(EXTENT(a,1)==EXTENT(b,1));
    std::string s = print2d(RARRAY(b));
    BOOST_CHECK(s=="1 2 3 4 5 6 7\n8 9 8 7 6 5 4\n");
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test3dautoconversion) 
{
    const int n=2;
    const int m=7;
    const int l=3;
    float b[n][m][l] = {{{1,2,3},{2,3,4},{3,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
                        {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,3},{4,3,2}}};
    const rarray<float,3> a = RARRAY(b);
    BOOST_CHECK(EXTENT(a,0)==EXTENT(b,0));
    BOOST_CHECK(EXTENT(a,1)==EXTENT(b,1));
    BOOST_CHECK(EXTENT(a,2)==EXTENT(b,2));
    std::string s = print3d(RARRAY(b));
    BOOST_CHECK(s=="{1,2,3}{2,3,4}{3,4,5}{4,5,6}{5,6,7}{6,7,8}{7,8,9}\n"
             "{8,7,6}{9,8,7}{8,7,6}{7,6,5}{6,5,4}{5,4,3}{4,3,2}\n");
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test4dautoconversion)
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
    BOOST_CHECK(EXTENT(a,0)==EXTENT(b,0));
    BOOST_CHECK(EXTENT(a,1)==EXTENT(b,1));
    BOOST_CHECK(EXTENT(a,2)==EXTENT(b,2));
    BOOST_CHECK(EXTENT(a,3)==EXTENT(b,3));
    std::string s = print4d(RARRAY(b));
    BOOST_CHECK(s==
          "{[1 2 3][2 3 4][3 4 5][4 5 6][5 6 7][6 7 8][7 8 9]}{[8 7 6][9 8 7][8 7 6][7 6 5][6 5 4][5 4 3][4 3 2]}\n"
          "{[1 2 3][2 3 6][3 6 5][6 5 9][5 9 7][9 7 8][7 8 9]}{[8 7 9][9 8 7][8 7 9][7 9 5][9 5 6][5 6 3][6 3 2]}\n");
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test5dautoconversion) 
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
    BOOST_CHECK(EXTENT(a,0)==EXTENT(b,0));
    BOOST_CHECK(EXTENT(a,1)==EXTENT(b,1));
    BOOST_CHECK(EXTENT(a,2)==EXTENT(b,2));
    BOOST_CHECK(EXTENT(a,3)==EXTENT(b,3));
    BOOST_CHECK(EXTENT(a,4)==EXTENT(b,4));
    std::string s = print5d(RARRAY(b));
    BOOST_CHECK(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
          "{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n");
}

////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test6dautoconversion) 
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
    BOOST_CHECK(EXTENT(a,0)==EXTENT(b,0));
    BOOST_CHECK(EXTENT(a,1)==EXTENT(b,1));
    BOOST_CHECK(EXTENT(a,2)==EXTENT(b,2));
    BOOST_CHECK(EXTENT(a,3)==EXTENT(b,3));
    BOOST_CHECK(EXTENT(a,4)==EXTENT(b,4));
    BOOST_CHECK(EXTENT(a,5)==EXTENT(b,5));
    std::string s = print6d(RARRAY(b));
    BOOST_CHECK(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
"{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n\n"
"{[(1,-2,-3)(2,-3,-4)(3,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-3)(4,-3,-2)]}{[(1,-2,-3)(2,-3,-6)(3,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-3)(6,-3,-2)]}\n"
"{[(1,-2,-7)(2,-7,-4)(7,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-7)(4,-7,-2)]}{[(1,-2,-7)(2,-7,-6)(7,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-7)(6,-7,-2)]}\n\n");
    s = print6d(RARRAY(c));
    BOOST_CHECK(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
"{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n\n"
"{[(1,-2,-3)(2,-3,-4)(3,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-3)(4,-3,-2)]}{[(1,-2,-3)(2,-3,-6)(3,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-3)(6,-3,-2)]}\n"
"{[(1,-2,-7)(2,-7,-4)(7,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-7)(4,-7,-2)]}{[(1,-2,-7)(2,-7,-6)(7,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-7)(6,-7,-2)]}\n\n");
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test7dautoconversion) 
{
    int seven[2][2][2][2][2][2][2] = {{{{{{{0}}}}}}}; 
    char expected_output[] =
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}";

    std::stringstream s;
    s << RARRAY(seven);
    BOOST_CHECK(s.str()==expected_output);
    BOOST_CHECK(EXTENT(seven,0) == 2);
    BOOST_CHECK(EXTENT(seven,1) == 2);
    BOOST_CHECK(EXTENT(seven,2) == 2);
    BOOST_CHECK(EXTENT(seven,3) == 2);
    BOOST_CHECK(EXTENT(seven,4) == 2);
    BOOST_CHECK(EXTENT(seven,5) == 2);
    BOOST_CHECK(EXTENT(seven,6) == 2);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test8dautoconversion) 
{
    int eight[2][2][2][2][2][2][2][2] = {{{{{{{{0}}}}}}}}; 
    char expected_output[] =
          "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
            "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}";
    std::stringstream s;
    s << RARRAY(eight);
    BOOST_CHECK(s.str()==expected_output);
    BOOST_CHECK(EXTENT(eight,0) == 2);
    BOOST_CHECK(EXTENT(eight,1) == 2);
    BOOST_CHECK(EXTENT(eight,2) == 2);
    BOOST_CHECK(EXTENT(eight,3) == 2);
    BOOST_CHECK(EXTENT(eight,4) == 2);
    BOOST_CHECK(EXTENT(eight,5) == 2);
    BOOST_CHECK(EXTENT(eight,6) == 2);
    BOOST_CHECK(EXTENT(eight,7) == 2);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test9dautoconversion) 
{
    int nine[2][2][2][2][2][2][2][2][2] = {{{{{{{{{0}}}}}}}}}; 
    char expected_output[] =
         "{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
          "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}";
    std::stringstream s;
    s << RARRAY(nine);
    BOOST_CHECK(s.str()==expected_output);
    BOOST_CHECK(EXTENT(nine,0) == 2);
    BOOST_CHECK(EXTENT(nine,1) == 2);
    BOOST_CHECK(EXTENT(nine,2) == 2);
    BOOST_CHECK(EXTENT(nine,3) == 2);
    BOOST_CHECK(EXTENT(nine,4) == 2);
    BOOST_CHECK(EXTENT(nine,5) == 2);
    BOOST_CHECK(EXTENT(nine,6) == 2);
    BOOST_CHECK(EXTENT(nine,7) == 2);
    BOOST_CHECK(EXTENT(nine,8) == 2);    
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test10dautoconversion)
{
    int ten[2][2][2][2][2][2][2][2][2][2] = {{{{{{{{{{0}}}}}}}}}}; 
    char expected_output[] =
        "{{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
          "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}},"
         "{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
          "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
           "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
            "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
             "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}}";
    std::stringstream s;
    s << RARRAY(ten);
    BOOST_CHECK(s.str()==expected_output);
    BOOST_CHECK(EXTENT(ten,0) == 2);
    BOOST_CHECK(EXTENT(ten,1) == 2);
    BOOST_CHECK(EXTENT(ten,2) == 2);
    BOOST_CHECK(EXTENT(ten,3) == 2);
    BOOST_CHECK(EXTENT(ten,4) == 2);
    BOOST_CHECK(EXTENT(ten,5) == 2);
    BOOST_CHECK(EXTENT(ten,6) == 2);
    BOOST_CHECK(EXTENT(ten,7) == 2);
    BOOST_CHECK(EXTENT(ten,8) == 2);
    BOOST_CHECK(EXTENT(ten,9) == 2);   
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test11dautoconversion)
{
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
        "{{{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
           "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}},"
          "{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
           "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}},"
         "{{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
           "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}},"
          "{{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}"
           ",{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}},"
           "{{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}},"
            "{{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}},"
             "{{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}},"
              "{{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}},{{{{0,0},{0,0}},{{0,0},{0,0}}},{{{0,0},{0,0}},{{0,0},{0,0}}}}}}}}}}}";
    std::stringstream s;
    s << RARRAY(eleven);
    BOOST_CHECK(s.str()==expected_output);
    BOOST_CHECK(EXTENT(eleven,0) == 2);
    BOOST_CHECK(EXTENT(eleven,1) == 2);
    BOOST_CHECK(EXTENT(eleven,2) == 2);
    BOOST_CHECK(EXTENT(eleven,3) == 2);
    BOOST_CHECK(EXTENT(eleven,4) == 2);
    BOOST_CHECK(EXTENT(eleven,5) == 2);
    BOOST_CHECK(EXTENT(eleven,6) == 2);
    BOOST_CHECK(EXTENT(eleven,7) == 2);
    BOOST_CHECK(EXTENT(eleven,8) == 2);
    BOOST_CHECK(EXTENT(eleven,9) == 2);
    BOOST_CHECK(EXTENT(eleven,10) == 2);   
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testoutput) 
{
    double a[5]={1,2,3,4,5};
    double b[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    double c[27]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
    rarray<double,1> q(a,5);
    rarray<double,2> r(b,4,4);
    rarray<double,3> s(c,3,3,3);
    std::stringstream out;
    out << q << r << s;
    BOOST_CHECK(out.str() == "{1,2,3,4,5}{{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}}{{{1,2,3},{4,5,6},{7,8,9}},{{10,11,12},{13,14,15},{16,17,18}},{{19,20,21},{22,23,24},{25,26,27}}}");

    std::stringstream instr("  \t\n{{{#2:14,5},{2,#3:{}2},{#7:{1,2,3},1}},{{4},{5,5},{6,6}},{{7,7},{8,8},{9,9}}}");
    std::string outstr("{{{14,5},{2,0},{0,1}},{{4,0},{5,5},{6,6}},{{7,7},{8,8},{9,9}}}");   
    rarray<int,3> intarray;
    instr >> intarray;
    intarray[1][0][1] = 0;
    intarray[0][2][0] = 0;
    std::stringstream check;
    check << intarray;
    BOOST_CHECK(check.str()==outstr);
    rarray<std::string,2> A(2,2);
    A[0][0] = "Hello, world";
    A[0][1] = "I like { and }";
    A[1][0] = "I prefer #";
    A[1][1] = "I'm easy.";
    rarray<std::string,2> B;
    std::stringstream sin("{{#12:Hello, world,#14:I like { and }},{#10:I prefer #,I'm easy.}}");
    sin >> B;
    BOOST_CHECK(A[0][0] == B[0][0]);
    BOOST_CHECK(A[0][1] == B[0][1]);
    BOOST_CHECK(A[1][0] == B[1][0]);
    BOOST_CHECK(A[1][1] == B[1][1]);
    float autoarr[2][2][2][2] = {{{{1,2},{3,4}},{{5,6},{7,8}}},{{{9,10},{11,12}},{{13,14},{15,16}}}};
    const std::string outcheck = "{{{{1,2},{3,4}},{{5,6},{7,8}}},{{{9,10},{11,12}},{{13,14},{15,16}}}}";
    std::stringstream sautoarr;
    sautoarr << RARRAY(autoarr);
    BOOST_CHECK(sautoarr.str()==outcheck);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testiterators) 
{
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
    BOOST_CHECK(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,");
    const rarray<double,2> rview = r;
    for (rarray<double,2>::const_iterator i=rview.begin(); i!=rview.end(); i++)
    {
        qout << *i << ',';
    }
    BOOST_CHECK(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,");
    std::stringstream rout;
    #if __cplusplus > 199711L
    #if __cplusplus != 201103L
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
    BOOST_CHECK(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,2,4,6,8,10,");
    #endif
    const rarray<double,1> qconst = q;
    #if __cplusplus > 199711L
    #if __cplusplus != 201103L
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
    BOOST_CHECK(rout.str() == "2,4,6,8,10,");
    #endif
    #ifndef RA_SKIPINTERMEDIATE
    std::stringstream check;
    #if __cplusplus > 199711L
    #if __cplusplus != 201103L
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
    BOOST_CHECK(check.str() == "20,22,24,26,28,30,42,44,46,19,20,21,22,23,24,25,26,27,");
    #endif
    #endif
    #if __cplusplus > 199711L
    auto sb = s.begin();
    auto se = s.end();
    BOOST_CHECK(not (sb==se));
    BOOST_CHECK(sb < se);
    BOOST_CHECK(sb <= se);
    BOOST_CHECK(se > sb);
    BOOST_CHECK(se >= sb);
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(testfill)
{
    rarray<float,2> a(3,3);
    a.fill(1.23f);
    for (int i=0;i<EXTENT(a,0);i++) {
        for (int j=0;j<EXTENT(a,1);j++) {
            BOOST_CHECK(a[i][j]==1.23f);
        }
    }
    rarray<float,1> b(5);
    b.fill(1.24f);
    for (int i=0;i<EXTENT(a,0);i++) {
        BOOST_CHECK(b[i]==1.24f);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_index)
{
    rarray<float,1> a(6);
    int ind;
    BOOST_CHECK(*a.index(a[0],&ind)==0);
    BOOST_CHECK(*a.index(a[1],&ind)==1);
    BOOST_CHECK(*a.index(a[2],&ind)==2);
    BOOST_CHECK(*a.index(a[3],&ind)==3);
    BOOST_CHECK(*a.index(a[4],&ind)==4);
    BOOST_CHECK(*a.index(a[5],&ind)==5);
    BOOST_CHECK(a.index(a[0],0)==0);
    BOOST_CHECK(a.index(a[1],0)==1);
    BOOST_CHECK(a.index(a[2],0)==2);
    BOOST_CHECK(a.index(a[3],0)==3);
    BOOST_CHECK(a.index(a[4],0)==4);
    BOOST_CHECK(a.index(a[5],0)==5);
    BOOST_CHECK(INDEX(a,a[0],0)==0);
    BOOST_CHECK(INDEX(a,a[1],0)==1);
    BOOST_CHECK(INDEX(a,a[2],0)==2);
    BOOST_CHECK(INDEX(a,a[3],0)==3);
    BOOST_CHECK(INDEX(a,a[4],0)==4);
    BOOST_CHECK(INDEX(a,a[5],0)==5);
    for (rarray<float,1>::iterator i=a.begin(); i != a.end(); i++) {
        a.index(i,&ind);
        int ind2=a.index(i,0);
        BOOST_CHECK(ind==ind2);
        *i = ind+1;
    }
#if __cplusplus <= 199711L
    for (rarray<float,1>::iterator  element = a.begin(); element != a.end(); ++element)
        *element *= a.index(*element,&ind)[0];
    for (rarray<float,1>::iterator  element = a.begin(); element != a.end(); ++element)
        *element *= a.index(*element,0);
#else
    for (auto& element: a)
        element *= a.index(element,&ind)[0];
    for (auto& element: a)
        element *= a.index(element,0);
#endif
    BOOST_CHECK(a[0]==0);
    BOOST_CHECK(a[1]==2);
    BOOST_CHECK(a[2]==12);
    BOOST_CHECK(a[3]==36);
    BOOST_CHECK(a[4]==80);
    BOOST_CHECK(a[5]==150);
    rarray<float,3> z(2,3,4);
    BOOST_CHECK(z.index(z[1][2][3],0)==1);
    BOOST_CHECK(z.index(z[1][2][3],1)==2);
    BOOST_CHECK(z.index(z[1][2][3],2)==3);
    rarray<float,3> b(2,2,2);
    int i[3];
    BOOST_CHECK(b.index(b[0][0][0],i)==i);
    BOOST_CHECK(b.index(b[0][0][0],i)[0]==0);
    BOOST_CHECK(b.index(b[0][0][1],i)[0]==0);
    BOOST_CHECK(b.index(b[0][1][0],i)[0]==0);
    BOOST_CHECK(b.index(b[0][1][1],i)[0]==0);
    BOOST_CHECK(b.index(b[1][0][0],i)[0]==1);
    BOOST_CHECK(b.index(b[1][0][1],i)[0]==1);
    BOOST_CHECK(b.index(b[1][1][0],i)[0]==1);
    BOOST_CHECK(b.index(b[1][1][1],i)[0]==1);
    BOOST_CHECK(b.index(b[0][0][0],i)[1]==0);
    BOOST_CHECK(b.index(b[0][0][1],i)[1]==0);
    BOOST_CHECK(b.index(b[0][1][0],i)[1]==1);
    BOOST_CHECK(b.index(b[0][1][1],i)[1]==1);
    BOOST_CHECK(b.index(b[1][0][0],i)[1]==0);
    BOOST_CHECK(b.index(b[1][0][1],i)[1]==0);
    BOOST_CHECK(b.index(b[1][1][0],i)[1]==1);
    BOOST_CHECK(b.index(b[1][1][1],i)[1]==1);
    BOOST_CHECK(b.index(b[0][0][0],i)[2]==0);
    BOOST_CHECK(b.index(b[0][0][1],i)[2]==1);
    BOOST_CHECK(b.index(b[0][1][0],i)[2]==0);
    BOOST_CHECK(b.index(b[0][1][1],i)[2]==1);
    BOOST_CHECK(b.index(b[1][0][0],i)[2]==0);
    BOOST_CHECK(b.index(b[1][0][1],i)[2]==1);
    BOOST_CHECK(b.index(b[1][1][0],i)[2]==0);
    BOOST_CHECK(b.index(b[1][1][1],i)[2]==1);
    float rbuf[3][3] = { {0,0,0}, 
                         {1,1,1}, 
                         {2,2,2} }; 
    float cbuf[3][3] = { {0,1,2}, 
                         {0,1,2},
                         {0,1,2} }; 
    rarray<float,2> r = RARRAY(rbuf);
    rarray<float,2> c = RARRAY(cbuf);
    for (rarray<float,2>::iterator i=r.begin(); i != r.end(); i++) {
        int ind[2];
        r.index(*i,ind);
        BOOST_CHECK(ind[0]==*i);
    }    
    for (rarray<float,2>::iterator i=c.begin(); i != c.end(); i++) {
        int ind[2];
        c.index(i,ind);
        BOOST_CHECK(ind[1]==*i);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_comma_assignment)
{
    rarray<double,1> b(8);
    b.fill(0);
    b = 1,2,3,6,5,4;
    BOOST_CHECK(b[0]==1);
    BOOST_CHECK(b[1]==2);
    BOOST_CHECK(b[2]==3);
    BOOST_CHECK(b[3]==6);
    BOOST_CHECK(b[4]==5);
    BOOST_CHECK(b[5]==4);
    BOOST_CHECK(b[6]==0);
    BOOST_CHECK(b[7]==0);
    rarray<double,3> a(3,4,2);
    a =  1,2,    3,6,   5,4,   7,8, 
         9,12,  11,10, 21,22, 23,26, 
        25,24,  27,28, 29,32, 31,30;
    BOOST_CHECK(a[0][0][0]== 1);
    BOOST_CHECK(a[0][0][1]== 2);
    BOOST_CHECK(a[0][1][0]== 3);
    BOOST_CHECK(a[0][1][1]== 6);
    BOOST_CHECK(a[0][2][0]== 5);
    BOOST_CHECK(a[0][2][1]== 4);
    BOOST_CHECK(a[0][3][0]== 7);
    BOOST_CHECK(a[0][3][1]== 8);
    BOOST_CHECK(a[1][0][0]== 9);
    BOOST_CHECK(a[1][0][1]==12);
    BOOST_CHECK(a[1][1][0]==11);
    BOOST_CHECK(a[1][1][1]==10);
    BOOST_CHECK(a[1][2][0]==21);
    BOOST_CHECK(a[1][2][1]==22);
    BOOST_CHECK(a[1][3][0]==23);
    BOOST_CHECK(a[1][3][1]==26);
    BOOST_CHECK(a[2][0][0]==25);
    BOOST_CHECK(a[2][0][1]==24);
    BOOST_CHECK(a[2][1][0]==27);
    BOOST_CHECK(a[2][1][1]==28);
    BOOST_CHECK(a[2][2][0]==29);
    BOOST_CHECK(a[2][2][1]==32);
    BOOST_CHECK(a[2][3][0]==31);
    BOOST_CHECK(a[2][3][1]==30);
    #ifndef RA_SKIPINTERMEDIATE
    a[1]       = 100,101,102,103,104,105,106,107;
    a[2][1]    = 200,201;
    a[2][2][0] = 300,301;   // on purpose using built-in comma operator which forgets the 301
    a[2][3][0] = (300,301); // on purpose using built-in comma operator which forgets the 300
    BOOST_CHECK(a[0][0][0]== 1);
    BOOST_CHECK(a[0][0][1]== 2);
    BOOST_CHECK(a[0][1][0]== 3);
    BOOST_CHECK(a[0][1][1]== 6);
    BOOST_CHECK(a[0][2][0]== 5);
    BOOST_CHECK(a[0][2][1]== 4);
    BOOST_CHECK(a[0][3][0]== 7);
    BOOST_CHECK(a[0][3][1]== 8);
    BOOST_CHECK(a[1][0][0]==100);
    BOOST_CHECK(a[1][0][1]==101);
    BOOST_CHECK(a[1][1][0]==102);
    BOOST_CHECK(a[1][1][1]==103);
    BOOST_CHECK(a[1][2][0]==104);
    BOOST_CHECK(a[1][2][1]==105);
    BOOST_CHECK(a[1][3][0]==106);
    BOOST_CHECK(a[1][3][1]==107);
    BOOST_CHECK(a[2][0][0]==25);
    BOOST_CHECK(a[2][0][1]==24);
    BOOST_CHECK(a[2][1][0]==200);
    BOOST_CHECK(a[2][1][1]==201);
    BOOST_CHECK(a[2][2][0]==300);
    BOOST_CHECK(a[2][2][1]==32);
    BOOST_CHECK(a[2][3][0]==301);
    BOOST_CHECK(a[2][3][1]==30);
    #endif
}

////////////////////////////////////////////////////////////////////////////////////////////

rarray<double,2> f(double a){
    rarray<double,2> z(2,3);
    z.fill(a);
    return z;
}

BOOST_AUTO_TEST_CASE(test_function_returning_rarray)
{
    rarray<double,2> b;
    b = f(4.0);
    BOOST_CHECK(b[0][0]==4.0);
    BOOST_CHECK(b[1][1]==4.0);
    BOOST_CHECK(b[0][2]==4.0);
    BOOST_CHECK(b[1][0]==4.0);
    BOOST_CHECK(b[0][1]==4.0);
    BOOST_CHECK(b[1][2]==4.0);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_reference_counting)
{
    rarray<double,2>* b;
    b = new rarray<double,2>(10,30);
    b->fill(13.0);
    rarray<double,2> c(*b);
    delete b;
    BOOST_CHECK(c.extent(0)==10);
    BOOST_CHECK(c.extent(1)==30);
    BOOST_CHECK(c[0][0]==13.0);
    BOOST_CHECK(c[9][29]==13.0);
    b = new rarray<double,2>(5,6);
    b->fill(5.0);
    rarray<double,1> d((*b)[3]);
    delete b;
    // BOOST_CHECK(d.extent(0)==6);
    // BOOST_CHECK(d[5]==5.0);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
