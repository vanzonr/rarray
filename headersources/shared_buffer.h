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

// declaration for unit testing:
namespace ra { template<class T> class shared_buffer; }
template<typename V> 
int internal_check(const ra::shared_buffer<V>& a,
                   bool datavalue_shouldbe, V* datavalue,
                   bool origvalue_shouldbe, V* origvalue,
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,
                   bool refscount_shouldbe, int refscount,
                   bool sizevalue_shouldbe, typename ra::shared_buffer<V>::size_type sizevalue);

/***************************************************************************/

namespace ra {

/***************************************************************************/

template<class T>
class shared_buffer
{
  public:

    typedef ssize_t size_type;

    // constructors 
    shared_buffer() noexcept;
    explicit shared_buffer(size_type asize);
    shared_buffer(size_type asize, T* adata) RA_NOEXCEPT(true);
    shared_buffer(const shared_buffer& other) noexcept;
    shared_buffer(shared_buffer&& from) noexcept;
    
    // assignment
    shared_buffer& operator=(const shared_buffer& other) noexcept;  // copy
    void operator=(shared_buffer&& from) noexcept;                  // move

    // destructor
    ~shared_buffer() noexcept;

    // element access without bounds checking
    const T& operator[](size_type index) const RA_NOEXCEPT(true);
    T& operator[](size_type index) RA_NOEXCEPT(true);
    
    // element access with bounds checking
    const T& at(size_type index) const;
    T& at(size_type index);

    // slice a part
    shared_buffer<T> slice(size_type from, size_type to);
    const shared_buffer<T> slice(size_type from, size_type to) const;

    // size
    size_type size() const noexcept;

    // create deep copy
    shared_buffer<T> copy() const;

    // iterators
    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    // reverse iterator
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    // resize
    void resize(size_type newsize, bool keep_content = false);
    
    // assign values
    void assign(const T& value);
    void assign(std::initializer_list<T> ilist);
    template<class InputIt>
    void assign_iter(InputIt first, InputIt last);
   
  private:
    
    T*        data_;
    T*        orig_;
    size_type size_;
    std::atomic<int>* refs_;

    // for testing:
    template<typename V> 
    friend int ::internal_check(const ra::shared_buffer<V>& a,
                   bool datavalue_shouldbe, V* datavalue,
                   bool origvalue_shouldbe, V* origvalue,
                   bool refsvalue_shouldbe, std::atomic<int>* refsvalue,
                   bool refscount_shouldbe, int refscount,
                   bool sizevalue_shouldbe, typename ra::shared_buffer<V>::size_type sizevalue);
    void uninit() noexcept;
    void incref() noexcept;
    void decref() noexcept;
    template<typename InputIt>
    shared_buffer(size_type asize, InputIt first, InputIt last) RA_NOEXCEPT(true);
};

/***************************************************************************/
/* FUTURE FEATURE :
#define RA_ALIGNMENT_IN_BYTES 64

template<class T>
void malign(void*& orig, void*& place, size_t size, size_t bytealignment)
{
    size_t paddedsizeinbytes = size*sizeof(T) + bytealignment - 1;
    void* some_orig = malloc(paddedsizeinbytes);
    if (some_orig) {        
        void* some_place = some_orig;
        if (std::align(bytealignment,
                       size*sizeof(T),
                       some_place,
                       paddedsizeinbytes)) {
            orig = some_orig;
            place = some_place;
        } else {
            free(some_orig);
            throw std::bad_alloc();
        }
    } else {
        throw std::bad_alloc();
    }
}
*/
    
template<class T>
shared_buffer<T>::shared_buffer() noexcept
{
    // uninitialized buffer
    uninit();
}

template<class T>
shared_buffer<T>::shared_buffer(size_type asize) 
  : data_(nullptr), orig_(nullptr), size_(0), refs_(nullptr)
{
    // construct buffer, exception safe
    auto to_be_data = std::unique_ptr<T[]>(new T[asize]);
    refs_ = new std::atomic<int>(1); // if this throws, let it   
    data_ = to_be_data.release();
    orig_ = data_;
    size_ = asize;
}

template<class T>
shared_buffer<T>::shared_buffer(size_type asize, T* adata) RA_NOEXCEPT(true)
  : data_(adata), orig_(nullptr), size_(asize), refs_(nullptr)
{
    // construct buffer as a wrapper
    RA_CHECKORSAY(adata, "nullptr given as data");
}

//private in this version at leastt
template<class T>
template<typename InputIt>
shared_buffer<T>::shared_buffer(size_type asize, InputIt first, InputIt last) RA_NOEXCEPT(true)
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

template<class T>
shared_buffer<T>::shared_buffer(const shared_buffer<T>& other) noexcept
  : data_(other.data_), orig_(other.orig_), size_(other.size_), refs_(other.refs_)
{
    // copy constructor
    incref();
}

template<class T>
shared_buffer<T>::shared_buffer(shared_buffer<T>&& from) noexcept
  : data_(from.data_), orig_(from.orig_), size_(from.size_), refs_(from.refs_)
{
    from.uninit();
}

template<class T>
shared_buffer<T>& shared_buffer<T>::operator=(const shared_buffer<T>& other) noexcept
{
    // copy assignment (shallow with ref counting)
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

template<class T>
void shared_buffer<T>::operator=(shared_buffer<T>&& from) noexcept
{
    decref();
    data_ = from.data_;
    orig_ = from.orig_;
    size_ = from.size_;
    refs_ = from.refs_;
    from.uninit();
}

template<class T>
const T& shared_buffer<T>::operator[](size_type index) const RA_NOEXCEPT(true)
{
    RA_CHECKORSAY(index >= 0 and index < size(), "element not in buffer");
    return data_[index];
}

template<class T>
T& shared_buffer<T>::operator[](size_type index) RA_NOEXCEPT(true)
{
    RA_CHECKORSAY(index >= 0 and index < size(), "element not in buffer");
    return data_[index];
}

template<class T>
const T& shared_buffer<T>::at(size_type index) const
{
    // element access with bounds checking
    if (index < 0 or index >= size_)
        throw std::out_of_range("shared_buffer::at");
    return data_[index];
}

template<class T>
T& shared_buffer<T>::at(size_type index)
{
    // element access with bounds checking
    if (index < 0 or index >= size_)
        throw std::out_of_range("shared_buffer::at");
    return data_[index];
}

template<class T>
shared_buffer<T> shared_buffer<T>::slice(size_type from, size_type to)
{
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

template<class T>
const shared_buffer<T> shared_buffer<T>::slice(size_type from, size_type to) const
{
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

template<class T>
shared_buffer<T>::~shared_buffer() noexcept
{
    decref();
}

template<class T>
typename shared_buffer<T>::size_type shared_buffer<T>::size() const noexcept
{
    return size_;
}

template<class T>
shared_buffer<T> shared_buffer<T>::copy() const    
{
    return shared_buffer<T>(size_,cbegin(), cend());
}

// iterating

template<class T>
typename shared_buffer<T>::iterator shared_buffer<T>::begin() noexcept
{
    return data_;
}

template<class T>
typename shared_buffer<T>::iterator shared_buffer<T>::end() noexcept
{
    return data_+size_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::cbegin() const noexcept
{
    return data_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::cend() const noexcept
{
    return data_+size_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::begin() const noexcept
{
    return data_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::end() const noexcept
{
    return data_+size_;
}

// reverse iterating

template<class T>
typename shared_buffer<T>::reverse_iterator shared_buffer<T>::rbegin()
{
    return std::reverse_iterator<iterator>(data_+size_);
}

template<class T>
typename shared_buffer<T>::reverse_iterator shared_buffer<T>::rend()
{
    return std::reverse_iterator<iterator>(data_);
}

template<class T>
typename shared_buffer<T>::const_reverse_iterator shared_buffer<T>::crbegin() const
{
    return std::reverse_iterator<const_iterator>(data_+size_);
}

template<class T>
typename shared_buffer<T>::const_reverse_iterator shared_buffer<T>::crend() const
{
    return std::reverse_iterator<const_iterator>(data_);
}

template<class T>
typename shared_buffer<T>::const_reverse_iterator shared_buffer<T>::rbegin() const
{
    return std::reverse_iterator<const_iterator>(data_+size_);
}

template<class T>
typename shared_buffer<T>::const_reverse_iterator shared_buffer<T>::rend() const
{
    return std::reverse_iterator<const_iterator>(data_);
}

template<class T>
void shared_buffer<T>::resize(size_type newsize, bool keep_content)
{
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

template<class T>
void shared_buffer<T>::assign(const T& value) // could throw if copy assignment throws
{
    for (int i=0;i<size_;i++)
        data_[i] = value;
}

template<class T>
template<class InputIt>
void shared_buffer<T>::assign_iter(InputIt first, InputIt last)
{
    resize(last-first);
    T* data = data_;
    for (InputIt it = first; it != last; it++)
        *(data++) = *it;
}

template<class T>
void shared_buffer<T>::assign(std::initializer_list<T> ilist)
{
    assign_iter(ilist.begin(), ilist.end());
}

/***************************************************************************/

template<class T>
void shared_buffer<T>::uninit() noexcept {
    data_ = nullptr;
    orig_ = nullptr;
    size_ = 0;
    refs_ = nullptr;
}

template<class T>
void shared_buffer<T>::incref() noexcept {
    if (refs_)
        (*refs_)++;
}

template<class T>
void shared_buffer<T>::decref() noexcept { // noexcept assumes
                                           // destructors of T do not
                                           // throw exceptions (as no
                                           // destructor should)
    if (refs_) {
        if (--(*refs_) == 0) {
            delete[] orig_;
            delete refs_;
            uninit();
        }
    }
}

/***************************************************************************/
}
#endif
