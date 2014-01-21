// rarray4dspeed - speed test for rarray.h
//
// note: intel compiler optimatizes much of the computations away!
//

#include "cstopwatch.h"
#include "rarray.h"
#include <vector>
#include <cstdlib>
#include <cmath>

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
#include <eigen3/Eigen/Dense>
#endif

const int repeat = 3;
//const int n = 13376; // requires ~2GB of storage
const int n = 140; // requires ~1GB of storage
//const int n = 96; // requires little

double case_exact(int repeat)
{
    double check = repeat*(n-1);
    if (2*(repeat/2)==repeat) 
        check += (repeat/2)*(3*repeat/2-2);       
    else
        check += (repeat-1)*(3*repeat-1)/4;    
    return double(n)*double(n)*double(n)*double(n)*check+double(n)*double(n)*double(n)*double(n)*double(n-1)*repeat;
}

extern void pass(float*,float*,int&); // calling this function between
                                      // loops prevents loop fusion
                                      // and unfair speed gains with
                                      // automatically arrays where
                                      // the intel compiler
                                      // optimatizes much of the
                                      // computations away!

double case_rarray(int repeat)
{
    double d = 0.0;
    rarray<float,4> a(n,n,n,n), b(n,n,n,n), c(n,n,n,n);
    //float[****] a[n][n][n][n], b[n][n][n][n], c[n][n][n][n];
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = l+i+repeat;
                        b[i][j][k][l] = k+j+repeat/2;
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

double case_auto(int repeat) 
{
    double d = 0.0;
    float a[n][n][n][n], b[n][n][n][n], c[n][n][n][n];
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = l+i+repeat;
                        b[i][j][k][l] = k+j+repeat/2;
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
                        a[i][j][k][l] = l+i+repeat;
                        b[i][j][k][l] = k+j+repeat/2;
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

double case_boost(int repeat)
{
#ifndef NOBOOST
    typedef boost::multi_array<float,4> array_type;
    typedef array_type::index index;
    double d = 0.0;
    array_type a(boost::extents[n][n][n][n]);
    array_type b(boost::extents[n][n][n][n]);
    array_type c(boost::extents[n][n][n][n]);
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = l+i+repeat;
                        b[i][j][k][l] = k+j+repeat/2;
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

double case_vector(int repeat) 
{
    double d = 0.0;
    std::vector< std::vector< std::vector< std::vector<float> > > > a(n);
    std::vector< std::vector< std::vector< std::vector<float> > > > b(n);
    std::vector< std::vector< std::vector< std::vector<float> > > > c(n);
    for (int i=0;i<n;i++) {
      a[i].reserve(n);
      b[i].reserve(n);
      c[i].reserve(n);
    }
    for (int i=0;i<n;i++) 
        for (int j=0;j<n;j++) {
            a[i][j].reserve(n);
            b[i][j].reserve(n);
            c[i][j].reserve(n);
        }
    for (int i=0;i<n;i++) 
        for (int j=0;j<n;j++) 
            for (int k=0;k<n;k++) {
                a[i][j][k].reserve(n);
                b[i][j][k].reserve(n);
                c[i][j][k].reserve(n);
            }
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a[i][j][k][l] = l+i+repeat;
                        b[i][j][k][l] = k+j+repeat/2;
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

double case_eigen(int repeat) 
{
    using namespace Eigen;
    double d = 0.0;
    Matrix<Matrix<float,Dynamic,Dynamic>,Dynamic,Dynamic> a(n,n);
    Matrix<Matrix<float,Dynamic,Dynamic>,Dynamic,Dynamic> b(n,n);
    Matrix<Matrix<float,Dynamic,Dynamic>,Dynamic,Dynamic> c(n,n);
    
    for (int i=0;i<n;i++) 
        for (int j=0;j<n;j++) {
            a(i,j).resize(n,n);
            b(i,j).resize(n,n);
            c(i,j).resize(n,n);
        }
    while (repeat--) {
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) {
                        a(i,j)(k,l) = l+i+repeat;
                        b(i,j)(k,l) = k+j+repeat/2;
                    }
        pass(&(a(0,0)(0,0)),&(b(0,0)(0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        c(i,j)(k,l) = a(i,j)(k,l)+b(i,j)(k,l);
        pass(&(c(0,0)(0,0)),&(c(0,0)(0,0)),repeat);
        for (int i=0;i<n;i++)
            for (int j=0;j<n;j++) 
                for (int k=0;k<n;k++) 
                    for (int l=0;l<n;l++) 
                        d += c(i,j)(k,l);
        pass(&(c(0,0)(0,0)),(float*)&d,repeat);
    }
    return d;
}

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
                        a(i,j,k,l) = l+i+repeat;
                        b(i,j,k,l) = k+j+repeat/2;
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

int main(int argc,char**argv) 
{
    double answer;
    int thiscase = (argc==1)?1:atoi(argv[1]);
    double check = case_exact(repeat);

    Stopwatch s = START;

    switch (thiscase) {
    case 0: 
        printf("exact:     ");
        fflush(stdout);
        answer = case_exact(repeat);
        break;
    case 1: 
        printf("rarray:    ");
        fflush(stdout);
        answer = case_rarray(repeat);
        break;
    case 2: 
        printf("automatic: ");
        fflush(stdout);
        answer = case_auto(repeat);
        break;
    case 3: 
        printf("dynamic:   ");
        fflush(stdout);
        answer = case_dyn(repeat);
        break;
    case 4: 
        printf("boost:     ");
        fflush(stdout);
        answer = case_boost(repeat);
        break;
    case 5: 
        break;
    case 6: 
        printf("vector:    ");
        fflush(stdout);
        answer = case_vector(repeat);
        break;
    case 7: 
        printf("blitz1:    ");
        fflush(stdout);
        answer = case_blitz_1(repeat);
        break;
    case 8: 
        printf("blitz2:    ");
        fflush(stdout);
        answer = case_blitz_2(repeat);
        break;
    case 9: 
        printf("eigen:     ");
        fflush(stdout);
        answer = case_eigen(repeat);
        break;
    }

    double eps = 1e-6;

    if (fabs(1-answer/check)>eps)
        printf("%lf does not match exact result of %lf\n", 
               answer/n/n, check/n/n);
    // else
    //     printf("%lf matches exact result!\n", answer/n/n);

    stopwatchStop(&s);
}

