//
// test_shared_buffer.cc - unit tests for the shared_buffer.h header.
//
// Copyright (c) 2018-2019  Ramses van Zon
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

#include "shared_buffer.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

template<typename V>
int internal_check(const ra::shared_buffer<V>& a,
                   bool datavalue_shouldbe, V* datavalue,
                   bool origvalue_shouldbe, V* origvalue,
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,
                   bool refscount_shouldbe, int refscount,
                   bool sizevalue_shouldbe, typename ra::shared_buffer<V>::size_type sizevalue)
{
    int errorcode = 0;
    bool equality;
    equality = (a.data_ == datavalue);
    if ((datavalue_shouldbe and not equality)  or  (not datavalue_shouldbe and equality))
        errorcode += 1;
    equality = (a.orig_ == origvalue);
    if ((origvalue_shouldbe and not equality)  or  (not origvalue_shouldbe and equality))
        errorcode += 2;
    equality = (a.refs_ == refsvalue);
    if ((refsvalue_shouldbe and not equality)  or  (not refsvalue_shouldbe and equality))
        errorcode += 4;
    if (a.refs_) {
        equality = (*(a.refs_) == refscount);
        if ((refscount_shouldbe and not equality)  or  (not refscount_shouldbe and equality)) 
            errorcode += 8;
    } else {
        if (refscount_shouldbe)
            errorcode += 8;
    }
    equality = (a.size_ == sizevalue);
    if ((sizevalue_shouldbe and not equality)  or  (not sizevalue_shouldbe and equality))
        errorcode += 16;
    return errorcode;
}

TEST_CASE("test default shared_buffer state")
{
    ra::shared_buffer<int> z;
    REQUIRE(0 == internal_check
            (z,
             true,  (int*)nullptr,      // data_
             true,  (int*)(z.cbegin()), // orig_
             true,  nullptr,            // refs_
             false, 0,                  // never mind ref count
             true,  0)                  // size_
            );
}

TEST_CASE("test deep and shallow copy")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    REQUIRE(0 == internal_check
            (a,
             false, (double*)nullptr,      // data_
             true,  (double*)(a.cbegin()), // orig_
             false, nullptr,               // refs_
             true,  1,                     // ref count
             true,  N)                     // size_
            );
    int i = 0;
    for (double& x: a) x = i++;
    ra::shared_buffer<double> b = a.copy();
    ra::shared_buffer<double> c = a;
    a[50] = 7;
    REQUIRE(b[50]+c[50]==57);
}

TEMPLATE_TEST_CASE("test non-owned buffer",
                   "",double,int)
{
    const int N = 100;
    TestType b[N];
    ra::shared_buffer<TestType> a(N,b);
    REQUIRE(0 == internal_check
            (a,
             true,  (TestType*)b,          // data_
             true,  (TestType*)nullptr,    // orig_
             true,  nullptr,            // refs_
             false, 0,                  // never mind ref count
             true,  N)                  // size_
            );
}

TEMPLATE_TEST_CASE("inspect internal structure and reference counting",
                   "",double,int)
{
    const int N = 100;
    ra::shared_buffer<TestType> a(N);
    {
        ra::shared_buffer<TestType> b(a);
        REQUIRE(0 == internal_check
            (b,
             true,  (TestType*)a.cbegin(),  // data_
             true,  (TestType*)a.cbegin(),  // orig_
             false, nullptr,              // refs_
             true,  2,                    // ref count
             true,  a.size())             // size_
            );
    }
    REQUIRE(0 == internal_check
            (a,
             false,  (TestType*)nullptr,   // data_
             false,  (TestType*)nullptr,   // orig_
             false,  nullptr,            // refs_
             true,  1,                  // never mind ref count
             true,  N)                  // size_
            );
    {
        TestType* d = (TestType*)a.begin();
        ra::shared_buffer<TestType> c;
        c = std::move(a);
        REQUIRE(0 == internal_check
            (c,
             true,  d,        // data_
             true,  d,        // orig_
             false, nullptr,  // refs_
             true,  1,        // ref count
             true,  N)        // size_
            );
    }
    REQUIRE(0 == internal_check
            (a,
             true,  (TestType*)nullptr,   // data_
             true,  (TestType*)nullptr,   // orig_
             true,  nullptr,            // refs_
             false, 0,                  // never mind ref count
             true,  0)                  // size_
            );
}

TEST_CASE("Test reference counting")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    ra::shared_buffer<double> b;
    b = a;
    REQUIRE(0 == internal_check
            (a,
             true,  (double*)b.begin(),  // data_
             true,  (double*)b.begin(),  // orig_
             false,  nullptr,            // refs_
             true,  2,                   //  ref count
             true,  b.size())            // size_
            );
}

TEST_CASE("Test constructor from shared_buffer")
{
    const int N = 100;
    ra::shared_buffer<double> a{ra::shared_buffer<double>(N)};
    REQUIRE(0 == internal_check
            (a,
             false, (double*)nullptr,            // data_
             true,  (double*)a.begin(),  // orig_
             false,  nullptr,            // refs_
             true,  1,                   //  ref count
             true,  N)                   // size_
            );
}


TEST_CASE("Test square bracket operator #1")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    REQUIRE(0 == internal_check
            (a,
             false, (double*)nullptr,     // data_
             true,  (double*)a.begin(),  // orig_
             false,  nullptr,            // refs_
             true,  1,                   //  ref count
             true,  N)                   // size_
            );
    int i = 0;
    for (double& x: a) x = i++;
    for (int j=0; j<N; j++) {
        REQUIRE(a[j] == j);
    }
}

TEST_CASE("Test square bracket operator #2")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    REQUIRE(0 == internal_check
            (a,
             false, (double*)nullptr,     // data_
             true,  (double*)a.begin(),  // orig_
             false,  nullptr,            // refs_
             true,  1,                   //  ref count
             true,  N)                   // size_
            );
    for (int j=0; j<N; j++) {
        a[j] = j;
        REQUIRE(a[j] == j);
    }
}

TEST_CASE("Test deep copy")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    int i = 0;
    for (double& x: a) x = i++;
    const ra::shared_buffer<double> b(a.copy());
    int j = 0;
    for (const double& x: b) REQUIRE(x == j++);
}

TEST_CASE("test cbegin and cend")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    int i = 0;
    for (double& x: a) x = i++;
    const ra::shared_buffer<double> b(a.copy());
    int j = 0;
    for (auto it=b.cbegin(); it != b.cend();it++)
        REQUIRE(*it == j++);
}

TEST_CASE("test assign")
{
    const int N = 100;
    ra::shared_buffer<int> a(N);
    a.assign(-15);
    for (const auto& x: a) REQUIRE(x == -15);
}

TEMPLATE_TEST_CASE("test .at()",
                   "",double,int)
{
    ra::shared_buffer<TestType> a;
    a.assign({1,2,3,4});
    REQUIRE(a.size()==4);
    REQUIRE(a[0]==1);
    REQUIRE(a[1]==2);
    REQUIRE(a[2]==3);
    REQUIRE(a[3]==4);
    REQUIRE(a.at(0)==1);
    REQUIRE(a.at(1)==2);
    REQUIRE(a.at(2)==3);
    REQUIRE(a.at(3)==4);
}

TEST_CASE("test .assign_iter()")
{
    ra::shared_buffer<int> a(6);
    a.assign({1,2,3,4});
    ra::shared_buffer<int> b;
    b.assign_iter(a.begin(),a.end());
    assert(b[0]==1);
    assert(b[1]==2);
    assert(b[2]==3);
    assert(b[3]==4);
}
    
TEST_CASE("test crbegin and crend")
{
    const int N = 100;
    ra::shared_buffer<double> a(N);
    int i = 0;
    for (double& x: a) x = i++;
    const ra::shared_buffer<double> b(a.copy());
    int j = N;
    for (auto it=a.crbegin(); it != a.crend();it++){
        REQUIRE(*it == --j);
    }
}

TEMPLATE_TEST_CASE("test .slice",
                   "",double,int)
{
    ra::shared_buffer<TestType> a;
    a.assign({1,2,3,4});
    ra::shared_buffer<TestType> b = a.slice(1,3);
    REQUIRE(b[0]==2);
    REQUIRE(b[1]==3);
    REQUIRE(b.size()==2);
    b[1]=10;
    REQUIRE(a[2]==10);
    ra::shared_buffer<TestType> c = a.slice(3,1);
    REQUIRE(c.size()==0);
}

TEMPLATE_TEST_CASE("test resize",
                   "",double,int)
{
    ra::shared_buffer<TestType> a(5);
    a.resize(4);
    REQUIRE(a.size()==4);
    ra::shared_buffer<TestType> b = a.slice(1,4);
    b[0]=1; b[1]=2; b[2]=3;
    REQUIRE(a[1]==b[0]);
    b.resize(2); // resize of a reference makes a new array
    b[0]=4;
    REQUIRE(a[1]==1);
    REQUIRE(b[0]==4); // no guarantees on b[1]
    ra::shared_buffer<TestType> c = a.slice(1,4);
    c.resize(2,true); // resize makes new array but now also copies
    REQUIRE(c[0]==a[1]);
    REQUIRE(c[1]==a[2]);
    c.resize(10,true); // resize makes new array but now also copies
    REQUIRE(c[0]==a[1]);
    REQUIRE(c[1]==a[2]);
}

TEST_CASE("Fail test not enough memory")
{
    const long long int N = 1000000000000000LL;
    REQUIRE_THROWS( ra::shared_buffer<double>(N) );
    REQUIRE_THROWS( ra::shared_buffer<int>(N) );
}

TEST_CASE("Fail test at out of bounds")
{
    const long long int N = 100;
    ra::shared_buffer<double> a(N);
    ra::shared_buffer<int> b(N);
    REQUIRE_THROWS( a.at(N) );
    REQUIRE_THROWS( b.at(N) );
    REQUIRE_THROWS( a.at(-1) );
    REQUIRE_THROWS( b.at(-1) );
}

TEMPLATE_TEST_CASE("Fail test slice out of bounds",
                   "",double,int)
{
    ra::shared_buffer<TestType> a;
    a.assign({1,2,3,4});
    ra::shared_buffer<TestType> b;
    REQUIRE_THROWS(b = a.slice(1,13));
}

TEMPLATE_TEST_CASE("Fail test resize for not enough memory",
                   "",double,int)
{
    ra::shared_buffer<TestType> a(5);
    const long long int N = 1000000000000000LL;
    REQUIRE_THROWS(a.resize(N));
}
