//
// test_rarray.cc - simple tests for rarray
//
// Copyright (c) 2019  Ramses van Zon
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

int main()
{
    rarray<double,2> a(100,100);
    double *const*z = a.ptr_array();
    a[2][3] = 4.4;
    int faultscaught=0;
    try {
        a[2][300] = 4.4;
    }
    catch (...)
    {
        std::cout << "Caught out of bounds\n";
        faultscaught++;
    }
    try {
        a[200][3] = 4.4;
    }
    catch (...)
    {
        std::cout << "Caught out of bounds\n";
        faultscaught++;
    }
    if (faultscaught<2)
        return 2;
    if (z[2][3]==4.4)
        return !(int(a[2][3])==4);
    else
        return 1;
}

