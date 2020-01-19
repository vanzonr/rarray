// rutexample1.cc - example for the rarray-unit-test (rut)
//
// Copyright (c) 2017-2019  Ramses van Zon
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

#include "rut.h"
#include <iostream>

int add( int i, int j ) { return i+j+1; }

TEST_SUITE(mytestsuite)    

UNIT_TEST(testname)
{
    WARN(1==1);
    // six ways to detect and report the same error:
    CHECK( add(2,2) == 4 );          // #1 continues on error
    REQUIRE( add(2,2) == 5 );        // #2 throws on error
    if ( add(2,2) != 4 )
        ERROR( "Ouch...");           // #3 continues on error
    if ( add(2,2) != 5 )
        FAIL( "Ouch..." );           // #4 throws on error
    if ( add(2,2) != 4 )
        throw "Oops...";                   // #5 throws on error
    return add(2,2) == 4 ? 0 : 1;          // #6 returns error code
}
END_UNIT_TEST

END_TEST_SUITE
