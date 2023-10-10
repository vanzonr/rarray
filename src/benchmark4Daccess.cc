//
// benchmark4Daccess.cc - speed test for rarray
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

#include "elapsed.h"
#include "rarray.h"
#include <vector>
#include <cstdlib>
#include <cmath>

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
const int n = 100; 

//////////////////////////////////////////////////////////////////////////////

double case_exact(int repeat)
{
    double check = repeat*(n-1);
    if (2*(repeat/2)==repeat) 
        check += (repeat/2)*(3*repeat/2-2);       
    else
        check += (repeat-1)*(3*repeat-1)/4;    
    return double(n)*double(n)*double(n)*double(n)*check+double(n)*double(n)*double(n)*double(n)*double(n-1)*repeat;
}

//////////////////////////////////////////////////////////////////////////////

extern void pass(float*,float*,int&); // calling this function between
                                      // loops prevents loop fusion
                                      // and unfair speed gains with
                                      // automatically arrays where
                                      // the intel compiler
                                      // optimatizes much of the
                                      // computations away!

//////////////////////////////////////////////////////////////////////////////

double case_rarray(int repeat)
{
    double d = 0.0;
    rarray<float,4> a(n,n,n,n), b(n,n,n,n), c(n,n,n,n);
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = static_cast<float>(l+i+repeat);
                        b[i][j][k][l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a[0][0][0][0],&b[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i][j][k][l] = a[i][j][k][l]+b[i][j][k][l];
        pass(&c[0][0][0][0],&c[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i][j][k][l];
        pass(&c[0][0][0][0],(float*)&d,repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

#if __cpp_multidimensional_subscript >= 202110L

double case_rarray23(int repeat)
{
    double d = 0.0;
    rarray<float,4> a(n,n,n,n), b(n,n,n,n), c(n,n,n,n);
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i,j,k,l] = static_cast<float>(l+i+repeat);
                        b[i,j,k,l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a[0,0,0,0],&b[0,0,0,0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i,j,k,l] = a[i,j,k,l]+b[i,j,k,l];
        pass(&c[0,0,0,0],&c[0,0,0,0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i,j,k,l];
        pass(&c[0,0,0,0],(float*)&d,repeat);
    }
    return d;
}

#endif

//////////////////////////////////////////////////////////////////////////////

double case_auto(int repeat) 
{
    double d = 0.0;
    float a[n][n][n][n], b[n][n][n][n], c[n][n][n][n];
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = static_cast<float>(l+i+repeat);
                        b[i][j][k][l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a[0][0][0][0],&b[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i][j][k][l] = a[i][j][k][l]+b[i][j][k][l];
        pass(&c[0][0][0][0],&c[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i][j][k][l];
        pass(&c[0][0][0][0],(float*)&d,repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_dyn(int repeat)
{
    double d = 0.0;
    float* adata = new float[(long long int)(n)*n*n*n];
    float* bdata = new float[(long long int)(n)*n*n*n];
    float* cdata = new float[(long long int)(n)*n*n*n];
    rarray<float,4> ararray(adata,n,n,n,n),brarray(bdata,n,n,n,n),crarray(cdata,n,n,n,n);
    float*const*const*const* a=ararray.ptr_array();
    float*const*const*const* b=brarray.ptr_array();
    float*const*const*const* c=crarray.ptr_array();
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = static_cast<float>(l+i+repeat);
                        b[i][j][k][l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a[0][0][0][0],&b[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i][j][k][l] = a[i][j][k][l]+b[i][j][k][l];
        pass(&c[0][0][0][0],&c[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i][j][k][l];
        pass(&c[0][0][0][0],(float*)&d,repeat);
    }
    delete[] adata;
    delete[] bdata;
    delete[] cdata;
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_boost(int repeat)
{
#ifndef NOBOOST
    typedef boost::multi_array<float,4> array_type;
    double d = 0.0;
    array_type a(boost::extents[n][n][n][n]);
    array_type b(boost::extents[n][n][n][n]);
    array_type c(boost::extents[n][n][n][n]);
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = static_cast<float>(l+i+repeat);
                        b[i][j][k][l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a[0][0][0][0],&b[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i][j][k][l] = a[i][j][k][l]+b[i][j][k][l];
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i][j][k][l];
        pass(&c[0][0][0][0],(float*)&d,repeat);
    }
    return d;
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_vector(int repeat) 
{
    double d = 0.0;
    std::vector< std::vector< std::vector< std::vector<float> > > > a(n);
    std::vector< std::vector< std::vector< std::vector<float> > > > b(n);
    std::vector< std::vector< std::vector< std::vector<float> > > > c(n);
    for (int i=0;i<n;i++) {
      a[i].resize(n);
      b[i].resize(n);
      c[i].resize(n);
    }
    for (int i=0;i<n;i++) 
        for (int j=0;j<n;j++) {
            a[i][j].resize(n);
            b[i][j].resize(n);
            c[i][j].resize(n);
        }
    for (int i=0;i<n;i++) 
        for (int j=0;j<n;j++) 
            for (int k=0;k<n;k++) {
                a[i][j][k].resize(n);
                b[i][j][k].resize(n);
                c[i][j][k].resize(n);
            }
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = static_cast<float>(l+i+repeat);
                        b[i][j][k][l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a[0][0][0][0],&b[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i][j][k][l] = a[i][j][k][l]+b[i][j][k][l];
        pass(&c[0][0][0][0],&c[0][0][0][0],repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i][j][k][l];
        pass(&c[0][0][0][0],(float*)&d,repeat);
    }
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_eigen(int repeat) 
{
    double d = 0.0;
#ifndef NOEIGEN3
    using namespace Eigen;
    Matrix<Matrix<float,Dynamic,Dynamic>,Dynamic,Dynamic> a(n,n);
    Matrix<Matrix<float,Dynamic,Dynamic>,Dynamic,Dynamic> b(n,n);
    Matrix<Matrix<float,Dynamic,Dynamic>,Dynamic,Dynamic> c(n,n);
    
    for (int i=0;i<n;i++) 
        for (int j=0;j<n;j++) {
            a(j,i).resize(n,n);
            b(j,i).resize(n,n);
            c(j,i).resize(n,n);
        }
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a(j,i)(l,k) = static_cast<float>(l+i+repeat);
                        b(j,i)(l,k) = static_cast<float>(k+j+repeat/2);
                    }
        pass(&(a(0,0)(0,0)),&(b(0,0)(0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c(j,i)(l,k) = a(j,i)(l,k)+b(j,i)(l,k);
        pass(&(c(0,0)(0,0)),&(c(0,0)(0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c(j,i)(l,k);
        pass(&(c(0,0)(0,0)),(float*)&d,repeat);
    }
#endif
    return d;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

double case_mdspan_ref(int repeat) 
{
    double d = 0.0;
#ifndef NOMDSPANREF
    namespace stdex = std::experimental;
    std::unique_ptr<float[]> adata(new float[n*n*n*n]);
    std::unique_ptr<float[]> bdata(new float[n*n*n*n]);
    std::unique_ptr<float[]> cdata(new float[n*n*n*n]);
    stdex::mdspan<float,std::extents<size_t,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent>>
        a{adata.get(), stdex::extents<size_t,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent>{n,n,n,n}},
        b{bdata.get(), stdex::extents<size_t,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent>{n,n,n,n}},
        c{cdata.get(), stdex::extents<size_t,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent,std::dynamic_extent>{n,n,n,n}};
#if __cpp_multidimensional_subscript >= 202110L
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i,j,k,l] = static_cast<float>(l+i+repeat);
                        b[i,j,k,l] = static_cast<float>(k+j+repeat/2);
                    }
        pass(&(a[0,0,0,0]),&(b[0,0,0,0]),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i,j,k,l] = a[i,j,k,l]+b[i,j,k,l];
        pass(&(c[0,0,0,0]),&(c[0,0,0,0]),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i,j,k,l];
        pass(&(c[0,0,0,0]),(float*)&d,repeat);
    }
#else
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a(i,j,k,l) = static_cast<float>(l+i+repeat);
                        b(i,j,k,l) = static_cast<float>(k+j+repeat/2);
                    }
        pass(&(a(0,0,0,0)),&(b(0,0,0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c(i,j,k,l) = a(i,j,k,l)+b(i,j,k,l);
        pass(&(c(0,0,0,0)),&(c(0,0,0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c(i,j,k,l);
        pass(&(c(0,0,0,0)),(float*)&d,repeat);
    }
#endif
#endif
    return d;
}

//////////////////////////////////////////////////////////////////////////////

double case_blitz_1(int repeat) 
{
#ifndef NOBLITZ
    double d = 0.0;
    blitz::Array<float,4> a(n,n,n,n);
    blitz::Array<float,4> b(n,n,n,n);
    blitz::Array<float,4> c(n,n,n,n);
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a(i,j,k,l) = static_cast<float>(l+i+repeat);
                        b(i,j,k,l) = static_cast<float>(k+j+repeat/2);
                    }
        pass(&a(0,0,0,0),&b(0,0,0,0),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c(i,j,k,l) = a(i,j,k,l)+b(i,j,k,l);
        pass(&c(0,0,0,0),&c(0,0,0,0),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c(i,j,k,l);
        pass(&c(0,0,0,0),(float*)&d,repeat);
    }
    return d;
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_blitz_2(int repeat) 
{
#ifndef NOBLITZ
    double d = 0.0;
    using namespace blitz;
    Array<float,4> a(n,n,n,n);
    Array<float,4> b(n,n,n,n);
    Array<float,4> c(n,n,n,n);
    while (repeat--) {
        firstIndex i;
        secondIndex j;
        thirdIndex k;
        fourthIndex l;
        a = l + i + repeat;
        b = k + j + repeat/2;
        pass(&a(0,0,0,0),&b(0,0,0,0),repeat);
        c = a + b;
        pass(&c(0,0,0,0),&c(0,0,0,0),repeat);
        for (int ai=0;ai<n;ai++)
            for (int aj=0;aj<n;aj++) 
                for (int ak=0;ak<n;ak++) 
                     for (int al=0;al<n;al++) 
                         d += c(ai,aj,ak,al);
        pass(&c(0,0,0,0),(float*)&d,repeat);
    }
    return d;
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////

double case_armadillo(int repeat)
{
#ifndef NOARMADILLO
    double d = 0.0;
    arma::Cube<float> a[n],b[n],c[n];
    for (int i=0;i<n;i++) {
        a[i] = arma::Cube<float>(n,n,n);
        b[i] = arma::Cube<float>(n,n,n);
        c[i] = arma::Cube<float>(n,n,n);
    }
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i](l,k,j) = static_cast<float>(l+i+repeat);
                        b[i](l,k,j) = static_cast<float>(k+j+repeat/2);
                    }
        pass(&(a[0](0,0,0)),&(b[0](0,0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c[i](l,k,j) = a[i](l,k,j)+b[i](l,k,j);
        pass(&(c[0](0,0,0)),&(c[0](0,0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c[i](l,k,j);
        pass(&(c[0](0,0,0)),(float*)&d,repeat);
    }
    return d;
#else
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int main(int argc,char**argv) 
{
    double answer = 0.0;
    int thiscase = (argc==1)?1:atoi(argv[1]);
    double check = case_exact(nrepeats);
    Stopwatch s = START;
    switch (thiscase) {
    case 0: 
        printf("exact:     ");
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
    double eps = 1e-6;
    if (fabs(1-answer/check)>eps) {
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

