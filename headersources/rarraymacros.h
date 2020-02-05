//
// rarraymacros.h - Checks and inlining macros used in runtime arrays
//
// Copyright (c) 2015-2020  Ramses van Zon
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

// Compiling with -DRA_BOUNDSCHECK switches on the RA_CHECKORSAY macro to
// check its first argument and throw an exception if it is not true.
// checkOrSay is intended to be used for bound checks.
#ifdef RA_BOUNDSCHECK
#include <string>
#define RA_CHECKORSAY(a, b) if (not(a)) throw std::out_of_range(std::string(b) + " in function " + std::string(__PRETTY_FUNCTION__))
#else
#define RA_CHECKORSAY(a, b) 
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
#   define RA_INLINE __attribute__((always_inline)) inline
# else
#   define RA_INLINE inline
# endif
#endif

// routines using INLINEF will be forced to inline
// routines using INLINE_ will not: these were deemed to expensive to inline from a compilation point of view
#define RA_INLINEF RA_INLINE
#define RA_INLINE_ inline
