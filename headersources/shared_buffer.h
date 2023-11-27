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

#ifndef SHAREDBUFFERH
#define SHAREDBUFFERH

#include "rarraymacros.h"
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <atomic>

// declaratieon for unit testing:                                          //TEST//
namespace ra {                                                             //TEST//   
namespace detail {                                                         //TEST//
template<class T> class shared_buffer;                                     //TEST//
}                                                                          //TEST//
}                                                                          //TEST//
template<typename V>                                                       //TEST//
int internal_check(const ra::detail::shared_buffer<V>& a,                  //TEST//
                   bool datavalue_shouldbe, V* datavalue,                  //TEST//
                   bool origvalue_shouldbe, V* origvalue,                  //TEST//
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,   //TEST//
                   bool refscount_shouldbe, int refscount,                 //TEST//
                   bool sizevalue_shouldbe,                                //TEST//
                   typename ra::detail::shared_buffer<V>::size_type sizevalue);    //TEST//

namespace ra {
namespace detail {
     
template<class T>
class shared_buffer
{
  public:

    typedef ssize_t size_type;

    // constructors 
    shared_buffer() noexcept
    {
        // uninitialized buffer
        uninit();
    }
    explicit shared_buffer(size_type asize)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr)
    {
        // construct buffer, exception safe
        auto to_be_data = std::unique_ptr<T[]>(new T[asize]);
        refs_ = new std::atomic<int>(1); // if this throws, let it   
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
    }    
    shared_buffer(size_type asize, T* adata) RA_NOEXCEPT(true)
    : data_(adata), orig_(nullptr), size_(asize), refs_(nullptr)
    {
        // construct buffer as a wrapper
        RA_CHECKORSAY(adata, "nullptr given as data");
    }
    shared_buffer(const shared_buffer& other) noexcept
    : data_(other.data_), orig_(other.orig_), size_(other.size_), refs_(other.refs_)
    {
        // copy constructor
        incref();
    }
    shared_buffer(shared_buffer&& from) noexcept
    : data_(from.data_), orig_(from.orig_), size_(from.size_), refs_(from.refs_)
    {
        from.uninit();
    }
    
    // copy and move assignment
    shared_buffer& operator=(const shared_buffer& other) noexcept {
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
    void operator=(shared_buffer&& from) noexcept {
        decref();
        data_ = from.data_;
        orig_ = from.orig_;
        size_ = from.size_;
        refs_ = from.refs_;
        from.uninit();
    }

    // destructor
    ~shared_buffer() noexcept {
        decref();
    }

    // element access without bounds checking (unless RA_BOUNDSCHECK is defined
    const T& operator[](size_type index) const RA_NOEXCEPT(true) {
        RA_CHECKORSAY(index >= 0 and index < size(), "element not in buffer");
        return data_[index];
    }
    T& operator[](size_type index) RA_NOEXCEPT(true) {
        RA_CHECKORSAY(index >= 0 and index < size(), "element not in buffer");
        return data_[index];
    }

    // element access with bounds checking
    const T& at(size_type index) const {
        // element access with bounds checking
        if (index < 0 or index >= size_)
            throw std::out_of_range("shared_buffer::at");
        return data_[index];
    }
    T& at(size_type index) {
        // element access with bounds checking
        if (index < 0 or index >= size_)
            throw std::out_of_range("shared_buffer::at");
        return data_[index];
    }

    // slice a part
    shared_buffer<T> slice(size_type from, size_type to) {
        // slice a part, checking bounds
        if (from < 0 or to < 0 or from > size_ or to > size_)
            throw std::out_of_range("shared_buffer::slice");
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to)
            result.size_ = to - from;
        return result;
    }
    const shared_buffer<T> slice(size_type from, size_type to) const {
        // slice a part, checking bounds
        if (from < 0 or to < 0 or from > size_ or to > size_)
            throw std::out_of_range("shared_buffer::slice");
        shared_buffer<T> result(*this);
        result.data_ += from;
        result.size_ = 0;
        if (from <= to)
            result.size_ = to - from;
        return result;
    }

    // size
    size_type size() const noexcept {
        return size_;
    }

    // create deep copy
    shared_buffer<T> copy() const {
        return shared_buffer<T>(size_,cbegin(), cend());
    }

    // iterators
    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin() noexcept {
        return data_;
    }
    iterator end() noexcept {
        return data_+size_;
    }
    const_iterator begin() const noexcept {
        return data_;
    }
    const_iterator end() const noexcept {
        return data_+size_;
    }
    const_iterator cbegin() const noexcept {
        return data_;
    }
    const_iterator cend() const noexcept {
        return data_+size_;
    }

    // reverse iterator
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    reverse_iterator rbegin() {
        return std::reverse_iterator<iterator>(data_+size_);
    }
    reverse_iterator rend() {
        return std::reverse_iterator<iterator>(data_);
    }
    const_reverse_iterator rbegin() const {
        return std::reverse_iterator<const_iterator>(data_+size_);
    }
    const_reverse_iterator rend() const {
        return std::reverse_iterator<const_iterator>(data_);
    }
    const_reverse_iterator crbegin() const  {
        return std::reverse_iterator<const_iterator>(data_+size_);
    }
    const_reverse_iterator crend() const {
        return std::reverse_iterator<const_iterator>(data_);
    }

    // resize
    void resize(size_type newsize, bool keep_content = false) {
        if ( (newsize<size_) and (refs_) and (*refs_)==1) {
            // optimize if this is known (from refs_) to be the only
            // instance of the buffer, and the new size is smaller:        
                size_ = newsize; // That's all? Yep!
            // note: this always keeps the content.
        } else {
            // create a new buffer
            std::atomic<int>* newrefs = new std::atomic<int>(1);
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
                try { // in case a copy assignment throws an exception
                    size_t n = ((size_<newsize)?size_:newsize);
                    for (size_t i=0; i<n; i++)
                        newdata[i] = data_[i];
                }
                catch (...) {// in case a copy assignment throws an exception
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
    void assign(const T& value) {
        for (int i=0;i<size_;i++)
            data_[i] = value;
    }
    void assign(std::initializer_list<T> ilist) {
        assign_iter(ilist.begin(), ilist.end());
    }
    template<class InputIt>
    void assign_iter(InputIt first, InputIt last) {
        resize(last-first);
        T* data = data_;
        for (InputIt it = first; it != last; it++)
            *(data++) = *it;
    }
   
  private:
    
    T*        data_;
    T*        orig_;
    size_type size_;
    std::atomic<int>* refs_;

    // for testing:                                                          //TEST//
    template<typename V>                                                     //TEST//
    friend int ::internal_check(const ra::detail::shared_buffer<V>& a,               //TEST// 
                   bool datavalue_shouldbe, V* datavalue,                    //TEST//
                   bool origvalue_shouldbe, V* origvalue,                    //TEST//
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,     //TEST//
                   bool refscount_shouldbe, int refscount,                   //TEST//
                   bool sizevalue_shouldbe,                                  //TEST//
                   typename ra::detail::shared_buffer<V>::size_type sizevalue);      //TEST//
    void uninit() noexcept  {
        data_ = nullptr;
        orig_ = nullptr;
        size_ = 0;
        refs_ = nullptr;
    }
    void incref() noexcept  {
        if (refs_)
            (*refs_)++;
    }
    // noexcept assumes destructors of T do not throw exceptions (as no destructor should)
    void decref() noexcept {
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
    shared_buffer(size_type asize, InputIt first, InputIt last)
    : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr)
    {
        // construct buffer, exception safe
        using noconstT = typename std::remove_const<T>::type;
        auto to_be_data = std::unique_ptr<T[]>(new T[asize]{*first});
        std::copy(first, last, const_cast<noconstT*>(to_be_data.get()));    
        refs_ = new std::atomic<int>(1); // if this throws, let it   
        data_ = to_be_data.release();
        orig_ = data_;
        size_ = asize;
     }

};

} 
}

#endif
