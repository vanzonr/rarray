// offsets.h
//
// Header for the helper class for rarray 2.x to compute pointer
// offsets in a multidimensional array.
//
// Copyright (c) 2018-2026  Ramses van Zon
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
#ifndef HEADERSOURCES_OFFSETS_H_  // EXCLUDE //
#define HEADERSOURCES_OFFSETS_H_  // EXCLUDE //

#include <cstdlib>
#include <vector>
#include <type_traits>
#include "rarraymacros.h"
#include "rarraytypes.h"

namespace ra {
namespace detail {

class Offsets {
 public:
    // offsets are computed by the constructor
    RA_FORCE_inline explicit Offsets(const std::vector<index_type>& extent)
    : rank_(static_cast<rank_type>(extent.size())) {
        if (rank_ > 0) {
            size_type noffsets = 0;
            ndataoffsets_ = 1;
            for (rank_type dim = rank_ - 1; dim-- != 0; ) {
                noffsets = extent[dim]*(1 + noffsets);
            }
            for (rank_type dim = 0 ; dim < rank_-1; dim++) {
                ndataoffsets_ *= extent[dim];
            }
            offsets_.reserve(noffsets);
            offsets_.resize(noffsets);
            if (noffsets > 1) {  // catches rank_ == 1 and zero-dimensioned arrays
                size_type offsetnum = 0;
                size_type extenttot = extent[0];
                for (rank_type dim = 1; dim < rank_ - 1; dim++) {
                    for (size_type j = 0; j < extenttot; j++) {
                        offsets_[offsetnum+j] = offsetnum + extenttot + (j*extent[dim]);
                    }
                    offsetnum += extenttot;
                    extenttot *= extent[dim];
                }
                ndataoffsets_ = extenttot;
                for (index_type j = 0; j < ndataoffsets_; j++) {
                   offsets_[offsetnum + j] = j*extent[rank_ - 1];
                }
            }
        }
    }
    // they can be applied to yield a pointer-to-pointer structure;
    // for rank>1, this structure will be allocated; it should be deallocate elsewhere
    // for rank==1, this structure is identical to data itself, so no allocation is done.
    template<class T> RA_FORCE_inline auto apply_offsets(T* data) const -> void*** {
        static_assert(sizeof(T*) == sizeof(void*) && sizeof(T*) == sizeof(void**),
                      "rarray's Offsets requires all pointers to have the same size");
        auto noffsets = (size_type)offsets_.size();
        void*** offsets = nullptr;
        if (ndataoffsets_ != 0 || noffsets != 0) {
            if (ndataoffsets_ == 1 && noffsets == 0) {  // happens only for rank==1
                offsets = reinterpret_cast<void***>(const_cast<typename std::remove_const<T>::type*>(data));
            } else {
                offsets = new void**[noffsets];
                size_type ioffset = 0;
                for (; ioffset < noffsets - ndataoffsets_; ioffset++) {
                    offsets[ioffset] = reinterpret_cast<void**>(offsets) + offsets_[ioffset];
                }
                for (; ioffset < noffsets; ioffset++) {
                    offsets[ioffset] = reinterpret_cast<void**>(
                                      const_cast<typename std::remove_const<T>::type*>(data)
                                      + offsets_[ioffset]);
                }
            }
        }
        return offsets;
    }
    // information about the the ptr-to-ptr structure
    RA_FORCE_inline auto get_num_data_offsets() const noexcept -> size_type {
        return ndataoffsets_;
    }
    RA_FORCE_inline auto get_num_offsets() const noexcept -> size_type {
        return (size_type)offsets_.size();
    }
    RA_FORCE_inline auto get_rank() const noexcept -> rank_type {
        return rank_;
    }

 private:
    rank_type rank_{};
    std::vector<size_type> offsets_;
    size_type ndataoffsets_{};
};

}  // namespace detail
}  // namespace ra
#endif  // EXCLUDE //
