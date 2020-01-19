// shared_buffer.h
//
// Header that defines a buffer class with reference counting (a bit
// like shared_array in boost, I suppose).  Can also hold a
// non-reference counted buffer, if one is passed to the constructor.
// Part of rarray 2.0. 
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


#ifndef SHAREDBUFFERH
#define SHAREDBUFFERH

#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <atomic>

int test_shared_buffer_main(); // for testing

/***************************************************************************/

namespace ra {
// for debugging purposes, every shape gets a one-character name.
static char _buffername = 'A'; 

/***************************************************************************/

template<class T>
class shared_buffer
{
  public:

    typedef ssize_t size_type;

    // constructors 
    shared_buffer();
    explicit shared_buffer(size_type size);
    shared_buffer(size_type size, T* data);
    shared_buffer(const shared_buffer& other);
    shared_buffer(shared_buffer&& from);
    
    // assignment
    shared_buffer& operator=(const shared_buffer& other);  // copy
    void operator=(shared_buffer&& from);                  // move

    // destructor
    ~shared_buffer();

    // element access without bounds checking
    const T& operator[](size_type index) const;
    T& operator[](size_type index);
    
    // element access with bounds checking
    const T& at(size_type index) const;
    T& at(size_type index);

    // slice a part
    shared_buffer<T> slice(size_type from, size_type to);
    //shared_buffer<const T> slice(size_type from, size_type to) const;

    // size
    size_type size() const;

    // create deep copy
    shared_buffer<T> copy() const;

    // iterators
    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

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

    friend int ::test_shared_buffer_main(); // for testing

    void uninit();
    void incref();
    void decref();
    char name_;
};

/***************************************************************************/
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
            orig = nullptr;
            place = nullptr;
        }
    }
}

template<class T>
shared_buffer<T>::shared_buffer()
{
    // uninitialized buffer
    name_ = _buffername++;
    uninit();
}

template<class T>
shared_buffer<T>::shared_buffer(size_type size)
    : data_(new T[size]), orig_(data_), size_(size), refs_(new std::atomic<int>(1))
{
    // construct buffer
    name_ = _buffername++;
}

template<class T>
shared_buffer<T>::shared_buffer(size_type size, T* data)
  : data_(data), orig_(nullptr), size_(size), refs_(nullptr)
{
    // construct buffer as a wrapper
    name_ = _buffername++;
}

template<class T>
shared_buffer<T>::shared_buffer(const shared_buffer<T>& other)
  : data_(other.data_), orig_(other.orig_), size_(other.size_), refs_(other.refs_)
{
    // copy constructor
    name_ = _buffername++;
    incref();
}

template<class T>
shared_buffer<T>::shared_buffer(shared_buffer<T>&& from)
  : data_(from.data_), orig_(from.orig_), size_(from.size_), refs_(from.refs_)
{
    name_ = _buffername++;
    from.uninit();
}

template<class T>
shared_buffer<T>& shared_buffer<T>::operator=(const shared_buffer<T>& other)
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
void shared_buffer<T>::operator=(shared_buffer<T>&& from)
{
    decref();
    data_ = from.data_;
    orig_ = from.orig_;
    size_ = from.size_;
    refs_ = from.refs_;
    from.uninit();
}

template<class T>
const T& shared_buffer<T>::operator[](size_type index) const
{
    return data_[index];
}

template<class T>
T& shared_buffer<T>::operator[](size_type index)
{
    return data_[index];
}

template<class T>
const T& shared_buffer<T>::at(size_type index) const
{
    // element access with bounds checking
    if (index < 0 or index > size_)
        throw std::out_of_range("shared_buffer::at");
    return data_[index];
}

template<class T>
T& shared_buffer<T>::at(size_type index)
{
    // element access with bounds checking
    if (index < 0 or index > size_)
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
    if (from <= to)
        result.size_ = to - from;
    else
        result.size_ = 0;
    return result;
}

template<class T>
shared_buffer<T>::~shared_buffer()
{
    decref();
}

template<class T>
typename shared_buffer<T>::size_type shared_buffer<T>::size() const
{
    return size_;
}

template<class T>
shared_buffer<T> shared_buffer<T>::copy() const    
{
    shared_buffer<T> result(size_);
    std::copy(cbegin(), cend(), result.begin());
    return result;
}

// iterating

template<class T>
typename shared_buffer<T>::iterator shared_buffer<T>::begin()
{
    return data_;
}

template<class T>
typename shared_buffer<T>::iterator shared_buffer<T>::end()    
{
    return data_+size_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::cbegin() const
{
    return data_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::cend() const
{
    return data_+size_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::begin() const
{
    return data_;
}

template<class T>
typename shared_buffer<T>::const_iterator shared_buffer<T>::end() const
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
        T* newdata = new T[newsize];
        T* neworig = newdata;
        std::atomic<int>* newrefs = new std::atomic<int>(1);
        // copy content if so requested
        if (keep_content) {
            size_t n = ((size_<newsize)?size_:newsize);
            for (size_t i=0; i<n; i++)
                newdata[i] = data_[i];
        }
        // delete if no references
        decref();
        // move to the new one
        data_ = newdata;
        orig_ = neworig;
        size_ = newsize;
        refs_ = newrefs;
    }
}

template<class T>
void shared_buffer<T>::assign(const T& value)
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
void shared_buffer<T>::uninit() {
    data_ = nullptr;
    orig_ = nullptr;
    size_ = 0;
    refs_ = nullptr;
}

template<class T>
void shared_buffer<T>::incref() {
    if (refs_)
        (*refs_)++;
}

template<class T>
void shared_buffer<T>::decref() {
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
