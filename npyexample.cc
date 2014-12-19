//
// npyexample.cc - example of writing rarrays to npy files.
//
// Copyright (c) 2013-2014  Ramses van Zon
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
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <complex>
#include "rarray.h"

struct my_traits_def { 
    enum { other = '?', INT='i', UNSIGNED='u', FLOAT='f', COMPLEX='c', BOOL='b' }; 
    enum { no_endian = '|',  little_endian = '<', big_endian = '>' };
};
template<typename T> struct my_traits                   {enum{numtype=my_traits_def::other};};
template<> struct my_traits<bool>                       {enum{numtype=my_traits_def::BOOL};};
template<> struct my_traits<char>                       {enum{numtype=my_traits_def::INT};};
template<> struct my_traits<short>                      {enum{numtype=my_traits_def::INT};};
template<> struct my_traits<int>                        {enum{numtype=my_traits_def::INT};};
template<> struct my_traits<long>                       {enum{numtype=my_traits_def::INT};};
template<> struct my_traits<long long>                  {enum{numtype=my_traits_def::INT};};
template<> struct my_traits<unsigned char>              {enum{numtype=my_traits_def::UNSIGNED};};
template<> struct my_traits<unsigned short>             {enum{numtype=my_traits_def::UNSIGNED};};
template<> struct my_traits<unsigned int>               {enum{numtype=my_traits_def::UNSIGNED};};
template<> struct my_traits<unsigned long>              {enum{numtype=my_traits_def::UNSIGNED};};
template<> struct my_traits<unsigned long long>         {enum{numtype=my_traits_def::UNSIGNED};};
template<> struct my_traits<float>                      {enum{numtype=my_traits_def::FLOAT};};
template<> struct my_traits<double>                     {enum{numtype=my_traits_def::FLOAT};};
template<> struct my_traits<long double>                {enum{numtype=my_traits_def::FLOAT};};
template<> struct my_traits<std::complex<float> >       {enum{numtype=my_traits_def::COMPLEX};};
template<> struct my_traits<std::complex<double> >      {enum{numtype=my_traits_def::COMPLEX};};
template<> struct my_traits<std::complex<long double> > {enum{numtype=my_traits_def::COMPLEX};};

template<typename T, int R>
char* newNpyHeader(const int* n, int& headersize)
{
    // Determine endianness (assumes that long is more than one byte) 
    char orderchar = my_traits_def::no_endian;
    if (sizeof(T) != 1) {
        const long one = 1; 
        const bool is_little_endian = *reinterpret_cast<const char*>(&one);
        orderchar = is_little_endian?my_traits_def::little_endian:my_traits_def::big_endian;
    }
    // Detect type
    const char typechar = char(my_traits<T>::numtype);
    // Output extents as dictionary using stringstream
    std::stringstream dict;
    dict << "{'descr': '" << orderchar << typechar << sizeof(T) << "', "
         << "'fortran_order': False, " << "'shape': (" << n[0];
    for (int i=1;i<R;i++)
        dict << ',' << n[i];
    dict << "), }\n";
    // pad to have header (incl. newline) be a multiple of 16 characters long
    int headlen = 11+dict.str().length();
    int add2len = int((headlen+15)/16)*16 - headlen;
    dict << std::string(add2len, ' ') << '\n'; // note the new line: part of the format!
    // get the string out
    const std::string dictstr = dict.str();
    int dictlen = dictstr.length();
    // allocate header
    char* header = new char[10+dictlen];
    // fill it
    header[0] = '\x93';
    header[1] = 'N';
    header[2] = 'U';
    header[3] = 'M';
    header[4] = 'P';
    header[5] = 'Y';
    header[6] = '\x01';
    header[7] = '\0';
    header[8] = char(dictlen%256);
    header[9] = char(dictlen/256);
    memcpy(header + 10, dictstr.c_str(), dictlen);
    // report back the size
    headersize = 10+dictlen;
    // return the pointer to the header
    return header;
}

int main()
{
    const int max=100;
    double z[max][max];
    
    for (int i=0;i<max;i++)
        for (int j=0;j<max;j++)
            z[i][j] = sin(i*.0314)*sin(j*0.0314);

    rarray<double,2> r(RARRAY(z));
    
    int   headersize;
    char* header = newNpyHeader<double,2>(r.shape(), headersize);
    
    std::cout.write(header, headersize);

    delete[] header;

    std::cout.write(reinterpret_cast<const char*>(r.data()),r.size()*sizeof(double));
                                   
    return 0;

}
