// shared_shape.h
//
// Header to define a 'shape', which is the pointer-to-pointer
// interface to a multidimensional array, but one that remembers its
// shape.  Uses reference counting for the pointer-to-pointer
// structure, but the actual data is a simple pointer.  One of the
// building blocks for rarray 2.x.
//
// Copyright (c) 2018-2020  Ramses van Zon
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

#ifndef SHARED_SHAPEH
#define SHARED_SHAPEH

#include "offsets.h"
#include <array>
#include <stdexcept>
#include <atomic>

int test_shared_shape_main();

/***************************************************************************/

namespace ra {

/***************************************************************************/

// class to hold the shape but not the content of a multi-dimensional array.

template<typename T,int R> 
struct PointerArray {
    typedef typename PointerArray<T,R-1>::type const*    type;         // const shape, recursive
    typedef typename PointerArray<T,R-1>::noconst_type*  noconst_type; // non-const variant
};
template<typename T> 
struct PointerArray<T,1> { // We end the recursion by specifically defining the R=1 case
    typedef T* type;     // no const as this would express that the elements are constant
    typedef T* noconst_type; 
};
template<typename T> 
struct PointerArray<T,0> { // We end the recursion also by specifically defining the R=0 case
    typedef T& type;     // no const as this would express that the elements are constant
    typedef T& noconst_type; 
};
/***************************************************************************/

template<class T, int R>
class shared_shape
{
  public:

    typedef typename PointerArray<T,R>::type ptrs_type;
    typedef ssize_t size_type;

    // constructors 
    shared_shape();                                         // non-functional shape
    shared_shape(const std::array<size_type,R>&extent, T*data);// construct shape 
    shared_shape(const shared_shape& other);                // copy constructor
    shared_shape(shared_shape&& other);                     // move constructor

    // assignment
    shared_shape& operator=(const shared_shape& other);     // copy
    void operator=(shared_shape&& other);                   // move

    // destructor
    ~shared_shape();

    // create a deep copy of the shape (not the data)
    shared_shape copy() const; 

    // change data or shape
    void relocate(T* newdata);                              // let shape point to other data block
    void reshape(const std::array<size_type,R>&extent);     // change shape (not the data)

    // get out pointers
    ptrs_type ptrs() const;                                 // get pointer-to-pointer structure
    T* data() const;                                        // get pointer to the data
    size_type size() const;                                 // get total number of elements
    size_type extent(int i) const;                          // get extent in dimension i
    const std::array<size_type,R>& extent() const;          // get total extent array
    
    // get subshape
    shared_shape<T,R-1> at(size_type r) const;
    //shared_shape<const T,R-1> at(size_type r) const;
    
  private:

    std::array<size_type,R> extent_;
    ptrs_type ptrs_;
    std::atomic<int>* refs_;
    void***   orig_;
    size_type noffsets_;
    size_type ndataoffsets_;

    void uninit();
    void incref() const;
    void decref();
    void copy_before_write();

    template<class U, int S> friend class shared_shape; // for "at"

    // for testing and debugging:
    friend int ::test_shared_shape_main();
};

// stop at() recursion at R=0. This is a non-shape, but at() for R=1 sets its elements
template<typename T> 
class shared_shape<T,0> {
  public:
    typedef T ptrs_type;
    typedef ssize_t size_type;
    //shared_shape() {}
  private:
    std::array<size_type,0> extent_;
    ptrs_type ptrs_;
    std::atomic<int>* refs_;
    void***   orig_;
    size_type noffsets_;
    size_type ndataoffsets_;
    void incref() {}
    template<class U, int S> friend class shared_shape; // for "at"
};
    
/***************************************************************************/

template<class T, int R>
shared_shape<T,R>::shared_shape()
{
    // uninitialized shape
    uninit();
}

template<class T, int R>
shared_shape<T,R>::shared_shape(const std::array<size_type,R>&extent, T*data)
  : extent_(extent), ptrs_(nullptr), refs_(nullptr), orig_(nullptr)
{
    // construct shape 
    Offsets P({extent_.begin(),extent_.end()});
    orig_ = P.apply_offsets(data);
    ptrs_ = reinterpret_cast<ptrs_type>(orig_);
    noffsets_ = P.get_num_offsets();
    ndataoffsets_ = P.get_num_data_offsets();
    if (R>1) {
        refs_ = new std::atomic<int>(1);
    } else {
        orig_ = nullptr;
    }
}

template<class T, int R>
shared_shape<T,R>::shared_shape(const shared_shape& other)
  : extent_(other.extent_),
    ptrs_(other.ptrs_),
    refs_(other.refs_),
    orig_(other.orig_),
    noffsets_(other.noffsets_),
    ndataoffsets_(other.ndataoffsets_)
{
    // copy constructor
    incref();
}

template<class T, int R>
shared_shape<T,R>::shared_shape(shared_shape&& other)
  : extent_(other.extent_),
    ptrs_(other.ptrs_),
    refs_(other.refs_),
    orig_(other.orig_),
    noffsets_(other.noffsets_),
    ndataoffsets_(other.ndataoffsets_)
{
    // move constructor
    other.uninit();
}

template<class T, int R>
shared_shape<T,R>& shared_shape<T,R>::operator=(const shared_shape& other)
{
    // copy assignment (shallow with ref counting)
    if (this != &other) {
        decref();
        extent_       = other.extent_;
        ptrs_         = other.ptrs_;
        refs_         = other.refs_;
        orig_         = other.orig_;
        noffsets_     = other.noffsets_;
        ndataoffsets_ = other.ndataoffsets_;
        incref();
    }
    return *this;
}

template<class T, int R>
void shared_shape<T,R>::operator=(shared_shape&& other)
{
    // move assignment
    decref();
    extent_       = other.extent_;
    ptrs_         = other.ptrs_;
    refs_         = other.refs_;
    orig_         = other.orig_;
    noffsets_     = other.noffsets_;
    ndataoffsets_ = other.ndataoffsets_;
    other.uninit();
}

template<class T, int R>
shared_shape<T,R>::~shared_shape()
{
    // memory deallocation governed by reference counting
    decref();
}

template<class T, int R>
void shared_shape<T,R>::copy_before_write()
{
    // Checks if other references to it exist and if so, copies itself.
    // Intended to be called before changing ptrs_, like a copy-on-write.
    if (R > 1 and refs_ and *refs_ > 1) 
        *this = this->copy();
}

template<class T, int R>
struct _data_from_ptrs_noffsets_ndataoffsets {
    static
    T* call(typename PointerArray<T,R>::type ptrs,
            typename shared_shape<T,R>::size_type noffsets,
            typename shared_shape<T,R>::size_type ndataoffsets)
    {
        return (T*)(ptrs[noffsets - ndataoffsets]);
    }
};

template<class T>
struct _data_from_ptrs_noffsets_ndataoffsets<T,1> {
    static T* call (typename PointerArray<T,1>::type ptrs,
                    typename shared_shape<T,1>::size_type noffsets,
                    typename shared_shape<T,1>::size_type ndataoffsets)
    {
        return (T*)(ptrs);
    }
};

template<class T, int R>
T* shared_shape<T,R>::data() const
{
    // returns a pointer to the first element of the data
    // return (T*)(ptrs_[noffsets_ - ndataoffsets_]);
    return _data_from_ptrs_noffsets_ndataoffsets<T,R>::call(ptrs_,noffsets_,ndataoffsets_);
}

template<class T, int R>
typename shared_shape<T,R>::size_type shared_shape<T,R>::size() const
{
    // get total number of elements
    return ndataoffsets_ * extent_[R-1];
}


template<class T, int R>
typename shared_shape<T,R>::size_type shared_shape<T,R>::extent(int i) const
{
    // get extent in dimension i
    return extent_[i];
}

template<class T, int R> 
const std::array<typename shared_shape<T,R>::size_type,R>& shared_shape<T,R>::extent() const {
    // get total extent array
    return extent_;
}

template<class T, int R>
void shared_shape<T,R>::relocate(T* newdata)
{
    // keep the shape, but have it point at a different block of data
    if (R==1) {
        // cast never really needed, for R=1, typeof(ptrs_)==typeof(newdata)
        ptrs_ = reinterpret_cast<ptrs_type>(newdata);
    } else if (R>1) {
        // let shape point to other datablock
        std::ptrdiff_t shift = newdata - data();
        if (shift != 0) {
            copy_before_write();
            for (size_type i = noffsets_ - ndataoffsets_; i < noffsets_; i++)
                orig_[i] = reinterpret_cast<void**>((T*)(orig_[i]) + shift);
        }
    }
}

template<class T, int R>
shared_shape<T,R> shared_shape<T,R>::copy() const
{
    // copy shape (not the data)
    shared_shape copy_of_this;
    copy_of_this.extent_ = extent_;
    copy_of_this.noffsets_ = noffsets_;
    copy_of_this.ndataoffsets_ = ndataoffsets_;
    if (R>1) {
        copy_of_this.refs_ = new std::atomic<int>(1);
        copy_of_this.orig_ = new void**[noffsets_];
        std::copy((void***)ptrs_, (void***)(ptrs_) + noffsets_, copy_of_this.orig_);
        std::ptrdiff_t shift = copy_of_this.orig_ - (void***)(ptrs_);
        for (size_type i = 0; i < noffsets_ - ndataoffsets_; i++)
            copy_of_this.orig_[i] += shift;
        copy_of_this.ptrs_ = reinterpret_cast<ptrs_type>(copy_of_this.orig_);
    } else {        
        copy_of_this.ptrs_ = ptrs_;
    }
    return copy_of_this;
}

template<class T, int R>
shared_shape<T,R-1> shared_shape<T,R>::at(size_type index) const
{
    // element access with bounds checking
    if (R < 1 or index < 0 or index >= extent_[0])
        throw std::out_of_range("shared_shape::at");
    shared_shape<T,R-1> result;
    if (R>1) {
        for (size_type i = 0; i < R-1; ++i)
            result.extent_[i] = extent_[i+1];
        result.ptrs_         = ptrs_[index];
        result.refs_         = refs_;
        result.orig_         = orig_;
        result.noffsets_     = noffsets_/extent_[0] - 1;
        result.ndataoffsets_ = ndataoffsets_/extent_[0];
        incref();
    }
    return result;
}

template<class T, int R>
void shared_shape<T,R>::reshape(const std::array<size_type,R>&extent)
{
    if (extent != extent_) {
        // should perhaps check it new extent is even valid
        *this = shared_shape<T,R>(extent, data());
    }
}

template<class T, int R>
typename shared_shape<T,R>::ptrs_type shared_shape<T,R>::ptrs() const
{
    // get pointer-to-pointer structure
    return ptrs_;
}

template<class T, int R>
void shared_shape<T,R>::uninit()
{
    // put this object into an uninitialized state
    ptrs_         = nullptr;
    orig_         = nullptr;
    refs_         = nullptr;
    noffsets_     = 0;
    ndataoffsets_ = 0;
    extent_.fill(0);
}

template<class T, int R>
void shared_shape<T,R>::incref() const
{
    // increase the reference counter
    if (refs_)
        (*refs_)++;
}

template<class T, int R>
void shared_shape<T,R>::decref()
{
    // decrease the reference counter
    if (refs_) {
        if (--(*refs_) == 0) {
            if (R>1)
                delete[] orig_;
            delete refs_;
            uninit();
        }
    }
}

/***************************************************************************/

}

#endif
