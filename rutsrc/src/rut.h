// rut.cc
//
// Header file for RUT (rarray unit test)
//
// Ramses van Zon, 2017
//

#ifndef RARRAYUTH
#define RARRAYUTH

#include <string>
#include <iostream>
#include <cmath>

// add to this list of tests
void add_test_to_testsuite(const std::string& testsuitename, const std::string& testname, int(*testfunc)());

// for unit tests not built with UNIT_TEST, need an external global counter
extern int _error_counter;

// a prefix for parallel tests (for mpi, this contains a rank, otherwise is empty)
extern std::string _prefix;

// macros for definining testsuites and tests
#define CONCAT0(x,y)  x##y
#define CONCAT(x,y)   CONCAT0(x,y)
#define TEST_SUITE(sn) namespace CONCAT(sn,__LINE__) { const std::string suitename = #sn;
#define END_TEST_SUITE  }
#define UNIT_TEST(tn) int tn(); struct CONCAT(Adder,__LINE__) { CONCAT(Adder,__LINE__)() { add_test_to_testsuite(suitename, #tn, tn); } } CONCAT(adder,__LINE__); int tn() { int _error_counter = 0; try {
#define END_UNIT_TEST } catch(...) { _error_counter = - _error_counter - 1; } throw _error_counter; }

// macro for importing an existing function
#define UNIT_TEST_IMPORT(tn)          int tn(); struct CONCAT(Adder,__LINE__) { CONCAT(Adder,__LINE__)() { add_test_to_testsuite(suitename, #tn, tn); } } CONCAT(adder,__LINE__); 
#define UNIT_TEST_NAMED_IMPORT(nm,tn) int tn(); struct Adder##nm { Adder##nm() { add_test_to_testsuite(suitename, #nm, tn); } } adder##nm; 

// macros for checking
#define ERROR(x)                     {_error_counter++; std::cerr<<_prefix<<__FILE__<<"("<< __LINE__<<"): " << "error: '" << x << "' failed\n";}
#define FAIL(x)                      {_error_counter++; std::cerr<<_prefix<<__FILE__<<"("<< __LINE__<<"): " << "error: '" << x << "' failed\n"; return _error_counter;}
#define ONWARN(x)                    {std::cerr<<_prefix<<__FILE__<<"("<< __LINE__<<"): warning: condition not satisfied: '" << x << "'\n"; }
#define WARN(x)                      {if (!(x)) ONWARN(#x)}
#define CHECK(x)                     {if (!(x)) ERROR(#x)}
#define REQUIRE(x)                   {if (!(x)) FAIL(#x)}
#define CHECK_EQUAL(x,y)             {if (!((x)==(y))) ERROR(#x) }
#define CHECK_CLOSE(x,y,eps)         {if (!(fabs((x)-(y))<eps)) ERROR(#x)}
#define CHECK_EQUAL_ARRAY(n,x,y)     {for (size_t i=0;i<n;++i) { if(!(x[i]==y[i])) { ERROR(#x ## "!=" ## #y); break; }}}
#define CHECK_CLOSE_ARRAY(n,x,y,eps) {for (size_t i=0;i<n;++i) { if(!(fabs(x[i]-y[i])<eps)){ ERROR(#x ## "!=~" ## #y); break;} }}
#define ALL_CLEAR                    0

#endif
