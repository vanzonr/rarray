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
#include <cassert>
#include <iostream>

int main()
{
    return test_shared_buffer_main();
}
    
int test_shared_buffer_main() 
{
    const int N = 100;
    {
        ra::shared_buffer<int> z;
        assert(z.data_ == nullptr);
        assert(z.orig_ == z.data_);
        assert(z.refs_ == nullptr);
        assert(z.size_ == 0);
    }

    {
        ra::shared_buffer<double> a(N);
        assert(a.data_ != nullptr);
        assert(a.orig_ == a.data_);
        assert(a.refs_ != nullptr);
        assert(*(a.refs_) == 1);
        assert(a.size_ == N);
        int i = 0;
        for (double& x: a) x = i++;
        ra::shared_buffer<double> b = a.copy();
        ra::shared_buffer<double> c = a;
        a[50] = 7;
        assert (b[50]+c[50]==57);
    }
    
    {
        float b[N];
        ra::shared_buffer<float> a(N,b);
        assert(a.data_ == b);
        assert(a.orig_ == nullptr);
        assert(a.refs_ == nullptr);
        assert(a.size_ == N);   
    }

    {
        ra::shared_buffer<double> a(N);
        {
            ra::shared_buffer<double> b(a);
            assert(a.data_ == b.data_);
            assert(b.orig_ == b.data_);
            assert(a.refs_ == b.refs_);
            assert(*(a.refs_) == 2);
            assert(a.size_ == b.size_);
        }
        assert(*(a.refs_) == 1);
        {
            double* d = a.data_;
            ra::shared_buffer<double> c;
            c = std::move(a);
            assert(c.data_ == d);
            assert(c.orig_ == c.data_);
            assert(c.refs_ != nullptr);
            assert(*(c.refs_) == 1);
            assert(c.size_ == N);
        }
        assert(a.data_ == nullptr);
        assert(a.orig_ == a.data_);
        assert(a.refs_ == nullptr);
        assert(a.size_ == 0);
    }
    
    {
        ra::shared_buffer<double> a(N);
        ra::shared_buffer<double> b;
        b = a;
        assert(a.data_ == b.data_);
        assert(b.orig_ == b.data_);
        assert(a.refs_ == b.refs_);
        assert(*(a.refs_) == 2);
        assert(a.size_ == b.size_);
    }
    
    {
        ra::shared_buffer<double> a{ra::shared_buffer<double>(N)};
        assert(a.data_ != nullptr);
        assert(a.orig_ == a.data_);
        assert(a.refs_ != nullptr);
        assert(*(a.refs_) == 1);
        assert(a.size_ == N);
    }

    {
        ra::shared_buffer<double> a(N);
        assert(a.data_ != nullptr);
        assert(a.orig_ == a.data_);
        assert(a.refs_ != nullptr);
        assert(*(a.refs_) == 1);
        assert(a.size_ == N);
        int i = 0;
        for (double& x: a) x = i++;
        for (int j=0; j<N; j++) {
            assert(a[j] == j);
        }
    }
    {
        ra::shared_buffer<double> a(N);
        assert(a.data_ != nullptr);
        assert(a.orig_ == a.data_);
        assert(a.refs_ != nullptr);
        assert(*(a.refs_) == 1);
        assert(a.size_ == N);
        int i = 0;
        for (int j=0; j<N; j++) {
            a[j] = j;
            assert(a[j] == j);
        }
    }

    {
        ra::shared_buffer<double> a(N);
        assert(&a[0] != nullptr);
        assert(a.size() == N);
    }

    {
        ra::shared_buffer<double> a(N);
        int i = 0;
        for (double& x: a) x = i++;
        const ra::shared_buffer<double> b(a.copy());
        int j = 0;
        for (const double& x: b) assert(x == j++);
    }
    
    {
        ra::shared_buffer<double> a(N);
        int i = 0;
        for (double& x: a) x = i++;
        const ra::shared_buffer<double> b(a.copy());
        int j = 0;
        for (auto it=b.cbegin(); it != b.cend();it++)
            assert(*it == j++);
    }
    
    {
        ra::shared_buffer<int> a(N);
        a.assign(-15);
        for (const auto& x: a) assert(x == -15);
    }
    {
        ra::shared_buffer<int> a;
        a.assign({1,2,3,4});
        assert(a.size()==4);
        assert(a[0]==1);
        assert(a[1]==2);
        assert(a[2]==3);
        assert(a[3]==4);
        assert(a.at(0)==1);
        assert(a.at(1)==2);
        assert(a.at(2)==3);
        assert(a.at(3)==4);
    }
    
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
    
    {
        ra::shared_buffer<double> a(N);
        int i = 0;
        for (double& x: a) x = i++;
        const ra::shared_buffer<double> b(a.copy());
        int j = N;
        for (auto it=a.crbegin(); it != a.crend();it++){
            assert(*it == --j);
        }
    }
    {
        ra::shared_buffer<int> a;
        a.assign({1,2,3,4});
        ra::shared_buffer<int> b = a.slice(1,3);
        assert(b[0]==2);
        assert(b[1]==3);
        assert(b.size()==2);
        b[1]=10;
        assert(a[2]==10);
    }
    

    return 0;
    
}
