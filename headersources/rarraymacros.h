//
// rarraymacros.h - Checks and inlining macros used in runtime arrays
//
// Copyright (c) 2015-2022  Ramses van Zon
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
#ifndef HEADERSOURCES_RARRAYMACROS_H_  // EXCLUDE //
#define HEADERSOURCES_RARRAYMACROS_H_  // EXCLUDE //
// Compiling with -DRA_BOUNDSCHECK switches on the RA_CHECKORSAY macro to
// check its first argument and throw an exception if it is not true.
// checkOrSay is intended to be used for bound checks.
#ifdef RA_BOUNDSCHECK
#include <string>
#define RA_CHECKORSAY(a, b) if (not(a)) throw std::out_of_range(std::string(b) + " in function " + std::string(__PRETTY_FUNCTION__) + " (rarray:" + std::to_string(__LINE__) + ")")
#define RA_noboundscheck false
#else
#define RA_CHECKORSAY(a, b)
#define RA_noboundscheck true
#endif

// Routines using RA_FORCE_inline instead of inline will be forced to
// inline RA_FORCE_inline includes the "inline" keyword.  Works for
// gcc, clang and intel compilers, and shuold work MS Visual C++, but
// untested.  For unsupported compilers, RA_FORCE_inline becomes just
// inline. For xlC, you'll likely need "-O4" to get full inlining.
//
#if !defined(RA_FORCE_inline)
# if defined(_MSC_VER)
#  if _MSC_VER >= 1900
#   define RA_FORCE_inline [[msvc::forceinline]] inline
#  else
#   define RA_FORCE_inline __forceinline inline
#  endif
# elif defined(__INTEL_COMPILER)
#  define RA_FORCE_inline  __forceinline inline
# elif defined(__clang__)
#  if __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ < 3)
#   define RA_FORCE_inline __attribute__((always_inline)) inline
#  else
#   define RA_FORCE_inline [[gnu::always_inline]] inline
#  endif
# elif defined(__GNUC__)
#  if __GNUC__ < 4 || (GNU_C == 4 && __GNUC_MINOR < 8)
#   define RA_FORCE_inline __attribute__((always_inline)) inline
#  else
#   define RA_FORCE_inline [[gnu::always_inline]] inline
#  endif
# else
#  define RA_FORCE_inline inline
# endif
#endif
#endif  // EXCLUDE //
