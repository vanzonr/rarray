//
// benchmark2Daccess.cc - speed test for rarray
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

#include <vector>
#include <cstdlib>
#include <cmath>
#include "elapsed.h"
#include "rarray.h"

//////////////////////////////////////////////////////////////////////////////

#ifndef NOBOOST
#include "boost/multi_array.hpp"
#endif

#ifndef NOBLITZ
#include "blitz/array.h"
#endif

#ifndef NOARMADILLO
#include <armadillo>
#endif

#ifndef NOEIGEN3
#include <Eigen/Dense>
#endif

#ifndef NOMDSPANREF
#include "mdspan.hpp"
#endif

//////////////////////////////////////////////////////////////////////////////

const int nrepeats = 3;
// const int n = 10000;
const int n = 5000;

//////////////////////////////////////////////////////////////////////////////

double case_exact(int repeat) {
    double check = repeat*(n-1);
    if (2*(repeat/2) == repeat)
        check += (repeat/2)*(3*repeat/2-2);
    else
        check += (repeat-1)*(3*repeat-1)/4;
    return n*n*check;
}

//////////////////////////////////////////////////////////////////////////////

extern void pass(float*, float*, int&);  // calling this function between
                                         // loops prevents loop fusion
                                         // and unfair speed gains with
                                         // automatically arrays where
                                         // the intel compiler
                                         // optimatizes much of the
                                         // computations away!

//////////////////////////////////////////////////////////////////////////////

double case_rarray(int repeat) {
    double d = 0.0;
    rarray<float, 2> a(n, n), b(n, n), c(n, n);
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i][j] = static_cast<float>(i+repeat);
                b[i][j] = static_cast<float>(j+repeat/2);
            }
        pass(&a[0][0], &b[0][0], repeat);
        for (int i = 0; i < n ; i++)
            for (int j = 0; j < n; j++)
                c[i][j] = a[i][j] + b[i][j];
        pass(&c[0][0], &c[0][0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i][j];
        pass(&c[0][0], reinterpret_cast<float*>(&d), repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

#if __cpp_multidimensional_subscript >= 202110L
double case_rarray23(int repeat) {
    double d = 0.0;
    rarray<float, 2> a(n, n), b(n, n), c(n, n);
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i, j] = static_cast<float>(i+repeat);
                b[i, j] = static_cast<float>(j+repeat/2);
            }
        pass(&a[0, 0], &b[0, 0], repeat);
        for (int i = 0; i <n; i++)
            for (int j = 0; j < n; j++)
                c[i, j] = a[i, j] + b[i, j];
        pass(&c[0, 0], &c[0, 0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i, j];
        pass(&c[0, 0], reinterpret_cast<float*>(&d), repeat);
    }
    return d;
}
#endif

//////////////////////////////////////////////////////////////////////////////

double case_auto(int repeat) {
    double d = 0.0;
    float a[n][n], b[n][n], c[n][n];
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i][j] = static_cast<float>(i+repeat);
                b[i][j] = static_cast<float>(j+repeat/2);
            }
        pass(a[0], b[0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c[i][j] = a[i][j] + b[i][j];
        pass(c[0], c[0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i][j];
        pass(c[0], reinterpret_cast<float*>(&d), repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_dyn(int repeat) {
    double d = 0.0;
    rarray<float, 2> a_array(n, n), b_array(n, n), c_array(n, n);
    float*const* a = a_array.ptr_array();
    float*const* b = b_array.ptr_array();
    float*const* c = c_array.ptr_array();
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i][j] = static_cast<float>(i+repeat);
                b[i][j] = static_cast<float>(j+repeat/2);
            }
        pass(a[0], b[0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c[i][j] = a[i][j] + b[i][j];
        pass(c[0], c[0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i][j];
        pass(c[0], reinterpret_cast<float*>(&d), repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_boost(int repeat) {
#ifndef NOBOOST
    using array_type = boost::multi_array<float, 2>;
    double d = 0.0;
    array_type a(boost::extents[n][n]);
    array_type b(boost::extents[n][n]);
    array_type c(boost::extents[n][n]);
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i][j] = static_cast<float>(i+repeat);
                b[i][j] = static_cast<float>(j+repeat/2);
            }
        pass(&a[0][0], &b[0][0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c[i][j] = a[i][j] + b[i][j];
        pass(&c[0][0], &c[0][0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i][j];
        pass(&c[0][0], reinterpret_cast<float*>(&d), repeat);
    }
    return d;
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_vector(int repeat) {
    double d = 0.0;
    std::vector< std::vector<float> > a(n);
    std::vector< std::vector<float> > b(n);
    std::vector< std::vector<float> > c(n);
    for (int i = 0; i < n; i++) {
      a[i].resize(n);
      b[i].resize(n);
      c[i].resize(n);
    }
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i][j] = static_cast<float>(i+repeat);
                b[i][j] = static_cast<float>(j+repeat/2);
            }
        pass(&a[0][0], &b[0][0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c[i][j] = a[i][j] + b[i][j];
        pass(&c[0][0], &c[0][0], repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i][j];
        pass(&c[0][0], reinterpret_cast<float*>(&d), repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_eigen(int repeat) {
#ifndef NOEIGEN3
    using Eigen::Matrix;
    using Eigen::Dynamic;
    double d = 0.0;
    Matrix<float, Dynamic, Dynamic> a(n, n);
    Matrix<float, Dynamic, Dynamic> b(n, n);
    Matrix<float, Dynamic, Dynamic> c(n, n);
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a(j, i) = static_cast<float>(i + repeat);
                b(j, i) = static_cast<float>(j + repeat/2);
            }
        pass(&(a(0, 0)), &(b(0, 0)), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c(j, i) = a(j, i) + b(j, i);
        pass(&(c(0, 0)), &(c(0, 0)), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c(j, i);
        pass(&(c(0, 0)), reinterpret_cast<float*>(&d), repeat);
    }
    return d;
#else
    return 0.0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_mdspan_ref(int repeat) {
#ifndef NOMDSPANREF
    namespace stdex = std::experimental;
    double d = 0.0;
    std::unique_ptr<float[]> adata(new float[n*n]);
    std::unique_ptr<float[]> bdata(new float[n*n]);
    std::unique_ptr<float[]> cdata(new float[n*n]);
    stdex::mdspan<float, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>>
        a{adata.get(), stdex::extents<size_t, std::dynamic_extent, std::dynamic_extent>{n, n}},
        b{bdata.get(), stdex::extents<size_t, std::dynamic_extent, std::dynamic_extent>{n, n}},
        c{cdata.get(), stdex::extents<size_t, std::dynamic_extent, std::dynamic_extent>{n, n}};
#if __cpp_multidimensional_subscript >= 202110L
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a[i, j] = static_cast<float>(i+repeat);
                b[i, j] = static_cast<float>(j+repeat/2);
            }
        pass(&(a[0, 0]), &(b[0, 0]), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c[i, j] = a[i, j] + b[i, j];
        pass(&(c[0, 0]), &(c[0, 0]), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c[i, j];
        pass(&(c[0, 0]), reinterpret_cast<float*>(&d), repeat);
    }
#else
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a(i, j) = static_cast<float>(i+repeat);
                b(i, j) = static_cast<float>(j+repeat/2);
            }
        pass(&(a(0, 0)), &(b(0, 0)), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c(i, j) = a(i, j) + b(i, j);
        pass(&(c(0, 0)), &(c(0, 0)), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c(i, j);
        pass(&(c(0, 0)), reinterpret_cast<float*>(&d), repeat);
    }
#endif
    return d;
#else
    return 0.0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_blitz_1(int repeat) {
#ifndef NOBLITZ
    double d = 0.0;
    blitz::Array<float, 2> a(n, n);
    blitz::Array<float, 2> b(n, n);
    blitz::Array<float, 2> c(n, n);
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a(i, j) = static_cast<float>(i+repeat);
                b(i, j) = static_cast<float>(j+repeat/2);
            }
        pass(a.data(), b.data(), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c(i, j) = a(i, j) + b(i, j);
        pass(c.data(), c.data(), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c(i, j);
        pass(c.data(), reinterpret_cast<float*>(&d), repeat);
    }
    return d;
#else
    return 0.0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_blitz_2(int repeat) {
#ifndef NOBLITZ
    double d = 0.0;
    using blitz::Array;
    using blitz::firstIndex;
    using blitz::secondIndex;
    Array<float, 2> a(n, n);
    Array<float, 2> b(n, n);
    Array<float, 2> c(n, n);
    while (repeat--) {
        firstIndex i;
        secondIndex j;
        a = i + repeat;
        b = j + repeat/2;
        pass(&a(0, 0), &b(0, 0), repeat);
        c = a + b;
        pass(&c(0, 0), &c(0, 0), repeat);
        for (int ai = 0; ai < n; ai++)
            for (int aj = 0; aj < n; aj++)
                d += c(ai, aj);
        pass(&c(0, 0), reinterpret_cast<float*>(&d), repeat);
    }
    return d;
#else
    return 0.0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_armadillo(int repeat) {
#ifndef NOARMADILLO
    using arma::Mat;
    double d = 0.0;
    Mat<float> a(n, n);
    Mat<float> b(n, n);
    Mat<float> c(n, n);
    while (repeat--) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a(j, i) = static_cast<float>(i+repeat);
                b(j, i) = static_cast<float>(j+repeat/2);
            }
        pass(&(a(0, 0)), &(b(0, 0)), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                c(j, i) = a(j, i) + b(j, i);
        pass(&(c(0, 0)), &(c(0, 0)), repeat);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d += c(j, i);
        pass(&(c(0, 0)), reinterpret_cast<float*>(&d), repeat);
    }
    return d;
#else
    return 0.0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

int main(int argc, char**argv) {
    Stopwatch s = START;
    double answer = 0.0;
    int thiscase = (argc == 1)?1:atoi(argv[1]);
    switch (thiscase) {
    case 0:
        printf("exact: ");
        fflush(stdout);
        answer = case_exact(nrepeats);
        break;
    case 1:
        printf("rarray:    ");
        fflush(stdout);
        answer = case_rarray(nrepeats);
        break;
    case 2:
        printf("automatic: ");
        fflush(stdout);
        answer = case_auto(nrepeats);
        break;
    case 3:
        printf("dynamic:   ");
        fflush(stdout);
        answer = case_dyn(nrepeats);
        break;
    case 4:
        printf("boost:     ");
        fflush(stdout);
        answer = case_boost(nrepeats);
        break;
    case 5:
        printf("armadillo: ");
        fflush(stdout);
        answer = case_armadillo(nrepeats);
        break;
    case 6:
        printf("vector:    ");
        fflush(stdout);
        answer = case_vector(nrepeats);
        break;
    case 7:
        printf("blitz1:    ");
        fflush(stdout);
        answer = case_blitz_1(nrepeats);
        break;
    case 8:
        printf("blitz2:    ");
        fflush(stdout);
        answer = case_blitz_2(nrepeats);
        break;
    case 9:
        printf("eigen:     ");
        fflush(stdout);
        answer = case_eigen(nrepeats);
        break;
    case 10:
        printf("mdspan_ref:");
        fflush(stdout);
        answer = case_mdspan_ref(nrepeats);
        break;
    #if __cpp_multidimensional_subscript >= 202110L
    case 11:
        printf("rarray-23: ");
        fflush(stdout);
        answer = case_rarray23(nrepeats);
        break;
    #endif
    }
    double check = case_exact(nrepeats);
    double eps = 1e-6;
    if (fabs(1-answer/check) >= eps) {
        if (fabs(answer) == 0.0) {
            printf("(skipped - library not installed) ");
            fflush(stdout);
        } else {
            printf("%lf does not match exact result of %lf\n",
                   answer/n/n, check/n/n);
            fflush(stdout);
        }
    }
    stopwatchStop(&s);
}

//////////////////////////////////////////////////////////////////////////////
