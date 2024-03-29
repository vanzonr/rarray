//
// test_offsets.cc - unit tests for the offsets.h header.
//
// Copyright (c) 2018-2022  Ramses van Zon
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

#include "offsets.h"
using ra::detail::Offsets;

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

const int N = 16;

TEST_CASE("0") {
    Offsets P{{}};
    REQUIRE(P.get_num_data_offsets() == 0);
    REQUIRE(P.get_num_offsets() == 0);
    REQUIRE(P.get_num_data_offsets() == 0);
}

TEST_CASE("1") {
    Offsets P{{N}};
    REQUIRE(P.get_num_data_offsets() == 1);
    REQUIRE(P.get_num_offsets() == 0);
    REQUIRE(P.get_num_data_offsets() == 1);
    int a[N];
    int* aptr = &(a[0]);
    for (int i = 0; i < N; i++)
        aptr[i] = i;
    int* p = reinterpret_cast<int*>(P.apply_offsets(a));
    for (int i1 = 0; i1 < N; i1++) {
        REQUIRE(p[i1] == i1);
    }
}

TEST_CASE("2") {
    Offsets P{{N, N/2}};  // 16 x 8
    REQUIRE(P.get_num_data_offsets() == N);
    REQUIRE(P.get_num_offsets() == N);
    REQUIRE(P.get_num_data_offsets() == N);
    int a[N][N/2];
    int* aptr = &(a[0][0]);
    for (int i = 0; i < N*(N/2); i++)
        aptr[i] = i;
    int*const* p = reinterpret_cast<int*const*>(P.apply_offsets(aptr));
    for (int i1 = 0; i1 < N; i1++)
        for (int i2 = 0; i2 < N/2; i2++) {
            REQUIRE(p[i1][i2] == i1*(N/2) +i2);
        }
    delete[] p;
}

TEST_CASE("3") {
    Offsets P{{N, N/2, N/4}};  // 16 x 8 x 4
    REQUIRE(P.get_num_data_offsets() == N*(N/2) );
    REQUIRE(P.get_num_offsets() == N*(N/2+1) );
    REQUIRE(P.get_num_data_offsets() == N*(N/2) );
    int a[N][N/2][N/4];
    int* aptr = &(a[0][0][0]);
    for (int i = 0; i < N*(N/2)*(N/4); i++)
        aptr[i] = i;
    int*const*const* p = reinterpret_cast<int*const*const*>(P.apply_offsets(aptr));
    for (int i1 = 0; i1 < N; i1++)
        for (int i2 = 0; i2 < N/2; i2++)
            for (int i3 = 0; i3 < N/4; i3++) {
                REQUIRE(p[i1][i2][i3] == (i1*(N/2) + i2)*(N/4) + i3);
            }
    delete[] p;
}

TEST_CASE("4") {
    Offsets P{{N, N/2, N/4, N/8}};  // 16 x 8 x 4 x 2
    REQUIRE(P.get_num_offsets() == N*(N/2*(N/4+1)+1));
    REQUIRE(P.get_num_data_offsets() == N*(N/2)*(N/4));
    int a[N][N/2][N/4][N/8];
    int* aptr = &a[0][0][0][0];
    for (int i = 0; i < N*(N/2)*(N/4)*(N/8); i++)
        aptr[i] = i;
    int*const*const*const* p = reinterpret_cast<int*const*const*const*>(P.apply_offsets(aptr));
    for (int i1 = 0; i1 < N; i1++)
        for (int i2 = 0; i2 < N/2; i2++)
            for (int i3 = 0; i3 < N/4; i3++)
                for (int i4 = 0; i4 < N/8; i4++) {
                    REQUIRE(p[i1][i2][i3][i4]
                            == ((i1*(N/2)+i2)*(N/4)+i3)*(N/8)+i4);
                }
    delete [] p;
}

TEST_CASE("5") {
    Offsets P{{N, N/2, N/4, N/8, N/16}};  // 16 x 8 x 4 x 2 x 1
    REQUIRE(P.get_num_offsets() == N*(N/2*(N/4*(N/8+1)+1)+1));
    REQUIRE(P.get_num_data_offsets() == N*(N/2)*(N/4)*(N/8));
    int a[N][N/2][N/4][N/8][N/16];
    int* aptr = &a[0][0][0][0][0];
    for (int i = 0; i < N*(N/2)*(N/4)*(N/8)*(N/16); i++)
        aptr[i] = i;
    int*const*const*const*const* p = reinterpret_cast<int*const*const*const*const*>(P.apply_offsets(aptr));
    for (int i1 = 0; i1 < N; i1++)
        for (int i2 = 0; i2 < N/2; i2++)
            for (int i3 = 0; i3 < N/4; i3++)
                for (int i4 = 0; i4 < N/8; i4++)
                    for (int i5 = 0; i5 < N/16; i5++) {
                        REQUIRE(p[i1][i2][i3][i4][i5]
                                == (((i1*(N/2)+i2)*(N/4)+i3)*(N/8)+i4)*(N/16)+i5);
                    }
    delete [] p;
}

TEST_CASE("6") {
    Offsets P{{}};
    REQUIRE(P.get_num_data_offsets() == 0);
    REQUIRE(P.get_num_offsets() == 0);
    REQUIRE(P.get_num_data_offsets() == 0);
    int* aptr = nullptr;
    void*** p = P.apply_offsets(aptr);
    REQUIRE(p == nullptr);
}
