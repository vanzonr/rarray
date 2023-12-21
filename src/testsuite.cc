//
// testsuite.cc - testsuite for rarray
//
// Copyright (c) 2013-2023  Ramses van Zon
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

#include <rarray>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <string>
#include <sstream>
#include <complex>
#include <algorithm>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

//////////////////////////////////////////////////////////////////////
// AUXILIARY STUFF
//////////////////////////////////////////////////////////////////////

template<typename T,ra::rank_type R> 
const T* getconstdata(const rarray<T,R>& a)
{
    return a.data();
}
//////////////////////////////////////////////////////////////////////
class Compound 
{
  public:
    Compound(): x(0), y(0) {}
    Compound(int anx, int any): x(anx), y(any) {}
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
    friend std::ostream& operator<<(std::ostream &o, const Compound& x);
};
std::ostream& operator<<(std::ostream &os, const Compound& x)
{
    os << '{' << x.x << ',' << x.y << '}';
    return os;
}
//////////////////////////////////////////////////////////////////////
template<typename T, ra::rank_type R>
struct array {
    T elements_[R];
    T& operator[](const ra::rank_type i) { return elements_[i]; }
    const T& operator[](const ra::rank_type i) const { return elements_[i]; }
    bool operator!=(const struct array& other) const {
        for (ra::rank_type i=0;i<R;i++)
            if (elements_[i] != other.elements_[i])
                return true;
        return false;
    }
    bool operator==(const struct array& other) const {
        return !operator!=(other);
    }       
};
template<typename T, ra::rank_type R>
std::ostream& operator<<(std::ostream &os, const array<T,R>& x)
{    
    os << '{';
    if (R>0) os << x[0];
    for (ra::rank_type r=1;r<R;r++)
        os << ',' << x[r];
    os << '}';
    return os;
}
//////////////////////////////////////////////////////////////////////
array<Compound,3> operator+(const array<Compound,3> &a,
                            const array<Compound,3> &b)
{
    array<Compound,3> result = {{a[0]+b[0],a[1]+b[1],a[2]+b[2]}};
    return result;
}

//////////////////////////////////////////////////////////////////////
//                    T H E   T E S T   S U I T E                   //
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("test constructors", "",
                   double, Compound, (array<Compound,3>), (std::complex<float>))
{
    using T = TestType;
    // Exercises the following constructors of the rarray<T,3> class
    //   rarray(int,int,int)
    //   rarray(const int*)
    //   rarray(const rarray<T,3>&)
    // as well as the destructor
    //   ~rarray()
    // And the following methods
    //   T* data()
    //   int extent(int)
    ra::size_type dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    const ra::size_type* asize = a.shape();
    REQUIRE(a.data());
    REQUIRE(a.size()==7*21*13);
    REQUIRE(asize);
    REQUIRE(asize[0] == dim[0]);
    REQUIRE(asize[1] == dim[1]);
    REQUIRE(asize[2] == dim[2]);
    REQUIRE_THROWS(a.extent(-1));
    REQUIRE_THROWS(a.extent(3));
    REQUIRE(a.extent(0) == dim[0]);
    REQUIRE(a.extent(1) == dim[1]);
    REQUIRE(a.extent(2) == dim[2]);
    REQUIRE(b.data());
    REQUIRE(b.size()==7*21*13);
    REQUIRE(b.extent(0) == dim[0]);
    REQUIRE(b.extent(1) == dim[1]);
    REQUIRE(b.extent(2) == dim[2]);
    REQUIRE(c.data());
    REQUIRE(c.size()==7*21*13);
    REQUIRE(c.extent(0) == dim[0]);
    REQUIRE(c.extent(1) == dim[1]);
    REQUIRE(c.extent(2) == dim[2]);
    REQUIRE(c.data()==b.data());
    b.clear();
    REQUIRE(b.empty());
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("test constructors 7dim test", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    ra::size_type dim[5] = {7,10,13,2,4};
    rarray<T,5> a5(7,10,13,2,4);
    rarray<T,5> b5(dim);
    rarray<T,5> c5(b5);
    // Never finished, it seems.
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("test constructors 7dim test 2", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    ra::size_type dim[7] = {7,10,13,2,4,5,21};
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

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("test constructors 7dim", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    ra::size_type dim[7] = {7,10,13,2,4,5,21};
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

    REQUIRE(a1.data());
    REQUIRE(b1.data());
    REQUIRE(c1.data()==b1.data());
    REQUIRE(a2.data());
    REQUIRE(b2.data());
    REQUIRE(c2.data()==b2.data());
    REQUIRE(a3.data());
    REQUIRE(b3.data());
    REQUIRE(c3.data()==b3.data());
    REQUIRE(a4.data());
    REQUIRE(b4.data());
    REQUIRE(c4.data()==b4.data());
    REQUIRE(a5.data());
    REQUIRE(b5.data());
    REQUIRE(c5.data()==b5.data());
    REQUIRE(a6.data());
    REQUIRE(b6.data());
    REQUIRE(c6.data()==b6.data());
    REQUIRE(b7.data());
    REQUIRE(c7.data()==b7.data());

    REQUIRE(b1.size()==7);
    REQUIRE(b2.size()==7*10);
    REQUIRE(b3.size()==7*10*13);
    REQUIRE(b4.size()==7*10*13*2);
    REQUIRE(b5.size()==7*10*13*2*4);
    REQUIRE(b6.size()==7*10*13*2*4*5);
    REQUIRE(b7.size()==7*10*13*2*4*5*21);

    REQUIRE(a1.extent(0) == dim[0]);
    REQUIRE(b1.extent(0) == dim[0]);
    REQUIRE(c1.extent(0) == dim[0]);
    REQUIRE(a1.shape());
    REQUIRE(a1.shape()[0] == dim[0]);

    REQUIRE(a2.extent(0) == dim[0]);
    REQUIRE(a2.extent(1) == dim[1]);
    REQUIRE(b2.extent(0) == dim[0]);
    REQUIRE(b2.extent(1) == dim[1]);
    REQUIRE(c2.extent(0) == dim[0]);
    REQUIRE(c2.extent(1) == dim[1]);

    REQUIRE(a3.extent(0) == dim[0]);
    REQUIRE(a3.extent(1) == dim[1]);
    REQUIRE(a3.extent(2) == dim[2]);
    REQUIRE(b3.extent(0) == dim[0]);
    REQUIRE(b3.extent(1) == dim[1]);
    REQUIRE(b3.extent(2) == dim[2]);
    REQUIRE(c3.extent(0) == dim[0]);
    REQUIRE(c3.extent(1) == dim[1]);
    REQUIRE(c3.extent(2) == dim[2]);

    REQUIRE(a4.extent(0) == dim[0]);
    REQUIRE(a4.extent(1) == dim[1]);
    REQUIRE(a4.extent(2) == dim[2]);
    REQUIRE(a4.extent(3) == dim[3]);
    REQUIRE(b4.extent(0) == dim[0]);
    REQUIRE(b4.extent(1) == dim[1]);
    REQUIRE(b4.extent(2) == dim[2]);
    REQUIRE(b4.extent(3) == dim[3]);
    REQUIRE(c4.extent(0) == dim[0]);
    REQUIRE(c4.extent(1) == dim[1]);
    REQUIRE(c4.extent(2) == dim[2]);
    REQUIRE(c4.extent(3) == dim[3]);

    REQUIRE(a5.extent(0) == dim[0]);
    REQUIRE(a5.extent(1) == dim[1]);
    REQUIRE(a5.extent(2) == dim[2]);
    REQUIRE(a5.extent(3) == dim[3]);
    REQUIRE(a5.extent(4) == dim[4]);
    REQUIRE(b5.extent(0) == dim[0]);
    REQUIRE(b5.extent(1) == dim[1]);
    REQUIRE(b5.extent(2) == dim[2]);
    REQUIRE(b5.extent(3) == dim[3]);
    REQUIRE(b5.extent(4) == dim[4]);
    REQUIRE(c5.extent(0) == dim[0]);
    REQUIRE(c5.extent(1) == dim[1]);
    REQUIRE(c5.extent(2) == dim[2]);
    REQUIRE(c5.extent(3) == dim[3]);
    REQUIRE(c5.extent(4) == dim[4]);

    REQUIRE(a6.extent(0) == dim[0]);
    REQUIRE(a6.extent(1) == dim[1]);
    REQUIRE(a6.extent(2) == dim[2]);
    REQUIRE(a6.extent(3) == dim[3]);
    REQUIRE(a6.extent(4) == dim[4]);
    REQUIRE(a6.extent(5) == dim[5]);
    REQUIRE(b6.extent(0) == dim[0]);
    REQUIRE(b6.extent(1) == dim[1]);
    REQUIRE(b6.extent(2) == dim[2]);
    REQUIRE(b6.extent(3) == dim[3]);
    REQUIRE(b6.extent(4) == dim[4]);
    REQUIRE(b6.extent(5) == dim[5]);
    REQUIRE(c6.extent(0) == dim[0]);
    REQUIRE(c6.extent(1) == dim[1]);
    REQUIRE(c6.extent(2) == dim[2]);
    REQUIRE(c6.extent(3) == dim[3]);
    REQUIRE(c6.extent(4) == dim[4]);
    REQUIRE(c6.extent(5) == dim[5]);

    REQUIRE(b7.extent(0) == dim[0]);
    REQUIRE(b7.extent(1) == dim[1]);
    REQUIRE(b7.extent(2) == dim[2]);
    REQUIRE(b7.extent(3) == dim[3]);
    REQUIRE(b7.extent(4) == dim[4]);
    REQUIRE(b7.extent(5) == dim[5]);
    REQUIRE(b7.extent(6) == dim[6]);
    REQUIRE(c7.extent(0) == dim[0]);
    REQUIRE(c7.extent(1) == dim[1]);
    REQUIRE(c7.extent(2) == dim[2]);
    REQUIRE(c7.extent(3) == dim[3]);
    REQUIRE(c7.extent(4) == dim[4]);
    REQUIRE(c7.extent(5) == dim[5]);
    REQUIRE(c7.extent(6) == dim[6]);

    a1.clear(); //optional here, as a1 will go out of scope
    b7.clear();

    REQUIRE(a1.empty());
    REQUIRE(b7.empty());
    REQUIRE(c7.empty() == false);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("testconstructors7dimbuf", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    ra::size_type dim[7] = {7,10,13,2,4,5,21};
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
        REQUIRE(a1.data());
        REQUIRE(b1.data());
        REQUIRE(a2.data());
        REQUIRE(b2.data());
        REQUIRE(a3.data());
        REQUIRE(b3.data());
        REQUIRE(a4.data());
        REQUIRE(b4.data());
        REQUIRE(a5.data());
        REQUIRE(b5.data());
        REQUIRE(a6.data());
        REQUIRE(b6.data());
        REQUIRE(b7.data());
        REQUIRE(a1.extent(0) == dim[0]);
        REQUIRE(b1.extent(0) == dim[0]);
        REQUIRE(a2.extent(0) == dim[0]);
        REQUIRE(a2.extent(1) == dim[1]);
        REQUIRE(b2.extent(0) == dim[0]);
        REQUIRE(b2.extent(1) == dim[1]);
        REQUIRE(a3.extent(0) == dim[0]);
        REQUIRE(a3.extent(1) == dim[1]);
        REQUIRE(a3.extent(2) == dim[2]);
        REQUIRE(b3.extent(0) == dim[0]);
        REQUIRE(b3.extent(1) == dim[1]);
        REQUIRE(b3.extent(2) == dim[2]);
        REQUIRE(a4.extent(0) == dim[0]);
        REQUIRE(a4.extent(1) == dim[1]);
        REQUIRE(a4.extent(2) == dim[2]);
        REQUIRE(a4.extent(3) == dim[3]);
        REQUIRE(b4.extent(0) == dim[0]);
        REQUIRE(b4.extent(1) == dim[1]);
        REQUIRE(b4.extent(2) == dim[2]);
        REQUIRE(b4.extent(3) == dim[3]);
        REQUIRE(a5.extent(0) == dim[0]);
        REQUIRE(a5.extent(1) == dim[1]);
        REQUIRE(a5.extent(2) == dim[2]);
        REQUIRE(a5.extent(3) == dim[3]);
        REQUIRE(a5.extent(4) == dim[4]);
        REQUIRE(b5.extent(0) == dim[0]);
        REQUIRE(b5.extent(1) == dim[1]);
        REQUIRE(b5.extent(2) == dim[2]);
        REQUIRE(b5.extent(3) == dim[3]);
        REQUIRE(b5.extent(4) == dim[4]);
        REQUIRE(a6.extent(0) == dim[0]);
        REQUIRE(a6.extent(1) == dim[1]);
        REQUIRE(a6.extent(2) == dim[2]);
        REQUIRE(a6.extent(3) == dim[3]);
        REQUIRE(a6.extent(4) == dim[4]);
        REQUIRE(a6.extent(5) == dim[5]);
        REQUIRE(b6.extent(0) == dim[0]);
        REQUIRE(b6.extent(1) == dim[1]);
        REQUIRE(b6.extent(2) == dim[2]);
        REQUIRE(b6.extent(3) == dim[3]);
        REQUIRE(b6.extent(4) == dim[4]);
        REQUIRE(b6.extent(5) == dim[5]);
        REQUIRE(b7.extent(0) == dim[0]);
        REQUIRE(b7.extent(1) == dim[1]);
        REQUIRE(b7.extent(2) == dim[2]);
        REQUIRE(b7.extent(3) == dim[3]);
        REQUIRE(b7.extent(4) == dim[4]);
        REQUIRE(b7.extent(5) == dim[5]);
        REQUIRE(b7.extent(6) == dim[6]);
    }
    delete[] buf;    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("test constructors 12dim", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    ra::size_type dim[12] = {2,3,4,3,2,3,4,3,2,3,4,3};
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
    REQUIRE(a7.data());
    REQUIRE(b7.data());
    REQUIRE(c7.data()==b7.data());
    REQUIRE(a8.data());
    REQUIRE(b8.data());
    REQUIRE(c8.data()==b8.data());
    REQUIRE(a9.data());
    REQUIRE(b9.data());
    REQUIRE(c9.data()==b9.data());
    REQUIRE(a10.data());
    REQUIRE(b10.data());
    REQUIRE(c10.data()==b10.data());
    REQUIRE(a11.data());
    REQUIRE(b11.data());
    REQUIRE(c11.data()==b11.data());
    REQUIRE(b12.data());
    REQUIRE(c12.data()==b12.data());
    REQUIRE( b7.size()==2*3*4*3*2*3*4);
    REQUIRE( b8.size()==2*3*4*3*2*3*4*3);
    REQUIRE( b9.size()==2*3*4*3*2*3*4*3*2);
    REQUIRE(b10.size()==2*3*4*3*2*3*4*3*2*3);
    REQUIRE(b11.size()==2*3*4*3*2*3*4*3*2*3*4);
    REQUIRE(b12.size()==2*3*4*3*2*3*4*3*2*3*4*3);
    REQUIRE(a7.extent(0) == dim[0]);
    REQUIRE(a7.extent(1) == dim[1]);
    REQUIRE(a7.extent(2) == dim[2]);
    REQUIRE(a7.extent(3) == dim[3]);
    REQUIRE(a7.extent(4) == dim[4]);
    REQUIRE(a7.extent(5) == dim[5]);
    REQUIRE(a7.extent(6) == dim[6]);
    REQUIRE(b7.extent(0) == dim[0]);
    REQUIRE(b7.extent(1) == dim[1]);
    REQUIRE(b7.extent(2) == dim[2]);
    REQUIRE(b7.extent(3) == dim[3]);
    REQUIRE(b7.extent(4) == dim[4]);
    REQUIRE(b7.extent(5) == dim[5]);
    REQUIRE(b7.extent(6) == dim[6]);
    REQUIRE(c7.extent(0) == dim[0]);
    REQUIRE(c7.extent(1) == dim[1]);
    REQUIRE(c7.extent(2) == dim[2]);
    REQUIRE(c7.extent(3) == dim[3]);
    REQUIRE(c7.extent(4) == dim[4]);
    REQUIRE(c7.extent(5) == dim[5]);
    REQUIRE(c7.extent(6) == dim[6]);
    REQUIRE(a8.extent(0) == dim[0]);
    REQUIRE(a8.extent(1) == dim[1]);
    REQUIRE(a8.extent(2) == dim[2]);
    REQUIRE(a8.extent(3) == dim[3]);
    REQUIRE(a8.extent(4) == dim[4]);
    REQUIRE(a8.extent(5) == dim[5]);
    REQUIRE(a8.extent(6) == dim[6]);
    REQUIRE(a8.extent(7) == dim[7]);
    REQUIRE(b8.extent(0) == dim[0]);
    REQUIRE(b8.extent(1) == dim[1]);
    REQUIRE(b8.extent(2) == dim[2]);
    REQUIRE(b8.extent(3) == dim[3]);
    REQUIRE(b8.extent(4) == dim[4]);
    REQUIRE(b8.extent(5) == dim[5]);
    REQUIRE(b8.extent(6) == dim[6]);
    REQUIRE(b8.extent(7) == dim[7]);
    REQUIRE(c8.extent(0) == dim[0]);
    REQUIRE(c8.extent(1) == dim[1]);
    REQUIRE(c8.extent(2) == dim[2]);
    REQUIRE(c8.extent(3) == dim[3]);
    REQUIRE(c8.extent(4) == dim[4]);
    REQUIRE(c8.extent(5) == dim[5]);
    REQUIRE(c8.extent(6) == dim[6]);
    REQUIRE(c8.extent(7) == dim[7]);
    REQUIRE(a9.extent(0) == dim[0]);
    REQUIRE(a9.extent(1) == dim[1]);
    REQUIRE(a9.extent(2) == dim[2]);
    REQUIRE(a9.extent(3) == dim[3]);
    REQUIRE(a9.extent(4) == dim[4]);
    REQUIRE(a9.extent(5) == dim[5]);
    REQUIRE(a9.extent(6) == dim[6]);
    REQUIRE(a9.extent(7) == dim[7]);
    REQUIRE(a9.extent(8) == dim[8]);
    REQUIRE(b9.extent(0) == dim[0]);
    REQUIRE(b9.extent(1) == dim[1]);
    REQUIRE(b9.extent(2) == dim[2]);
    REQUIRE(b9.extent(3) == dim[3]);
    REQUIRE(b9.extent(4) == dim[4]);
    REQUIRE(b9.extent(5) == dim[5]);
    REQUIRE(b9.extent(6) == dim[6]);
    REQUIRE(b9.extent(7) == dim[7]);
    REQUIRE(b9.extent(8) == dim[8]);
    REQUIRE(c9.extent(0) == dim[0]);
    REQUIRE(c9.extent(1) == dim[1]);
    REQUIRE(c9.extent(2) == dim[2]);
    REQUIRE(c9.extent(3) == dim[3]);
    REQUIRE(c9.extent(4) == dim[4]);
    REQUIRE(c9.extent(5) == dim[5]);
    REQUIRE(c9.extent(6) == dim[6]);
    REQUIRE(c9.extent(7) == dim[7]);
    REQUIRE(c9.extent(8) == dim[8]);
    REQUIRE(a10.extent(0) == dim[0]);
    REQUIRE(a10.extent(1) == dim[1]);
    REQUIRE(a10.extent(2) == dim[2]);
    REQUIRE(a10.extent(3) == dim[3]);
    REQUIRE(a10.extent(4) == dim[4]);
    REQUIRE(a10.extent(5) == dim[5]);
    REQUIRE(a10.extent(6) == dim[6]);
    REQUIRE(a10.extent(7) == dim[7]);
    REQUIRE(a10.extent(8) == dim[8]);
    REQUIRE(a10.extent(9) == dim[9]);
    REQUIRE(b10.extent(0) == dim[0]);
    REQUIRE(b10.extent(1) == dim[1]);
    REQUIRE(b10.extent(2) == dim[2]);
    REQUIRE(b10.extent(3) == dim[3]);
    REQUIRE(b10.extent(4) == dim[4]);
    REQUIRE(b10.extent(5) == dim[5]);
    REQUIRE(b10.extent(6) == dim[6]);
    REQUIRE(b10.extent(7) == dim[7]);
    REQUIRE(b10.extent(8) == dim[8]);
    REQUIRE(b10.extent(9) == dim[9]);
    REQUIRE(c10.extent(0) == dim[0]);
    REQUIRE(c10.extent(1) == dim[1]);
    REQUIRE(c10.extent(2) == dim[2]);
    REQUIRE(c10.extent(3) == dim[3]);
    REQUIRE(c10.extent(4) == dim[4]);
    REQUIRE(c10.extent(5) == dim[5]);
    REQUIRE(c10.extent(6) == dim[6]);
    REQUIRE(c10.extent(7) == dim[7]);
    REQUIRE(c10.extent(8) == dim[8]);
    REQUIRE(c10.extent(9) == dim[9]);
    REQUIRE(a11.extent( 0) == dim[ 0]);
    REQUIRE(a11.extent( 1) == dim[ 1]);
    REQUIRE(a11.extent( 2) == dim[ 2]);
    REQUIRE(a11.extent( 3) == dim[ 3]);
    REQUIRE(a11.extent( 4) == dim[ 4]);
    REQUIRE(a11.extent( 5) == dim[ 5]);
    REQUIRE(a11.extent( 6) == dim[ 6]);
    REQUIRE(a11.extent( 7) == dim[ 7]);
    REQUIRE(a11.extent( 8) == dim[ 8]);
    REQUIRE(a11.extent( 9) == dim[ 9]);
    REQUIRE(a11.extent(10) == dim[10]);
    REQUIRE(b11.extent( 0) == dim[ 0]);
    REQUIRE(b11.extent( 1) == dim[ 1]);
    REQUIRE(b11.extent( 2) == dim[ 2]);
    REQUIRE(b11.extent( 3) == dim[ 3]);
    REQUIRE(b11.extent( 4) == dim[ 4]);
    REQUIRE(b11.extent( 5) == dim[ 5]);
    REQUIRE(b11.extent( 6) == dim[ 6]);
    REQUIRE(b11.extent( 7) == dim[ 7]);
    REQUIRE(b11.extent( 8) == dim[ 8]);
    REQUIRE(b11.extent( 9) == dim[ 9]);
    REQUIRE(b11.extent(10) == dim[10]);
    REQUIRE(c11.extent( 0) == dim[ 0]);
    REQUIRE(c11.extent( 1) == dim[ 1]);
    REQUIRE(c11.extent( 2) == dim[ 2]);
    REQUIRE(c11.extent( 3) == dim[ 3]);
    REQUIRE(c11.extent( 4) == dim[ 4]);
    REQUIRE(c11.extent( 5) == dim[ 5]);
    REQUIRE(c11.extent( 6) == dim[ 6]);
    REQUIRE(c11.extent( 7) == dim[ 7]);
    REQUIRE(c11.extent( 8) == dim[ 8]);
    REQUIRE(c11.extent( 9) == dim[ 9]);
    REQUIRE(c11.extent(10) == dim[10]);
    REQUIRE(b12.extent( 0) == dim[ 0]);
    REQUIRE(b12.extent( 1) == dim[ 1]);
    REQUIRE(b12.extent( 2) == dim[ 2]);
    REQUIRE(b12.extent( 3) == dim[ 3]);
    REQUIRE(b12.extent( 4) == dim[ 4]);
    REQUIRE(b12.extent( 5) == dim[ 5]);
    REQUIRE(b12.extent( 6) == dim[ 6]);
    REQUIRE(b12.extent( 7) == dim[ 7]);
    REQUIRE(b12.extent( 8) == dim[ 8]);
    REQUIRE(b12.extent( 9) == dim[ 9]);
    REQUIRE(b12.extent(10) == dim[10]);
    REQUIRE(b12.extent(11) == dim[11]);
    REQUIRE(c12.extent( 0) == dim[ 0]);
    REQUIRE(c12.extent( 1) == dim[ 1]);
    REQUIRE(c12.extent( 2) == dim[ 2]);
    REQUIRE(c12.extent( 3) == dim[ 3]);
    REQUIRE(c12.extent( 4) == dim[ 4]);
    REQUIRE(c12.extent( 5) == dim[ 5]);
    REQUIRE(c12.extent( 6) == dim[ 6]);
    REQUIRE(c12.extent( 7) == dim[ 7]);
    REQUIRE(c12.extent( 8) == dim[ 8]);
    REQUIRE(c12.extent( 9) == dim[ 9]);
    REQUIRE(c12.extent(10) == dim[10]);
    REQUIRE(c12.extent(11) == dim[11]);    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("test constructors 12dim buf", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    ra::size_type dim[12] = {2,3,4,3,2,3,4,3,2,3,4,3};
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
        REQUIRE(a7.data());
        REQUIRE(b7.data());
        REQUIRE(a8.data());
        REQUIRE(b8.data());
        REQUIRE(a9.data());
        REQUIRE(b9.data());
        REQUIRE(a10.data());
        REQUIRE(b10.data());
        REQUIRE(a11.data());
        REQUIRE(b11.data());
        REQUIRE(b12.data());
        REQUIRE(a7.extent(0) == dim[0]);
        REQUIRE(a7.extent(1) == dim[1]);
        REQUIRE(a7.extent(2) == dim[2]);
        REQUIRE(a7.extent(3) == dim[3]);
        REQUIRE(a7.extent(4) == dim[4]);
        REQUIRE(a7.extent(5) == dim[5]);
        REQUIRE(a7.extent(6) == dim[6]);
        REQUIRE(b7.extent(0) == dim[0]);
        REQUIRE(b7.extent(1) == dim[1]);
        REQUIRE(b7.extent(2) == dim[2]);
        REQUIRE(b7.extent(3) == dim[3]);
        REQUIRE(b7.extent(4) == dim[4]);
        REQUIRE(b7.extent(5) == dim[5]);
        REQUIRE(b7.extent(6) == dim[6]);
        REQUIRE(a8.extent(0) == dim[0]);
        REQUIRE(a8.extent(1) == dim[1]);
        REQUIRE(a8.extent(2) == dim[2]);
        REQUIRE(a8.extent(3) == dim[3]);
        REQUIRE(a8.extent(4) == dim[4]);
        REQUIRE(a8.extent(5) == dim[5]);
        REQUIRE(a8.extent(6) == dim[6]);
        REQUIRE(a8.extent(7) == dim[7]);
        REQUIRE(b8.extent(0) == dim[0]);
        REQUIRE(b8.extent(1) == dim[1]);
        REQUIRE(b8.extent(2) == dim[2]);
        REQUIRE(b8.extent(3) == dim[3]);
        REQUIRE(b8.extent(4) == dim[4]);
        REQUIRE(b8.extent(5) == dim[5]);
        REQUIRE(b8.extent(6) == dim[6]);
        REQUIRE(b8.extent(7) == dim[7]);
        REQUIRE(a9.extent(0) == dim[0]);
        REQUIRE(a9.extent(1) == dim[1]);
        REQUIRE(a9.extent(2) == dim[2]);
        REQUIRE(a9.extent(3) == dim[3]);
        REQUIRE(a9.extent(4) == dim[4]);
        REQUIRE(a9.extent(5) == dim[5]);
        REQUIRE(a9.extent(6) == dim[6]);
        REQUIRE(a9.extent(7) == dim[7]);
        REQUIRE(a9.extent(8) == dim[8]);
        REQUIRE(b9.extent(0) == dim[0]);
        REQUIRE(b9.extent(1) == dim[1]);
        REQUIRE(b9.extent(2) == dim[2]);
        REQUIRE(b9.extent(3) == dim[3]);
        REQUIRE(b9.extent(4) == dim[4]);
        REQUIRE(b9.extent(5) == dim[5]);
        REQUIRE(b9.extent(6) == dim[6]);
        REQUIRE(b9.extent(7) == dim[7]);
        REQUIRE(b9.extent(8) == dim[8]);
        REQUIRE(a10.extent(0) == dim[0]);
        REQUIRE(a10.extent(1) == dim[1]);
        REQUIRE(a10.extent(2) == dim[2]);
        REQUIRE(a10.extent(3) == dim[3]);
        REQUIRE(a10.extent(4) == dim[4]);
        REQUIRE(a10.extent(5) == dim[5]);
        REQUIRE(a10.extent(6) == dim[6]);
        REQUIRE(a10.extent(7) == dim[7]);
        REQUIRE(a10.extent(8) == dim[8]);
        REQUIRE(a10.extent(9) == dim[9]);
        REQUIRE(b10.extent(0) == dim[0]);
        REQUIRE(b10.extent(1) == dim[1]);
        REQUIRE(b10.extent(2) == dim[2]);
        REQUIRE(b10.extent(3) == dim[3]);
        REQUIRE(b10.extent(4) == dim[4]);
        REQUIRE(b10.extent(5) == dim[5]);
        REQUIRE(b10.extent(6) == dim[6]);
        REQUIRE(b10.extent(7) == dim[7]);
        REQUIRE(b10.extent(8) == dim[8]);
        REQUIRE(b10.extent(9) == dim[9]);
        REQUIRE(a11.extent( 0) == dim[ 0]);
        REQUIRE(a11.extent( 1) == dim[ 1]);
        REQUIRE(a11.extent( 2) == dim[ 2]);
        REQUIRE(a11.extent( 3) == dim[ 3]);
        REQUIRE(a11.extent( 4) == dim[ 4]);
        REQUIRE(a11.extent( 5) == dim[ 5]);
        REQUIRE(a11.extent( 6) == dim[ 6]);
        REQUIRE(a11.extent( 7) == dim[ 7]);
        REQUIRE(a11.extent( 8) == dim[ 8]);
        REQUIRE(a11.extent( 9) == dim[ 9]);
        REQUIRE(a11.extent(10) == dim[10]);
        REQUIRE(b11.extent( 0) == dim[ 0]);
        REQUIRE(b11.extent( 1) == dim[ 1]);
        REQUIRE(b11.extent( 2) == dim[ 2]);
        REQUIRE(b11.extent( 3) == dim[ 3]);
        REQUIRE(b11.extent( 4) == dim[ 4]);
        REQUIRE(b11.extent( 5) == dim[ 5]);
        REQUIRE(b11.extent( 6) == dim[ 6]);
        REQUIRE(b11.extent( 7) == dim[ 7]);
        REQUIRE(b11.extent( 8) == dim[ 8]);
        REQUIRE(b11.extent( 9) == dim[ 9]);
        REQUIRE(b11.extent(10) == dim[10]);
        REQUIRE(b12.extent( 0) == dim[ 0]);
        REQUIRE(b12.extent( 1) == dim[ 1]);
        REQUIRE(b12.extent( 2) == dim[ 2]);
        REQUIRE(b12.extent( 3) == dim[ 3]);
        REQUIRE(b12.extent( 4) == dim[ 4]);
        REQUIRE(b12.extent( 5) == dim[ 5]);
        REQUIRE(b12.extent( 6) == dim[ 6]);
        REQUIRE(b12.extent( 7) == dim[ 7]);
        REQUIRE(b12.extent( 8) == dim[ 8]);
        REQUIRE(b12.extent( 9) == dim[ 9]);
        REQUIRE(b12.extent(10) == dim[10]);
        REQUIRE(b12.extent(11) == dim[11]);
    }
    delete[] buf;    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

namespace global {
    
double d1 = -2.2, d2 = 7.1;
Compound c1(1,2), c2(-7,13);
array<Compound,3> a1 = {{Compound(1,2),Compound(3,4),Compound(5,6)}};
array<Compound,3> a2 = {{Compound(-1,-2),Compound(3,-4),Compound(5,-6)}};
std::complex<float> x1(1.f,2.f);
std::complex<float> x2(-1.f,2.f);
template<class T> const T& get_value_1();
template<class T> const T& get_value_2();
template<> const double& get_value_1<double>() {return d1;}
template<> const double& get_value_2<double>() {return d2;}
template<> const Compound& get_value_1<Compound>() {return c1;}
template<> const Compound& get_value_2<Compound>() {return c2;}
template<> const array<Compound,3>& get_value_1<array<Compound,3>>() {return a1;}
template<> const array<Compound,3>& get_value_2<array<Compound,3>>() {return a2;}
template<> const std::complex<float>& get_value_1<std::complex<float>>() {return x1;}
template<> const std::complex<float>& get_value_2<std::complex<float>>() {return x2;}
    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
void testaccessors(T value1, T value2) 
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
    REQUIRE(value1!=value2);  // required for the test to work
    ra::size_type dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    const ra::size_type l=a.extent(0);
    const ra::size_type m=a.extent(1);
    const ra::size_type n=a.extent(2);
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                a[i][j][k] = value1;
                b[i][j][k] = value2;
            }
        }
    }
    for (ra::size_type i=0; i<l; i++)
        for (ra::size_type j=0; j<m; j++)
            for (ra::size_type k=0; k<n; k++)
                REQUIRE(a[i][j][k] == value1);   
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                REQUIRE(b[i][j][k] == value2);   
            }
        }
    }
    b[6][1][0] = value1;
    REQUIRE(c[6][1][0] == value1);    
}
TEST_CASE("testaccessors_double")
{
    testaccessors<double>(global::d1,global::d2);
}
TEST_CASE("testaccessors_Compound")
{
    testaccessors<Compound>(global::c1,global::c2);
}
TEST_CASE("testaccessors_array_Compound_3")
{
    testaccessors<array<Compound,3> >(global::a1,global::a2);
}

#if __cpp_multidimensional_subscript >= 202110L

template<typename T> 
void testaccessors23(T value1, T value2) 
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
    REQUIRE(value1!=value2);  // required for the test to work
    ra::size_type dim[3] = {7,21,13};
    rarray<T,3> a(7,21,13);
    rarray<T,3> b(dim);
    rarray<T,3> c(b);
    const ra::size_type l=a.extent(0);
    const ra::size_type m=a.extent(1);
    const ra::size_type n=a.extent(2);
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                a[i,j,k] = value1;
                b[i,j,k] = value2;
            }
        }
    }
    for (ra::size_type i=0; i<l; i++)
        for (ra::size_type j=0; j<m; j++)
            for (ra::size_type k=0; k<n; k++)
                REQUIRE(a[i,j,k] == value1);   
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                REQUIRE(b[i,j,k] == value2);   
            }
        }
    }
    b[6,1,0] = value1;
    REQUIRE(c[6,1,0] == value1);    
}
TEST_CASE("testaccessors23_double")
{
    testaccessors23<double>(global::d1,global::d2);
}
TEST_CASE("testaccessors23_Compound")
{
    testaccessors23<Compound>(global::c1,global::c2);
}
TEST_CASE("testaccessors23_array_Compound_3")
{
    testaccessors23<array<Compound,3> >(global::a1,global::a2);
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("testsliceconstructor", "",
                   double,
                   Compound,
                   (array<Compound,3>),
                   (std::complex<float>))
{
    using T = TestType;
    // Exercises the following methods of the rarray<T,3> class
    //   rarray_intermediate operator[](int i);
    //   const T* data() const;
    // Exercises the following constructor of the rarray<T,2> class
    //   rarray(const rarray_intermediate&)
    // Uses previously checked
    //   rarray(int,int,int)
    //   T* data();
    rarray<T,3> a(7,21,13);
    rarray<T,2> b(a.at(2));
    rarray<T,1> c(b.at(2));
    const rarray<T,2> d(a.at(2));
    const rarray<T,1> e(b.at(2));
    const T* tan = getconstdata(rarray<T,2>(a.at(1)));
    T* tac = a.at(1).data();
    REQUIRE(tan==tac);
    REQUIRE(a.at(1).extent(0)==21);
    REQUIRE(a.at(1).extent(1)==13);
    REQUIRE(a.at(1).shape()[1]==13);
    REQUIRE(a.at(1).at(6).extent(0)==13);
    REQUIRE(a.at(1).at(6).shape()[0]==13);
    REQUIRE(a.at(1).size()==21*13);
    REQUIRE(a.at(1).at(6).size()==13);
    T* p1 = a.at(3).at(2).data();
    T* p2 = a.at(3).data();
    REQUIRE(p1);
    REQUIRE(p2);
    REQUIRE_THROWS(b.at(-1));
    REQUIRE_THROWS(c.at(100));
    REQUIRE_THROWS(d.at(100));
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
void testcopy2d(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,2> copy() const;
    rarray<T,2> b(4,3);
    const ra::size_type l=b.extent(0);
    const ra::size_type m=b.extent(1);
    T value3 = value1;
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            b[i][j] = value3;
            value3 = value3+value2;
        }
    }
    rarray<T,2> d(b.copy());
    REQUIRE(d.data()!=b.data());
    REQUIRE(d.extent(0)==b.extent(0));
    REQUIRE(d.extent(1)==b.extent(1));
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            REQUIRE(b[i][j]==d[i][j]);
        }
    }    
}
TEST_CASE("testcopy2d_double")
{
    testcopy2d<double>(global::d1,global::d2);
}
TEST_CASE("testcopy2d_Compound")
{
    testcopy2d<Compound>(global::c1,global::c2);
}
TEST_CASE("testcopy2d_array_Compound_3")
{
    testcopy2d<array<Compound,3> >(global::a1,global::a2);
}

#if __cpp_multidimensional_subscript >= 202110L

template<typename T> 
void testcopy2d23(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,2> copy() const;
    rarray<T,2> b(4,3);
    const ra::size_type l=b.extent(0);
    const ra::size_type m=b.extent(1);
    T value3 = value1;
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            b[i,j] = value3;
            value3 = value3+value2;
        }
    }
    rarray<T,2> d(b.copy());
    REQUIRE(d.data()!=b.data());
    REQUIRE(d.extent(0)==b.extent(0));
    REQUIRE(d.extent(1)==b.extent(1));
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            REQUIRE(b[i,j]==d[i,j]);
        }
    }    
}
TEST_CASE("testcopy2d23_double")
{
    testcopy2d23<double>(global::d1,global::d2);
}
TEST_CASE("testcopy2d23_Compound")
{
    testcopy2d23<Compound>(global::c1,global::c2);
}
TEST_CASE("testcopy2d23_array_Compound_3")
{
    testcopy2d23<array<Compound,3> >(global::a1,global::a2);
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
void testcopy3d(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,3> copy() const;
    rarray<T,3> b(100,40,3);
    //rarray<T,3> b(1,1,2);
    const ra::size_type l=b.extent(0);
    const ra::size_type m=b.extent(1);
    const ra::size_type n=b.extent(2);
    T value3 = value1;
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                b[i][j][k] = value3;
                value3 = value3+value2;
            }
        }
    }
    rarray<T,3> d(b.copy());
    REQUIRE(d.data()!=b.data());
    REQUIRE(d.extent(0)==b.extent(0));
    REQUIRE(d.extent(1)==b.extent(1));
    REQUIRE(d.extent(2)==b.extent(2));
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                REQUIRE(b[i][j][k]==d[i][j][k]);
            }
        }
    }
}
TEST_CASE("testcopy3d_double")
{
    testcopy3d<double>(global::d1,global::d2);
}
TEST_CASE("testcopy3d_Compound")
{
    testcopy3d<Compound>(global::c1,global::c2);
}
TEST_CASE("testcopy3d_array_Compound_3")
{
    testcopy3d<array<Compound,3> >(global::a1,global::a2);
}

#if __cpp_multidimensional_subscript >= 202110L

template<typename T> 
void testcopy3d23(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,3> copy() const;
    rarray<T,3> b(100,40,3);
    //rarray<T,3> b(1,1,2);
    const ra::size_type l=b.extent(0);
    const ra::size_type m=b.extent(1);
    const ra::size_type n=b.extent(2);
    T value3 = value1;
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                b[i,j,k] = value3;
                value3 = value3+value2;
            }
        }
    }
    rarray<T,3> d(b.copy());
    REQUIRE(d.data()!=b.data());
    REQUIRE(d.extent(0)==b.extent(0));
    REQUIRE(d.extent(1)==b.extent(1));
    REQUIRE(d.extent(2)==b.extent(2));
    for (ra::size_type i=0; i<l; i++) {
        for (ra::size_type j=0; j<m; j++) {
            for (ra::size_type k=0; k<n; k++) {
                REQUIRE(b[i,j,k]==d[i,j,k]);
            }
        }
    }
}
TEST_CASE("testcopy3d23_double")
{
    testcopy3d23<double>(global::d1,global::d2);
}
TEST_CASE("testcopy3d23_Compound")
{
    testcopy3d23<Compound>(global::c1,global::c2);
}
TEST_CASE("testcopy3d23_array_Compound_3")
{
    testcopy3d23<array<Compound,3> >(global::a1,global::a2);
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<typename T> 
void testcopy1d(T value1, T value2) 
{
    // Tests following methods:
    //   rarray<T,1> copy() const;
    rarray<T,1> b(100);
    const ra::size_type n=b.extent(0);
    T value3 = value1;
    for (ra::size_type i=0; i<n; i++) {
        b[i] = value3;
        value3 = value3+value2;
    }
    rarray<T,1> d(b.copy());
    REQUIRE(d.data()!=b.data());
    REQUIRE(d.extent(0)==b.extent(0));
    for (ra::size_type i=0; i<n; i++) {
        REQUIRE(b[i]==d[i]);
    }
    
}

TEST_CASE("testcopy1d_double")
{
    testcopy1d<double>(global::d1,global::d2);
}


TEST_CASE("testcopy1d_Compound")
{
    testcopy1d<Compound>(global::c1,global::c2);
}


TEST_CASE("testcopy1d_array_Compound_3")
{
    testcopy1d<array<Compound,3> >(global::a1,global::a2);
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
    const ra::size_type l=B.extent(0);
    const ra::size_type m=C.extent(1);
    const ra::size_type n=B.extent(1);
    for (ra::size_type i=0;i<l;i++) {
        for (ra::size_type j=0;j<m;j++) {
            A[i][j] = 0;
            for (ra::size_type k=0;k<n;k++) {
                A[i][j] += B[i][k]*C[k][j];
            }
        }
    }
}
TEMPLATE_TEST_CASE("testmmm", "", int, double, (std::complex<float>))
{                  
    using T = TestType;
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
    for (ra::index_type i=0;i<3;i++) {
        for (ra::index_type j=0;j<3;j++) {
            REQUIRE(a[i][j]==adata[i*3+j]);
        }
    }
    
}

#if __cpp_multidimensional_subscript >= 202110L

// matrix matrix mutiple A=B*C
template<class T>
void mmm23(rarray<T,2> &A, const rarray<T,2>& B, const rarray<T,2>& C)
{
    assert(A.extent(0)==B.extent(0));
    assert(B.extent(1)==C.extent(0));
    assert(C.extent(1)==A.extent(1));
    const ra::size_type l=B.extent(0);
    const ra::size_type m=C.extent(1);
    const ra::size_type n=B.extent(1);
    for (ra::size_type i=0;i<l;i++) {
        for (ra::size_type j=0;j<m;j++) {
            A[i,j] = 0;
            for (ra::size_type k=0;k<n;k++) {
                A[i,j] += B[i,k]*C[k,j];
            }
        }
    }
}
TEMPLATE_TEST_CASE("testmmm23", "", int, double, (std::complex<float>))
{                  
    using T = TestType;
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
    mmm23(a,b,c);
    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            REQUIRE(a[i,j]==adata[i*3+j]);
        }
    }
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print1d(const rarray<float,1> &a) 
{
    std::stringstream s;
    const ra::size_type n = EXTENT(a,0);
    for (ra::size_type i=0;i<n-1;i++)
        s << a[i] << ' ';
    s << a[n-1];
    return s.str();
}

TEST_CASE("test1dautoconversions")
{
    const int n=9;
    float b[n] = {1,2,3,4,5,6,7,8,9};
    const rarray<float,1> a(b);
    REQUIRE(EXTENT(a,0)==EXTENT(b,0));
    std::string s = print1d(rarray<float,1>(b));
    REQUIRE(s=="1 2 3 4 5 6 7 8 9");
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void print1d_1(float* a, ra::size_type n, std::ostream &out) 
{
    for (ra::size_type i=0;i<n;i++) 
        out << a[i] << ' ';
    out << std::endl;
}

void print1d_2(const float* a, ra::size_type n, std::ostream &out) 
{
    for (ra::size_type i=0;i<n;i++) 
        out << a[i] << ' ';
    out << std::endl;
}

void print1d_3(const rarray<float,1> &a, std::ostream &out) 
{
    for (ra::size_type i=0;i<a.extent(0);i++) 
        out << a[i] << ' ';
    out << std::endl;
}

void print1d_4(const rarray<const float,1>& a, std::ostream &out)
{
    for (ra::size_type i=0;i<a.extent(0);i++) 
        out << a[i] << ' ';
    out << std::endl;
}

TEST_CASE("test1dconversions")
{
    const int n=9;
    rarray<float,1> a(n);
    for (int i=0;i<n;i++)
        a[i] = static_cast<float>(i)+1;
    const rarray<float,1>& c=a;
    std::stringstream s1,s2,s3,s4,s5,s6,s7,s8;
    print1d_1(c.ptr_array(), c.extent(0), s1);
    REQUIRE(s1.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.noconst_ptr_array(), c.extent(0), s2);
    REQUIRE(s2.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_1(a.data(), c.extent(0), s3);
    REQUIRE(s3.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_2(c.data(), c.extent(0), s4);
    REQUIRE(s4.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_3(c, s5);
    REQUIRE(s5.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(a.const_ref(), s6);
    REQUIRE(s6.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(c.const_ref(), s7);
    REQUIRE(s7.str()=="1 2 3 4 5 6 7 8 9 \n");
    print1d_4(c, s8); // test implicit conversion
    REQUIRE(s8.str()=="1 2 3 4 5 6 7 8 9 \n");
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// print2d_1 takes a double-pointer matrix, whose elements and row
// pointers could be changed. Dangerous.
// - Not const-correct, but common in non-const (often C) libraries.  
// - Will require a call to ptr_array_no_const
void print2d_1(float**a, ra::size_type n, ra::size_type m, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_2 takes a matrix with const elements, but whose row
// pora::size_typeers could in principle be changed. Dangerous, but not
// uncommon!
// - Not const-correct.
// - Requires a ptr_array_no_mid_const of a shapeal 2d array
void print2d_2(const float**a, ra::size_type n, ra::size_type m, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_3 takes a matrix, which is a pora::size_typeer to a set of pora::size_typeers. The
// row pora::size_typeers are constant, but the elements would be changable.
// - Not (logically) const-correct.
// - A non-const shapeal 2d array can be passed right in.
void print2d_3(float *const* a, ra::size_type n, ra::size_type m, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) 
            cout << a[i][j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_4 takes a constant matrix, as a set of pora::size_typeers to rows. Both
// the row pora::size_typeers and the elements are const, and can't be changed.
// - Const-correct.
// - A const shapeal 2d array can be passed right in.
void print2d_4(const float*const*a, ra::size_type n, ra::size_type m, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) 
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
void print2d_5(float *a, ra::size_type n, ra::size_type m, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) 
            cout << a[i*m+j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}

// print2d_6 wants the constant matrix as a contiguous memory block.
// because of const, print2d_6 cannot change the elements of a. 
// - Const-correct
// - A const shapeal 2d array can be passed right in.
void print2d_6(const float *a, ra::size_type n, ra::size_type m, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) 
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
    for (ra::size_type i=0;i<a.extent(0);i++) {
        for (ra::size_type j=0;j<a.extent(1);j++) 
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
  for (ra::size_type i=0;i<a.extent(0);i++) {
    for (ra::size_type j=0;j<a.extent(1);j++) 
      cout << a[i][j] << ' ';
    cout << '\n';
  }
  cout << '\n';
}

TEST_CASE("test2dconversions")
{
    const int n = 9;
    const int m = 5;
    rarray<float,2> a(n,m);
    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        a[i][j]=float(i+1)*10+float(j+1);
    rarray<float,1> atoo = a.at(1);
    atoo=a.at(1); // not really testing runtime
    const rarray<float,2>& c=a; // note the const
    std::stringstream s1,s2,s3,s4,s5,s6,s7,s8,s9;
 // print2d_1(c, a.extent(0), a.extent(1), s1); won't work, one needs:
    print2d_1(c.noconst_ptr_array(), c.extent(0), c.extent(1), s1);
    REQUIRE(s1.str()==
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
    REQUIRE(s2.str()==s1.str());
    print2d_3(c.ptr_array(), c.extent(0), c.extent(1), s3);
    REQUIRE(s3.str()==s1.str());
    print2d_4(c.ptr_array(), c.extent(0), c.extent(1), s4);
    REQUIRE(s4.str()==s1.str());
    print2d_5(a.data(), c.extent(0), c.extent(1), s5);
    REQUIRE(s5.str()==s1.str());
    print2d_6(c.data(), c.extent(0), c.extent(1), s6);
    REQUIRE(s6.str()==s1.str());
    print2d_7(c, s7);
    REQUIRE(s7.str()==s1.str());
    print2d_8(c.const_ref(), s8);
    REQUIRE(s8.str()==s1.str());
    print2d_8(c, s9);
    REQUIRE(s9.str()==s1.str());
}

#if __cpp_multidimensional_subscript >= 202110L
void print2d_723(const rarray<float,2> &a, std::ostream& cout)
{
    for (ra::size_type i=0;i<a.extent(0);i++) {
        for (ra::size_type j=0;j<a.extent(1);j++) 
            cout << a[i,j] << ' ';
        cout << '\n';
    }
    cout << '\n';
}
void print2d_823(const rarray<const float,2> &a, std::ostream& cout)
{
  for (ra::size_type i=0;i<a.extent(0);i++) {
    for (ra::size_type j=0;j<a.extent(1);j++) 
      cout << a[i,j] << ' ';
    cout << '\n';
  }
  cout << '\n';
}
TEST_CASE("test2dconversions23")
{
    const int n = 9;
    const int m = 5;
    rarray<float,2> a(n,m);
    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        a[i,j]=float(i+1)*10+float(j+1);
    rarray<float,1> atoo = a.at(1);
    atoo=a.at(1); // not really testing runtime
    const rarray<float,2>& c=a; // note the const
    std::stringstream s1,s7,s8;
    print2d_1(c.noconst_ptr_array(), c.extent(0), c.extent(1), s1);
    REQUIRE(s1.str()==
          "11 12 13 14 15 \n"
          "21 22 23 24 25 \n"
          "31 32 33 34 35 \n" 
          "41 42 43 44 45 \n"
          "51 52 53 54 55 \n"
          "61 62 63 64 65 \n"
          "71 72 73 74 75 \n"
          "81 82 83 84 85 \n"
          "91 92 93 94 95 \n\n");
    print2d_723(c, s7);
    REQUIRE(s7.str()==s1.str());
    print2d_823(c.const_ref(), s8);
    REQUIRE(s8.str()==s1.str());
}
#endif



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// print_1 takes a double-pointer tensor, whose elements and row
// pointers could be changed. Dangerous.
// - Not const-correct, but common in non-const (often C) libraries.  
// - Will require a call to no_const
void print3d_1(float***a, ra::size_type n, ra::size_type m, ra::size_type l, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            for (ra::size_type k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

// print3d_2 takes a tensor whose elements are constant, but whose row
// pora::size_typeers could in principle be changed. Dangerous, but common!
// - Not const-correct.
// - Requires a mid_const_cast of a shapeal 3d array
void print3d_2(const float***a, ra::size_type n, ra::size_type m, ra::size_type l, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++)  {
            for (ra::size_type k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

// print3d_3 takes a tensor, which is a pora::size_typeer to a set of pora::size_typeers. The
// row pora::size_typeers are constant, but the elements would be changable.
// - Not (logically) const-correct.
// - A non-const shaped 3d array can be passed right in.
void print3d_3(float *const*const* a, ra::size_type n, ra::size_type m, ra::size_type l, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++)  {
            for (ra::size_type k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

// print3d_4 takes a constant tensor, as a set of pora::size_typeers to rows. Both
// the row pora::size_typeers and the elements are const, and can't be changed.
// - Const-correct.
// - A const shaped 3d array can be passed right in.
void print3d_4(const float*const*const*a, ra::size_type n, ra::size_type m, ra::size_type l, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            for (ra::size_type k=0;k<l;k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

// print3d_5 wants the tensor as a contiguous memory block.
// because of absence of const, print3d_5 could change the elements of a. 
// Dangerous, and very common.
// - Not (logically) const-correct
// - Requires a const-cast.
void print3d_5(float *a, ra::size_type n, ra::size_type m, ra::size_type l, std::ostream& cout)
{    
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            for (ra::size_type k=0;k<l;k++) 
                cout << a[(i*m+j)*l+k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

// print3d_6 wants the constant tensor as a contiguous memory block.
// because of const, print3d_6 cannot change the elements of a. 
// - Const-correct
// - A const shapeal 3d array can be passed right in.
void print3d_6(const float *a, ra::size_type n, ra::size_type m, ra::size_type l, std::ostream& cout)
{
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            for (ra::size_type k=0;k<l;k++) 
                cout << a[(i*m+j)*l+k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

// print3d_7 takes the wrapper 3d class, which already contains its dimensions
// because of const, print3d_7 cannot change the elements of a.
// - Const-correct.
// - A non-const shaped 3d array can, of course, be passed right in.
void print3d_7(const rarray<float,3> &a, std::ostream& cout)
{
    for (ra::size_type i=0;i<a.extent(0);i++) {
        for (ra::size_type j=0;j<a.extent(1);j++) {
            for (ra::size_type k=0;k<a.extent(2);k++) 
                cout << a[i][j][k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

TEST_CASE("test3dconversions")
{
    const int n = 9;
    const int m = 5;
    const int l = 2;
    rarray<float,3> a(n,m,l);
    std::stringstream s1,s2,s3,s4,s5,s6,s7;

    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        for (int k=0;k<l;k++)
          a[i][j][k]=float(((i+1)*10+j+1)*10+k+1);
    
    const rarray<float,3>& c=a; // note the const: not enough

    //  const rarray<float,3>* pa = &a;
  //print3d_1(c, c.extent(0), c.extent(1), c.extent(2)); //won't work, one needs:
    print3d_1(c.noconst_ptr_array(), c.extent(0), c.extent(1), c.extent(2), s1); 
    REQUIRE(s1.str()==
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
    REQUIRE(s2.str()==s1.str());    
    print3d_3(c.ptr_array(), c.extent(0), c.extent(1), c.extent(2), s3); 
    REQUIRE(s3.str()==s1.str());
    print3d_4(c.ptr_array(), c.extent(0), c.extent(1), c.extent(2), s4);
    REQUIRE(s4.str()==s1.str());
 // print3d_5(c, c.extent(0), c.extent(1), c.extent(2)); won't work, one needs
    print3d_5(a.data(), c.extent(0), c.extent(1), c.extent(2), s5);
    REQUIRE(s5.str()==s1.str());
    print3d_6(c.data(), c.extent(0), c.extent(1), c.extent(2), s6);
    REQUIRE(s6.str()==s1.str());
    print3d_7(c, s7);
    REQUIRE(s7.str()==s1.str());
}

#if __cpp_multidimensional_subscript >= 202110L

void print3d_723(const rarray<float,3> &a, std::ostream& cout)
{
    for (ra::size_type i=0;i<a.extent(0);i++) {
        for (ra::size_type j=0;j<a.extent(1);j++) {
            for (ra::size_type k=0;k<a.extent(2);k++) 
                cout << a[i,j,k] << ' ';
            cout << "      \t";
        }
        cout << '\n';
    }
    cout << '\n';
}

TEST_CASE("test3dconversions23")
{
    const int n = 9;
    const int m = 5;
    const int l = 2;
    rarray<float,3> a(n,m,l);
    std::stringstream s7;
    for (int i=0;i<n;i++)
      for (int j=0;j<m;j++)
        for (int k=0;k<l;k++)
          a[i,j,k]=float(((i+1)*10+j+1)*10+k+1);    
    const rarray<float,3>& c=a;
    print3d_723(c, s7);
    REQUIRE(s7.str()==
     "111 112       \t121 122       \t131 132       \t141 142       \t151 152       \t\n"
     "211 212       \t221 222       \t231 232       \t241 242       \t251 252       \t\n"
     "311 312       \t321 322       \t331 332       \t341 342       \t351 352       \t\n"
     "411 412       \t421 422       \t431 432       \t441 442       \t451 452       \t\n"
     "511 512       \t521 522       \t531 532       \t541 542       \t551 552       \t\n"
     "611 612       \t621 622       \t631 632       \t641 642       \t651 652       \t\n"
     "711 712       \t721 722       \t731 732       \t741 742       \t751 752       \t\n"
     "811 812       \t821 822       \t831 832       \t841 842       \t851 852       \t\n"
     "911 912       \t921 922       \t931 932       \t941 942       \t951 952       \t\n\n");
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print2d(const rarray<float,2> &a) 
{
    std::stringstream s;
    const ra::size_type n = EXTENT(a,0);
    const ra::size_type m = EXTENT(a,1);
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m-1;j++)
            s << a[i][j] << ' ';
        s << a[i][m-1];
        s << '\n';
    }
    return s.str();
}

TEST_CASE("test2dautoconversion") 
{
    const int n=2;
    const int m=7;
    float b[n][m] = {{1,2,3,4,5,6,7},
                     {8,9,8,7,6,5,4}};
    const rarray<float,2> a(b);
    REQUIRE(EXTENT(a,0)==EXTENT(b,0));
    REQUIRE(EXTENT(a,1)==EXTENT(b,1));
    std::string s = print2d(rarray<float,2>(b));
    REQUIRE(s=="1 2 3 4 5 6 7\n8 9 8 7 6 5 4\n");
    
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print3d(const rarray<float,3> &a) 
{
    std::stringstream s;
    const ra::size_type n = EXTENT(a,0);
    const ra::size_type m = EXTENT(a,1);
    const ra::size_type l = EXTENT(a,2);
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            s << '{';
            for (ra::size_type k=0;k<l-1;k++)
                s << a[i][j][k] << ',';
            s << a[i][j][l-1] << '}';
        }
        s << '\n';
    }
    return s.str();
}

TEST_CASE("test3dautoconversion") 
{
    const int n=2;
    const int m=7;
    const int l=3;
    float b[n][m][l] = {{{1,2,3},{2,3,4},{3,4,5},{4,5,6},{5,6,7},{6,7,8},{7,8,9}},
                        {{8,7,6},{9,8,7},{8,7,6},{7,6,5},{6,5,4},{5,4,3},{4,3,2}}};
    const rarray<float,3> a(b);
    REQUIRE(EXTENT(a,0)==EXTENT(b,0));
    REQUIRE(EXTENT(a,1)==EXTENT(b,1));
    REQUIRE(EXTENT(a,2)==EXTENT(b,2));
    std::string s = print3d(rarray<float,3>(b));
    REQUIRE(s=="{1,2,3}{2,3,4}{3,4,5}{4,5,6}{5,6,7}{6,7,8}{7,8,9}\n"
             "{8,7,6}{9,8,7}{8,7,6}{7,6,5}{6,5,4}{5,4,3}{4,3,2}\n");
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print4d(const rarray<float,4> &a) 
{
    std::stringstream s;
    const ra::size_type n = EXTENT(a,0);
    const ra::size_type m = EXTENT(a,1);
    const ra::size_type p = EXTENT(a,2);
    const ra::size_type q = EXTENT(a,3);
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            s << '{';
            for (ra::size_type k=0;k<p;k++) {
                s << '[';
                for (ra::size_type l=0;l<q-1;l++) {
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

TEST_CASE("test4dautoconversion") 
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
    const rarray<float,4> a(b);
    REQUIRE(EXTENT(a,0)==EXTENT(b,0));
    REQUIRE(EXTENT(a,1)==EXTENT(b,1));
    REQUIRE(EXTENT(a,2)==EXTENT(b,2));
    REQUIRE(EXTENT(a,3)==EXTENT(b,3));
    std::string s = print4d(rarray<float,4>(b));
    REQUIRE(s==
          "{[1 2 3][2 3 4][3 4 5][4 5 6][5 6 7][6 7 8][7 8 9]}{[8 7 6][9 8 7][8 7 6][7 6 5][6 5 4][5 4 3][4 3 2]}\n"
          "{[1 2 3][2 3 6][3 6 5][6 5 9][5 9 7][9 7 8][7 8 9]}{[8 7 9][9 8 7][8 7 9][7 9 5][9 5 6][5 6 3][6 3 2]}\n");
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print5d(const rarray<float,5> &a) 
{
    std::stringstream s;
    const ra::size_type n = EXTENT(a,0);
    const ra::size_type m = EXTENT(a,1);
    const ra::size_type p = EXTENT(a,2);
    const ra::size_type q = EXTENT(a,3);
    const ra::size_type r = EXTENT(a,4);
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            s << '{';
            for (ra::size_type k=0;k<p;k++) {
                s << '[';
                for (ra::size_type l=0;l<q-1;l++) {
                    s << '(';
                    for (ra::size_type h=0;h<r-1;h++) {
                        s << a[i][j][k][l][h] << ',';
                    }
                    s << a[i][j][k][l][r-1];
                    s << ')';
                }
                s << '(';
                for (ra::size_type h=0;h<r-1;h++) {
                    s << a[i][j][k][q-1][h] << ',';
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

TEST_CASE("test5dautoconversion") 
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
    const rarray<float,5> a(b);
    REQUIRE(EXTENT(a,0)==EXTENT(b,0));
    REQUIRE(EXTENT(a,1)==EXTENT(b,1));
    REQUIRE(EXTENT(a,2)==EXTENT(b,2));
    REQUIRE(EXTENT(a,3)==EXTENT(b,3));
    REQUIRE(EXTENT(a,4)==EXTENT(b,4));
    std::string s = print5d(rarray<float,5>(b));
    REQUIRE(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
          "{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n");
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::string print6d(const rarray<float,6> &a) 
{
    std::stringstream s;
    const ra::size_type t = EXTENT(a,0);
    const ra::size_type n = EXTENT(a,1);
    const ra::size_type m = EXTENT(a,2);
    const ra::size_type p = EXTENT(a,3);
    const ra::size_type q = EXTENT(a,4);
    const ra::size_type r = EXTENT(a,5);
    for (ra::size_type b=0; b<t; b++) {
    for (ra::size_type i=0;i<n;i++) {
        for (ra::size_type j=0;j<m;j++) {
            s << '{';
            for (ra::size_type k=0;k<p;k++) {
                s << '[';
                for (ra::size_type l=0;l<q-1;l++) {
                    s << '(';
                    for (ra::size_type h=0;h<r-1;h++) {
                        s << a[b][i][j][k][l][h] << ',';
                    }
                    s << a[b][i][j][k][l][r-1];
                    s << ')';
                }
                s << '(';
                for (ra::size_type h=0;h<r-1;h++) {
                    s << a[b][i][j][k][q-1][h] << ',';
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

TEST_CASE("test6dautoconversion") 
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
    const rarray<float,6> a(b);
    const rarray<float,6> c(a);
    REQUIRE(EXTENT(a,0)==EXTENT(b,0));
    REQUIRE(EXTENT(a,1)==EXTENT(b,1));
    REQUIRE(EXTENT(a,2)==EXTENT(b,2));
    REQUIRE(EXTENT(a,3)==EXTENT(b,3));
    REQUIRE(EXTENT(a,4)==EXTENT(b,4));
    REQUIRE(EXTENT(a,5)==EXTENT(b,5));
    std::string s = print6d(rarray<float,6>(b));
    REQUIRE(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
"{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n\n"
"{[(1,-2,-3)(2,-3,-4)(3,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-3)(4,-3,-2)]}{[(1,-2,-3)(2,-3,-6)(3,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-3)(6,-3,-2)]}\n"
"{[(1,-2,-7)(2,-7,-4)(7,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-7)(4,-7,-2)]}{[(1,-2,-7)(2,-7,-6)(7,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-7)(6,-7,-2)]}\n\n");
    s = print6d(rarray<float,6>(c));
    REQUIRE(s==
          "{[(1,2,3)(2,3,4)(3,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,3)(4,3,2)]}{[(1,2,3)(2,3,6)(3,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,3)(6,3,2)]}\n"
"{[(1,2,7)(2,7,4)(7,4,5)(4,5,6)(5,6,7)(6,7,8)(7,8,9)][(8,7,6)(9,8,7)(8,7,6)(7,6,5)(6,5,4)(5,4,7)(4,7,2)]}{[(1,2,7)(2,7,6)(7,6,5)(6,5,9)(5,9,7)(9,7,8)(7,8,9)][(8,7,9)(9,8,7)(8,7,9)(7,9,5)(9,5,6)(5,6,7)(6,7,2)]}\n\n"
"{[(1,-2,-3)(2,-3,-4)(3,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-3)(4,-3,-2)]}{[(1,-2,-3)(2,-3,-6)(3,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-3)(6,-3,-2)]}\n"
"{[(1,-2,-7)(2,-7,-4)(7,-4,-5)(4,-5,-6)(5,-6,-7)(6,-7,-8)(7,-8,-9)][(8,-7,-6)(9,-8,-7)(8,-7,-6)(7,-6,-5)(6,-5,-4)(5,-4,-7)(4,-7,-2)]}{[(1,-2,-7)(2,-7,-6)(7,-6,-5)(6,-5,-9)(5,-9,-7)(9,-7,-8)(7,-8,-9)][(8,-7,-9)(9,-8,-7)(8,-7,-9)(7,-9,-5)(9,-5,-6)(5,-6,-7)(6,-7,-2)]}\n\n");
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testassignment")
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
    REQUIRE(b.data()==a.data());
    //REQUIRE(b.ptr_array()==a.ptr_array()); // not yet, b has its own ptr_array.
    REQUIRE(b.extent(0)==a.extent(0));
    REQUIRE(b.extent(1)==a.extent(1));
    REQUIRE(b.extent(2)==a.extent(2));
    rarray<float,2> e;
    e = a.at(2);
    REQUIRE(e.data()==a.at(2).data());
    REQUIRE(e.extent(0)==a.extent(1));
    REQUIRE(e.extent(1)==a.extent(2));    
    rarray<float,1> c(2048);
    rarray<float,1> d;
    d = c;
    REQUIRE(d.data()==c.data());
    REQUIRE(d.extent(0)==c.extent(0));
    
}


#if __cpp_multidimensional_subscript >= 202110L

TEST_CASE("testassignment23")
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
                a[i,j,k] = float(l++);
    b = a;
    REQUIRE(b.data()==a.data());
    //REQUIRE(b.ptr_array()==a.ptr_array()); // not yet, b has its own ptr_array.
    REQUIRE(b.extent(0)==a.extent(0));
    REQUIRE(b.extent(1)==a.extent(1));
    REQUIRE(b.extent(2)==a.extent(2));
    rarray<float,2> e;
    e = a.at(2);
    REQUIRE(e.data()==a.at(2).data());
    REQUIRE(e.extent(0)==a.extent(1));
    REQUIRE(e.extent(1)==a.extent(2));    
    rarray<float,1> c(2048);
    rarray<float,1> d;
    d = c;
    REQUIRE(d.data()==c.data());
    REQUIRE(d.extent(0)==c.extent(0));
    
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void testconstintermediatefunction(const rarray<float,3>& a, const float* data1check)
{
    const float* atoo=a.at(1).data();
    REQUIRE(atoo==data1check); 
    REQUIRE(a.at(1).ptr_array());
    REQUIRE(a.at(1).noconst_ptr_array());
    REQUIRE(a.at(1).const_ref().ptr_array());
    REQUIRE(a.at(1).at(2).ptr_array());
    REQUIRE(a.at(1).at(2).noconst_ptr_array());
    REQUIRE(a.at(1).at(2).const_ref().ptr_array());
}

TEST_CASE("testconstintermediate")
{
    rarray<float,3> a(7,8,9);
    int l=0;
    for (int i=0;i<7;i++)
        for (int j=0;j<8;j++)
            for (int k=0;k<9;k++)
                a[i][j][k] = float(l++);
    testconstintermediatefunction(a,a.at(1).data());
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void fill_1d_rarray(rarray<float,1> a, float value)
{
    for (unsigned int i=0; i < a.size(); i++)
        a[i] = value;
}

TEST_CASE("testintermediateconversion")
{
    rarray<float,2> a(10,10);
    a[2][7]=14;
    auto b = a.at(2);
    fill_1d_rarray(b, 13);
    REQUIRE(a[2][7]==13.f);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testreshape")
{
    ra::size_type dim[7] = {7,10,13,2,4,5,21};
    ra::size_type dimr[7] = {21,5,4,2,13,10,7};
    ra::size_type dim12[12] = {2,3,4,3,2,3,4,3,2,3,4,3};
    ra::size_type dimr12[12] = {4,3,2,3,4,3,2,3,2,3,4,3};//first 7 in reversed order
    rarray<float,1> a(dim), atoo(a.data(),a.shape());
    rarray<float,2> b(dim), b2(b);
    rarray<float,3> c(dim), ctoo(c);
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
    rarray<float,1> novela2(atoo);
    novela[3] = 4;
    novela.reshape(4, ra::RESIZE::ALLOWED);   
    a.reshape(dim);
    a.reshape(*dim);
    REQUIRE_THROWS( a.reshape(*dim+1,ra::RESIZE::ALLOWED) );
    REQUIRE(novela.extent(0)==4);
    REQUIRE(novela[3]==4.f);
    REQUIRE(novela2.extent(0)==7);
    REQUIRE_THROWS( novela2.reshape(10,ra::RESIZE::ALLOWED) );
    novela2.reshape(4, ra::RESIZE::ALLOWED);
    REQUIRE(novela2.extent(0)==4);
    REQUIRE(novela2[3]==4.f);
    b[5][6] = 5;
    rarray<float,2> novelb(b);
    rarray<float,2> novelb2(novelb);
    novelb.reshape(10,7);
    b.reshape(dim);
    REQUIRE_THROWS( b.reshape(dim[0]+1,dim[1]+1,ra::RESIZE::ALLOWED) );
    REQUIRE(novelb.extent(0)==10);
    REQUIRE(novelb.extent(1)==7);
    REQUIRE(novelb2.extent(0)==7);
    REQUIRE(novelb2.extent(1)==10);
    REQUIRE(novelb[8][0] == 5.f);
    c[4][8][3] = 6;
    rarray<float,3> novelc(c);
    novelc.reshape(10,7,13);
    REQUIRE_THROWS( novelc.reshape(1000,1000,1000,ra::RESIZE::ALLOWED) );
    REQUIRE(novelc.extent(0)==10);
    REQUIRE(novelc.extent(1)==7);
    REQUIRE(novelc.extent(2)==13);
    REQUIRE(novelc[6][6][3] == 6.f);
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
    noveld.reshape(2,2,2,2, ra::RESIZE::ALLOWED); 
    REQUIRE(noveld.extent(0)==2);
    REQUIRE(noveld.extent(1)==2);
    REQUIRE(noveld.extent(2)==2);
    REQUIRE(noveld.extent(3)==2);
    REQUIRE_THROWS( noveld.reshape(10,10,10,10, ra::RESIZE::ALLOWED) );
    novele.reshape(13,7,10,2,4, ra::RESIZE::ALLOWED);
    REQUIRE(novele.extent(0)==13);
    REQUIRE(novele.extent(1)==7);
    REQUIRE(novele.extent(2)==10);
    REQUIRE(novele.extent(3)==2);
    REQUIRE(novele.extent(4)==4);
    REQUIRE_THROWS( novele.reshape(100,100,100,100,100, ra::RESIZE::ALLOWED) ); 
    novelf.reshape(5,6,1,13,10,7, ra::RESIZE::ALLOWED);
    REQUIRE(novelf.extent(0)==5);
    REQUIRE(novelf.extent(1)==6);
    REQUIRE(novelf.extent(2)==1);
    REQUIRE(novelf.extent(3)==13);
    REQUIRE(novelf.extent(4)==10);
    REQUIRE(novelf.extent(5)==7);
    REQUIRE_THROWS( novelf.reshape(100,100,100,100,100,100, ra::RESIZE::ALLOWED) );
    novelg.reshape(dimr, ra::RESIZE::ALLOWED);
    REQUIRE(novelg.extent(0)==dimr[0]);
    REQUIRE(novelg.extent(1)==dimr[1]);
    REQUIRE(novelg.extent(2)==dimr[2]);
    REQUIRE(novelg.extent(3)==dimr[3]);
    REQUIRE(novelg.extent(4)==dimr[4]);
    REQUIRE(novelg.extent(5)==dimr[5]);
    REQUIRE(novelg.extent(6)==dimr[6]);
    novelg.reshape(2,5,6,1,13,10,7, ra::RESIZE::ALLOWED);  
    REQUIRE(novelg.extent(0)==2);
    REQUIRE(novelg.extent(1)==5);
    REQUIRE(novelg.extent(2)==6);
    REQUIRE(novelg.extent(3)==1);
    REQUIRE(novelg.extent(4)==13);
    REQUIRE(novelg.extent(5)==10);
    REQUIRE(novelg.extent(6)==7);
    REQUIRE_THROWS( novelg.reshape(100,100,100,100,100,100,100, ra::RESIZE::ALLOWED) );
    novelh.reshape(4,3,2,3,4,3,2,3, ra::RESIZE::ALLOWED);
    REQUIRE(novelh.extent(0)==4);
    REQUIRE(novelh.extent(1)==3);
    REQUIRE(novelh.extent(2)==2);
    REQUIRE(novelh.extent(3)==3);
    REQUIRE(novelh.extent(4)==4);
    REQUIRE(novelh.extent(5)==3);
    REQUIRE(novelh.extent(6)==2);
    REQUIRE(novelh.extent(7)==3);
    REQUIRE_THROWS( novelh.reshape(100,100,100,100,100,100,100,100, ra::RESIZE::ALLOWED) );
    noveli.reshape(4,3,2,3,4,3,2,3,2, ra::RESIZE::ALLOWED);
    REQUIRE(noveli.extent(0)==4);
    REQUIRE(noveli.extent(1)==3);
    REQUIRE(noveli.extent(2)==2);
    REQUIRE(noveli.extent(3)==3);
    REQUIRE(noveli.extent(4)==4);
    REQUIRE(noveli.extent(5)==3);
    REQUIRE(noveli.extent(6)==2);
    REQUIRE(noveli.extent(7)==3);
    REQUIRE(noveli.extent(8)==2);
    REQUIRE_THROWS( noveli.reshape(100,100,100,100,100,100,100,100,100, ra::RESIZE::ALLOWED) );
    novelj.reshape(4,3,2,3,4,3,2,3,2,3, ra::RESIZE::ALLOWED);
    REQUIRE(novelj.extent(0)==4);
    REQUIRE(novelj.extent(1)==3);
    REQUIRE(novelj.extent(2)==2);
    REQUIRE(novelj.extent(3)==3);
    REQUIRE(novelj.extent(4)==4);
    REQUIRE(novelj.extent(5)==3);
    REQUIRE(novelj.extent(6)==2);
    REQUIRE(novelj.extent(7)==3);
    REQUIRE(novelj.extent(8)==2);
    REQUIRE(novelj.extent(9)==3);
    REQUIRE_THROWS( novelj.reshape(100,100,100,100,100,100,100,100,100,100, ra::RESIZE::ALLOWED) );
    novelk.reshape(4,3,2,3,4,3,2,3,2,3,4, ra::RESIZE::ALLOWED);
    REQUIRE(novelk.extent(0)==4);
    REQUIRE(novelk.extent(1)==3);
    REQUIRE(novelk.extent(2)==2);
    REQUIRE(novelk.extent(3)==3);
    REQUIRE(novelk.extent(4)==4);
    REQUIRE(novelk.extent(5)==3);
    REQUIRE(novelk.extent(6)==2);
    REQUIRE(novelk.extent(7)==3);
    REQUIRE(novelk.extent(8)==2);
    REQUIRE(novelk.extent(9)==3);
    REQUIRE(novelk.extent(10)==4);
    REQUIRE_THROWS( novelk.reshape(100,100,100,100,100,100,100,100,100,100,100, ra::RESIZE::ALLOWED) );
    novell.reshape(dimr12);
    REQUIRE(novell.extent(0)==dimr12[0]);
    REQUIRE(novell.extent(1)==dimr12[1]);
    REQUIRE(novell.extent(2)==dimr12[2]);
    REQUIRE(novell.extent(3)==dimr12[3]);
    REQUIRE(novell.extent(4)==dimr12[4]);
    REQUIRE(novell.extent(5)==dimr12[5]);
    REQUIRE(novell.extent(6)==dimr12[6]);
    REQUIRE(novell.extent(7)==dimr12[7]);
    REQUIRE(novell.extent(8)==dimr12[8]);
    REQUIRE(novell.extent(9)==dimr12[9]);
    REQUIRE(novell.extent(10)==dimr12[10]);
    REQUIRE(novell.extent(11)==dimr12[11]);
    dimr12[0] += 1000;
    REQUIRE_THROWS( novell.reshape(dimr12) );
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("test7dautoconversion")
{
    int seven[2][2][2][2][2][2][2] = {{{{{{{0}}}}}}}; 
    char expected_output[] =
           "{\n{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n},\n"
            "{\n{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n},\n"
             "{\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n},\n{\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n},\n{\n{\n{0,0},\n{0,0}\n},\n{\n{0,0},\n{0,0}\n}\n}\n}\n}\n}\n}";

    std::stringstream s;
    s << rarray<int,7>(seven);
    REQUIRE(s.str()==expected_output);
    REQUIRE(EXTENT(seven,0) == 2);
    REQUIRE(EXTENT(seven,1) == 2);
    REQUIRE(EXTENT(seven,2) == 2);
    REQUIRE(EXTENT(seven,3) == 2);
    REQUIRE(EXTENT(seven,4) == 2);
    REQUIRE(EXTENT(seven,5) == 2);
    REQUIRE(EXTENT(seven,6) == 2);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

TEST_CASE("test8dautoconversion") 
{
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
    s << rarray<int,8>(eight);
    REQUIRE(s.str()==expected_output);
    REQUIRE(EXTENT(eight,0) == 2);
    REQUIRE(EXTENT(eight,1) == 2);
    REQUIRE(EXTENT(eight,2) == 2);
    REQUIRE(EXTENT(eight,3) == 2);
    REQUIRE(EXTENT(eight,4) == 2);
    REQUIRE(EXTENT(eight,5) == 2);
    REQUIRE(EXTENT(eight,6) == 2);
    REQUIRE(EXTENT(eight,7) == 2);
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

TEST_CASE("test9dautoconversion")
{
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
    s << rarray<int,9>(nine);
    REQUIRE(s.str()==expected_output);
    REQUIRE(EXTENT(nine,0) == 2);
    REQUIRE(EXTENT(nine,1) == 2);
    REQUIRE(EXTENT(nine,2) == 2);
    REQUIRE(EXTENT(nine,3) == 2);
    REQUIRE(EXTENT(nine,4) == 2);
    REQUIRE(EXTENT(nine,5) == 2);
    REQUIRE(EXTENT(nine,6) == 2);
    REQUIRE(EXTENT(nine,7) == 2);
    REQUIRE(EXTENT(nine,8) == 2);
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

TEST_CASE("test10dautoconversion")
{
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
    s << rarray<int,10>(ten);
    REQUIRE(s.str()==expected_output);
    REQUIRE(EXTENT(ten,0) == 2);
    REQUIRE(EXTENT(ten,1) == 2);
    REQUIRE(EXTENT(ten,2) == 2);
    REQUIRE(EXTENT(ten,3) == 2);
    REQUIRE(EXTENT(ten,4) == 2);
    REQUIRE(EXTENT(ten,5) == 2);
    REQUIRE(EXTENT(ten,6) == 2);
    REQUIRE(EXTENT(ten,7) == 2);
    REQUIRE(EXTENT(ten,8) == 2);
    REQUIRE(EXTENT(ten,9) == 2);
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

TEST_CASE("test11dautoconversion")
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
    s << rarray<int,11>(eleven);
    REQUIRE(s.str()==expected_output);
    REQUIRE(EXTENT(eleven,0) == 2);
    REQUIRE(EXTENT(eleven,1) == 2);
    REQUIRE(EXTENT(eleven,2) == 2);
    REQUIRE(EXTENT(eleven,3) == 2);
    REQUIRE(EXTENT(eleven,4) == 2);
    REQUIRE(EXTENT(eleven,5) == 2);
    REQUIRE(EXTENT(eleven,6) == 2);
    REQUIRE(EXTENT(eleven,7) == 2);
    REQUIRE(EXTENT(eleven,8) == 2);
    REQUIRE(EXTENT(eleven,9) == 2);
    REQUIRE(EXTENT(eleven,10) == 2);
}


TEST_CASE("test12failextent")
{
    int twelve[2][2][2][2][2][2][2][2][2][2][2][2];
    REQUIRE(EXTENT(twelve,0) == 2);
    REQUIRE(EXTENT(twelve,1) == 2);
    REQUIRE(EXTENT(twelve,2) == 2);
    REQUIRE(EXTENT(twelve,3) == 2);
    REQUIRE(EXTENT(twelve,4) == 2);
    REQUIRE(EXTENT(twelve,5) == 2);
    REQUIRE(EXTENT(twelve,6) == 2);
    REQUIRE(EXTENT(twelve,7) == 2);
    REQUIRE(EXTENT(twelve,8) == 2);
    REQUIRE(EXTENT(twelve,9) == 2);
    REQUIRE(EXTENT(twelve,10) == 2);
    REQUIRE_THROWS(EXTENT(twelve,11)==2);
}

TEST_CASE("testzerooutput")
{
    char expected_output[] = "{{{{{{{}}}}}}}\n";
    std::stringstream s;
    s << rarray<int,7>();
    REQUIRE(s.str()==expected_output);
}

TEST_CASE("testinput")
{
    char input[] = "{{1,2},{3,4}}";
    std::stringstream s(input);
    rarray<int,2> d;
    s >> d;
    REQUIRE(d[0][0] == 1);
    REQUIRE(d[0][1] == 2);
    REQUIRE(d[1][0] == 3);
    REQUIRE(d[1][1] == 4);

    std::stringstream smallerinput("{{11},{22}}");
    smallerinput >> d;
    REQUIRE(d.extent(0) == 2);
    REQUIRE(d.extent(1) == 1);
    REQUIRE(d[0][0] == 11);
    REQUIRE(d[1][0] == 22);
        
    char wronginput[] = "{3,4}";
    std::stringstream wrongs(wronginput);
    rarray<int,2> wrongd;
    REQUIRE_THROWS(wrongs >> wrongd);
    REQUIRE(wrongs.fail());
    REQUIRE(wrongd.size() == 0);
}


#if __cpp_multidimensional_subscript >= 202110L

TEST_CASE("testinput23")
{
    char input[] = "{{1,2},{3,4}}";
    std::stringstream s(input);
    rarray<int,2> d;
    s >> d;
    REQUIRE(d[0,0] == 1);
    REQUIRE(d[0,1] == 2);
    REQUIRE(d[1,0] == 3);
    REQUIRE(d[1,1] == 4);

    std::stringstream smallerinput("{{11},{22}}");
    smallerinput >> d;
    REQUIRE(d.extent(0) == 2);
    REQUIRE(d.extent(1) == 1);
    REQUIRE(d[0,0] == 11);
    REQUIRE(d[1,0] == 22);
    
    char wronginput[] = "{3,4}";
    std::stringstream wrongs(wronginput);
    rarray<int,2> wrongd;
    REQUIRE_THROWS(wrongs >> wrongd);
    REQUIRE(wrongs.fail());
    REQUIRE(wrongd.size() == 0);
}

#endif


///////////////////////////////////////////////////////////////////


TEST_CASE("testcomplexinput")
{
    std::stringstream instr("{{(0,0),(1,0)},{(0,1),(1,1)}}");  
    std::complex<float> zero(0,0), one(1,0), i(0,1), oneplusi(1,1);
    rarray<std::complex<float>,2> cfarray;
    instr >> cfarray;
    REQUIRE(cfarray.extent(0) == 2);
    REQUIRE(cfarray.extent(1) == 2);
    REQUIRE(cfarray[0][0] == zero);
    REQUIRE(cfarray[0][1] == one);
    REQUIRE(cfarray[1][0] == i);
    REQUIRE(cfarray[1][1] == oneplusi);
    std::stringstream wronginstr("{{(0,0),(1,0)}bla,{(0,1),(1,1)}}");
    REQUIRE_THROWS(wronginstr >> cfarray);
}

///////////////////////////////////////////////////////////////////

TEST_CASE("testoutput")
{
    double a[5]={1,2,3,4,5};
    double b[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    double c[27]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
    rarray<double,1> q(a,5);
    rarray<double,2> r(b,4,4);
    rarray<double,3> s(c,3,3,3);
    std::stringstream out;
    out << q << r << s;
    std::string outstr1 = out.str();
    REQUIRE(outstr1 == "{1,2,3,4,5}{\n{1,2,3,4},\n{5,6,7,8},\n{9,10,11,12},\n{13,14,15,16}\n}{\n{\n{1,2,3},\n{4,5,6},\n{7,8,9}\n},\n{\n{10,11,12},\n{13,14,15},\n{16,17,18}\n},\n{\n{19,20,21},\n{22,23,24},\n{25,26,27}\n}\n}");

    std::stringstream instr("  \t\n{{{#2:14,5},{2,#3:{}2},{#7:{1,2,3},1}},{{4},{5,5},{6,6}},{{7,7},{8,8},{9,9}}}");
    std::string outstr("{\n{\n{14,5},\n{2,0},\n{0,1}\n},\n{\n{4,0},\n{5,5},\n{6,6}\n},\n{\n{7,7},\n{8,8},\n{9,9}\n}\n}");
    
    rarray<int,3> intarray;

    instr >> intarray;
    intarray[1][0][1] = 0;
    intarray[0][2][0] = 0;

    std::stringstream check;
    check << intarray;
    REQUIRE(check.str()==outstr);

    rarray<std::string,2> A(2,2);
    A[0][0] = "Hello, world";
    A[0][1] = "I like { and }";
    A[1][0] = "I prefer #";
    A[1][1] = "I'm easy.";
    
    rarray<std::string,2> B;

    std::stringstream sin("{{#12:Hello, world,#14:I like { and }},{#10:I prefer #,I'm easy.}}");
    sin >> B;

    REQUIRE(A[0][0] == B[0][0]);
    REQUIRE(A[0][1] == B[0][1]);
    REQUIRE(A[1][0] == B[1][0]);
    REQUIRE(A[1][1] == B[1][1]);

    float autoarr[2][2][2][2] = {{{{1,2},{3,4}},{{5,6},{7,8}}},{{{9,10},{11,12}},{{13,14},{15,16}}}};
    const std::string outcheck = "{\n{\n{\n{1,2},\n{3,4}\n},\n{\n{5,6},\n{7,8}\n}\n},\n{\n{\n{9,10},\n{11,12}\n},\n{\n{13,14},\n{15,16}\n}\n}\n}";
    std::stringstream sautoarr;
    sautoarr << RARRAY(autoarr);
    REQUIRE(sautoarr.str()==outcheck);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testiterators")
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
    for (rarray<double,2>::const_iterator i=r.at(1).cbegin(); i!=r.at(1).cend(); i++)
    {
        qout << *i << ',';
    }
    REQUIRE(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,");
    const rarray<double,2> rview = r;
    for (rarray<double,2>::const_iterator i=rview.begin(); i!=rview.end(); i++)
    {
        qout << *i << ',';
    }
    REQUIRE(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,");
    std::stringstream rout;
#if __cplusplus <= 199711L
    for (auto ap = q.begin(); ap != q.end(); ++ap)
    {
        *ap *= 2;
    }
#else
    for (auto& aa: q)
    {
        aa *= 2;
    }
#endif
    for (rarray<double,2>::const_iterator i=q.cbegin(); i!=q.cend(); i++)
    {
        qout << *i << ',';
    }
    REQUIRE(qout.str() == "3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,7,8,9,10,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,2,4,6,8,10,");
    const rarray<double,1> qconst = q;
#if __cplusplus <= 199711L
    for (auto bp=qconst.begin(); bp != qconst.end(); ++bp)
    {
        rout << (*bp) << ',';
    }
#else
    for (const auto& bb: qconst)
    {
        rout << bb << ',';
    }
#endif
    REQUIRE(rout.str() == "2,4,6,8,10,");
    std::stringstream check;
    for (auto& cc: s.at(1))
        cc *= 2;
    for (auto& dd: s.at(1).at(2))
        dd += 10;
    for (const auto& cc: s.at(1))
        check << cc << ',';
    for (rarray<double,2>::const_iterator i=s.at(2).cbegin(); i!=s.at(2).cend(); i++)
    {
        check << *i << ',';
    }
    REQUIRE(check.str() == "20,22,24,26,28,30,42,44,46,19,20,21,22,23,24,25,26,27,");
    auto sb = s.begin();
    auto se = s.end();
    REQUIRE(not (sb==se));
    REQUIRE(sb < se);
    REQUIRE(sb <= se);
    REQUIRE(se > sb);
    REQUIRE(se >= sb);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testfill")
{
    rarray<float,2> a(3,3);
    a.fill(1.23f);
    for (ra::size_type i=0;i<EXTENT(a,0);i++) {
        for (ra::size_type j=0;j<EXTENT(a,1);j++) {
            REQUIRE(a[i][j]==1.23f);
        }
    }    
    rarray<float,1> b(5);
    b.fill(1.24f);
    for (ra::size_type i=0;i<EXTENT(a,0);i++) {
        REQUIRE(b[i]==1.24f);
    }
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("testassign1", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,1> a;
    a.assign(12, x1);
    REQUIRE(a.extent(0) == 12); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign2", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,2> a;
    a.assign(3,4, x1);
    REQUIRE(a.extent(0)==3);
    REQUIRE(a.extent(1)==4);
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign3", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,3> a;
    a.assign(2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
         REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign4", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,4> a;
    a.assign(2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign5", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,5> a;
    a.assign(2,2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign6", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,6> a;
    a.assign(2,2,2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign7", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
 {
     using T = TestType;
     T x1 = global::get_value_1<T>();
     rarray<T,7> a;
     a.assign(2,2,2,2,2,2,2, x1);
     for (const auto& i: xrange(a.rank()))
         REQUIRE(a.extent(i) == 2); 
     for (const auto& x: a) 
         REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign8", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,8> a;
    a.assign(2,2,2,2,2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign9", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,9> a;
    a.assign(2,2,2,2,2,2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign10", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,10> a;
    a.assign(2,2,2,2,2,2,2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

TEMPLATE_TEST_CASE("testassign11", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
{
    using T = TestType;
    T x1 = global::get_value_1<T>();
    rarray<T,11> a;
    a.assign(2,2,2,2,2,2,2,2,2,2,2, x1);
    for (const auto& i: xrange(a.rank()))
        REQUIRE(a.extent(i) == 2); 
    for (const auto& x: a) 
        REQUIRE(x==x1);
}

#if __cpp_multidimensional_subscript >= 202110L

TEST_CASE("testfill23")
{
    rarray<float,2> a(3,3);
    a.fill(1.23f);
    for (ra::size_type i=0;i<EXTENT(a,0);i++) {
        for (ra::size_type j=0;j<EXTENT(a,1);j++) {
            REQUIRE(a[i,j]==1.23f);
        }
    }    
    rarray<float,1> b(5);
    b.fill(1.24f);
    for (ra::size_type i=0;i<EXTENT(a,0);i++) {
        REQUIRE(b[i]==1.24f);
    }
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testindex")
{
    rarray<float,1> a(6);
    REQUIRE(a.index(a[0])[0]==0);
    REQUIRE(a.index(a[1])[0]==1);
    REQUIRE(a.index(a[2])[0]==2);
    REQUIRE(a.index(a[3])[0]==3);
    REQUIRE(a.index(a[4])[0]==4);
    REQUIRE(a.index(a[5])[0]==5);
    REQUIRE(a.index(a[0],0)==0);
    REQUIRE(a.index(a[1],0)==1);
    REQUIRE(a.index(a[2],0)==2);
    REQUIRE(a.index(a[3],0)==3);
    REQUIRE(a.index(a[4],0)==4);
    REQUIRE(a.index(a[5],0)==5);
    REQUIRE(INDEX(a,a[0],0)==0);
    REQUIRE(INDEX(a,a[1],0)==1);
    REQUIRE(INDEX(a,a[2],0)==2);
    REQUIRE(INDEX(a,a[3],0)==3);
    REQUIRE(INDEX(a,a[4],0)==4);
    REQUIRE(INDEX(a,a[5],0)==5);
    REQUIRE_THROWS(a.index(a[10],0));
    for (auto i=a.begin(); i != a.end(); i++) {
        auto ind = a.index(i);
        ra::size_type ind2=a.index(i,0);
        REQUIRE(ind[0]==ind2);
        *i =float(ind[0]+1);
    }
    for (auto& element: a)
        element *= float(a.index(element)[0]);
    for (auto& element: a)
        element *= float(a.index(element,0));
    REQUIRE(a[0]==0.f);
    REQUIRE(a[1]==2.f);
    REQUIRE(a[2]==12.f);
    REQUIRE(a[3]==36.f);
    REQUIRE(a[4]==80.f);
    REQUIRE(a[5]==150.f);

    rarray<float,3> z(2,3,4.f);
    REQUIRE(z.index(z[1][2][3],0)==1);
    REQUIRE(z.index(z[1][2][3],1)==2);
    REQUIRE(z.index(z[1][2][3],2)==3);


    rarray<float,3> b(2,2,2);
    REQUIRE(b.index(b[0][0][0])[0]==0);
    REQUIRE(b.index(b[0][0][1])[0]==0);
    REQUIRE(b.index(b[0][1][0])[0]==0);
    REQUIRE(b.index(b[0][1][1])[0]==0);
    REQUIRE(b.index(b[1][0][0])[0]==1);
    REQUIRE(b.index(b[1][0][1])[0]==1);
    REQUIRE(b.index(b[1][1][0])[0]==1);
    REQUIRE(b.index(b[1][1][1])[0]==1);
    REQUIRE(b.index(b[0][0][0])[1]==0);
    REQUIRE(b.index(b[0][0][1])[1]==0);
    REQUIRE(b.index(b[0][1][0])[1]==1);
    REQUIRE(b.index(b[0][1][1])[1]==1);
    REQUIRE(b.index(b[1][0][0])[1]==0);
    REQUIRE(b.index(b[1][0][1])[1]==0);
    REQUIRE(b.index(b[1][1][0])[1]==1);
    REQUIRE(b.index(b[1][1][1])[1]==1);
    REQUIRE(b.index(b[0][0][0])[2]==0);
    REQUIRE(b.index(b[0][0][1])[2]==1);
    REQUIRE(b.index(b[0][1][0])[2]==0);
    REQUIRE(b.index(b[0][1][1])[2]==1);
    REQUIRE(b.index(b[1][0][0])[2]==0);
    REQUIRE(b.index(b[1][0][1])[2]==1);
    REQUIRE(b.index(b[1][1][0])[2]==0);
    REQUIRE(b.index(b[1][1][1])[2]==1);

    float rbuf[3][3] = { {0,0,0}, 
                         {1,1,1}, 
                         {2,2,2} }; 
    float cbuf[3][3] = { {0,1,2}, 
                         {0,1,2},
                         {0,1,2} }; 
    rarray<float,2> r = RARRAY(rbuf);
    rarray<float,2> c = RARRAY(cbuf);
    //rarray<float,2> c(cbuf);

    for (auto i=r.begin(); i != r.end(); i++) {
        REQUIRE(r.index(*i)[0]==*i);
    }
    
    for (auto i=c.begin(); i != c.end(); i++) {
        REQUIRE(c.index(i)[1]==*i);
    }
}

#if __cpp_multidimensional_subscript >= 202110L

TEST_CASE("testindex23")
{
    rarray<float,3> z(2,3,4);
    REQUIRE(z.index(z[1,2,3],0)==1);
    REQUIRE(z.index(z[1,2,3],1)==2);
    REQUIRE(z.index(z[1,2,3],2)==3);


    rarray<float,3> b(2,2,2);
    REQUIRE(b.index(b[0,0,0])[0]==0);
    REQUIRE(b.index(b[0,0,1])[0]==0);
    REQUIRE(b.index(b[0,1,0])[0]==0);
    REQUIRE(b.index(b[0,1,1])[0]==0);
    REQUIRE(b.index(b[1,0,0])[0]==1);
    REQUIRE(b.index(b[1,0,1])[0]==1);
    REQUIRE(b.index(b[1,1,0])[0]==1);
    REQUIRE(b.index(b[1,1,1])[0]==1);
    REQUIRE(b.index(b[0,0,0])[1]==0);
    REQUIRE(b.index(b[0,0,1])[1]==0);
    REQUIRE(b.index(b[0,1,0])[1]==1);
    REQUIRE(b.index(b[0,1,1])[1]==1);
    REQUIRE(b.index(b[1,0,0])[1]==0);
    REQUIRE(b.index(b[1,0,1])[1]==0);
    REQUIRE(b.index(b[1,1,0])[1]==1);
    REQUIRE(b.index(b[1,1,1])[1]==1);
    REQUIRE(b.index(b[0,0,0])[2]==0);
    REQUIRE(b.index(b[0,0,1])[2]==1);
    REQUIRE(b.index(b[0,1,0])[2]==0);
    REQUIRE(b.index(b[0,1,1])[2]==1);
    REQUIRE(b.index(b[1,0,0])[2]==0);
    REQUIRE(b.index(b[1,0,1])[2]==1);
    REQUIRE(b.index(b[1,1,0])[2]==0);
    REQUIRE(b.index(b[1,1,1])[2]==1);

    float rbuf[3][3] = { {0,0,0}, 
                         {1,1,1}, 
                         {2,2,2} }; 
    float cbuf[3][3] = { {0,1,2}, 
                         {0,1,2},
                         {0,1,2} }; 
    rarray<float,2> r = RARRAY(rbuf);
    rarray<float,2> c = RARRAY(cbuf);

    for (auto i=r.begin(); i != r.end(); i++) {
        REQUIRE(r.index(*i)[0]==*i);
    }
    
    for (auto i=c.begin(); i != c.end(); i++) {
        REQUIRE(c.index(i)[1]==*i);
    }
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testcomma_assignment")
{
    rarray<double,1> emptyarray;
    try {        
        emptyarray = 1,2;
        #ifdef RA_BOUNDSCHECK       
        REQUIRE(false);
        #endif
    } catch (...) { REQUIRE(true); }
    emptyarray = rarray<double,1>(ra::size_type(0));
    try {
        emptyarray = 1;
        #ifdef RA_BOUNDSCHECK
        REQUIRE(false);
        #endif
    } catch(...) { REQUIRE(true); }
    REQUIRE(emptyarray.size() == 0);
    rarray<double,2> emptyarray2;
    emptyarray2 = rarray<double,2>(ra::size_type(0),ra::size_type(0));
    try {
        emptyarray2 = 1;
        #ifdef RA_BOUNDSCHECK
        REQUIRE(false);
        #endif
    } catch(...) { REQUIRE(true); }
    REQUIRE(emptyarray2.size() == 0);
    rarray<double,1> b(8);
    b.fill(0);
    b = 1,2,3,6,5,4;
    REQUIRE(b[0]==1.);
    REQUIRE(b[1]==2.);
    REQUIRE(b[2]==3.);
    REQUIRE(b[3]==6.);
    REQUIRE(b[4]==5.);
    REQUIRE(b[5]==4.);
    REQUIRE(b[6]==0.);
    REQUIRE(b[7]==0.);

    rarray<double,3> a(3,4,2);

    a =  1,2,    3,6,   5,4,   7,8, 
         9,12,  11,10, 21,22, 23,26, 
        25,24,  27,28, 29,32, 31,30;
    REQUIRE(a[0][0][0]== 1.);
    REQUIRE(a[0][0][1]== 2.);
    REQUIRE(a[0][1][0]== 3.);
    REQUIRE(a[0][1][1]== 6.);
    REQUIRE(a[0][2][0]== 5.);
    REQUIRE(a[0][2][1]== 4.);
    REQUIRE(a[0][3][0]== 7.);
    REQUIRE(a[0][3][1]== 8.);
    REQUIRE(a[1][0][0]== 9.);
    REQUIRE(a[1][0][1]==12.);
    REQUIRE(a[1][1][0]==11.);
    REQUIRE(a[1][1][1]==10.);
    REQUIRE(a[1][2][0]==21.);
    REQUIRE(a[1][2][1]==22.);
    REQUIRE(a[1][3][0]==23.);
    REQUIRE(a[1][3][1]==26.);
    REQUIRE(a[2][0][0]==25.);
    REQUIRE(a[2][0][1]==24.);
    REQUIRE(a[2][1][0]==27.);
    REQUIRE(a[2][1][1]==28.);
    REQUIRE(a[2][2][0]==29.);
    REQUIRE(a[2][2][1]==32.);
    REQUIRE(a[2][3][0]==31.);
    REQUIRE(a[2][3][1]==30.);

    REQUIRE_THROWS( a.at(-1) );
    REQUIRE_THROWS( a.at(100000000) );

    a.at(1)             = 100,101,102,103,104,105,106,107;
    a.at(2).at(1)       = 200,201;
    a.at(2).at(2).at(0) = 300; // = 300,301; //  forgets the 301
    a.at(2).at(3).at(0) = (301); // (300,301); // forgets the 300
    a.at(2).at(3)[0] = (301); // (300,301); // on purpose using built-in comma operator which forgets the 300
    REQUIRE(a[0][0][0]== 1.);
    REQUIRE(a[0][0][1]== 2.);
    REQUIRE(a[0][1][0]== 3.);
    REQUIRE(a[0][1][1]== 6.);
    REQUIRE(a[0][2][0]== 5.);
    REQUIRE(a[0][2][1]== 4.);
    REQUIRE(a[0][3][0]== 7.);
    REQUIRE(a[0][3][1]== 8.);
    REQUIRE(a[1][0][0]==100.);
    REQUIRE(a[1][0][1]==101.);
    REQUIRE(a[1][1][0]==102.);
    REQUIRE(a[1][1][1]==103.);
    REQUIRE(a[1][2][0]==104.);
    REQUIRE(a[1][2][1]==105.);
    REQUIRE(a[1][3][0]==106.);
    REQUIRE(a[1][3][1]==107.);
    REQUIRE(a[2][0][0]==25.);
    REQUIRE(a[2][0][1]==24.);
    REQUIRE(a[2][1][0]==200.);
    REQUIRE(a[2][1][1]==201.);
    REQUIRE(a[2][2][0]==300.);
    REQUIRE(a[2][2][1]==32.);
    REQUIRE(a[2][3][0]==301.);
    REQUIRE(a[2][3][1]==30.);

}

#if __cpp_multidimensional_subscript >= 202110L

TEST_CASE("testcomma_assignment23")
{
    rarray<double,3> a(3,4,2);

    a =  1,2,    3,6,   5,4,   7,8, 
         9,12,  11,10, 21,22, 23,26, 
        25,24,  27,28, 29,32, 31,30;
    REQUIRE(a[0,0,0]== 1.);
    REQUIRE(a[0,0,1]== 2.);
    REQUIRE(a[0,1,0]== 3.);
    REQUIRE(a[0,1,1]== 6.);
    REQUIRE(a[0,2,0]== 5.);
    REQUIRE(a[0,2,1]== 4.);
    REQUIRE(a[0,3,0]== 7.);
    REQUIRE(a[0,3,1]== 8.);
    REQUIRE(a[1,0,0]== 9.);
    REQUIRE(a[1,0,1]==12.);
    REQUIRE(a[1,1,0]==11.);
    REQUIRE(a[1,1,1]==10.);
    REQUIRE(a[1,2,0]==21.);
    REQUIRE(a[1,2,1]==22.);
    REQUIRE(a[1,3,0]==23.);
    REQUIRE(a[1,3,1]==26.);
    REQUIRE(a[2,0,0]==25.);
    REQUIRE(a[2,0,1]==24.);
    REQUIRE(a[2,1,0]==27.);
    REQUIRE(a[2,1,1]==28.);
    REQUIRE(a[2,2,0]==29.);
    REQUIRE(a[2,2,1]==32.);
    REQUIRE(a[2,3,0]==31.);
    REQUIRE(a[2,3,1]==30.);
}

#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testlinspace")
{
    int a = 1, b = 30;
    auto r = linspace(a,b);
    int i = a;
    for (auto x: r) {
        REQUIRE(x==i);
        i++;
    }
    auto rev = linspace(b,a);
    int irev = b;
    for (auto x: rev) {
        REQUIRE(x==irev);
        irev--;
    }
    auto r2 = linspace(0,30,4);
    int check2[] = {0,10,20,30};
    int j = 0;
    for (auto y: r2) {
        REQUIRE(y == check2[j]);
        j++;
    }
    auto r3 = linspace(0,30,3,false);
    int check3[] = {0,10,20};
    int k = 0;
    for (auto z: r3) {
        REQUIRE(z == check3[k]);
        k++;
    }
    auto r4 = linspace(0.0, 30.0, 4);
    double check4[] = {0.0, 10.0, 20.0, 30.0};
    int l = 0;
    for (auto zz: r3) {
        REQUIRE(zz == check4[l]);
        l++;
    }
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testsort")
{
    int n=10;
    rvector<double> s(n);
    s = 4.1, 4.2, 4.3, 4.0, 3.1,
        4.4, 5.0, -1.1, -2.2, 4.5;
    std::sort(s.begin(), s.end());
    // expected:
    rvector<double> e(n);
    e = -2.2, -1.1, 3.1, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 5.0;
    for (int i=0;i<n;i++)
        REQUIRE(s[i]==e[i]);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testxrange")
{
    int i=0;
    for (auto z: xrange(10))
        REQUIRE(z==i++);
    REQUIRE(i==10);
    i=2;
    for (auto z: xrange(2, 7))
        REQUIRE(z==i++);
    REQUIRE(i==7);
    i=4;
    for (auto z: xrange(4, 1, -1))
        REQUIRE(z==i--);
    REQUIRE(i==1);
    i=4;
    for (auto z: xrange(4.25, 1.25, -1))
        REQUIRE(z==.25+(i--));
    REQUIRE(i==1);
    i=4;
    for (auto z: xrange(4.25, 1.20, -1))
        REQUIRE(z==.25+(i--));
    REQUIRE(i==0);
    int sum=0;
    for (auto z: xrange(100))
        sum += z;
    REQUIRE(sum==4950);
    ra::Xrange<int> r(1,11,3);
    sum = 0;
    for (int z: r) sum += z;
    REQUIRE(sum==22);
    ra::Xrange<double> rd(1,11,3.2);
    double dsum = 0;
    for (double z: rd) dsum += z;
    REQUIRE(fabs(dsum-(4.0+6*3.2))<1e-6);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("testnarrowconversions")
{
    float f6[1][2]={{0.0,0.0}};
    rarray<float,2> g6 = RARRAY(f6);
    REQUIRE(g6.extent(0)==1);
    REQUIRE(g6.extent(1)==2);
    REQUIRE(g6[0][0]==0.0f);
    REQUIRE(g6[0][1]==0.0f);
    
    float f[1][1][1]={{{2}}};
    rarray<float,3> g = RARRAY(f);
    REQUIRE(g.extent(0)==1);
    REQUIRE(g.extent(1)==1);
    REQUIRE(g.extent(2)==1);
    REQUIRE(g[0][0][0]==2.f);

    float f4[1][1][2]={{{10.0,20.0}}};
    rarray<float,3> g4 = RARRAY(f4);
    REQUIRE(g4.extent(0)==1);
    REQUIRE(g4.extent(1)==1);
    REQUIRE(g4.extent(2)==2);
    REQUIRE(g4[0][0][0]==10.0f);
    REQUIRE(g4[0][0][1]==20.0f);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEST_CASE("morerarrayio")
{
    std::stringstream s("{{1,2},{3,#1:47,4}}");
    rarray<double,2> Y;
    s >> Y;
    REQUIRE(Y.extent(0) == 2);
    REQUIRE(Y.extent(1) == 3);
    REQUIRE(Y[1][1]==47.);
}


rarray<int,2> func(rarray<int,2> A){
    rarray<int,2> B(10,10);
    return B;
}

TEST_CASE("memoryleakofonepointtwo")
{
    rarray<int,2> A(10,10);
    rarray<int,2> B;
    A = func(A);
    // running 'testsuite' will not catch this memory leak that was
    // present in rarray 1.2, but running it through valgrind will, or
    // rather, should show that it is no longer present in rarray 2.0.
}


//////////////////////////////////////////////////////////////////////

double get_element_111(double *const*const*x)
{
    return x[1][1][1];
}

double get_element_1(double *x)
{
    return x[1];
}

TEST_CASE("test_explicit_conversion_to_const_ptr") {
    rtensor<double> t(10,10,10);
    double*const*const* z = t.ptr_array();
    REQUIRE(z!=nullptr);
    double a = 5;
    t[1][1][1] = a;
    double b = get_element_111(t.ptr_array());
    REQUIRE(a==b);    
    rvector<double> y(10);
    double c = 6;
    y[1] = c;
    double d = get_element_1(y.ptr_array());
    REQUIRE(c==d);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("testat", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
 {
     using T = TestType;
     rarray<T,1> a(7);
     T x1 = global::get_value_1<T>();
     a.fill(x1);
     REQUIRE(a.at(2) == x1);
 }

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

TEMPLATE_TEST_CASE("constvector2constarray", "",
                    double,
                    Compound,
                    (array<Compound,3>),
                    (std::complex<float>))
 {
     using T = TestType;
     T val = global::get_value_1<T>();
     rvector<T> a(9);
     a.fill(val);
     rmatrix<const T> b = [](const rvector<T>& x) {
         rmatrix<const T> y(x.data(),3,3);
         return y;
     }(a);
     for (auto& x: b)
         REQUIRE(x==val);
 }

double sum2d(const rarray<const double,2> &s) {
    double x=0;
    for (int i=0; i<s.extent(0); i++) 
        for (int j=0; j<s.extent(1); j++)
            x += s[i][j];
    return x;
}

TEST_CASE("converting_from_const_automatic_arrays")
{
    const double printme[4][4] = { { 1.0, 1.2, 1.4, 1.6},
                                   { 2.0, 2.2, 2.4, 2.6},
                                   { 3.0, 3.2, 3.4, 3.6},
                                   { 4.0, 4.2, 4.4, 4.6} };
    double sumall1 = sum2d(RARRAY(printme));
    rarray<const double,2> a = RARRAY(printme).copy();
    double sumall2 = sum2d(a);
    REQUIRE(sumall1 == sumall2);
}

