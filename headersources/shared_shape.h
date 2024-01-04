// shared_shape.h
//
// Header to define a 'shape', which is the pointer-to-pointer
// interface to a multidimensional array, but one that remembers its
// shape.  Uses reference counting for the pointer-to-pointer
// structure, but the actual data is a simple pointer.  One of the
// building blocks for rarray 2.x.
//
// Copyright (c) 2018-2024  Ramses van Zon
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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERma
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef HEADERSOURCES_SHARED_SHAPE_H_  // EXCLUDE //
#define HEADERSOURCES_SHARED_SHAPE_H_  // EXCLUDE //

#include <array>
#include <stdexcept>
#include <memory>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <functional>
#include <string>
#include "rarraymacros.h"
#include "rarraytypes.h"
#include "offsets.h"

int test_shared_shape_main();  // EXCLUDE //

namespace ra {
namespace detail {

// class to hold the shape but not the content of a multi-dimensional array.

template<typename T, rank_type R>
struct PointerArray {
    using type = typename PointerArray<T, R-1>::type const*;            // const shape, recursive
    using noconst_type = typename PointerArray<T, R-1>::noconst_type*;  // non-const variant
};
template<typename T>
struct PointerArray<T, 1> {  // We end the recursion by specifically defining the R=1 case
    using type = T*;        // no const as this would express that the elements are constant
    using noconst_type = T*;
};
template<typename T>
struct PointerArray<T, 0> {  // We also end the recursion by specifically defining the R=0 case, this is needed for the Bracket classes
    using type = T&;
    using noconst_type = T&;
};

template<class T, rank_type R>
struct DataFromPtrs;

template<class T, rank_type R>
struct DataArrayFromPtrs;

template<class T, rank_type R>
class shared_shape {
 public:
    using ptrs_type = typename PointerArray<T, R>::type;
    using size_type = ::ra::size_type;
    // constructors
    // non-functional shape
    inline shared_shape() noexcept {
        uninit();
    }
    // construct shape
    inline shared_shape(const std::array<size_type, R>& anextent, T* adata)
    : extent_(anextent), ptrs_(nullptr), refs_(nullptr), orig_(nullptr) {
        // construct shape
        ra::detail::Offsets P({extent_.begin(), extent_.end()});
        auto to_be_orig = std::unique_ptr<void**[]>(P.apply_offsets(adata));  // this could throw, but only if its allocation fails, so no resource leak
        if (R > 1)
            refs_ = new std::atomic<int>(1);  // if throws, to_be_orig gets dealloced
        orig_ = to_be_orig.release();  // no exceptions, so now store orig_
        ptrs_ = reinterpret_cast<ptrs_type>(orig_);
        if (R == 1)
            orig_ = nullptr;  // avoids "double delete"
        ndataoffsets_ = P.get_num_data_offsets();
    }
    // copy constructor
    inline shared_shape(const shared_shape& other) noexcept
    : extent_(other.extent_), ptrs_(other.ptrs_), refs_(other.refs_), orig_(other.orig_),
      ndataoffsets_(other.ndataoffsets_) {
        incref();
    }
    // move constructor
    inline shared_shape(shared_shape&& other) noexcept
    : extent_(other.extent_), ptrs_(other.ptrs_), refs_(other.refs_), orig_(other.orig_),
      ndataoffsets_(other.ndataoffsets_) {
        other.uninit();
    }
    // (shallow) copy and move assignment
    inline auto operator=(const shared_shape& other) noexcept -> shared_shape& {
        if (this != &other) {
            decref();
            extent_       = other.extent_;
            ptrs_         = other.ptrs_;
            refs_         = other.refs_;
            orig_         = other.orig_;
            ndataoffsets_ = other.ndataoffsets_;
            incref();
        }
        return *this;
    }
    inline void operator=(shared_shape&& other) noexcept {
        decref();
        extent_       = other.extent_;
        ptrs_         = other.ptrs_;
        refs_         = other.refs_;
        orig_         = other.orig_;
        ndataoffsets_ = other.ndataoffsets_;
        other.uninit();
    }
    // destructor
    inline ~shared_shape() noexcept {
        decref();
    }
    // create a deep copy of the shape (not the data)
    inline auto copy() const -> shared_shape {        
        return shared_shape(extent_, data());
    }
    // change data or shape
    // let shape point to other data block
    inline void relocate(T* newdata) {
        if (R == 1) {
            // no cast needed, as for R=1, typeof(ptrs_)==typeof(newdata);
            // but because we need this to work for c++11, can't use if
            // constexpr(), so the following must compile for all R:
            ptrs_ = reinterpret_cast<ptrs_type>(newdata);
        } else if (R > 1) {
            // let shape point to other datablock
            std::ptrdiff_t shift = reinterpret_cast<const char*>(newdata) - reinterpret_cast<const char*>(data());
            if (shift != 0) {
                if (refs_ && *refs_ > 1)
                    *this = this->copy();
                char** data_array = DataArrayFromPtrs<T, R>::call(ptrs_);
                for (size_type i = 0; i < ndataoffsets_; i++)
                    data_array[i] += shift;
            }
        }
    }
    // change shape (not the data)
    inline void reshape(const std::array<size_type, R>& newextent) {
        if (newextent != extent_) {
            // should check if new extent is even valid
            if (size() ==
                std::accumulate(&newextent[0], &newextent[R],
                                1, std::multiplies<size_type>()))
                *this = shared_shape<T, R>(newextent, data());
            else
                throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        }
    }
    // get pointer-to-pointer structure
    inline auto ptrs() const noexcept -> ptrs_type {
        return ptrs_;
    }
    // get pointer to the data
    inline auto data() const noexcept -> T* {
        return DataFromPtrs<T, R>::call(ptrs_);
    }
    // get total number of elements
    inline auto size() const noexcept -> size_type {
        return ndataoffsets_ * extent_[R-1];
    }
    // get extent in dimension i
    inline auto extent(rank_type i) const -> size_type {
        if (i < 0 || i >= R)
            throw std::out_of_range("shared_shape::extent(int)");
        return extent_[i];
    }
    // get total extent array
    inline auto extent() const noexcept -> const std::array<size_type, R>& {
            return extent_;
    }
    // get subshape with bounds checking
    inline auto at(size_type index) const -> shared_shape<T, R-1> {
        if (R < 1 || index < 0 || index >= extent_[0])
            throw std::out_of_range("shared_shape::at");
        shared_shape<T, R-1> result;
        if (R > 1) {
            for (rank_type i = 0; i < R-1; ++i)
                result.extent_[i] = extent_[i+1];
            result.ptrs_         = ptrs_[index];
            result.refs_         = refs_;
            result.orig_         = orig_;
            result.ndataoffsets_ = ndataoffsets_/extent_[0];
            incref();
        }
        return result;
    }
    // get subshape with bounds checking
    inline auto slice(size_type beginindex, size_type endindex) const -> shared_shape<T, R> {
        if (R < 1 || beginindex < 0 || beginindex >= extent_[0]
            || endindex < 0 || endindex > extent_[0])
            throw std::out_of_range("shared_shape::slice");
        shared_shape<T, R> result;
        if (R > 1 && beginindex < endindex) {
            result.extent_[0]    = endindex - beginindex;
            for (rank_type i = 1; i < R; ++i)
                result.extent_[i] = extent_[i];
            result.ptrs_         = ptrs_ + beginindex;
            result.refs_         = refs_;
            result.orig_         = orig_;
            result.ndataoffsets_ = result.extent_[0]*(ndataoffsets_/extent_[0]);
            incref();
        }
        return result;
    }

 private:
    std::array<size_type, R> extent_;
    ptrs_type                ptrs_;
    std::atomic<int>*        refs_;
    void***                  orig_;
    size_type                ndataoffsets_;
    // put this object into an uninitialized state
    inline void uninit() noexcept {
        ptrs_         = nullptr;
        orig_         = nullptr;
        refs_         = nullptr;
        ndataoffsets_ = 0;
        extent_.fill(0);
    }
    inline void incref() const noexcept {
        if (refs_)
            (*refs_)++;
    }
    inline void decref() noexcept {
        // decref assumes the T::~T() does not throw
        if (refs_) {
            if (--(*refs_) == 0) {
                if (R > 1)
                    delete[] orig_;
                delete refs_;
                uninit();
            }
        }
    }
    template<class U, int S> friend class shared_shape;  // for "at"
    // for testing and debugging:             // EXCLUDE //
    friend int ::test_shared_shape_main();  // EXCLUDE //
};

// stop at() recursion at R=0. This is a non-shape, but at() for R=1 sets its elements
template<typename T>
class shared_shape<T, 0> {
 public:
    using ptrs_type = T;
    using size_type = ra::size_type;

 private:
    std::array<size_type, 0> extent_;
    ptrs_type                ptrs_;
    std::atomic<int>*        refs_;
    void***                  orig_;
    size_type                ndataoffsets_;
    inline void incref() const noexcept {}
    template<class U, int S> friend class shared_shape;  // for "at"
};

template<class T, rank_type R>
struct DataFromPtrs {
    static inline auto call(typename PointerArray<T, R>::type ptrs) noexcept -> T* {
        return DataFromPtrs<T, R-1>::call(*ptrs);
    }
};

template<class T>
struct DataFromPtrs<T, 1> {
    static inline auto call(typename PointerArray<T, 1>::type ptrs) noexcept -> T* {
        return reinterpret_cast<T*>(ptrs);
    }
};

template<class T, rank_type R>
struct DataArrayFromPtrs {
    static inline auto call(typename PointerArray<T, R>::type ptrs) noexcept -> char** {
        return DataArrayFromPtrs<T, R-1>::call(*ptrs);
    }
};

template<class T>
struct DataArrayFromPtrs<T, 2> {
    static inline auto call(typename PointerArray<T, 2>::type ptrs) noexcept -> char** {
        return reinterpret_cast<char**>(const_cast<typename PointerArray<typename std::remove_const<T>::type, 2>::noconst_type>(ptrs));
    }
};

template<class T>
struct DataArrayFromPtrs<T, 1> {
    static inline auto call(typename PointerArray<T, 1>::type ptrs) noexcept -> char** {
        return nullptr;  // should never be called, just there to
                         // terminate the template recursion at
                         // compile time.
    }
};

}  // namespace detail
}  // namespace ra

#endif  // EXCLUDE //
