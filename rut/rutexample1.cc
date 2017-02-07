// example for the rarray-unit-test (rut)
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

