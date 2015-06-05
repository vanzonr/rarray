//
// rarraymacros.h - Checks and inlining macros used in runtime arrays
//
// Copyright (c) 2015  Ramses van Zon
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

// When running rarraytestsuite.cc compiled with -DRA_TRACETEST, the
// following macro produced output to be used to determine which
// functions are exercised.
#ifdef RA_TRACETEST
#define RA_IFTRACESAY(a) std::cerr << "IFTRACE " << __FILE__ << '@' << __LINE__<< ":\t" << a << std::endl;
#else
#define RA_IFTRACESAY(a) 
#endif

// Compiling with -DRA_BOUNDSCHECK switches on the checkOrSay macro to
// check its first argument and throw an exception if it is not true.
// checkOrSay is intended to be used for bound checks.
#ifdef RA_BOUNDSCHECK
#define RA_CHECKORSAY(a, b) if (not(a)) throw std::out_of_range(std::string(b) + " in function " + std::string(__PRETTY_FUNCTION__))
// BOUNDCHECK is incompatible with SKIPINTERMEDIATE (see below)
#ifdef RA_SKIPINTERMEDIATE
#undef RA_SKIPINTERMEDIATE
#endif
#else
#define RA_CHECKORSAY(a, b) 
#endif

#if __cplusplus <= 199711L
#define RA_NULLPTR 0 
#else
#define RA_NULLPTR nullptr
#endif

// For g++ and icpc, RA_INLINE forces inlining, even without optimization.
// In all other cases, RA_INLINE=inline, and inlining may not occur.
// Note for xlC: 
//    In version 10, you need "-O4" to get full inlining.
//    In version 11, "-O2 -qinline=level=6" suffices.
//
#if not defined(RA_INLINE)
# if defined(__INTEL_COMPILER)
#   define RA_INLINE  __forceinline
# elif defined(__GNUC__)
#   define RA_INLINE inline __attribute__((always_inline)) 
# else
#   define RA_INLINE inline
# endif
#endif

// routines using INLINEF will be forced to inline
// routines using INLINE_ will not: these were deemed to expensive to inline from a compilation point of view
#define RA_INLINEF RA_INLINE
#define RA_INLINE_ inline
