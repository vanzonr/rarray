// rut.cc
//
// Implementation part of RUT (rarray unit test)
//
// Ramses van Zon, 2017
//
#include "rut.h"
#ifdef _MPI
#include <mpi.h>
#endif
#include <iostream>
#include <exception>
#include <utility>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iomanip>

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

// define a prefix; this contains the rank if run as mpi, else, it
// is an empty string.
std::string _prefix;

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

// Keep the code simpler by defining a few macros.
#ifdef _MPI

// synchronize processes (so tests run at the same time).
#define BARRIER {std::cout.flush();std::cerr.flush();MPI_Barrier(MPI_COMM_WORLD);}
// print only once, by process 0
#define PRINT_ONCE(out) {BARRIER; if (rank==0) std::cerr << out.str(); BARRIER;}
void print_gather(int rank, int size, const std::stringstream& out) 
{
   const std::string s = out.str();
   const int mylen = s.length();
   int mypos;
   int slen[size], spos[size];
   MPI_Scan(&mylen, &mypos, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
   mypos -= mylen;
   MPI_Allgather(&mypos, 1, MPI_INT, spos, 1, MPI_INT, MPI_COMM_WORLD);
   MPI_Allgather(&mylen, 1, MPI_INT, slen, 1, MPI_INT, MPI_COMM_WORLD);
   int stotallen = spos[size-1]+slen[size-1];
   char* all = new char[stotallen+1];
   MPI_Gatherv(s.c_str(), mylen, MPI_CHAR, all, slen, spos, MPI_CHAR, 0, MPI_COMM_WORLD);
   if (rank==0) {
      all[stotallen] = '\0';
      std::cerr << all;
      std::cerr.flush();
   }
   delete[] all;
}
// print from all processes, by gather on process 0
#define PRINT_GATHER(out) print_gather(rank,size,out)
// rank prefix precomputation
std::string computePrefix(int rank, int size)
{
    std::stringstream prefixss;
    int n = 1;
    while (size>=10) {
       n++;
       size/=10;
    }
    prefixss << "[rank " << std::setfill('0') << std::setw(n) << rank << "] ";
    return prefixss.str();
}
// MPI error handler
void mpi_handler(MPI_Comm* comm, int *error_code, ...)
{
   char error_string[MPI_MAX_ERROR_STRING];
   int error_string_length;
   MPI_Error_string(*error_code, error_string, &error_string_length);
   ERROR(error_string);
   throw "This is the mpi error handler\n";
}

#else

#define BARRIER
#define PRINT_ONCE(out) std::cerr << out.str();
#define PRINT_GATHER(out) {std::cerr << out.str(); std::cerr.flush();}

#endif

// driver routine: at some point will be able to select which suites
// and test to run
int main(int argc, char* argv[])
{
    // deal with possibility of mpi tests
    #ifdef _MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Errhandler errhandler;
    MPI_Comm_create_errhandler(&mpi_handler, &errhandler);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    _prefix = computePrefix(rank,size);
    BARRIER;
    #else
    _prefix = "";
    #endif
    // All registered test suites will be run. 
    bool any_error = false; // See if any of them detected issues
    // test suites are in the global 'testsuite' variable
    int testsuiteindex = 1;
    for (testsuitelist_t::iterator tsit = testsuites().begin(); tsit != testsuites().end(); ++tsit)
    {
        std::stringstream suiteoutput;
        // each item in the testsuite list is a pair of, 
        // first: a name of the test, and second: a pointer to a list
        // of test functions.
        int numerror = 0;
        suiteoutput << "Performing testsuite '" << tsit->first << "' ("<<(testsuiteindex++)<<"/"<<testsuites().size() <<")" << std::endl;
        PRINT_ONCE(suiteoutput);
        // each item in the list of test functions is a pair of first:
        // a name of the test, and second: a pointer to the test
        // function.
        std::vector<std::pair<std::string,int(*)()> >::iterator it;
        int testindex = 1;
        int testspassed = 0;
        for (it = tsit->second.begin(); it != tsit->second.end(); ++it)
        {
            std::stringstream testheader;
            testheader << "Running test case '" << it->first << "' (" << (testindex++) << "/" << tsit->second.size() << ")" << std::endl;
            PRINT_ONCE(testheader);
            std::stringstream testoutput;
            try {
                // if the function is external, it'll use the global
                // _error_counter: check it
                ::_error_counter = 0;
                // call the test function
                int exitcode = (int)(it->second());
                // function only returns using if the fucntion
                // explicit returns. If in a RUT TEST SUITE, it throws
                // an exception.
                if (exitcode==0) {
                    if (::_error_counter == 0) {
                        testoutput << ">>>> " << _prefix << "No errors in '" << it->first << "'.\n";
                        testspassed++;
                    } else {
                        testoutput << ">>>> " << _prefix << abs(::_error_counter) << " error(s) in '" << it->first << "'." << std::endl;
                        numerror += abs(::_error_counter);
                    }
                } else {
                    if (::_error_counter == 0) {
                        testoutput << ">>>> " << _prefix << "An error signaled in '" << it->first << "'. Exit code of function: " << exitcode << std::endl;
                        numerror += 1;
                    } else {
                        testoutput << ">>>> " << _prefix << abs(::_error_counter)+1 << " error(s) detected in '" << it->first << "'. Exit code of function: " << exitcode << std::endl;
                        numerror += abs(::_error_counter)+1;
                    }
                }
            }
            catch (int error_counter){
                // add local and global counter
                numerror += abs(error_counter + ::_error_counter);
                if (error_counter < 0) {                    
                    testoutput << ">>>> " << _prefix << abs(error_counter) << " error(s) detected in '" << it->first << "' (test threw exception).\n";
                } else if (error_counter > 0) {                    
                    testoutput << ">>>> " << _prefix << abs(error_counter) << " error(s) detected in '" << it->first << "'.\n";
                } else {
                    testoutput << ">>>> " << _prefix << "No errors in '" << it->first << "'.\n";
                    testspassed++;
                }
            }
            catch (...) {
                numerror ++;
                testoutput << ">>>> " << _prefix << "errors detected in '" << it->first << "'.\n";
            }            
            PRINT_GATHER(testoutput);
        }
        BARRIER;
        std::stringstream finalstr;
        finalstr << ">>>>>>>> Finished testsuite '" << tsit->first << "' containing " << tsit->second.size() << " tests. " << std::endl;
        PRINT_ONCE(finalstr);
        std::stringstream finalstrperrank;
        // report number of errors in this test suite and set
        // any_error flag
        if (numerror != 0) {
            any_error = true;
            if (numerror==1) 
                finalstrperrank << ">>>>>>>> " << _prefix << testspassed << " tests passed. Detected 1 error." << std::endl;
            else
                finalstrperrank << ">>>>>>>> " << _prefix << testspassed << " tests passed. Detected " << abs(numerror) << " errors." << std::endl;
        } else {
            finalstrperrank << ">>>>>>>> " << _prefix << testspassed << " tests passed. No errors." << std::endl;
        }
        PRINT_GATHER(finalstrperrank);
    }
    BARRIER;
    // The proper, if verbose, way to get an error code out of mpi is to
    // use MPI_Abort.
    #ifdef _MPI
    if (any_error) 
       MPI_Abort(MPI_COMM_WORLD, 1);
    else
       MPI_Finalize();
    #endif
    return any_error?1:0;
}
