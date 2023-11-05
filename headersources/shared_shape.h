// shared_shape.h
//
// Header to define a 'shape', which is the pointer-to-pointer
// interface to a multidimensional array, but one that remembers its
// shape.  Uses reference counting for the pointer-to-pointer
// structure, but the actual data is a simple pointer.  One of the
// building blocks for rarray 2.x.
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

#ifndef SHARED_SHAPEH
#define SHARED_SHAPEH

#include "rarraymacros.h"
#include "offsets.h"
#include <array>
#include <stdexcept>
#include <memory>
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
struct PointerArray<T,0> { // We also end the recursion by specifically defining the R=0 case, this is needed for the _Bracket classes
    typedef T& type;     
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
    shared_shape() noexcept;                                      // non-functional shape
    shared_shape(const std::array<size_type,R>&anextent, T*adata);// construct shape 
    shared_shape(const shared_shape& other) noexcept;             // copy constructor
    shared_shape(shared_shape&& other) noexcept;                  // move constructor

    // assignment
    shared_shape& operator=(const shared_shape& other) noexcept; // copy
    void operator=(shared_shape&& other) noexcept;               // move

    // destructor
    ~shared_shape();

    // create a deep copy of the shape (not the data)
    shared_shape copy() const; 

    // change data or shape
    void relocate(T* newdata);                              // let shape point to other data block
    void reshape(const std::array<size_type,R>&newextent);  // change shape (not the data)

    // get out pointers
    ptrs_type ptrs() const noexcept;                        // get pointer-to-pointer structure
    T* data() const noexcept;                               // get pointer to the data
    size_type size() const noexcept;                        // get total number of elements
    size_type extent(int i) const;                          // get extent in dimension i
    const std::array<size_type,R>& extent() const noexcept; // get total extent array
    
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

    void uninit() noexcept;
    void incref() const noexcept;
    void decref() noexcept;
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
  private:
    std::array<size_type,0> extent_;
    ptrs_type ptrs_;
    std::atomic<int>* refs_;
    void***   orig_;
    size_type noffsets_;
    size_type ndataoffsets_;
    void incref() const noexcept {}
    template<class U, int S> friend class shared_shape; // for "at"
};
    
/***************************************************************************/

template<class T, int R>
shared_shape<T,R>::shared_shape() noexcept
{
    // uninitialized shape
    uninit();
}

template<class T, int R>
shared_shape<T,R>::shared_shape(const std::array<size_type,R>&anextent, T*adata)
  : extent_(anextent), ptrs_(nullptr), refs_(nullptr), orig_(nullptr)
{
    // construct shape 
    Offsets P({extent_.begin(),extent_.end()});
    auto to_be_orig = std::unique_ptr<void**[]>(P.apply_offsets(adata)); // this could throw, but only if its allocation fails, so no resource leak
    if (R>1) 
        refs_ = new std::atomic<int>(1); // if throws, to_be_orig gets dealloced
    orig_ = to_be_orig.release(); // no exceptions, so now store orig_
    ptrs_ = reinterpret_cast<ptrs_type>(orig_);
    if (R==1)
        orig_ = nullptr;  // avoids "double delete"
    noffsets_ = P.get_num_offsets();
    ndataoffsets_ = P.get_num_data_offsets();
}

template<class T, int R>
shared_shape<T,R>::shared_shape(const shared_shape& other) noexcept
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
shared_shape<T,R>::shared_shape(shared_shape&& other) noexcept
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
shared_shape<T,R>& shared_shape<T,R>::operator=(const shared_shape& other) noexcept
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
void shared_shape<T,R>::operator=(shared_shape&& other) noexcept
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
            typename shared_shape<T,R>::size_type ndataoffsets) noexcept
    {
        return reinterpret_cast<T*>(
                  const_cast<typename PointerArray<T,R-1>::noconst_type>(
                    ptrs[noffsets - ndataoffsets]
                  )
               );
    }
};

template<class T>
struct _data_from_ptrs_noffsets_ndataoffsets<T,1> {
    static T* call (typename PointerArray<T,1>::type ptrs,
                    typename shared_shape<T,1>::size_type noffsets,
                    typename shared_shape<T,1>::size_type ndataoffsets) noexcept
    {
        return reinterpret_cast<T*>(ptrs);
    }
};

template<class T, int R>
T* shared_shape<T,R>::data() const noexcept
{
    // returns a pointer to the first element of the data
    // return (T*)(ptrs_[noffsets_ - ndataoffsets_]);
    return _data_from_ptrs_noffsets_ndataoffsets<T,R>::call(ptrs_,noffsets_,ndataoffsets_);
}

template<class T, int R>
typename shared_shape<T,R>::size_type shared_shape<T,R>::size() const noexcept
{
    // get total number of elements
    return ndataoffsets_ * extent_[R-1];
}


template<class T, int R>
typename shared_shape<T,R>::size_type shared_shape<T,R>::extent(int i) const
{
    // get extent in dimension i
    if (i < 0 or i >= R)
        throw std::out_of_range("shared_shape::extent(int)");
    return extent_[i];
}

template<class T, int R> 
const std::array<typename shared_shape<T,R>::size_type,R>& shared_shape<T,R>::extent() const noexcept {
    // get total extent array
    return extent_;
}

template<class T, int R>
void shared_shape<T,R>::relocate(T* newdata)
{
    // keep the shape, but have it point at a different block of data
    if (R==1) {
        // no cast needed, as for R=1, typeof(ptrs_)==typeof(newdata);
        // but because we need this to work for c++11, can't use if
        // constexpr(), so the following must compile for all R:
        ptrs_ = reinterpret_cast<ptrs_type>(newdata);
    } else if (R>1) {
        // let shape point to other datablock
        std::ptrdiff_t shift = reinterpret_cast<const char*>(newdata) - reinterpret_cast<const char*>(data());
        if (shift != 0) {
            copy_before_write();
            for (size_type i = noffsets_ - ndataoffsets_; i < noffsets_; i++)
                orig_[i] = reinterpret_cast<void**>(reinterpret_cast<char*>(orig_[i]) + shift);
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
        using noconstT = typename std::remove_const<T>::type;
        void*** old_eff_orig = reinterpret_cast<void***>(
                                   const_cast<typename PointerArray<noconstT,R>::noconst_type>(
                                     ptrs_ ));
        std::copy(old_eff_orig, old_eff_orig + noffsets_, copy_of_this.orig_);
        std::ptrdiff_t shift = reinterpret_cast<char*>(copy_of_this.orig_)
                               - reinterpret_cast<char*>(old_eff_orig); 
        for (size_type i = 0; i < noffsets_ - ndataoffsets_; i++)
            copy_of_this.orig_[i] = reinterpret_cast<void**>(reinterpret_cast<char*>(copy_of_this.orig_[i]) + shift);        
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
void shared_shape<T,R>::reshape(const std::array<size_type,R>&newextent)
{
    if (newextent != extent_) {
        // should check if new extent is even valid
        *this = shared_shape<T,R>(newextent, data());
    }
}

template<class T, int R>
typename shared_shape<T,R>::ptrs_type shared_shape<T,R>::ptrs() const noexcept
{
    // get pointer-to-pointer structure
    return ptrs_;
}

template<class T, int R>
void shared_shape<T,R>::uninit() noexcept
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
void shared_shape<T,R>::incref() const noexcept
{
    // increase the reference counter
    if (refs_)
        (*refs_)++;
}

template<class T, int R>
void shared_shape<T,R>::decref() noexcept // assuming the T::~T() does not throw   
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
