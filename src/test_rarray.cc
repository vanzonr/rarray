//
// test_rarray.cc - simple tests for rarray
//
// Copyright (c) 2019-2023  Ramses van Zon
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

int main() {

    rmatrix<int> P((int[2][2]){});   // points at a temporary!!!
    P[1][1] = 20;  // Not guarranteed to work, in fact, it should not,
                   // but often does because how compilers
                   // optimize/implement the temporary array
    std::cout << P << "\n";
    auto V = rvector<int>((int[4]){1, 2, 3, 4}).copy();  // this is safe, though
                                                         // still not standard
                                                         // c++. Also,
                                                         // the "<int>" may be

                                                         // omitted in c++20,
                                                         // but this not
                                                         // supported yet by clang
    // (and so also not by aocc and intel)
    std::cout << V << "\n";
    auto M = rmatrix<int>((int[][2]){{1, 2}, {3, 4}}).copy();  // same c++20 remark
    std::cout << M << "\n";
    const int Nstack[2][2]{{1, -2}, {-3, 4}};
    rarray<const int, 2> N(Nstack);  // even in c++20, "rarray N(Nstack);" does not work
                                     // because the compiler can't determine that R==2.
    std::cout << N << "\n";

    ///////

    // construct:
    // 1) Object<int>
    // 2) Object<const int>
    // 3) Object // template parameter deduction (c++17)
    // from:
    // a.i int A[2][2]{{1,2},{3,4}};
    // a.ii const int Aconst[2][2]{{1,2},{3,4}};
    // b.i (int[2][2]){{1,2},{3,4}};
    // b.ii (const int[2][2]){{1,2},{3,4}};

    // 1.a.i  : X create reference to A's data
    {
        int A[2][2]{{1, 2}, {3, 4}};
        auto x = rarray<int, 2>(A);
    }
    // 1.a.ii : - should not compile, as x would refer to Aconst's data
    {
        //// const int A[2][2]{{1,2},{3,4}};
        //// auto x = rarray<int,2>(A);
    }
    // 1.b.i  : Y create reference of the temporary (dangling!)
    {
        auto x = rarray<int, 2>((int[2][2]){{1, 2}, {3, 4}});
    }
    // 1.b.ii : Z create reference of the const temporary (dangling, but:)
    // should not compile
    {
        //// auto x = rarray<int,2>((const int[2][2]){{1,2},{3,4}}).copy();
    }
    // 2.a.i  : X create (const) reference to A's data
    {
        int A[2][2]{{1, 2}, {3, 4}};
        auto x = rarray<const int, 2>(A);
    }
    // 2.a.ii : X create (const) reference to A's const data
    {
        const int A[2][2]{{1, 2}, {3, 4}};
        auto x = rarray<const int, 2>(A);
    }
    // 2.b.i  : Z create reference to the temporary (dangling!)
    {
        auto x = rarray<const int, 2>((int[2][2]){{1, 2}, {3, 4}});
    }
    // 2.b.ii : Y create a const referencey of the temporary (dangling!)
    {
        auto x = rarray<const int, 2>((const int[2][2]){{1, 2}, {3, 4}});
    }

    ///////

    rarray<double, 3> a(2, 100, 100);
    double *const*const*z = a.ptr_array();

#if __cpp_multidimensional_subscript >= 202110L
    // 3.a.i  : X create (const) reference to A's data
    {
        int A[2][2]{{1, 2}, {3, 4}};
        auto x = rmatrix(A);
    }
    // 3.a.ii : X create (const) reference to A's const data
    {
        const int A[2][2]{{1, 2}, {3, 4}};
        auto x = rmatrix(A);
    }
    // 3.b.i  : Z create reference to the temporary (dangling!)
    {
        auto x = rmatrix((int[2][2]){{1, 2}, {3, 4}});
    }
    // 3.b.ii : Y create a const referencey of the temporary (dangling!)
    {
        auto x = rmatrix((const int[2][2]){{1, 2}, {3, 4}});
    }
    std::cout << "Simple C++23 test\n";
    a[1, 2, 3] = 4.4;
    int faultscaught = 0;
    try {
        a[1, 2, 300] = 4.4;
    }
    catch (...) {
        std::cout << "Caught out of bounds\n";
        faultscaught++;
    }
    try {
        a[1, 200, 3] = 4.4;
    }
    catch (...) {
        std::cout << "Caught out of bounds\n";
        faultscaught++;
    }
    if (faultscaught < 2)
        return 2;
    if (z[1][2][3] == 4.4)
        return !(static_cast<int>(a[1, 2, 3]) == 4);
    else
        return 1;
#else
    a[1][2][3] = 4.4;
    int faultscaught = 0;
    try {
        a[1][2][300] = 4.4;
    }
    catch (...) {
        std::cout << "Caught out of bounds\n";
        faultscaught++;
    }
    try {
        a[1][200][3] = 4.4;
    }
    catch (...) {
        std::cout << "Caught out of bounds\n";
        faultscaught++;
    }
    if (faultscaught < 2)
        return 2;
    if (z[1][2][3] == 4.4)
        return !(static_cast<int>(a[1][2][3]) == 4);
    else
        return 1;

#endif
}

