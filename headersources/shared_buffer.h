// shared_buffer.h
//
// Header that defines a buffer class with reference counting (a bit
// like shared_array in boost, I suppose).  Can also hold a
// non-reference counted buffer, if one is passed to the constructor.
// Part of rarray 2.x.
//
// Copyright (c) 2018-2023  Ramses van Zon
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
int internal_check(const ra::detail::shared_buffer<V>& a,                  // EXCLUDE //
                   bool datavalue_shouldbe, V* datavalue,                  // EXCLUDE //
                   bool origvalue_shouldbe, V* origvalue,                  // EXCLUDE //
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,   // EXCLUDE //
                   bool refscount_shouldbe, int refscount,                 // EXCLUDE //
                   bool sizevalue_shouldbe,                                // EXCLUDE //
          typename ra::detail::shared_buffer<V>::size_type sizevalue);     // EXCLUDE //

namespace ra {
namespace detail {

/// Internal shared buffer class
template<class T>
class shared_buffer {
 public:
    using size_type = ::ra::size_type;
    // constructors
    inline shared_buffer() noexcept {
        // uninitialized buffer
        uninit();
    }
    explicit inline shared_buffer(size_type asize)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr) {
        // construct buffer, exception safe
        auto to_be_data = std::unique_ptr<T[]>(new T[asize]);
        refs_ = new std::atomic<int>(1);  // if this throws, let it
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
    }
    inline shared_buffer(size_type asize, T* adata) noexcept(RA_noboundscheck)
    : data_(adata), orig_(nullptr), size_(asize), refs_(nullptr) {
        // construct buffer as a wrapper
        RA_CHECKORSAY(adata, "nullptr given as data");
    }
    // copy constructor
    inline shared_buffer(const shared_buffer& other) noexcept
    : data_(other.data_), orig_(other.orig_), size_(other.size_), refs_(other.refs_) {
        incref();
    }
    inline shared_buffer(shared_buffer&& from) noexcept
    : data_(from.data_), orig_(from.orig_), size_(from.size_), refs_(from.refs_) {
        from.uninit();
    }
    // copy and move assignment
    inline auto operator=(const shared_buffer& other) noexcept -> shared_buffer& {
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
    inline void operator=(shared_buffer&& from) noexcept {
        decref();
        data_ = from.data_;
        orig_ = from.orig_;
        size_ = from.size_;
        refs_ = from.refs_;
        from.uninit();
    }
    // destructor
    inline ~shared_buffer() noexcept {
        decref();
    }
    // element access without bounds checking (unless RA_BOUNDSCHECK is defined
    inline auto operator[](size_type index) const noexcept(RA_noboundscheck) -> const T& {
        RA_CHECKORSAY(index >= 0 && index < size(), "element not in buffer");
        return data_[index];
    }
    inline auto operator[](size_type index) noexcept(RA_noboundscheck) -> T& {
        RA_CHECKORSAY(index >= 0 && index < size(), "element not in buffer");
        return data_[index];
    }
    // element access with bounds checking
    inline auto at(size_type index) const -> const T& {
        // element access with bounds checking
        if (index < 0 || index >= size_)
            throw std::out_of_range("shared_buffer::at");
        return data_[index];
    }
    inline auto at(size_type index) -> T& {
        // element access with bounds checking
        if (index < 0 || index >= size_)
            throw std::out_of_range("shared_buffer::at");
        return data_[index];
    }
    // slice a part
    inline auto slice(size_type from, size_type to) -> shared_buffer<T> {
        // slice a part, checking bounds
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
        // slice a part, checking bounds
        if (from < 0 || to < 0 || from > size_ || to > size_)
            throw std::out_of_range("shared_buffer::slice");
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to)
            result.size_ = to - from;
        return result;
    }
    // size
    inline auto size() const noexcept -> size_type {
        return size_;
    }
    // create deep copy
    inline auto copy() const -> shared_buffer<T> {
        return shared_buffer<T>(size_, cbegin(), cend());
    }
    // iterators
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
    // reverse iterator
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
    // resize
    inline void resize(size_type newsize, bool keep_content = false) {
        if ( newsize < size_ && refs_ && (*refs_) == 1 ) {
            // optimize if this is known (from refs_) to be the only
            // instance of the buffer, and the new size is smaller:
            size_ = newsize;  // That's all? Yep!
            // note: this always keeps the content.
        } else {
            // create a new buffer
            auto newrefs = new std::atomic<int>(1);
            T* newdata;
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
                    size_type n = ((size_ < newsize)?size_:newsize);
                    for (size_type i = 0; i < n; i++)
                        newdata[i] = data_[i];
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
    // for testing:                                                              // EXCLUDE //
    template<typename V>                                                         // EXCLUDE //
    friend int ::internal_check(const ra::detail::shared_buffer<V>& a,           // EXCLUDE //
                   bool datavalue_shouldbe, V* datavalue,                        // EXCLUDE //
                   bool origvalue_shouldbe, V* origvalue,                        // EXCLUDE //
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,         // EXCLUDE //
                   bool refscount_shouldbe, int refscount,                       // EXCLUDE //
                   bool sizevalue_shouldbe,                                      // EXCLUDE //
                   typename ra::detail::shared_buffer<V>::size_type sizevalue);  // EXCLUDE //
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
    // noexcept assumes destructors of T do not throw exceptions (as no destructor should)
    inline void decref() noexcept {
        if (refs_) {
            if (--(*refs_) == 0) {
                delete[] orig_;
                delete refs_;
                uninit();
            }
        }
    }
    // private in this version at least
    template<typename InputIt>
    inline shared_buffer(size_type asize, InputIt first, InputIt last)
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
