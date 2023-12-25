//
// elapsed.h - header file for elapsed time functions in c/c++
//
// Copyright (c) 2013-2023  Ramses van Zon
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
#ifndef HEADERSOURCES_ELAPSED_H_
#define HEADERSOURCES_ELAPSED_H_

#include <sys/time.h>
#include <cstdio>

using Stopwatch = double;

inline double elapsed_etime() {
    struct timeval t;
    gettimeofday(&t, 0);
    return static_cast<double>(t.tv_sec) + 0.000001 * static_cast<double>(t.tv_usec);
}

inline void elapsed_write(double time) {
    int continu = 0;
    int recapsec = 0;
    auto itime = static_cast<decltype(1ULL)>(time*1000000);
    if (itime >= 1000) {
        if (itime > 86400000000ULL) {
            fprintf(stderr, "%llud:", itime/86400000000ULL);
            itime %= 86400000000ULL;
            continu = 1;
            recapsec = 1;
        }
        if (itime > 3600000000ULL) {
            fprintf(stderr, "%lludh:", itime/3600000000ULL);
            itime %= 3600000000ULL;
            continu = 1;
            recapsec = 1;
        }
        if (itime > 60000000ULL) {
            fprintf(stderr, "%llum:", itime/60000000ULL);
            itime %= 60000000ULL;
            continu = 1;
            recapsec = 1;
        }
        fprintf(stderr, "%llu.", itime/1000000);
        itime %= 1000000;
        if (itime > 1000 || continu == 1) {
            fprintf(stderr, "%03llus", itime/1000);
            itime %= 1000;
        }
    } else {
        fprintf(stderr, "%llu microsec.", itime);
    }
    if (recapsec == 1)
        fprintf(stderr, " (%ds)", static_cast<int>(time));
}

#define START -elapsed_etime()

inline void stopwatchStop(Stopwatch *sw) {
    *sw += elapsed_etime();
    elapsed_write(*sw);
    fprintf(stderr, " elapsed\n");
    *sw = -elapsed_etime();
}

#endif  // HEADERSOURCES_ELAPSED_H_
