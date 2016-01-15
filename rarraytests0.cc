//
// rarraytests.cc - testsuite for rarray.h, using Boost.Test
//
// Copyright (c) 2015  Ramses van Zon
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
#include "rarrayio.h"
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


int main()
{
    // Exercises the constructors of the rarray class
    int dim[3] = {7,21,13};
    rarray<double,3> a(7,21,13);
    rarray<double,3> b(dim);
    rarray<double,3> c(b);
    const int* asize = a.shape();
    assert(a.data());
    assert(a.size()==7*21*13);
    assert(asize);
    assert(asize[0] == dim[0]);
    assert(asize[1] == dim[1]);
    assert(asize[2] == dim[2]);
    assert(a.extent(0) == dim[0]);
    assert(a.extent(1) == dim[1]);
    assert(a.extent(2) == dim[2]);
    assert(b.data());
    assert(b.size()==7*21*13);
    assert(b.extent(0) == dim[0]);
    assert(b.extent(1) == dim[1]);
    assert(b.extent(2) == dim[2]);
    assert(c.data());
    assert(c.size()==7*21*13);
    assert(c.extent(0) == dim[0]);
    assert(c.extent(1) == dim[1]);
    assert(c.extent(2) == dim[2]);
    assert(c.data()==b.data());
    b.clear();
    assert(b.is_clear());
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
