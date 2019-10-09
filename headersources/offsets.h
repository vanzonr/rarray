// offsets.h
//
// Header for the helper class for rarray 2.0 to compute pointer
// offsets in a multidimensional array.
//
// Copyright (c) 2018-2019  Ramses van Zon
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
#ifndef OFFSETSH
#define OFFSETSH

#include <cstdlib>
#include <vector>
#include <type_traits>

/***************************************************************************/

static char _ptrptrname = 'A';

class Offsets
{
  public:

    // offsets are computed by the constructor
    Offsets(const std::vector<ssize_t>& dims);

    // they can be applied to yield a pointer-to-pointer structure;
    // for rank>1, this structure will be allocated; it should be deallocate elsewhere
    // for rank==1, this structure is identical to data itself, so no allocation is done.
    template<class T>  void*** apply_offsets(T* data) const;

    // the ptr-to-ptr structure 
    ssize_t get_num_data_offsets() const;
    ssize_t get_num_offsets() const;
    ssize_t get_rank() const;
    
  private:

    ssize_t rank_;
    std::vector<unsigned long long> offsets_;
    ssize_t ndataoffsets_;

    // for testing and debugging purposes
    char name_;
    friend int test_offsets_main(); 
};

/***************************************************************************/

inline Offsets::Offsets(const std::vector<ssize_t>& dims)
{
    name_ = _ptrptrname++;
    std::vector<ssize_t> extent(dims);
    rank_ = extent.size();
    ssize_t noffsets = 0;
    ndataoffsets_ = 0;
    if (rank_ > 0) {
        ndataoffsets_ = 1;
        for (ssize_t i = rank_ - 1; i--; )
            noffsets = extent[i]*(1 + noffsets);
        for (ssize_t i = 0 ; i < rank_-1; i++ ) 
            ndataoffsets_ *= extent[i];
        offsets_.reserve(noffsets);
        offsets_.resize(noffsets);
        if (noffsets > 1) { // catches rank_ == 1 and zero-dimensioned arrays
            ssize_t offsetnum = 0;
            ssize_t extenttot = extent[0];
            for (ssize_t i = 1; i < rank_ - 1; i++) {
                for (ssize_t j = 0; j < extenttot; j++) 
                    offsets_[offsetnum+j] = offsetnum + extenttot + j*extent[i];
                offsetnum += extenttot;
                extenttot *= extent[i];
            }
            ndataoffsets_ = extenttot;
            for (ssize_t j = 0; j < ndataoffsets_; j++) 
               offsets_[offsetnum + j] = j*extent[rank_ - 1];
        }
    }
}

template<class T>
inline void*** Offsets::apply_offsets(T* data) const
{
    ssize_t noffsets = offsets_.size();
    if (ndataoffsets_ == 0 && noffsets == 0)
        return nullptr;
    else if (ndataoffsets_ == 1 && noffsets == 0) // that happens only for rank==1
        return (void***)(data);
    else {
        void*** offsets = new void**[noffsets];
        ssize_t i = 0;
        for (;i < noffsets - ndataoffsets_; i++)            
            offsets[i] = (void**)offsets + offsets_[i];
        for (;i < noffsets; i++)            
            offsets[i] = reinterpret_cast<void**>(
                              const_cast<typename std::remove_const<T>::type*>(data)
                              + offsets_[i]);
        return offsets;
    }
}

inline ssize_t Offsets::get_num_data_offsets() const
{
    return ndataoffsets_;
}

inline ssize_t Offsets::get_num_offsets() const
{
    return offsets_.size();
}

inline ssize_t Offsets::get_rank() const
{
    return rank_;
}

/***************************************************************************/

#endif
