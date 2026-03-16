// shared_buffer.h
//
// Header that defines a buffer class with reference counting (a bit
// like shared_array in boost, I suppose).  Can also hold a
// non-reference counted buffer, if one is passed to the constructor.
// Part of rarray 2.x.
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
#ifndef HEADERSOURCES_SHARED_BUFFER_H_  // EXCLUDE //
#define HEADERSOURCES_SHARED_BUFFER_H_  // EXCLUDE //

#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <atomic>
#include "rarraymacros.h"
#include "rarraytypes.h"

// declaratieon for unit testing:                                          // EXCLUDE //
namespace ra {                                                             // EXCLUDE //
namespace detail {                                                         // EXCLUDE //
template<class T> class shared_buffer;                                     // EXCLUDE //
}                                                                          // EXCLUDE //
}                                                                          // EXCLUDE //
template<typename V>                                                       // EXCLUDE //
auto internal_check(const ra::detail::shared_buffer<V>& abuf,              // EXCLUDE //
                   bool datavalue_shouldbe, V* datavalue,                  // EXCLUDE //
                   bool origvalue_shouldbe, V* origvalue,                  // EXCLUDE //
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,   // EXCLUDE //
                   bool refscount_shouldbe, int refscount,                 // EXCLUDE //
                   bool sizevalue_shouldbe,                                // EXCLUDE //
          typename ra::detail::shared_buffer<V>::size_type sizevalue) -> int; // EXCLUDE //

namespace ra {
namespace detail {

/// Internal shared buffer class
template<class T>
class shared_buffer {
 public:
    using size_type = ::ra::size_type;
    // constructors
    RA_FORCE_inline shared_buffer() noexcept {
        // uninitialized buffer
        uninit();
    }
    RA_FORCE_inline explicit shared_buffer(size_type asize)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr) {
        // construct buffer, exception safe
        using Tnoconst = typename std::remove_const<T>::type;
        auto to_be_data = std::unique_ptr<Tnoconst[]>(new Tnoconst[asize]);
        refs_ = new std::atomic<int>(1);  // if this throws, let it
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
    }
    RA_FORCE_inline shared_buffer(size_type asize, T* adata) noexcept(RA_noboundscheck)
    : data_(adata), orig_(nullptr), size_(asize), refs_(nullptr) {
        // construct buffer as a wrapper
        RA_CHECKORSAY(adata, "nullptr given as data");
    }
    // copy constructor
    RA_FORCE_inline shared_buffer(const shared_buffer& other) noexcept
    : data_(other.data_), orig_(other.orig_), size_(other.size_), refs_(other.refs_) {
        incref();
    }
    RA_FORCE_inline shared_buffer(shared_buffer&& from) noexcept
    : data_(from.data_), orig_(from.orig_), size_(from.size_), refs_(from.refs_) {
        from.uninit();
    }
    // copy and move assignment
    RA_FORCE_inline auto operator=(const shared_buffer& other) noexcept -> shared_buffer& {
        // shallow assignment with ref counting
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
    RA_FORCE_inline auto operator=(shared_buffer&& from) noexcept -> shared_buffer& {
        decref();
        data_ = from.data_;
        orig_ = from.orig_;
        size_ = from.size_;
        refs_ = from.refs_;
        from.uninit();
        return *this;
    }
    // destructor
    RA_FORCE_inline ~shared_buffer() noexcept {
        decref();
    }
    // element access without bounds checking (unless RA_BOUNDSCHECK is defined
    RA_FORCE_inline auto operator[](size_type index) const noexcept(RA_noboundscheck) -> const T& {
        RA_CHECKORSAY(index >= 0 && index < size(), "element not in buffer");
        return data_[index];
    }
    RA_FORCE_inline auto operator[](size_type index) noexcept(RA_noboundscheck) -> T& {
        RA_CHECKORSAY(index >= 0 && index < size(), "element not in buffer");
        return data_[index];
    }
    // element access with bounds checking
    RA_FORCE_inline auto at(size_type index) const -> const T& {
        // element access with bounds checking
        if (index < 0 || index >= size_) {
            throw std::out_of_range("shared_buffer::at");
        }
        return data_[index];
    }
    RA_FORCE_inline auto at(size_type index) -> T& {
        // element access with bounds checking
        if (index < 0 || index >= size_) {
            throw std::out_of_range("shared_buffer::at");
        }
        return data_[index];
    }
    // slice a part
    RA_FORCE_inline auto slice(size_type from, size_type to) -> shared_buffer<T> {
        // slice a part, checking bounds
        if (from < 0 || to < 0 || from > size_ || to > size_) {
            throw std::out_of_range("shared_buffer::slice");
        }
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to) {
            result.size_ = to - from;
        }
        return result;
    }
    RA_FORCE_inline auto slice(size_type from, size_type to) const -> const shared_buffer<T> {
        // slice a part, checking bounds
        if (from < 0 || to < 0 || from > size_ || to > size_) {
            throw std::out_of_range("shared_buffer::slice");
        }
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to) {
            result.size_ = to - from;
        }
        return result;
    }
    // size
    RA_FORCE_inline auto size() const noexcept -> size_type {
        return size_;
    }
    // create deep copy
    RA_FORCE_inline auto copy() const -> shared_buffer<T> {
        return shared_buffer<T>(size_, cbegin(), cend());
    }
    // iterators
    using iterator = T*;
    using const_iterator = const T*;
    RA_FORCE_inline auto begin() noexcept -> iterator {
        return data_;
    }
    RA_FORCE_inline auto end() noexcept -> iterator {
        return data_+size_;
    }
    RA_FORCE_inline auto begin() const noexcept -> const_iterator {
        return data_;
    }
    RA_FORCE_inline auto end() const noexcept -> const_iterator {
        return data_+size_;
    }
    RA_FORCE_inline auto cbegin() const noexcept -> const_iterator {
        return data_;
    }
    RA_FORCE_inline auto cend() const noexcept -> const_iterator {
        return data_+size_;
    }
    // reverse iterator
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    RA_FORCE_inline auto rbegin() -> reverse_iterator {
        return std::reverse_iterator<iterator>(data_+size_);
    }
    RA_FORCE_inline auto rend() -> reverse_iterator {
        return std::reverse_iterator<iterator>(data_);
    }
    RA_FORCE_inline auto rbegin() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_+size_);
    }
    RA_FORCE_inline auto rend() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_);
    }
    RA_FORCE_inline auto crbegin() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_+size_);
    }
    RA_FORCE_inline auto crend() const -> const_reverse_iterator {
        return std::reverse_iterator<const_iterator>(data_);
    }
    // resize
    RA_FORCE_inline void resize(size_type newsize, bool keep_content = false) {
        if ( newsize < size_ && refs_ != nullptr && (*refs_) == 1 ) {
            // optimize if this is known (from refs_) to be the only
            // instance of the buffer, and the new size is smaller:
            size_ = newsize;  // That's all? Yep!
            // note: this always keeps the content.
        } else {
            // create a new buffer
            auto newrefs = new std::atomic<int>(1);
            T* newdata = nullptr;
            try {
                newdata = new T[newsize];
            }
            catch (...) {
                delete newrefs;
                throw;
            }
            // copy content if so requested
            if (keep_content) {
                try {  // in case a copy assignment throws an exception
                    size_type minsize = ((size_ < newsize)?size_:newsize);
                    for (size_type i = 0; i < minsize; i++) {
                        newdata[i] = data_[i];
                    }
                }
                catch (...) {  // in case a copy assignment throws an exception
                    delete newrefs;
                    delete[] newdata;
                    throw;
                }
            }
            // delete if no references
            decref();
            // move to the new one
            data_ = newdata;
            orig_ = newdata;
            size_ = newsize;
            refs_ = newrefs;
       }
    }
    // assign values (could throw if copy assignment throws)
    RA_FORCE_inline void fill(const T& value) {
        for (size_type i = 0; i < size_; i++) {
            data_[i] = value;
        }
    }
    void assign(size_type count, const T& value) {
        resize(count);
        fill(value);
    }
    template<class InputIt,
             class = typename std::enable_if<
                 std::is_convertible<decltype(*InputIt()), T>::value>::type>
    RA_FORCE_inline void assign(InputIt first, InputIt last) {
        resize(last-first);
        T* data = data_;
        for (InputIt it = first; it != last; it++) {
            *(data++) = *it;
        }
    }
    RA_FORCE_inline void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

 private:
    T*        data_;
    T*        orig_;
    size_type size_;
    std::atomic<int>* refs_;
    // for testing:                                                              // EXCLUDE //
    template<typename V>                                                         // EXCLUDE //
    friend auto ::internal_check(const ra::detail::shared_buffer<V>& abuf,       // EXCLUDE //
                   bool datavalue_shouldbe, V* datavalue,                        // EXCLUDE //
                   bool origvalue_shouldbe, V* origvalue,                        // EXCLUDE //
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,         // EXCLUDE //
                   bool refscount_shouldbe, int refscount,                       // EXCLUDE //
                   bool sizevalue_shouldbe,                                      // EXCLUDE //
                   typename ra::detail::shared_buffer<V>::size_type sizevalue) -> int;  // EXCLUDE //
    RA_FORCE_inline void uninit() noexcept {
        data_ = nullptr;
        orig_ = nullptr;
        size_ = 0;
        refs_ = nullptr;
    }
    RA_FORCE_inline void incref() noexcept {
        if (refs_ != nullptr) {
            (*refs_)++;
        }
    }
    // noexcept assumes destructors of T do not throw exceptions (as no destructor should)
    RA_FORCE_inline void decref() noexcept {
        if (refs_ != nullptr) {
            if (--(*refs_) == 0) {
                delete[] orig_;
                delete refs_;
                uninit();
            }
        }
    }
    // private in this version at least
    template<typename InputIt>
    RA_FORCE_inline shared_buffer(size_type asize, InputIt first, InputIt last)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr) {
        // construct buffer, exception safe
        using noconstT = typename std::remove_const<T>::type;
        #ifndef __ibmxl__
        auto to_be_data = std::unique_ptr<T[]>(new T[asize]{*first});
        #else
        // ibm xl compiler does not support "new T[N]{init}" nor "new const T[N]", so have to cast const away:
        auto to_be_data = std::unique_ptr<T[]>(const_cast<T*>(new typename std::remove_const<T>::type[asize]));
        #endif
        std::copy(first, last, const_cast<noconstT*>(to_be_data.get()));
        refs_ = new std::atomic<int>(1);  // if this throws, let it
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
     }
};

}  // namespace detail
}  // namespace ra
#endif  // EXCLUDE //
