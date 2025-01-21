//
// rarray - Runtime arrays: template classes for pointer-based,
//          runtime, reference counted, multi-dimensional
//          arrays.  Documentation in rarraydoc.pdf
//
// Copyright (c) 2013-2025  Ramses van Zon
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
#ifndef RARRAY_H_
#define RARRAY_H_
#include <string>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <atomic>
#include <array>
#include <numeric>
#include <functional>
#include <list>
#include <utility>
#include <cstdlib>
#include <vector>
#include <type_traits>
#if __cplusplus >= 201103L
#include <cmath>
#include <iostream>
#include <sstream>
#include <initializer_list>
#define RA_VERSION "v2.8.0"
#define RA_VERSION_NUMBER 2008000
#ifdef RA_BOUNDSCHECK
#define RA_CHECKORSAY(a, b) if (not(a)) throw std::out_of_range(std::string(b) + " in function " + std::string(__PRETTY_FUNCTION__) + " (rarray:" + std::to_string(__LINE__) + ")")
#define RA_noboundscheck false
#else
#define RA_CHECKORSAY(a, b)
#define RA_noboundscheck true
#endif
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
namespace ra {
    using size_type = ssize_t;
    using index_type = ssize_t;
    using rank_type = int;
}  // namespace ra
namespace ra {
namespace detail {
template<class T>
class shared_buffer {
 public:
    using size_type = ::ra::size_type;
    inline shared_buffer() noexcept {
        uninit();
    }
    explicit inline shared_buffer(size_type asize)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr) {
        auto to_be_data = std::unique_ptr<T[]>(new T[asize]);
        refs_ = new std::atomic<int>(1);
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
    }
    inline shared_buffer(size_type asize, T* adata) noexcept(RA_noboundscheck)
    : data_(adata), orig_(nullptr), size_(asize), refs_(nullptr) {
        RA_CHECKORSAY(adata, "nullptr given as data");
    }
    inline shared_buffer(const shared_buffer& other) noexcept
    : data_(other.data_), orig_(other.orig_), size_(other.size_), refs_(other.refs_) {
        incref();
    }
    inline shared_buffer(shared_buffer&& from) noexcept
    : data_(from.data_), orig_(from.orig_), size_(from.size_), refs_(from.refs_) {
        from.uninit();
    }
    inline auto operator=(const shared_buffer& other) noexcept -> shared_buffer& {
        if (this != &other) {
            decref();
            data_ = other.data_;
            orig_ = other.orig_;
            size_ = other.size_;
            refs_ = other.refs_;
            incref();
        }
        return *this;
    }
    inline void operator=(shared_buffer&& from) noexcept {
        decref();
        data_ = from.data_;
        orig_ = from.orig_;
        size_ = from.size_;
        refs_ = from.refs_;
        from.uninit();
    }
    inline ~shared_buffer() noexcept {
        decref();
    }
    inline auto operator[](size_type index) const noexcept(RA_noboundscheck) -> const T& {
        RA_CHECKORSAY(index >= 0 && index < size(), "element not in buffer");
        return data_[index];
    }
    inline auto operator[](size_type index) noexcept(RA_noboundscheck) -> T& {
        RA_CHECKORSAY(index >= 0 && index < size(), "element not in buffer");
        return data_[index];
    }
    inline auto at(size_type index) const -> const T& {
        if (index < 0 || index >= size_)
            throw std::out_of_range("shared_buffer::at");
        return data_[index];
    }
    inline auto at(size_type index) -> T& {
        if (index < 0 || index >= size_)
            throw std::out_of_range("shared_buffer::at");
        return data_[index];
    }
    inline auto slice(size_type from, size_type to) -> shared_buffer<T> {
        if (from < 0 || to < 0 || from > size_ || to > size_)
            throw std::out_of_range("shared_buffer::slice");
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to)
            result.size_ = to - from;
        return result;
    }
    inline auto slice(size_type from, size_type to) const -> const shared_buffer<T> {
        if (from < 0 || to < 0 || from > size_ || to > size_)
            throw std::out_of_range("shared_buffer::slice");
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to)
            result.size_ = to - from;
        return result;
    }
    inline auto size() const noexcept -> size_type {
        return size_;
    }
    inline auto copy() const -> shared_buffer<T> {
        return shared_buffer<T>(size_, cbegin(), cend());
    }
    using iterator = T*;
    using const_iterator = const T*;
    inline auto begin() noexcept -> iterator {
        return data_;
    }
    inline auto end() noexcept -> iterator {
        return data_+size_;
    }
    inline auto begin() const noexcept -> const_iterator {
        return data_;
    }
    inline auto end() const noexcept -> const_iterator {
        return data_+size_;
    }
    inline auto cbegin() const noexcept -> const_iterator {
        return data_;
    }
    inline auto cend() const noexcept -> const_iterator {
        return data_+size_;
    }
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    inline auto rbegin() -> reverse_iterator {
        return std::reverse_iterator<iterator>(data_+size_);
    }
    inline auto rend() -> reverse_iterator {
        return std::reverse_iterator<iterator>(data_);
    }
    inline auto rbegin() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_+size_);
    }
    inline auto rend() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_);
    }
    inline auto crbegin() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_+size_);
    }
    inline auto crend() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_);
    }
    inline void resize(size_type newsize, bool keep_content = false) {
        if ( newsize < size_ && refs_ && (*refs_) == 1 ) {
            size_ = newsize;
        } else {
            auto newrefs = new std::atomic<int>(1);
            T* newdata;
            try {
                newdata = new T[newsize];
            }
            catch (...) {
                delete newrefs;
                throw;
            }
            if (keep_content) {
                try {
                    size_type n = ((size_ < newsize)?size_:newsize);
                    for (size_type i = 0; i < n; i++)
                        newdata[i] = data_[i];
                }
                catch (...) {
                    delete newrefs;
                    delete[] newdata;
                    throw;
                }
            }
            decref();
            data_ = newdata;
            orig_ = newdata;
            size_ = newsize;
            refs_ = newrefs;
       }
    }
    inline void fill(const T& value) {
        for (size_type i = 0; i < size_; i++)
            data_[i] = value;
    }
    void assign(size_type count, const T& value) {
        resize(count);
        fill(value);
    }
    template<class InputIt,
             class = typename std::enable_if<
                 std::is_convertible<decltype(*InputIt()), T>::value>::type>
    inline void assign(InputIt first, InputIt last) {
        resize(last-first);
        T* data = data_;
        for (InputIt it = first; it != last; it++)
            *(data++) = *it;
    }
    inline void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }
 private:
    T*        data_;
    T*        orig_;
    size_type size_;
    std::atomic<int>* refs_;
    inline void uninit() noexcept {
        data_ = nullptr;
        orig_ = nullptr;
        size_ = 0;
        refs_ = nullptr;
    }
    inline void incref() noexcept {
        if (refs_)
            (*refs_)++;
    }
    inline void decref() noexcept {
        if (refs_) {
            if (--(*refs_) == 0) {
                delete[] orig_;
                delete refs_;
                uninit();
            }
        }
    }
    template<typename InputIt>
    inline shared_buffer(size_type asize, InputIt first, InputIt last)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr) {
        using noconstT = typename std::remove_const<T>::type;
        #ifndef __ibmxl__
        auto to_be_data = std::unique_ptr<T[]>(new T[asize]{*first});
        #else
        auto to_be_data = std::unique_ptr<T[]>(const_cast<T*>(new typename std::remove_const<T>::type[asize]));
        #endif
        std::copy(first, last, const_cast<noconstT*>(to_be_data.get()));
        refs_ = new std::atomic<int>(1);
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
     }
};
}  // namespace detail
}  // namespace ra
namespace ra {
namespace detail {
class Offsets {
 public:
    inline explicit Offsets(const std::vector<index_type>& extent)
    : rank_(static_cast<rank_type>(extent.size())), ndataoffsets_(0) {
        if (rank_ > 0) {
            size_type noffsets = 0;
            ndataoffsets_ = 1;
            for (rank_type i = rank_ - 1; i--; )
                noffsets = extent[i]*(1 + noffsets);
            for (rank_type i = 0 ; i < rank_-1; i++)
                ndataoffsets_ *= extent[i];
            offsets_.reserve(noffsets);
            offsets_.resize(noffsets);
            if (noffsets > 1) {
                size_type offsetnum = 0;
                size_type extenttot = extent[0];
                for (rank_type i = 1; i < rank_ - 1; i++) {
                    for (size_type j = 0; j < extenttot; j++)
                        offsets_[offsetnum+j] = offsetnum + extenttot + j*extent[i];
                    offsetnum += extenttot;
                    extenttot *= extent[i];
                }
                ndataoffsets_ = extenttot;
                for (index_type j = 0; j < ndataoffsets_; j++)
                   offsets_[offsetnum + j] = j*extent[rank_ - 1];
            }
        }
    }
    template<class T> inline auto apply_offsets(T* data) const -> void*** {
        static_assert(sizeof(T*) == sizeof(void*) && sizeof(T*) == sizeof(void**),
                      "rarray's Offsets requires all pointers to have the same size");
        auto noffsets = (size_type)offsets_.size();
        if (ndataoffsets_ == 0 && noffsets == 0) {
            return nullptr;
        } else if (ndataoffsets_ == 1 && noffsets == 0) {
            return reinterpret_cast<void***>(const_cast<typename std::remove_const<T>::type*>(data));
        } else {
            void*** offsets = new void**[noffsets];
            size_type i = 0;
            for (; i < noffsets - ndataoffsets_; i++)
                offsets[i] = reinterpret_cast<void**>(offsets) + offsets_[i];
            for (; i < noffsets; i++)
                offsets[i] = reinterpret_cast<void**>(
                                  const_cast<typename std::remove_const<T>::type*>(data)
                                  + offsets_[i]);
            return offsets;
        }
    }
    inline auto get_num_data_offsets() const noexcept -> size_type {
        return ndataoffsets_;
    }
    inline auto get_num_offsets() const noexcept -> size_type {
        return (size_type)offsets_.size();
    }
    inline auto get_rank() const noexcept -> rank_type {
        return rank_;
    }
 private:
    rank_type rank_;
    std::vector<size_type> offsets_;
    size_type ndataoffsets_;
};
}  // namespace detail
}  // namespace ra
namespace ra {
namespace detail {
template<typename T, rank_type R>
struct PointerArray {
    using type = typename PointerArray<T, R-1>::type const*;
    using noconst_type = typename PointerArray<T, R-1>::noconst_type*;
};
template<typename T>
struct PointerArray<T, 1> {
    using type = T*;
    using noconst_type = T*;
};
template<typename T>
struct PointerArray<T, 0> {
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
    inline shared_shape() noexcept {
        uninit();
    }
    inline shared_shape(const std::array<size_type, R>& anextent, T* adata)
    : extent_(anextent), ptrs_(nullptr), refs_(nullptr), orig_(nullptr) {
        ra::detail::Offsets P({extent_.begin(), extent_.end()});
        auto to_be_orig = std::unique_ptr<void**[]>(P.apply_offsets(adata));
        if (R > 1)
            refs_ = new std::atomic<int>(1);
        orig_ = to_be_orig.release();
        ptrs_ = reinterpret_cast<ptrs_type>(orig_);
        if (R == 1)
            orig_ = nullptr;
        ndataoffsets_ = P.get_num_data_offsets();
    }
    inline shared_shape(const shared_shape& other) noexcept
    : extent_(other.extent_), ptrs_(other.ptrs_), refs_(other.refs_), orig_(other.orig_),
      ndataoffsets_(other.ndataoffsets_) {
        incref();
    }
    inline shared_shape(shared_shape&& other) noexcept
    : extent_(other.extent_), ptrs_(other.ptrs_), refs_(other.refs_), orig_(other.orig_),
      ndataoffsets_(other.ndataoffsets_) {
        other.uninit();
    }
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
    inline ~shared_shape() noexcept {
        decref();
    }
    inline auto copy() const -> shared_shape {
        return shared_shape(extent_, data());
    }
    inline void relocate(T* newdata) {
        if (R == 1) {
            ptrs_ = reinterpret_cast<ptrs_type>(newdata);
        } else if (R > 1) {
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
    inline void reshape(const std::array<size_type, R>& newextent) {
        if (newextent != extent_) {
            if (size() ==
                std::accumulate(&newextent[0], &newextent[R],
                                1, std::multiplies<size_type>()))
                *this = shared_shape<T, R>(newextent, data());
            else
                throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        }
    }
    inline auto ptrs() const noexcept -> ptrs_type {
        return ptrs_;
    }
    inline auto data() const noexcept -> T* {
        return DataFromPtrs<T, R>::call(ptrs_);
    }
    inline auto size() const noexcept -> size_type {
        return ndataoffsets_ * extent_[R-1];
    }
    inline auto extent(rank_type i) const -> size_type {
        if (i < 0 || i >= R)
            throw std::out_of_range("shared_shape::extent(int)");
        return extent_[i];
    }
    inline auto extent() const noexcept -> const std::array<size_type, R>& {
            return extent_;
    }
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
        if (refs_) {
            if (--(*refs_) == 0) {
                if (R > 1)
                    delete[] orig_;
                delete refs_;
                uninit();
            }
        }
    }
    template<class U, int S> friend class shared_shape;
};
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
    template<class U, int S> friend class shared_shape;
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
        return nullptr;
    }
};
}  // namespace detail
}  // namespace ra
namespace ra {
enum class RESIZE { NO, ALLOWED };
enum class MISSING { SKIP, DEFAULT, REPEAT };
namespace detail {
template<typename T> class CommaOp;
template<typename T, rank_type R, typename P> class Bracket;
template<typename T, rank_type R, typename P> class ConstBracket;
#define ExOp class
template<typename T, rank_type R, ExOp AOP, typename A1, typename A2, typename A3> class Expr;
template<typename T>
void init_shape_fill(unsigned N, size_type* sz, const T& l)
{}
template<typename U>
void init_shape_fill(unsigned N, size_type* sz,
                     const std::initializer_list<U>& l) {
    sz[0] = std::max(sz[0], static_cast<size_type>(l.size()));
    if (N >= 2)
        for (const auto& lsub : l)
            init_shape_fill(N - 1, sz + 1, lsub);
}
template<unsigned N, typename U>
auto init_shape(const std::initializer_list<U>& l) -> std::array<size_type, N> {
    std::array<size_type, N> sz{0};
    init_shape_fill(N,  &sz[0], l);
    return sz;
}
template<typename T>
struct init_list_prop {
    enum { rank = 0 };
    using  type = T;
};
template<typename U>
struct init_list_prop<std::initializer_list<U>> {
    enum { rank = init_list_prop<U>::rank + 1 };
    using  type = typename init_list_prop<U>::type;
};
}  // namespace detail
template<typename T, rank_type R>
class rarray {
 public:
    using value_type = T;
    using difference_type = ::ra::size_type;
    using size_type = ::ra::size_type;
    using iterator = T*;
    using const_iterator = const T*;
    using parray_t = typename detail::PointerArray<T, R>::type;
    using noconst_parray_t = typename detail::PointerArray<T, R>::noconst_type;
    inline rarray()
    : buffer_(), shape_() {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline explicit rarray(size_type n0)
    : buffer_(n0),
      shape_({n0}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline rarray(size_type n0, size_type n1)
    : buffer_(n0*n1),
      shape_({n0, n1}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline rarray(size_type n0, size_type n1, size_type n2)
    : buffer_(n0*n1*n2),
      shape_({n0, n1, n2}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3)
    : buffer_(n0*n1*n2*n3),
      shape_({n0, n1, n2, n3}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4)
    : buffer_(n0*n1*n2*n3*n4),
      shape_({n0, n1, n2, n3, n4}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5)
    : buffer_(n0*n1*n2*n3*n4*n5),
      shape_({n0, n1, n2, n3, n4, n5}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6)
    : buffer_(n0*n1*n2*n3*n4*n5*n6),
      shape_({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7, size_type n8)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7, size_type n8, size_type n9)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7, size_type n8, size_type n9,
                  size_type n10)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin())
    {}
    explicit inline rarray(const size_type* anextent)
    : buffer_(std::accumulate(anextent, anextent+R, 1, std::multiplies<size_type>())),
      shape_(reinterpret_cast<const std::array<size_type, R>&>(*anextent), buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline rarray(T* buffer, size_type n0)
    : buffer_(n0, buffer),
      shape_({n0}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline rarray(T* buffer, size_type n0, size_type n1)
    : buffer_(n0*n1, buffer),
      shape_({n0, n1}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2)
    : buffer_(n0*n1*n2, buffer),
      shape_({n0, n1, n2}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3)
    : buffer_(n0*n1*n2*n3, buffer),
      shape_({n0, n1, n2, n3}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4)
    : buffer_(n0*n1*n2*n3*n4, buffer),
      shape_({n0, n1, n2, n3, n4}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5)
    : buffer_(n0*n1*n2*n3*n4*n5, buffer),
      shape_({n0, n1, n2, n3, n4, n5}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6)
    : buffer_(n0*n1*n2*n3*n4*n5*n6, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7, size_type n8)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7, size_type n8,
                  size_type n9)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7, size_type n8,
                  size_type n9, size_type n10)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer)
    {}
    inline rarray(T* buffer, const size_type* anextent)
    : buffer_(std::accumulate(anextent, anextent+R, 1, std::multiplies<size_type>()), buffer),
      shape_(reinterpret_cast<const std::array<size_type, R>&>(*anextent), buffer)
    {}
    template<std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 1>::type>
    inline explicit rarray(T (&a)[Z])
    : buffer_(Z, a),
      shape_({Z}, buffer_.begin())
    {}
    template<std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 2>::type>
    inline explicit rarray(T (&a)[Y][Z])
    : buffer_(Y*Z, *a),
      shape_({Y, Z}, buffer_.begin())
    {}
    template<std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 3>::type>
    inline explicit rarray(T (&a)[X][Y][Z])
    : buffer_(X*Y*Z, **a),
      shape_({X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 4>::type>
    inline explicit rarray(T (&a)[W][X][Y][Z])
    : buffer_(W*X*Y*Z, ***a),
      shape_({W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 5>::type>
    inline explicit rarray(T (&a)[V][W][X][Y][Z])
    : buffer_(V*W*X*Y*Z, ****a),
      shape_({V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 6>::type>
    inline explicit rarray(T (&a)[U][V][W][X][Y][Z])
    : buffer_(U*V*W*X*Y*Z,  *****a),
      shape_({U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t T_, std::size_t U, std::size_t V, std::size_t W, std::size_t X,
             std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 7>::type>
    inline explicit rarray(T (&a)[T_][U][V][W][X][Y][Z])
    : buffer_(T_*U*V*W*X*Y*Z,  ******a),
      shape_({T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t S, std::size_t T_, std::size_t U, std::size_t V, std::size_t W,
             std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 8>::type>
    inline explicit rarray(T (&a)[S][T_][U][V][W][X][Y][Z])
    : buffer_(S*T_*U*V*W*X*Y*Z,  *******a),
      shape_({S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t R_, std::size_t S, std::size_t T_, std::size_t U, std::size_t V,
             std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 9>::type>
    inline explicit rarray(T (&a)[R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(R_*S*T_*U*V*W*X*Y*Z,  ********a),
      shape_({R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_, std::size_t U,
             std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 10>::type>
    inline explicit rarray(T (&a)[Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(Q*R_*S*T_*U*V*W*X*Y*Z,  *********a),
      shape_({Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t P, std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_,
             std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 11>::type>
    inline explicit rarray(T (&a)[P][Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(P*Q*R_*S*T_*U*V*W*X*Y*Z,  **********a),
      shape_({P, Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 1>::type>
    inline explicit rarray(T (&&a)[Z])
    : buffer_(Z, a),
      shape_({Z}, buffer_.begin())
    {}
    template<std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 2>::type>
    inline explicit rarray(T (&&a)[Y][Z])
    : buffer_(Y*Z, *a),
      shape_({Y, Z}, buffer_.begin())
    {}
    template<std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 3>::type>
    inline explicit rarray(T (&&a)[X][Y][Z])
    : buffer_(X*Y*Z, **a),
      shape_({X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 4>::type>
    inline explicit rarray(T (&&a)[W][X][Y][Z])
    : buffer_(W*X*Y*Z, ***a),
      shape_({W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 5>::type>
    inline explicit rarray(T (&&a)[V][W][X][Y][Z])
    : buffer_(V*W*X*Y*Z, ****a),
      shape_({V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 6>::type>
    inline explicit rarray(T (&&a)[U][V][W][X][Y][Z])
    : buffer_(U*V*W*X*Y*Z,  *****a),
      shape_({U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t T_, std::size_t U, std::size_t V, std::size_t W, std::size_t X,
             std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 7>::type>
    inline explicit rarray(T (&&a)[T_][U][V][W][X][Y][Z])
    : buffer_(T_*U*V*W*X*Y*Z,  ******a),
      shape_({T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t S, std::size_t T_, std::size_t U, std::size_t V, std::size_t W,
             std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 8>::type>
    inline explicit rarray(T (&&a)[S][T_][U][V][W][X][Y][Z])
    : buffer_(S*T_*U*V*W*X*Y*Z,  *******a),
      shape_({S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t R_, std::size_t S, std::size_t T_, std::size_t U, std::size_t V,
             std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 9>::type>
    inline explicit rarray(T (&&a)[R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(R_*S*T_*U*V*W*X*Y*Z,  ********a),
      shape_({R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_, std::size_t U,
             std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 10>::type>
    inline explicit rarray(T (&&a)[Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(Q*R_*S*T_*U*V*W*X*Y*Z,  *********a),
      shape_({Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t P, std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_,
             std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 11>::type>
    inline explicit rarray(T (&&a)[P][Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(P*Q*R_*S*T_*U*V*W*X*Y*Z,  **********a),
      shape_({P, Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    RA_FORCE_inline rarray(const rarray<T, R> &a) noexcept
    : buffer_(a.buffer_),
      shape_(a.shape_)
    {}
    inline auto operator=(const rarray<T, R> &a) noexcept -> rarray& {
        buffer_ = a.buffer_;
        shape_ = a.shape_;
        return *this;
    }
    inline rarray(rarray<T, R>&& x) noexcept
    : buffer_(std::move(x.buffer_)),
      shape_(std::move(x.shape_))
    {}
    inline auto operator=(rarray<T, R>&& x) noexcept -> rarray& {
        buffer_ = std::move(x.buffer_);
        shape_ = std::move(x.shape_);
        return *this;
    }
    inline auto operator=(const T& e) noexcept(RA_noboundscheck && std::is_nothrow_copy_constructible<T>()) -> detail::CommaOp<T> {
        RA_CHECKORSAY(!empty(), "assignment to unsized array");
        RA_CHECKORSAY(size() > 0, "assignment with more elements than in array");
        T* first = &(buffer_[0]);
        if (size() > 0)
            *first = e;
        else
            return detail::CommaOp<T>(nullptr, nullptr);
        detail::CommaOp<T> co(first+1, first+size()-1);
        return co;
    }
    RA_FORCE_inline ~rarray() = default;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline void fill(const detail::Expr<T, R, AOP, A1, A2, A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline void form(const detail::Expr<T, R, AOP, A1, A2, A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator+=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator-=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator*=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator/=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator%=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void reshape(size_type n0, RESIZE resize_policy = RESIZE::NO) {
        const size_type newsize = n0;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= n0) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void reshape(size_type n0, size_type n1, RESIZE resize_policy = RESIZE::NO) {
        const size_type newsize = n0*n1;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5*n6;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        }
        else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize=n0*n1*n2*n3*n4*n5*n6*n7;
        if (size() == n0*n1*n2*n3*n4*n5*n6*n7) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        } else if  (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5*n6*n7*n8;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8, size_type n9,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5*n6*n7*n8*n9;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8, size_type n9, size_type n10,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize =  n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        }
    }
    inline void reshape(const size_type* newshape, RESIZE resize_policy = RESIZE::NO) {
        const size_type newsize = std::accumulate(newshape, newshape+R,
                                            1, std::multiplies<size_type>());
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>((const std::array<size_type, R>&)(*newshape), buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>((const std::array<size_type, R>&)(*newshape), buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        }
    }
    inline auto copy() const -> rarray {
        rarray<T, R> clone;
        clone.buffer_ = buffer_.copy();
        clone.shape_ = shape_.copy();
        clone.shape_.relocate(clone.buffer_.begin());
        return clone;
    }
    template<typename U = T,
             typename = typename std::enable_if<!std::is_const<U>::value>::type>
    inline operator const rarray<const U, R>&() const noexcept {
        return const_ref();
    }
    constexpr static auto rank() noexcept -> int {
        return R;
    }
    inline auto empty() const noexcept -> bool {
        return buffer_.cbegin() == nullptr;
    }
    inline auto extent(int i) const -> size_type {
        return shape_.extent(i);
    }
    RA_FORCE_inline auto shape() const noexcept -> const size_type* {
        return &(shape_.extent()[0]);
    }
    inline auto size() const noexcept -> size_type {
        return shape_.size();
    }
    inline auto data() noexcept -> T* {
        return buffer_.begin();
    }
    inline auto data() const noexcept -> const T* {
        return buffer_.begin();
    }
    inline auto ptr_array() const noexcept -> parray_t {
        return shape_.ptrs();
    }
    inline auto noconst_ptr_array() const noexcept -> noconst_parray_t {
        return const_cast<noconst_parray_t>(shape_.ptrs());
    }
    inline auto const_ref() const noexcept -> const rarray<const T, R>& {
        return reinterpret_cast<const rarray<const T, R>&>(*this);
    }
    RA_FORCE_inline void clear() noexcept {
        shape_ = detail::shared_shape<T, R>();
        buffer_ = detail::shared_buffer<T>();
    }
    inline void fill(const T& value) {
        buffer_.fill(value);
    }
 private:
    template<rank_type R_ = R,
             class = typename std::enable_if<R_ == 1>::type>
    inline void fill_g(std::initializer_list<T> list,
                       MISSING missing_policy) {
        auto filler = list.begin();
        for (size_type i = 0; i < extent(0); i++) {
            if (filler != list.end()) {
                buffer_[i] = *filler++;
                if (filler == list.end()) {
                    if (missing_policy == MISSING::REPEAT)
                        filler = list.begin();
                    else if (missing_policy == MISSING::SKIP)
                        break;
                }
            } else {
                buffer_[i] = T{};
            }
        }
    }
    template<typename U, rank_type R_ = R,
             class = typename std::enable_if<R_ != 1>::type>
    inline void fill_g(std::initializer_list<U> list,
                       MISSING missing_policy) {
        auto filler = list.begin();
        for (size_type i = 0; i < extent(0); i++) {
            if (filler != list.end()) {
                at(i).fill_g(*filler++, missing_policy);
                if (filler == list.end()) {
                    if (missing_policy == MISSING::REPEAT)
                        filler = list.begin();
                    else if (missing_policy == MISSING::SKIP)
                        break;
                }
            } else {
                at(i).fill(T{});
            }
        }
    }
    template<typename U>
    inline void form_g(std::initializer_list<U> list,
                         MISSING missing_policy) {
        std::array<size_type, R> newshape = detail::init_shape<R>(list);
        size_type newsize = std::accumulate(newshape.begin(), newshape.end(),
                                            1, std::multiplies<size_type>());
        buffer_ = detail::shared_buffer<T>(newsize);
        shape_ = detail::shared_shape<T, R>(newshape, buffer_.begin());
        fill_g(list, missing_policy);
    }
 public:
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void fill(std::initializer_list<T> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<T>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void form(std::initializer_list<T> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void form(std::initializer_list<std::initializer_list<T>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void form(size_type n0, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0);
        shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void form(size_type n0, size_type n1, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1);
        shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void form(size_type n0, size_type n1, size_type n2, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8, size_type n9,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                     size_type n4, size_type n5, size_type n6, size_type n7,
                     size_type n8, size_type n9, size_type n10,
                     const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10},
                                            buffer_.begin());
        buffer_.fill(value);
    }
    inline auto begin() noexcept -> iterator {
        return buffer_.begin();
    }
    inline auto begin() const noexcept -> const_iterator {
        return buffer_.begin();
    }
    inline auto cbegin() const noexcept -> const_iterator {
        return buffer_.cbegin();
    }
    inline auto end() noexcept -> iterator {
        return buffer_.end();
    }
    inline auto end() const noexcept -> const_iterator {
        return buffer_.end();
    }
    inline auto cend() const noexcept -> const_iterator {
        return buffer_.cend();
    }
    inline auto index(const T& a, int i) const -> size_type {
        ptrdiff_t linearindex = &a - &(buffer_[0]);
        if (linearindex < 0 || linearindex >= size())
            throw "element not in array";
        const size_type* extent_ = shape();
        for (int j = R-1; j > i; j--)
            linearindex /= extent_[j];
        return linearindex % extent_[i];
    }
    inline auto index(const iterator& iter, int i) const -> size_type {
        return index(*iter, i);
    }
    inline auto index(const T& a) const -> std::array<size_type, R> {
        std::array<size_type, R> ind;
        ptrdiff_t linearindex = &a - &(buffer_[0]);
        RA_CHECKORSAY(linearindex >= 0 && linearindex < size(), "element not in array");
        int j = R;
        const size_type* extent_ = shape();
        while (j-->0) {
            ind[j] = linearindex % extent_[j];
            linearindex /= extent_[j];
        }
        return ind;
    }
    inline auto index(const iterator& i) const -> std::array<size_type, R> {
        return index(*i);
    }
    template<class U = rarray<T, R-1>>
    RA_FORCE_inline auto at(size_type i)
    -> typename std::enable_if<R != 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        size_type stride = size()/extent(0);
        return ra::rarray<T, R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
    }
    template<class U = T&>
    RA_FORCE_inline auto at(size_type i)
    -> typename std::enable_if<R == 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        return shape_.ptrs()[i];
    }
    template<class U = const rarray<T, R-1>>
    RA_FORCE_inline auto at(size_type i) const
    -> typename std::enable_if<R != 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        size_type stride = size()/extent(0);
        return ra::rarray<T, R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
    }
    template<class U = const T&>
    RA_FORCE_inline auto at(size_type i) const
    -> typename std::enable_if<R == 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        return shape_.ptrs()[i];
    }
    RA_FORCE_inline auto slice(size_type beginindex, size_type endindex) -> rarray {
        if (beginindex < 0 || beginindex >= shape_.extent(0)
            || endindex < 0 || endindex > shape_.extent(0))
            throw std::out_of_range("rarray<T, R>::slice");
        size_type stride = size()/extent(0);
        return {buffer_.slice(beginindex*stride, endindex*stride),
                shape_.slice(beginindex, endindex)};
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i)
    -> typename std::enable_if<R == 1, U&>::type {
        RA_CHECKORSAY(i >= 0 && i < extent(0), "index out of range of array");
        return buffer_.begin()[i];
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i)
    -> typename std::enable_if<R != 1, typename detail::Bracket<U, R-1, rarray> >::type {
        return { *this, i, this->shape() };
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i) const
    -> typename std::enable_if<R == 1, const U&>::type {
        RA_CHECKORSAY(i >= 0 && i < extent(0), "index out of range of array");
        return buffer_.cbegin()[i];
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i) const
    -> typename std::enable_if<R != 1, typename detail::ConstBracket<U, R-1, rarray>>::type {
        return { *this, i, this->shape() };
    }
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type i, Ts... args)
    -> typename std::enable_if<R == sizeof...(Ts)+1, T&>::type {
        return operator[](i)[args...];
    }
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type i, Ts... args) const
    -> typename std::enable_if<R == sizeof...(Ts)+1, const T&>::type {
        return operator[](i)[args...];
    }
    #endif
    RA_FORCE_inline auto leval(size_type i) const -> const T&;
 private:
    friend class detail::Bracket<T, R-1, rarray<T, R>>;
    friend class detail::ConstBracket<T, R-1, rarray<T, R>>;
    RA_FORCE_inline auto private_at(size_type i) -> typename detail::PointerArray<T, R-1>::type {
        return shape_.ptrs()[i];
    }
    RA_FORCE_inline auto private_at(size_type i) const -> typename detail::PointerArray<T, R-1>::type {
        return shape_.ptrs()[i];
    }
    friend class rarray<T, R+1>;
    inline rarray(detail::shared_buffer<T>&& abuffer, detail::shared_shape<T, R>&& ashape) :
        buffer_(std::forward<detail::shared_buffer<T>>(abuffer)),
        shape_(std::forward<detail::shared_shape<T, R>>(ashape))
    {}
    inline rarray(const detail::shared_buffer<T>&& abuffer, detail::shared_shape<T, R>&& ashape) :
        buffer_(std::forward<detail::shared_buffer<T>>(const_cast<detail::shared_buffer<T>&& >(abuffer))),
        shape_(std::forward<detail::shared_shape<T, R>>(ashape))
    {}
    detail::shared_buffer<T>  buffer_;
    detail::shared_shape<T, R> shape_;
};
namespace detail {
template<typename T>
class CommaOp {
 public:
    RA_FORCE_inline auto operator,(const T& e) -> CommaOp& {
        RA_CHECKORSAY(ptr_ != nullptr && last_ != nullptr, "invalid comma operator");
        RA_CHECKORSAY(ptr_ <= last_, "assignment with more elements than in array");
        if (ptr_ && ptr_ <= last_)
            *ptr_++ = e;
        return *this;
    }
 private:
    RA_FORCE_inline CommaOp(T* ptr, T* last) noexcept(RA_noboundscheck)
    : ptr_(ptr), last_(last) {
        RA_CHECKORSAY(ptr_ != nullptr && last_ != nullptr, "invalid comma operator");
    }
    T *ptr_;
    T * const last_;
    template<typename, int> friend class ra::rarray;
};
template<typename T, rank_type R, typename P>
class Bracket {
 private:
    P&               parent_;  ///< what array/array expression is being accessed
    size_type        index_;
    const size_type* shape_;
 public:
    RA_FORCE_inline auto operator[](size_type nextindex) noexcept(RA_noboundscheck) -> Bracket<T, R-1, Bracket> {
        return { *this, nextindex, shape_ + 1 };
    }
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    Bracket(const Bracket&) = delete;
    Bracket(const Bracket&&) = delete;
    auto operator=(const Bracket&) -> Bracket& = delete;
    auto operator=(const Bracket&&) -> Bracket& = delete;
    ~Bracket() = default;
 private:
    RA_FORCE_inline Bracket(P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >=0 && index_ < shape_[0], "index out of range of array");
    }
    RA_FORCE_inline auto private_at(size_type nextindex) noexcept(RA_noboundscheck) -> decltype(parent_.private_at(index_)[nextindex]) {
        return parent_.private_at(index_)[nextindex];
    }
    template<typename, int> friend class ra::rarray;
    template<typename, int, typename> friend class detail::Bracket;
 public:
    RA_FORCE_inline auto at(size_type nextindex) -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type nextindex, Ts... args) -> typename std::enable_if<R == sizeof...(Ts)+1, T&>::type {
        return operator[](nextindex)[args...];
    }
    #endif
};
template<typename T, typename P>
class Bracket<T, 1, P> {
 private:
    P&               parent_;  ///< what array/array expression is being accessed
    size_type        index_;
    const size_type* shape_;
 public:
    RA_FORCE_inline auto operator[](size_type nextindex) noexcept(RA_noboundscheck) -> T& {
        RA_CHECKORSAY(nextindex >= 0 && nextindex < shape_[1], "index out of range of array");
        return parent_.private_at(index_)[nextindex];
    }
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    Bracket(const Bracket&) = delete;
    Bracket(const Bracket&&) = delete;
    auto operator=(const Bracket&) -> Bracket& = delete;
    auto operator=(const Bracket&&) -> Bracket& = delete;
    ~Bracket() = default;
 private:
    RA_FORCE_inline Bracket(P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >= 0 && index_ < shape_[0], "index out of range of array");
    }
    template<typename, int> friend class ra::rarray;
    template<typename, int, typename> friend class detail::Bracket;
 public:
    RA_FORCE_inline auto at(size_type nextindex) -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
};
template<typename T, rank_type R, typename P>
class ConstBracket {
 private:
    const P&       parent_;
    size_type      index_;
 public:
    RA_FORCE_inline auto operator[](size_type nextindex) const noexcept(RA_noboundscheck) -> ConstBracket<T, R-1, ConstBracket> {
        return { *this, nextindex, shape_ + 1 };
    }
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    ConstBracket(const ConstBracket&) = delete;
    ConstBracket(const ConstBracket&&) = delete;
    auto operator=(const ConstBracket&) -> ConstBracket& = delete;
    auto operator=(const ConstBracket&&) -> ConstBracket& = delete;
    ~ConstBracket() = default;
 private:
    const size_type* shape_;
    RA_FORCE_inline ConstBracket(const P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >= 0 && index_ < shape_[0], "index out of range of array");
    }
    RA_FORCE_inline auto private_at(size_type nextindex) const noexcept(RA_noboundscheck) -> decltype(parent_.private_at(index_)[nextindex]) {
        return parent_.private_at(index_)[nextindex];
    }
    template<typename, int> friend class ra::rarray;
    template<typename, int, typename> friend class detail::ConstBracket;
 public:
    RA_FORCE_inline auto at(size_type nextindex) const -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type nextindex, Ts... args) -> typename std::enable_if<R == sizeof...(Ts)+1, const T&>::type {
        return operator[](nextindex)[args...];
    }
    #endif
};
template<typename T, typename P>
class ConstBracket<T, 1, P> {
 private:
    const P& parent_;
    size_type  index_;
 public:
    RA_FORCE_inline auto operator[](size_type nextindex) const noexcept(RA_noboundscheck) -> const T& {
        RA_CHECKORSAY(nextindex >=0 && nextindex < shape_[1], "index out of range of array");
        return parent_.private_at(index_)[nextindex];
    }
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    ConstBracket(const ConstBracket&) = delete;
    ConstBracket(const ConstBracket&&) = delete;
    auto operator=(const ConstBracket&) -> ConstBracket& = delete;
    auto operator=(const ConstBracket&&) -> ConstBracket& = delete;
    ~ConstBracket() = default;
 private:
    const size_type* shape_;
    RA_FORCE_inline ConstBracket(const P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >= 0 && index_ < shape_[0], "index out of range of array");
    }
    template<typename, int> friend class ra::rarray;
    template<typename, int, typename> friend class detail::ConstBracket;
 public:
    RA_FORCE_inline auto at(size_type nextindex) const -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
};
template<typename U>
auto make_rarray_g(std::initializer_list<U> list, ra::MISSING missing_policy) ->  rarray<typename ra::detail::init_list_prop<decltype(list)>::type,ra::detail::init_list_prop<decltype(list)>::rank>
{
    rarray<typename ra::detail::init_list_prop<decltype(list)>::type, ra::detail::init_list_prop<decltype(list)>::rank> a;
    a.form(list, missing_policy);
    return a;
}
}  // namespace detail
template<typename S>
inline auto linspace(S x1, S x2, size_type n = 0, bool end_incl = true) -> rarray<S, 1> {
    if (n == 0) {
        if (x2 > x1)
            n = static_cast<int>(x2 - x1 + end_incl);
        else
            n = static_cast<int>(x1 - x2 + end_incl);
    }
    rarray<S, 1> x(n);
    for (size_type i = 0; i < n; i++)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wconversion"
        x[i] = x1 + static_cast<S>(((x2-x1)*i)/(n-end_incl));
        #pragma GCC diagnostic pop
    if (end_incl)
        x[n-1] = x2;
    return x;
}
template<class T>
class Xrange {
 private:
    struct const_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::input_iterator_tag;
        inline const_iterator(): i_(0), di_(1), b_(0) {}
        inline const_iterator(T i, T di, T b): i_(i), di_(di), b_(b) {}
        inline auto operator!=(const const_iterator& other) const -> bool {
            return i_ != other.i_;
        }
        inline auto operator==(const const_iterator& other) const -> bool {
            return i_ == other.i_ && di_ == other.di_ && b_ == other.b_;
        }
        inline auto operator++() -> const_iterator& {
            i_ += di_;
            if (di_ > 0 && i_ >= b_)
               i_ = b_;
            if (di_ < 0 && i_ <= b_)
               i_ = b_;
            return *this;
        }
        inline auto operator++(int) -> const_iterator {
            const const_iterator temp = *this;
            this->operator++();
            return temp;
        }
        inline auto operator*() const -> const T& {
            return i_;
        }
        T i_, di_, b_;
    };
    T a_, b_, d_;
 public:
    inline Xrange(T a, T b, T d)
    : a_(a), b_(a + static_cast<T>(static_cast<T>(std::ceil(static_cast<double>(b-a)/static_cast<double>(d)))*d)), d_(d)
    {}
    inline auto begin() const -> const_iterator {
        return const_iterator(a_, d_, b_);
    }
    inline auto end() const -> const_iterator {
        return const_iterator(b_, d_, b_);
    }
    inline auto size() const -> size_t  {
        return static_cast<size_t>((b_-a_)/d_);
    }
};
template<class T>
inline auto xrange(T end) -> Xrange<T> {
    return Xrange<T>(static_cast<T>(0), end, static_cast<T>(1));
}
template<class S, class T>
inline auto xrange(S begin, T end) -> Xrange<T> {
    return Xrange<T>(static_cast<T>(begin), end, static_cast<T>(1));
}
template<class S, class T, class U>
inline auto xrange(S begin, T end, U step) -> Xrange<T> {
    return Xrange<T>(static_cast<T>(begin), end, static_cast<T>(step));
}
template<typename A>
inline auto extent(const A &a, int i) -> size_type {
    switch (i) {
    case 0: return std::extent<A, 0>();
    case 1: return std::extent<A, 1>();
    case 2: return std::extent<A, 2>();
    case 3: return std::extent<A, 3>();
    case 4: return std::extent<A, 4>();
    case 5: return std::extent<A, 5>();
    case 6: return std::extent<A, 6>();
    case 7: return std::extent<A, 7>();
    case 8: return std::extent<A, 8>();
    case 9: return std::extent<A, 9>();
    case 10: return std::extent<A, 10>();
    default: throw std::out_of_range("ra::extent"); return 0;
    }
}
template<typename T, rank_type R>
inline auto extent(const rarray<T, R> &a, int i) -> size_type {
    return a.extent(i);
}
template<typename T>
auto make_rarray(std::initializer_list<T> list,
                 MISSING missing_policy = MISSING::DEFAULT) -> rarray<T,1>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<T>> list,
                 MISSING missing_policy = MISSING::DEFAULT) -> rarray<T,2>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,3>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,4>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,5>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,6>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,7>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,8>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,9>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,10>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,11>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(size_type n0, const T& fillvalue) -> rarray<T,1>
{
    rarray<T,1> result(n0);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0, size_type n1, const T& fillvalue) -> rarray<T,2>
{
    rarray<T,2> result(n0,n1);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,const T& fillvalue) -> rarray<T,3>
{
    rarray<T,3> result(n0,n1,n2);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,const T& fillvalue) -> rarray<T,4>
{
    rarray<T,4> result(n0,n1,n2,n3);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,const T& fillvalue) -> rarray<T,5>
{
    rarray<T,5> result(n0,n1,n2,n3,n4);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,const T& fillvalue) -> rarray<T,6>
{
    rarray<T,6> result(n0,n1,n2,n3,n4,n5);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,const T& fillvalue) -> rarray<T,7>
{
    rarray<T,7> result(n0,n1,n2,n3,n4,n5,n6);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,const T& fillvalue) -> rarray<T,8>
{
    rarray<T,8> result(n0,n1,n2,n3,n4,n5,n6,n7);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,const T& fillvalue) -> rarray<T,9>
{
    rarray<T,9> result(n0,n1,n2,n3,n4,n5,n6,n7,n8);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,const T& fillvalue) -> rarray<T,10>
{
    rarray<T,10> result(n0,n1,n2,n3,n4,n5,n6,n7,n8,n9);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10,const T& fillvalue) -> rarray<T,11>
{
    rarray<T,11> result(n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10);
    result.fill(fillvalue);
    return result;
}
}  // namespace ra
namespace std {
template<typename T, ra::rank_type R>
struct remove_all_extents<ra::rarray<T, R>> {
    using type = T;
};
template<typename T, ra::rank_type R>
struct rank<ra::rarray<T, R>> {
    static const size_t value = R;
};
}  // namespace std
namespace ra {
namespace detail {
template<typename T, rank_type R>
struct Deref {
    static inline auto access(typename PointerArray<T, R>::type p,
                              const size_type* indices) -> T& {
        return Deref<T, R-1>::access(p[indices[0]-1], indices+1);
    }
};
template<typename T>
struct Deref<T, 1>  {
    static inline auto access(typename PointerArray<T, 1>::type p,
                              const size_type* indices) -> T& {
        return p[indices[0]-1];
    }
};
template<typename T>
struct StringToValue {
    static inline void get(const std::string& input, T& output) {
        std::stringstream str(input);
        str >> output;
    }
};
template<>
struct StringToValue<std::string> {
    static inline void get(const std::string& input, std::string& output) {
        output = input;
    }
};
enum class token { BRACEOPEN, BRACECLOSE, COMMA, DATASTRING, END };
inline auto toch(const token& Token) -> char {
    switch (Token) {
    case token::BRACEOPEN:  return '{';
    case token::BRACECLOSE: return '}';
    case token::COMMA:      return ',';
    case token::DATASTRING: return '$';
    case token::END:        return '.';
    default: return '\0';
    }
}
template<typename T, rank_type R>
inline auto text_output(std::ostream &o, const rarray<T, R>& r)
-> std::ostream& {
    if (!r.empty()) {
        o << "{\n";
        for (size_type i = 0; i < r.extent(0); i++)  {
            if (i > 0)
                o << ",\n";
            o << r.at(i);
        }
        o << "\n}";
    } else {
        for (int i = 0; i < R; i++)
            o << '{';
        for (int i = 0; i < R; i++)
            o << '}';
        o << "\n";
    }
    return o;
}
template<typename T>
inline auto text_output(std::ostream &o, const rarray<T, 1>& r)
-> std::ostream& {
    if (!r.empty()) {
        o << '{';
        for (size_type i = 0; i < r.extent(0); i++) {
            if (i) o << ',';
            std::stringstream strstr;
            std::string result;
            const T& val = r.at(i);
            strstr << val;
            result = strstr.str();
            if (result.find_first_of("{,}#") != std::string::npos
                && !
                (result[0] == '(' && result[result.size()-1] == ')'
                 && result.substr(1, result.size()-2).find_first_of(")") == std::string::npos) )
                o << '#' << result.size() << ':';
            o << result;
        }
        o << '}';
    } else {
        o << "{}";
    }
    return o;
}
static inline auto get_but_eat_newline(std::istream & in) -> char {
    char ch1 = '\n';
    while (ch1 == '\n' && !in.eof())
        in >> ch1;
    return ch1;
}
static inline auto get_but_eat_whitespace(std::istream & in) -> char {
    char ch1;
    in >> ch1;
    return ch1;
}
template<rank_type R>
inline auto parse_shape(std::istream & in) -> std::pair<std::list<std::pair<token, std::string>>, size_type[R]> {
    std::pair<std::list<std::pair<token, std::string>>, size_type[R]> wholeresult;
    size_t init_file_ptr = in.tellg();
    try {
        std::list<std::pair<token, std::string>>& result = wholeresult.first;
        size_type* shape = wholeresult.second;
        size_type current_shape[R];
        for (rank_type i = 0; i < R; i++) {
            current_shape[i] = 1;
            shape[i] = 0;
            if (get_but_eat_newline(in) != '{')
                throw std::istream::failure("Format error");
            else
                result.push_back({token::BRACEOPEN, ""});
        }
        int current_depth = R-1;
        while (current_depth >= 0) {
            if (current_depth == R-1) {
                char         lastchar;
                std::string  word = "";
                do {
                    if (word == "") {
                        lastchar = get_but_eat_newline(in);
                    } else {
                        in.get(lastchar);
                    }
                    if (lastchar != ',' && lastchar != '}') {
                        word += lastchar;
                    }
                    if (word == "#") {
                        word = "";
                        std::string skipstr;
                        do {
                            in.get(lastchar);
                            skipstr += lastchar;
                        } while (lastchar != ':');
                        int skip = atoi(skipstr.c_str());
                        for (int i = 0; i < skip; i++) {
                            char nextchar;
                            in.get(nextchar);
                            word += nextchar;
                        }
                    } else if (word == "(") {
                        const int safeguardcount = 1024*1024;
                        int count = 0;
                        while (lastchar != ')' && count < safeguardcount) {
                            in.get(lastchar);
                            word += lastchar;
                            count++;
                        }
                        in.get(lastchar);
                    }
                    if (lastchar == ',') {
                        result.push_back({token::DATASTRING, word});
                        result.push_back({token::COMMA, ""});
                        word = "";
                        current_shape[current_depth]++;
                    }
                } while (lastchar != '}');
                result.push_back({token::DATASTRING, word});
                result.push_back({token::BRACECLOSE, ""});
                if (shape)
                    if (shape[current_depth] < current_shape[current_depth])
                        shape[current_depth] = current_shape[current_depth];
                current_depth--;
            } else {
                switch (get_but_eat_whitespace(in)) {
                   case ',':
                    result.push_back({token::COMMA, ""});
                    current_shape[current_depth]++;
                    break;
                  case '{':
                    result.push_back({token::BRACEOPEN, ""});
                    current_depth++;
                    current_shape[current_depth] = 1;
                    break;
                  case '}':
                    result.push_back({token::BRACECLOSE, ""});
                    if (shape)
                          if (shape[current_depth] < current_shape[current_depth])
                              shape[current_depth] = current_shape[current_depth];
                    current_depth--;
                    break;
                  default:
                    throw std::istream::failure("Format error");
                }
            }
        }
        result.push_back({token::END, ""});
    }
    catch (std::istream::failure& e) {
        in.seekg(init_file_ptr, in.beg);
        in.setstate(std::ios::failbit);
        throw;
    }
    return wholeresult;
}
template<typename T, rank_type R>
inline void parse_strings(const std::pair<std::list<std::pair<token, std::string>>,
                          size_type[R]> & tokens,
                          typename PointerArray<T, R>::type p) {
    size_type index[R];
    int current_depth = -1;
    for (auto& tokenpair : tokens.first) {
        switch (tokenpair.first) {
        case token::BRACEOPEN:
            current_depth++;
            index[current_depth] = 1;
            break;
        case token::BRACECLOSE:
            current_depth--;
            break;
        case token::COMMA:
            index[current_depth]++;
            break;
        case token::DATASTRING:
            StringToValue<T>::get(tokenpair.second, Deref<T, R>::access(p, index));
            break;
        case token::END:
            break;
        }
        if (tokenpair.first == token::END) break;
    }
}
}  // namespace detail
template<typename T, rank_type R>
inline auto operator<<(std::ostream &o, const rarray<T, R>& r) -> std::ostream& {
    return detail::text_output(o, r);
}
template<typename T, rank_type R>
inline auto operator>>(std::istream &in, rarray<T, R>& r) -> std::istream& {
    auto X = detail::parse_shape<R>(in);
    size_type* Xextent = X.second;
    if (std::accumulate(Xextent, Xextent+R, 1, std::multiplies<size_type>())
        <= r.size())
        r.reshape(Xextent, RESIZE::ALLOWED);
    else
        r = rarray<T, R>(Xextent);
    detail::parse_strings<T, R>(X, r.ptr_array());
    return in;
}
}  // namespace ra
#undef RA_CHECKORSAY
#undef RA_FORCE_inline
#undef RA_noboundscheck
#define EXTENT(A, I)   ra::extent(A, I)
#define RARRAY(A)      rarray<typename std::remove_all_extents<decltype(A)>::type, std::rank<decltype(A)>::value>(A)
#define INDEX(A, X, I) RARRAY(A).index(X, I)
using ra::rarray;
using ra::linspace;
using ra::xrange;
using ra::make_rarray;
template<typename T> using rvector = rarray<T, 1>;
template<typename T> using rmatrix = rarray<T, 2>;
template<typename T> using rtensor = rarray<T, 3>;
#else
#error "This file requires C++11 or newer support."
#endif
#endif
