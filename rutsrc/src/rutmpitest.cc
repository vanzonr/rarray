// test of the rarray-unit-test (rut)
#include "rut.h"
#include <iostream>

TEST_SUITE(mytestsuite)    

UNIT_TEST(testname)
{
    std::cout << "Hello world" << std::endl;
    CHECK(1==1);
}
END_UNIT_TEST

UNIT_TEST(testname2)
{
    std::cout << "Hello universe" << std::endl;
}
END_UNIT_TEST

UNIT_TEST(nearequal)
{
    CHECK_CLOSE(1.04, 1.00, 0.05);
}
END_UNIT_TEST

int fails() {
    return 1==1;
}
UNIT_TEST_IMPORT(fails)

int func() {
    CHECK_EQUAL(1+1,2);
    return ALL_CLEAR;
}
UNIT_TEST_NAMED_IMPORT(one_plus_one, func)

END_TEST_SUITE

