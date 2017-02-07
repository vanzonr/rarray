// rut.cc
//
// Implementation part of RUT (rarray unit test)
//
// Ramses van Zon, 2017
//
#include "rut.h"
#include <iostream>
#include <exception>
#include <utility>
#include <vector>
#include <cstdlib>

// a test is name and a function
typedef std::pair<std::string,int(*)()>     test_t;
// a list of tests is a vector of tests
typedef std::vector<test_t>                 testlist_t;
// a testsuite is a name plus a list of tests
typedef std::pair<std::string, testlist_t>  testsuite_t;
// all testsuites are collected in a testsuitelist
typedef std::vector<testsuite_t>            testsuitelist_t;

// create a singleton testsuitelist_t
static testsuitelist_t& testsuites()
{
    static testsuitelist_t* _testsuites = new testsuitelist_t;
    return *_testsuites;
}

// for unit tests not built with UNIT_TEST, need an external global counter
int _error_counter = 0;

// method for building testsuites
void add_test_to_testsuite(const std::string& testsuitename, const std::string& testname, int(*testfunc)())
{
    // find the test suite by name 
    testsuitelist_t::iterator tsit; 
    for (tsit = testsuites().begin();
         tsit != testsuites().end();
         ++tsit)
    {
        if (tsit->first == testsuitename)
            break;
    }
    testsuite_t* ts; // will store the handle to the testsuite
    // create new testsuite if it does not exist yet
    if (tsit == testsuites().end()) {
        testsuites().push_back(testsuite_t(testsuitename,testlist_t()));
        ts = &(testsuites().back());
    } else {
        ts = &(*tsit);
    }
    // add to test suite
    if (testname[0]=='(' and testname[testname.length()-1]==')') {
        // strip enclosing '(' and ')' from the test name
        ts->second.push_back(test_t(testname.substr(1,testname.length()-2),testfunc));
    } else {
        ts->second.push_back(test_t(testname,testfunc));
    }
}

// driver routine: at some point will be able to select which suites and test to run
int main(int argc, char* arv[])
{
    // All registered test suites will be run. 
    bool any_error = false; // See if any of them detected issues
    // test suites are in the global 'testsuite' variable
    int testsuiteindex = 1;
    for (testsuitelist_t::iterator tsit = testsuites().begin(); tsit != testsuites().end(); ++tsit)
    {
        // each item in the testsuite list is a pair of, first: a name of the test, and second: a pointer to a list of test functions.
        int numerror = 0;
        std::cout << "Performing testsuite '" << tsit->first << "' ("<<(testsuiteindex++)<<"/"<<testsuites().size() <<")\n";
        // each item in the list of test functions is a pair of first: a name of the test, and second: a pointer to the test function.
        std::vector<std::pair<std::string,int(*)()> >::iterator it;
        int testindex = 1;
        int testspassed = 0;
        for (it = tsit->second.begin(); it != tsit->second.end(); ++it)
        {
            std::cout << "Running test case '" << it->first << "' (" << (testindex++) << "/" << tsit->second.size() << ")\n";
            try {
                // if the function is external, it'll use the global _error_counter: check it
                ::_error_counter = 0;
                // call the test function
                int exitcode = (int)(it->second());
                // function only returns using if the fcuntion explicit returns. If in a RUT TEST SUITE, it throws an exception.
                if (exitcode==0) {
                    if (::_error_counter == 0) {
                        std::cout << ">>>> No errors in '" << it->first << "'.\n";
                        testspassed++;
                    } else {
                        std::cout << ">>>> " << abs(::_error_counter) << " error(s) in '" << it->first << "'.\n";
                        numerror += abs(::_error_counter);
                    }
                } else {
                    if (::_error_counter == 0) {
                        std::cout << ">>>> An error signaled in '" << it->first << "'. Exit code of function: " << exitcode << "\n";
                        numerror += 1;
                    } else {
                        std::cout << ">>>> " << abs(::_error_counter)+1 << " error(s) detected in '" << it->first << "'. Exit code of function: " << exitcode << "\n";
                        numerror += abs(::_error_counter)+1;
                    }
                }
            }
            catch (int error_counter){
                // add local and global counter
                numerror += abs(error_counter + ::_error_counter);
                if (error_counter < 0) {                    
                    std::cout << ">>>> " << abs(error_counter) << " error(s) detected in '" << it->first << "' (test threw exception).\n";
                } else if (error_counter < 0) {                    
                    std::cout << ">>>> " << abs(error_counter) << " error(s) detected in '" << it->first << "'.\n";
                } else {
                    std::cout << ">>>> No errors in '" << it->first << "'.\n";
                    testspassed++;
                }
            }
            catch (...) {
                numerror ++;
                std::cout << ">>>> errors detected in '" << it->first << "'.\n";
            }
            
        }
        // report number of errors in this test suite and set any_error flag
        if (numerror != 0) {
            any_error = true;
            if (numerror==1) 
                std::cout << ">>>>>> Finished testsuite '" << tsit->first << "' containing " << tsit->second.size() << " tests. "<< testspassed << " tests passed. Detected 1 error.\n";
            else
                std::cout << ">>>>>> Finished testsuite '" << tsit->first << "' containing " << tsit->second.size() << " tests. "<< testspassed << " tests passed. Detected " << abs(numerror) << " errors.\n";
        } else {
            std::cout << ">>>>>> Finished testsuite '" << tsit->first << "' containing " << tsit->second.size() << " tests. "<< testspassed << " tests passed. No errors.\n";
        }
    }
    return any_error?1:0;
}
