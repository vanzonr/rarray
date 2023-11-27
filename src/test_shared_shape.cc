//
// test_offsets.cc - unit tests for the shared_shape.h header.
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

#include "shared_shape.h"
#include <cassert>
#include <iostream>

int main()
{
    return test_shared_shape_main();
}

int test_shared_shape_main()
{
    const int N = 16;
    {
        std::cerr << "Test 0\n";
        ra::detail::shared_shape<int,1> sh;
        assert(sh.extent_ == (std::array<ra::detail::shared_shape<int,1>::size_type ,1>()));
        assert(sh.ptrs_ == nullptr);
        assert(sh.orig_ == nullptr);
        assert(sh.noffsets_ == 0);
        assert(sh.ndataoffsets_ == 0);
    }
    {
        std::cerr << "Test 1\n";
        int a[N];
        ra::detail::shared_shape<int,1> sh( {N}, &(a[0]) );
        assert(sh.noffsets_ == 0);
        assert(sh.ndataoffsets_ == 1);
        for (int i=0;i<N;i++)
            a[i] = i;
        int* p = sh.ptrs();
        for (int i1 = 0; i1 < N; i1++) {
            assert(p[i1]==i1);
        }
    }
    {
        std::cerr << "Test 2\n";
        int a[N][N/2];
        ra::detail::shared_shape<int,2> sh( {N,N/2}, &(a[0][0]) );  // 16 x 8 
        assert(sh.noffsets_ == N);
        assert(sh.ndataoffsets_ == N);
        int* aptr=&(a[0][0]);
        for ( int i=0;i<N*(N/2);i++)
            aptr[i] = i;
        int*const* p = sh.ptrs();
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++) {
                assert(p[i1][i2] == i1*(N/2) +i2);
            }
    }
    {
        std::cerr << "Test 3\n";        
        int a[N][N/2][N/4];
        ra::detail::shared_shape<int,3> sh{{N,N/2,N/4},&(a[0][0][0])};  // 16 x 8 x 4
        assert(sh.noffsets_ == N*(N/2+1));
        assert(sh.ndataoffsets_ == N*(N/2));
        int* aptr = &(a[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr[i] = i;
        int*const*const* p = sh.ptrs();
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++) {
                    assert(p[i1][i2][i3] == (i1*(N/2)  +i2)*(N/4)+i3);
                }       
    }
    {
        std::cerr << "Test 4\n";        
        int a[N][N/2][N/4][N/8];
        ra::detail::shared_shape<int,4> sh{{N,N/2,N/4,N/8},&(a[0][0][0][0])}; // 16 x 8 x 4 x 2
        assert(sh.noffsets_ == N*(N/2*(N/4+1)+1));
        assert(sh.ndataoffsets_ == N*(N/2)*(N/4));        
        int* aptr = &a[0][0][0][0];
        for ( int i=0;i<N*(N/2)*(N/4)*(N/8);i++)
            aptr[i] = i;
        int*const*const*const* p = sh.ptrs();
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++)
                    for (int i4 = 0; i4 < N/8; i4++) {
                        assert(p[i1][i2][i3][i4]
                               == ((i1*(N/2)+i2)*(N/4)+i3)*(N/8)+i4);
                    }
    }
    {
        std::cerr << "Test 5\n";        
        int a[N][N/2][N/4][N/8][N/16];
        ra::detail::shared_shape<int,5> sh({N,N/2,N/4,N/8,N/16},&(a[0][0][0][0][0])); // 16 x 8 x 4 x 2 x 1
        assert(sh.noffsets_ == N*(N/2*(N/4*(N/8+1)+1)+1));
        assert(sh.ndataoffsets_ == N*(N/2)*(N/4)*(N/8));
        int* aptr = &a[0][0][0][0][0];
        for ( int i=0;i<N*(N/2)*(N/4)*(N/8)*(N/16);i++)
            aptr[i] = i;
        int*const*const*const*const* p = sh.ptrs();
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++)
                    for (int i4 = 0; i4 < N/8; i4++)
                        for (int i5 = 0; i5 < N/16; i5++) {
                            assert(p[i1][i2][i3][i4][i5]
                                   == (((i1*(N/2)+i2)*(N/4)+i3)*(N/8)+i4)*(N/16)+i5);
                        }
    }
    {
        // shallow copy
        std::cerr << "Test 6\n";
        int a[N][N/2][N/4];
        ra::detail::shared_shape<int,3> sh{{N,N/2,N/4},&(a[0][0][0])};  // 16 x 8 x 4
        assert(sh.noffsets_ == N*(N/2+1));
        assert(sh.ndataoffsets_ == N*(N/2));
        int* aptr = &(a[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr[i] = i;
        int*const*const* p = sh.ptrs();
        ra::detail::shared_shape<int,3> sh2 = sh;
        assert(*(sh.refs_) == 2);
        assert(sh.refs_ == sh2.refs_);
        int*const*const* p2 = sh2.ptrs();
        assert(p == p2);
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++) {
                    assert(p[i1][i2][i3] == p2[i1][i2][i3]);
                }   
        
    }
    {
        // deep copy
        std::cerr << "Test 7\n";
        int a[N][N/2][N/4];
        ra::detail::shared_shape<int,3> sh{{N,N/2,N/4},&(a[0][0][0])};  // 16 x 8 x 4
        assert(sh.noffsets_ == N*(N/2+1));
        assert(sh.ndataoffsets_ == N*(N/2));
        int* aptr = &(a[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr[i] = i;
        int*const*const* p = sh.ptrs();
        ra::detail::shared_shape<int,3> sh2 = sh.copy();
        assert(*(sh.refs_) == 1);
        assert(*(sh2.refs_) == 1);
        assert(sh.refs_ != sh2.refs_);
        int*const*const* p2 = sh2.ptrs();
        assert(p != p2);
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++) {
                    assert(p[i1][i2][i3] == p2[i1][i2][i3]);
                }           
    }
    {
        // .relocate
        std::cerr << "Test 8\n";
        int a[N][N/2][N/4];
        ra::detail::shared_shape<int,3> sh{{N,N/2,N/4},&(a[0][0][0])};  // 16 x 8 x 4
        assert(sh.noffsets_ == N*(N/2+1));
        assert(sh.ndataoffsets_ == N*(N/2));
        int* aptr = &(a[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr[i] = i;
        int*const*const* p = sh.ptrs();
        ra::detail::shared_shape<int,3> sh2 = sh;
        int a2[N][N/2][N/4];
        int* aptr2 = &(a2[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr2[i] = i+1;
        sh2.relocate(aptr2);
        assert(*(sh.refs_) == 1);
        assert(*(sh2.refs_) == 1);
        assert(sh.refs_ != sh2.refs_);
        int*const*const* p2 = sh2.ptrs();
        assert(p != p2);
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++) {
                    assert(p[i1][i2][i3] + 1 == p2[i1][i2][i3]);
                }           
    }    
    {
        // .reshape
        std::cerr << "Test 9\n";        
        int a[N][N/2][N/4];
        ra::detail::shared_shape<int,3> sh{{N,N/2,N/4},&(a[0][0][0])};  // 16 x 8 x 4
        assert(sh.noffsets_ == N*(N/2+1));
        assert(sh.ndataoffsets_ == N*(N/2));
        int* aptr = &(a[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr[i] = i;
        int*const*const* p = sh.ptrs();
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++) {
                    assert(p[i1][i2][i3] == (i1*(N/2)  +i2)*(N/4)+i3);
                }       
        sh.reshape({N/4,N/2,N});
        p = sh.ptrs();
        for (int i1 = 0; i1 < N/4; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N; i3++) {
                    assert(p[i1][i2][i3] == (i1*(N/2)  +i2)*(N)+i3);
                }       
        
    }
    {
         std::cerr << "Test 10\n";  
         int a[N][N/2][N/4][N/8];
         ra::detail::shared_shape<int,4> sh{{N,N/2,N/4,N/8},&(a[0][0][0][0])}; // 16 x 8 x 4 x 2
         assert(sh.noffsets_ == N*(N/2*(N/4+1)+1));
         assert(sh.ndataoffsets_ == N*(N/2)*(N/4));        
         int* aptr = &a[0][0][0][0];
         for ( int i=0;i<N*(N/2)*(N/4)*(N/8);i++)
             aptr[i] = i;
         ra::detail::shared_shape<int,3> subsh = sh.at(1);
         int*const*const* p2 = subsh.ptrs();
         int i1 = 1;
         for (int i2 = 0; i2 < N/2; i2++)
             for (int i3 = 0; i3 < N/4; i3++)
                 for (int i4 = 0; i4 < N/8; i4++) {
                     assert(p2[i2][i3][i4]
                            == ((i1*(N/2)+i2)*(N/4)+i3)*(N/8)+i4);
                 }
    }
    {
        std::cerr << "Test 11\n";
        int a[N][N/2];
        ra::detail::shared_shape<int,2> sh( {N,N/2}, &(a[0][0]) );  // 16 x 8 
        assert(sh.noffsets_ == N);
        assert(sh.ndataoffsets_ == N);
        bool caught = false;
        try {
            ra::detail::shared_shape<int,1> subsh = sh.at(N+1);
        }
        catch (...) {
            caught = true;
        }
        assert(caught);
    }
    {
        // deep copy const
        std::cerr << "Test 12\n";
        int a[N][N/2][N/4];
        ra::detail::shared_shape<const int,3> sh{{N,N/2,N/4},&(a[0][0][0])};  // 16 x 8 x 4
        assert(sh.noffsets_ == N*(N/2+1));
        assert(sh.ndataoffsets_ == N*(N/2));
        int* aptr = &(a[0][0][0]);
        for (int i=0;i<N*(N/2)*(N/4);i++)
            aptr[i] = i;
        const int*const*const* p = sh.ptrs();
        ra::detail::shared_shape<const int,3> sh2 = sh.copy();  //failed so far
        assert(*(sh.refs_) == 1);
        assert(*(sh2.refs_) == 1);
        assert(sh.refs_ != sh2.refs_);
        const int*const*const* p2 = sh2.ptrs();
        assert(p != p2);
        for (int i1 = 0; i1 < N; i1++)
            for (int i2 = 0; i2 < N/2; i2++)
                for (int i3 = 0; i3 < N/4; i3++) {
                    assert(p[i1][i2][i3] == p2[i1][i2][i3]);
                }           
    }
    
    return 0;
}
