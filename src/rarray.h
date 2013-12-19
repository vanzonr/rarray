//
// rarray.h 
//
// Runtime arrays
//
// Template classes for pointer-based, runtime dimensionalized,
// multi-dimensional arrays.
//
// There are a number of design points of the 'rarray' array
// 
// 1. Having dynamically allocated multidimensional array that
//    combine the convenience of automatic c++ arrays with that of the
//    typical textbook dynamically allocated pointer-to-pointer
//    structure. Thus one would ideally be able to write
//
//       double [*][*] a = new double[100][100];
//       ...
//       a[4][2] = ...
//
//    and a 'double[*][*]' would be a variant of double** (really,
//    double *const*, see below), which remembers its index
//    ranges. However, this would require a modification of the
//    language, so we work with rarray instead, it becomes:
//
//       rarray<double,2> a = rarray<double,2>(100,100);
//       rarray<double,2> a = rnew<double>(100,100);
//       ...
//       a[4][2] = ...
//
//    or just
//
//       rarray<double,2> a(100,100);
//       ...
//       a[4][2] = ...
//
//    Essentially, rarray<TYPE,1>=TYPE[*], rarray<TYPE,2> =
//    TYPE[*][*], rarray<TYPE,3> = TYPE[*][*][*] etc. More verbose
//    then I'd like, but no more than needed within the constraints of
//    the c++ language.
//
//    The compatibility requirement with pointer-to-pointer structures
//    is achieve by allocating a pointer-to-pointer structure. This
//    accounts for most of the memory overhead of using rarray.
//
// 2. Having rarrays know their sizes, so that can be passed to
//    functions as a single argument. eg.
//
//       void printme(rarray<double,2>& a) {
//          for (int i=0;i<a.extent(0);i++) {
//             for (int j=0;j<a.extent(1);j++) 
//                cout << ' ' << a.out;
//             cout << endl;
//          }
//       }
//       printme(a);
//
//    Note: passing an rarray by value instead of passing the
//    reference would make a shallow, reference counted copy. There is
//    a copy function to create a deep copy.
//
// 3. Enabling interplay with libraries: this is achieved by
//    guarranteeing contiguous elements in the multi-dimensional
//    array, and a way to get this data out:
//
//       double* adata = a.data();
//
//    Relatedly, it should be allowed to use an external buffer, e.g.
//
//       double* adata = new double[100*100];
//       rarray<double,2> = rarray<double,2>(adata,100,100);
//
//    This is similar to a placement new, except the constructors of
//    the data are called in the first line instead of in the
//    definition of the rarray.
//    
//    The guarrantee of contiguity means strided arrays are not supported.
//
// 4. Optionally allowing bounds checking, triggered by defining the
//    BOUNDSCHECK compiler constant.
//
// 5. Avoiding a lot of cluttered sematics around const with pointer
//    to pointer structers.
//
// Precompiler notes:
//
// 1. Temporary intermediate expression in a multi-bracketed
//    expression (such as a[5] in a[5][6][7]) are represented by an
//    intermediate class called rarray_intermediate.  This prevents
//    assignment to such expressions and allows the optional bound
//    checking to work.  Most compilers (gcc, intel) can optimize
//    these intermediate classes away when bounds checking is
//    off. However some compilers (e.g. pgi) cannot do this and suffer
//    from speed degredation because of these intermediate objects.
//
//    To get full speedup for these less-optimizing compilers, you can
//    define the preprocessor constant SKIPINTERMEDIATE (usually with
//    a -DSKIPINTERMEDIATE compiler option).  Intermediate expression
//    in a multi-bracketed expression are then replaced by bare
//    pointers-to-pointers.
//
// 2. If compiled with BOUNDSCHECK defined (usually with a
//    -DBOUNDSCHECK compiler option), the rarray library implements
//    bounds check functionality.  It throws a standard out_of_range
//    exception is a range check condition is not satisfied.
//
//    When BOUNDSCHECK is set, it will switch off SKIPINTERMEDIATE.
//
// More documentation in docrarray.pdf
//
// (c) 2013 Ramses van Zon - SciNet/University of Toronto 
//

#ifndef RARRAY_H
#define RARRAY_H

#include <string>
#include <sstream>

////////////////////////////////////////////////////////////////////////////

// Define internal types

////////  DEFINITION struct PointerArray<T,R>  ////////

// PointerArray: a struct to recursively generates types T**... and
// T*const*... as typedefs called noconst_pointer_array_t and pointer_array_t inside the
// struct PointerArray<T,R>.
// 
// More specifically:
//
//   The type 
//      PointerArray<T,1>::type
//   is equal to the type
//      T*
//   ie., the pointer type for a one dimensional c array, 
//
//   The type 
//      PointerArray<T,2>::type
//   is equal to the type
//      T*const*
//   which is the pointer type for a two-dimensional c array.
//   The reason for the const is that the shape of this array is supposed to
//   remain at its fixed rectangular form. 
//
//   The type
//      PointerArray<T,3>::type 
//   is equal to
//      T*const*const*, 
//   the appropriate pointer for a 3d c array, 
//
//   etc.
//
//   Because one encounters non-const correct pointer-to-pointers
//   often (when allocating the pointer-to-pointer structure by hand,
//   one has little choice but to use these), the same struct
//   recursion defined PointerArray<T,R> as T****, i.e.,
// 
//      PointerArray<T,1>::noconst_type = T* 
//      PointerArray<T,2>::noconst_type = T**
//      PointerArray<T,3>::noconst_type = T***
//      ...
//
// Recursive definition of PointerArray<T,R> in terms of PointerArray<T,R-1>:
template<typename T,int R> 
struct PointerArray {
    typedef typename PointerArray<T,R-1>::type const*    type;     // const shape
    typedef typename PointerArray<T,R-1>::noconst_type*  noconst_type;// non-const variant
};
// Now we end the recursion by specifically defining the R=1 case:
template<typename T> 
struct PointerArray<T,1> {
    typedef T* type;         // note there is not const: a const
                             // here would express that the elements
                             // of the multidimensional arrays are
                             // constant, not that its shape is
                             // constant. We do want to be able to
                             // have multidimensional arrays of
                             // constant shape but with modifiable
                             // elements, so const here would not be
                             // appropriate. Finally note that one
                             // can express non-modifiable element
                             // using PointerArray<const T,R>.
    typedef T* noconst_type; // There would never have been a const here
};

////////  END DEFINITION struct PointerArray<T,R>  ////////


////////  DEFINITION struct unconst<T>  ////////

// Define a struct to strip constness from a type:
// Generically, do not strip a const:
template<typename T> 
struct Unconst { 
    typedef T type; // non-stripping, intended for typed without const
};
// Override this non-const-stripped type with a stripped one using
// the following instantiation which applies for types with const only.
template<typename T> 
struct Unconst<const T> {  
    typedef T type; // note that the const is gone
};

////////  END DEFINITION struct Unconst<T>  ////////

// forward definition of the class for intermediate bracket expressions:

template<typename T,int R> class rarray_intermediate;

// function to get some automatic pointer conversion working with templates:

template<typename T> inline T* get_pointer(T*t);

////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////                                                ////
////               I N T E R F A C E                ////
////                                                ////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


////////  DEFINITION class rarray<T,R>  ////////

template<typename T,int R> 
class rarray {

  public:

    // associated types
    typedef T*        iterator;
    typedef const T*  const_iterator;
    typedef int       difference_type;
    typedef int       size_type;

    // chained pointer types:
    typedef typename PointerArray<T,R>::type          parray_t;
    typedef typename PointerArray<T,R>::noconst_type  noconst_parray_t;

    // constructors 
    rarray(); // uninitialized

    // creating their own buffer:
    rarray(int n0, int n1);  // for R=2
    rarray(int n0, int n1, int n2);  // for R=3
    rarray(int n0, int n1, int n2, int n3);  // for R=4
    rarray(int n0, int n1, int n2, int n3, int n4);  // for R=5
    rarray(int n0, int n1, int n2, int n3, int n4, int n5);  // for R=6
    rarray(const int* dim);  // for any R (the only way for R>6)

    // constructors from an existing buffer
    rarray(T* buffer, int n0, int n1);  // for R=2
    rarray(T* buffer, int n0, int n1, int n2);  // for R=3
    rarray(T* buffer, int n0, int n1, int n2, int n3);  // for R=4
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4);  // for R=5
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5);  // for R=6
    rarray(T* buffer, const int* dim);  // for any R (the only way for R>6)

    // destructor:
    ~rarray();

    // make undefined
    void free();

    // return a copy:
    rarray<T,R> copy() const;

    // retrieve array size in dimension i:
    int extent(int i) const;

    // retrieve array sizes in all dimensions:
    const int* extents() const;

    // retrieve the total number of elements:
    int size() const;

    // return T* to the internal data:
    T* data();
    const T* data() const;

    // return T*const*.. acting similarly to this rarray when using []:
    inline parray_t ptr() const; // as this is really just a cast, keep const

    // return  T**.. acting similarly to this rarray when using []:
    noconst_parray_t cptr() const;  // as this is really just a cast, keep const

    // create a reference to this that treats elements as constant:
    rarray<const T,R>&  cref() const;

    // access elements 
   #ifndef SKIPINTERMEDIATE
    // through intermediate object:
    inline rarray_intermediate<T,R-1> operator[](int i);
    inline rarray_intermediate<const T,R-1> operator[](int i) const;
   #else
    // or if SKIPINTERMEDIATE is set, by implicit case to a T**..:
    inline operator typename PointerArray<T,R>::type (); 
    inline operator typename PointerArray<const T,R>::type () const; 
   #endif

    // copy constructors:
    rarray(const rarray<T,R> &a);
    rarray(const rarray_intermediate<T,R> &a);

    // assignment operators:
    rarray<T,R>& operator=(const rarray<T,R> &a); 
    rarray<T,R>& operator=(const rarray_intermediate<T,R> &a);

  private:

    parray_t  parray_;        // start of the array of pointers
    T*        origin_;        // the original contiguous buffer
    char**    parrayorigin_;  // original start of the array of pointers
    int*      rcount_;        // reference count for the array
    bool      manage_;        // does the container own the data buffer?
    int       extent_[R];     // array of number of elements in each dimension
    int       volume_;        // total number of data buffer elements
    //T*        buffer_;        // start of current contiguous data buffer
    T* buffer_() const { return base(parray_); }

    // store dimension information and compute volume_:
    void setn(const int* dim); 

    // setup new rarray object:
    void init_shallow (T*const& origin, int* rcount, T* buffer, const int* dim, char** parrayorigin, parray_t parray);  
    void init_parray(T*const& origin, T* buffer, const int* dim);
    void init_data(const int* dim, int dimtot);

    // cleanup routine:
    void fini();

    // allocate the chain of pointers, except the base:
    static parray_t new_except_base(T* buffer, const int* dim, char**& parrayorigin);

    // find base of a chain of pointers:
    static T* base(parray_t parray);

    friend class rarray_intermediate<T,R>;

}; // end definition rarray<T,R>

// Define rarray<T,1> to stop the recursion in operator[]:

template<typename T> 
class rarray<T,1> {

  public:

    // associated types
    typedef T*        iterator;
    typedef const T*  const_iterator;
    typedef int       difference_type;
    typedef int       size_type;

    // chained pointer types:
    typedef typename PointerArray<T,1>::type          parray_t;
    typedef typename PointerArray<T,1>::noconst_type  noconst_parray_t;

    // constructors:
    rarray(); // uninitialized

    // creating their own buffer:
    rarray(int n0);
    rarray(const int* dim);  // for conformity

    // from an existing buffer:
    rarray(T* buffer, int n0);
    rarray(T* buffer, const int* dim);  // for conformity

    // destructor:
    ~rarray();

    // make undefined
    void free();

    // return a copy:
    rarray<T,1> copy() const;

    // retrieve array size in dimension i:
    int extent(int i) const ;

    // retrieve array sizes in all dimensions:
    const int* extents() const ;

    // retrieve the total number of elements:
    int size() const;

    // return T* to the internal data:
    T* data();
    const T* data() const;

    // return T*const*.. acting similarly to this rarray when using []:
    inline parray_t ptr() const; // as this is really just a cast, keep const

    // return  T**.. acting similarly to this rarray when using []:
    noconst_parray_t cptr() const;  // as this is really just a cast, keep const

    // create a reference to this that treats elements as constant:
    rarray<const T,1>&  cref() const;

    // access elements through intermediate object:
   #ifndef SKIPINTERMEDIATE
    // through a T& pointer a the element:
    inline T& operator[](int i);
    inline const T& operator[](int i) const;
   #else
    // through implicit conversion to a pointer:
    inline operator typename PointerArray<T,1>::type ();
    inline operator typename PointerArray<const T,1>::type () const;
   #endif

    // copy constructors:
    rarray(const rarray<T,1> &a);
    rarray(const rarray_intermediate<T,1> &a);

    // assignment operators:
    rarray<T,1>& operator=(const rarray<T,1> &a);
    rarray<T,1>& operator=(const rarray_intermediate<T,1> &a) ;

  private:

    parray_t  parray_;        // start of the pointer-chain
    T*        origin_;        // the original contiguous data buffer
    char**    parrayorigin_;  // original start of the pointer-chain
    int*      rcount_;        // reference count for the original buffer
    bool      manage_;        // does the container own the data buffer?
    int       extent_[1];     // array of number of elements in each dimension
    int       volume_;        // total number of buffer elements
    //T*        buffer_;        // start of current contiguous buffer
    T* buffer_() const { return base(parray_); }

    // store dimension information and compute volume_:
    void setn(const int* dim); 

    // setup new rarray object:
    void init_shallow (T*const& origin, int* rcount, T* buffer, const int* dim, char** parrayorigin, parray_t parray);  
    void init_parray(T*const& origin, T* buffer, const int* dim);
    void init_data(const int* dim, int dimtot);

    // cleanup routine:
    void fini() ;

    // allocate the chain of pointers, except the base:
    static parray_t new_except_base(T* buffer, const int* dim, char**& parrayorigin);

    // find base of a chain of pointers:
    static T* base(parray_t parray) ;

    friend class rarray_intermediate<T,1>;

}; // end definition rarray<T,1>

////////  END DEFINITION class rarray<T,R>  ////////

//////////////////////////////////////////////////////////////////////////////

////////  DEFINITION class rarray_intermediate <T,R>  ////////

// rarray_intermediate class: interface like rarray without assignment. Really
// only a reference to one, used in intermediate expressions produced
// by square brackets.

template<typename T,int R> 
class rarray_intermediate {

  public:

    // associated types
    typedef T*        iterator;
    typedef const T*  const_iterator;
    typedef int       difference_type;
    typedef int       size_type;

    // chained pointer types:
    typedef typename PointerArray<T,R>::type          parray_t;
    typedef typename PointerArray<T,R>::noconst_type  noconst_parray_t;

    // retrieve array size in dimension i:
    inline int extent(int i) const;

    // retrieve array sizes in all dimensions:
    const int* extents() const; 

    // retrieve the total number of elements:
    int size() const;

    // return T* to the internal pointer to the data:
    T* data();

    // return T*const*.. acting similarly to this rarray when using []:
    inline parray_t ptr() const; // as this is really just a cast, keep const

    // return T**.. acting similarly to this rarray when using []:
    noconst_parray_t cptr() const; // as this is really just a cast, keep const

    // create a reference to this that treats elements as constant:
    rarray_intermediate<const T,R>& cref() const; 

    // element access:
    inline rarray_intermediate<T,R-1> operator[](int i); 

  protected:

    parray_t   const   parray_;        // start of the pointer-chain
    T*         const&  origin_;        // points to original contiguous buffer
    char**     const   parrayorigin_;  // original start of the pointer-chain
    int*       const&  rcount_;        // rcount_ of original contiguous buffer
    bool       const   manage_;        // does the container own the data/buffer
    const int* const   extent_;        // number of elements in each dimension

    // constructor:
    inline rarray_intermediate(T*const&     origin, 
                               int*const&   rcount, 
                               const int*   dim, 
                               parray_t     parray,
                               char** const parrayorigin, 
                               bool         manage);

    // can be called only by:
    friend class rarray<T,R>; 
    friend class rarray<T,R+1>;
    friend class rarray_intermediate<T,R+1>;
    friend class rarray<typename Unconst<T>::type,R+1>;

}; // end definition rarray_intermediate<T,R>

// Define rarray_intermediate<T,1> to stop the recursion in operator[]:

template<typename T> class rarray_intermediate<T,1> {

  public:

    // associated types
    typedef T*        iterator;
    typedef const T*  const_iterator;
    typedef int       difference_type;
    typedef int       size_type;

    // chained pointer types:
    typedef typename PointerArray<T,1>::type          parray_t;
    typedef typename PointerArray<T,1>::noconst_type  noconst_parray_t;

    // retrieve array size in dimension i:
    int extent(int i) const;

    // retrieve array sizes in all dimensions:
    const int* extents() const; 

    // retrieve the total number of elements:
    int size() const; 

    // return T* to the internal pointer to the data:
    T* data();

    // return T*const*.. acting similarly to this rarray when using []:
    inline parray_t ptr() const; // as this is really just a cast, keep const

    // return T**.. acting similarly to this rarray when using []:
    noconst_parray_t cptr() const;  // as this is really just a cast, keep const

    // create a reference to this that treats elements as constant:
    rarray_intermediate<const T,1>& cref() const;

    // element access:
    inline T& operator[](int i);

  protected:

    parray_t   const   parray_;        // start of the pointer-chain
    T*         const&  origin_;        // points to original contiguous buffer
    char**     const   parrayorigin_;  // original start of the pointer-chain
    int*       const&  rcount_;        // reference count of rarray data
    bool       const   manage_;        // does the container own the data/buffer
    const int* const   extent_;        // number of elements in each dimension

    // constructor:
    inline rarray_intermediate(T*const&     origin, 
                               int*const&   rcount,
                               const int*   dim, 
                               parray_t     parray, 
                               char** const parrayorigin, 
                               bool         manage);

    // can only be used by:
    friend class rarray<T,1>; 
    friend class rarray<T,2>;
    friend class rarray_intermediate<T,2>;
    friend class rarray<typename Unconst<T>::type,2>;

};  // end of class template definition of rarray_intermediate<T,1>.

////////  END DEFINITION class rarray_intermediate <T,R>  ////////


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////                                                ////
////          I M P L E M E N T A T I O N           ////
////                                                ////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


#ifdef TRACETEST
  
    #include <iostream>

    #define profileSay(a) std::cerr << "PROFILE " << __FILE__ << '@' << __LINE__<< ":\t" << a << std::endl;

#else

    #define profileSay(a) 

#endif

#ifdef BOUNDSCHECK

    #include <stdexcept>

    #define checkOrSay(a, b) if (not(a)) throw std::out_of_range(b)

    // Boundschecking needs to switch off SKIPINTERMEDIATE if it
    // was set lest only the first dimension gets checked.

    #ifdef SKIPINTERMEDIATE
        #undef SKIPINTERMEDIATE
    #endif

#else

    #define checkOrSay(a, b) 

#endif

#if __cplusplus <= 199711L

    #define nullptr 0 

#endif

////////////////////////////////////////////////////////////////////////////

// rarray<T,R> and rarray_intermediate<T,R> constructors:

// ones creating their own buffer:

// constructor for an undefined array:
template<typename T, int R>
rarray<T,R>::rarray() 
  : parray_(nullptr), 
    origin_(nullptr), 
    parrayorigin_(nullptr), 
    rcount_(nullptr), 
    manage_(false)//, buffer_(nullptr)
{
    profileSay("rarray<T,R>::rarray()");
}

// R=1 case of the same
template<typename T>
rarray<T,1>::rarray() 
  : origin_(nullptr), 
    parray_(nullptr), 
    parrayorigin_(nullptr), 
    rcount_(nullptr),
    manage_(false) //, buffer_(nullptr)
{
    profileSay("rarray<T,1>::rarray()");
}

// Constructors that allocate memory

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1)  // for R=2
{
    profileSay("rarray<T,R>::rarray(int n0,int n1)");
    checkOrSay(R==2, "wrong rank in constructor");
    const int dim[] = {n0,n1};
    init_data(dim, n0*n1);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2)  // for R=3
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2)");
    checkOrSay(R==3, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2};
    init_data(dim, n0*n1*n2);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2, int n3)  // for R=4
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2,int n3)");
    checkOrSay(R==4, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3};
    init_data(dim, n0*n1*n2*n3);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4)  // for R=5
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2,int n3,int n4)");
    checkOrSay(R==5, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4};
    init_data(dim, n0*n1*n2*n3*n4);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5)  // for R=6
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2,int n3,int n4,int n5)");
    checkOrSay(R==6, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4,n5};
    init_data(dim, n0*n1*n2*n3*n4*n5);
}

template<typename T,int R>
rarray<T,R>::rarray(const int* dim)  // for any R (the only way for R>6)
{
    profileSay("rarray<T,R>::rarray(const int*dim)");
    int dimtot = 1;
    for (int i=0; i<R; i++)
        dimtot*=dim[i];
    init_data(dim, dimtot);
}

template<typename T>
rarray<T,1>::rarray(int n0)
{
    profileSay("rarray<T,1>::rarray(int n0)");
    init_data(&n0, n0);
}

template<typename T>
rarray<T,1>::rarray(const int* dim)  // for uniformity
{
    profileSay("rarray<T,1>::rarray(const int*dim)");
    init_data(dim, dim[0]);
}

// from existing buffers:

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, int n0, int n1)  // for R=2
{
    profileSay("rarray<T,R>::rarray(T*buffer,int n0,int n1)");
    checkOrSay(R==2, "wrong rank in constructor");
    const int dim[] = {n0,n1};
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer,int n0,int n1,int n2)  // for R=3
{
    profileSay("rarray<T,R>::rarray(T*buffer,int n0,int n1,int n2)");
    checkOrSay(R==3, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2};
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer,int n0,int n1,int n2,int n3)  // for R=4
{
    profileSay("rarray<T,R>::rarray(T*buffer,int n0,int n1,int n2,int n3)");
    checkOrSay(R==4, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3};
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer,int n0,int n1,int n2,int n3,int n4)  // for R=5
{
    profileSay("rarray<T,R>::rarray(T*buffer,int n0,int n1,int n2,int n3,int n4)");
    checkOrSay(R==5, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4};
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer,int n0,int n1,int n2,int n3,int n4,int n5)  // for R=6
{
    profileSay("rarray<T,R>::rarray(T*buffer,int n0,int n1,int n2,int n3,int n4,int n5)");
    checkOrSay(R==6, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4,n5};
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, const int* dim)  // for any I (the only way for R>6)
{
    profileSay("rarray<T,R>::rarray(T*buffer,const int*dim)");
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T>
rarray<T,1>::rarray(T* buffer, int n0)
{
    profileSay("rarray<T,1>::rarray(T*buffer,int n0)");
    const int dim[] = {n0};
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T>
rarray<T,1>::rarray(T* buffer, const int* dim)  // for any I (the only way for R>6)
{
    profileSay("rarray<T,1>::rarray(T*buffer,const int*dim)");
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = false;
}

template<typename T,int R> inline 
rarray_intermediate<T,R>::rarray_intermediate(T*const&    origin, 
                                              int*const&  rcount,
                                              const int*  dim, 
                                              parray_t    parray,
                                              char**const parrayorigin, 
                                              bool        manage)
  : parray_(parray),
    origin_(origin), 
    parrayorigin_(parrayorigin),
    rcount_(rcount), 
    manage_(manage),
    extent_(dim)
{
    profileSay("rarray_intermediate<T,R>::rarray_intermediate(T*const&origin,int*const&rcount,const int*n,parray_t parray,char**const parrayorigin,bool manage)");
}

template<typename T> inline 
rarray_intermediate<T,1>::rarray_intermediate(T*const&    origin, 
                                              int*const&  rcount,
                                              const int*  dim, 
                                              parray_t    parray,
                                              char**const parrayorigin, 
                                              bool        manage)
  : parray_(parray),
    origin_(origin), 
    parrayorigin_(parrayorigin),
    rcount_(rcount), 
    manage_(manage),
    extent_(dim)
{
    profileSay("rarray_intermediate<T,1>::rarray_intermediate(T*const&origin_,int*const&rcount,const int*n,parray_t parray,char**const parrayorigin,bool manage)");
}

// rarray<T,R> destructor (rarray_intermediate has none)

template<typename T,int R>
rarray<T,R>::~rarray()
{
    profileSay("rarray<T,R>::~rarray()");
    fini();
}

template<typename T>
rarray<T,1>::~rarray()
{
    profileSay("rarray<T,1>::~rarray()");
    fini();
}

// public cleanup routine
template<typename T,int R>
void rarray<T,R>::free()
{
    profileSay("void rarray<T,R>::free()");
    fini();
}

template<typename T>
void rarray<T,1>::free()
{
    profileSay("void rarray<T,1>::free()");
    fini();
}

// rarray<T,R> and rarray_intermediate<T,R> copy constructors

template<typename T,int R>
rarray<T,R>::rarray(const rarray<T,R> &a) 
{
    profileSay("rarray<T,R>::rarray(const rarray<T,R>&a)");
    init_shallow(a.origin_, a.rcount_, a.buffer_(), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
    manage_ = a.manage_;
}

template<typename T,int R>
rarray<T,R>::rarray(const rarray_intermediate<T,R> &a) 
{
    profileSay("rarray<T,R>::rarray(const rarray_intermediate<T,R>&a)");
    init_shallow(a.origin_, a.rcount_, base(a.parray_), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
    manage_ = a.manage_;
}

template<typename T>
rarray<T,1>::rarray(const rarray<T,1> &a) 
{
    profileSay("rarray<T,1>::rarray(const rarray<T,1>&a)"); 
    init_shallow(a.origin_, a.rcount_, a.buffer_(), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
    manage_ = a.manage_;
}

template<typename T>
rarray<T,1>::rarray(const rarray_intermediate<T,1> &a) 
{
    profileSay("rarray<T,1>::rarray(const rarray_intermediate<T,1>&a)");
    init_shallow(a.origin_, a.rcount_, base(a.parray_), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
    manage_ = a.manage_;
}

// create a copy

template<typename T, int R>
rarray<T,R> rarray<T,R>::copy() const
{
    profileSay("rarray<T,R> rarray<T,R>::copy() const");
    if (origin_ != nullptr) { 
        // if initialized
        rarray<T,R> result(extent_);
        std::copy(buffer_(), buffer_()+volume_, result.buffer_());
        return result;
    } else {
        // else return uninitialized coy
        return rarray();
    }
}

template<typename T>
rarray<T,1> rarray<T,1>::copy() const 
{
    profileSay("rarray<T,1> rarray<T,1>::copy() const");
    if (origin_ != nullptr) { // if initialized
        //copy    
        rarray<T,1> result(extent_);
        std::copy(buffer_(), buffer_()+volume_, result.buffer_());
        return result;
    } else {
        // else return uninitialized coy
        return rarray();
    }
}
// rarray method to retrieve array size in dimension i

template<typename T,int R>
int rarray<T,R>::extent(int i) const 
{
    profileSay("int rarray<T,R>::extent(int i) const");
    checkOrSay(i >=0 and i < R, "wrong dimension");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return extent_[i];
}

template<typename T>
int rarray<T,1>::extent(int i) const 
{
    profileSay("int rarray<T,1>::extent(int i) const");
    checkOrSay(i >=0 and i < 1, "wrong dimension");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return extent_[i];
}

template<typename T,int R> inline 
int rarray_intermediate<T,R>::extent(int i) const 
{
    profileSay("int rarray_intermediate<T,R>::extent(int i) const");
    checkOrSay(i >=0 and i < R, "wrong dimension");  
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");      
    return extent_[i];
}

template<typename T> inline 
int rarray_intermediate<T,1>::extent(int i) const 
{
    profileSay("int rarray_intermediate<T,1>::extent(int i) const");
    checkOrSay(i >=0 and i < 1, "wrong dimension");   
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");     
    return extent_[i];
}

// rarray<T,R> and rarray_intermediate<T,R> methods to retrieve array sizes in
// all dimensions

template<typename T,int R>
const int* rarray<T,R>::extents() const 
{
    profileSay("const int* rarray<T,R>::extents() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return extent_;
}

template<typename T>
const int* rarray<T,1>::extents() const 
{
    profileSay("const int* rarray<T,1>::extents() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return extent_;
}

// ...for rarray_intermediate

template<typename T,int R> 
const int* rarray_intermediate<T,R>::extents() const 
{
    profileSay("const int* rarray_intermediate<T,R>::extents() const");
    return extent_;
}

template<typename T> 
const int* rarray_intermediate<T,1>::extents() const 
{
    profileSay("const int* rarray_intermediate<T,1>::extents() const");
    return extent_;
}

// rarray method to retrieve the total number of elements

template<typename T,int R>
int rarray<T,R>::size() const 
{ 
    profileSay("int rarray<T,R>::size() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return volume_;
}

template<typename T>
int rarray<T,1>::size() const 
{ 
    profileSay("int rarray<T,1>::size() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return volume_;
}

// ...for rarray_intermediate

template<typename T,int R> 
int rarray_intermediate<T,R>::size() const 
{
    profileSay("int rarray_intermediate<T,R>::size() const");
    // multiplying the sizes in all dimensions
    int dimtot = 1;       
    for (int i=0; i<R; i++)
        dimtot *= extent_[i];
    return dimtot;
}

template<typename T> 
int rarray_intermediate<T,1>::size() const 
{ 
    profileSay("int rarray_intermediate<T,1>::size() const");
    // multiplying the sizes in all dimensions
    int dimtot = 1;        
    for (int i=0; i<1; i++)
        dimtot *= extent_[i];
    return dimtot;
}

// rarray method to return pointer of type T* to the internal data

template<typename T,int R>
T* rarray<T,R>::data()
{
    profileSay("T* rarray<T,R>::data()");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return buffer_();
}

template<typename T,int R>
const T* rarray<T,R>::data() const 
{
    profileSay("const T* rarray<T,R>::data() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return buffer_();
}

template<typename T>
T* rarray<T,1>::data()
{
    profileSay("T* rarray<T,1>::data()");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return buffer_();
}

template<typename T>
const T* rarray<T,1>::data() const 
{
    profileSay("const T* rarray<T,1>::data() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return buffer_();
}

// ...for rarray_intermediate

template<typename T,int R> 
T* rarray_intermediate<T,R>::data()
{
    profileSay("T* rarray_intermediate<T,R>::data()");
    return rarray<T,R>::base(parray_);
}

template<typename T> 
T* rarray_intermediate<T,1>::data()
{
    profileSay("T* rarray_intermediate<T,1>::data()");
    return rarray<T,1>::base(parray_);
}

// rarray method to return T*const*.. acting similarly to this rarray
// when using []:

template<typename T,int R> inline 
typename rarray<T,R>::parray_t 
rarray<T,R>::ptr() const 
{
    profileSay("rarray<T,R>::parray_t rarray<T,R>::ptr() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}

template<typename T> inline 
typename rarray<T,1>::parray_t 
rarray<T,1>::ptr() const 
{
    profileSay("rarray<T,1>::parray_t rarray<T,1>::ptr() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}

// ...for rarray_intermediate

template<typename T,int R> inline 
typename rarray_intermediate<T,R>::parray_t 
rarray_intermediate<T,R>::ptr() const 
{
    profileSay("rarray_intermediate<T,R>::parray_t rarray_intermediate<T,R>::ptr() const");
    return parray_;
}
    
template<typename T> inline 
typename rarray_intermediate<T,1>::parray_t 
rarray_intermediate<T,1>::ptr() const 
{
    profileSay("rarray_intermediate<T,1>::parray_t rarray_intermediate<T,1>::ptr() const");
    return parray_;
}

// rarray method to return  T**.. acting similarly to this rarray when using []

template<typename T,int R>
typename rarray<T,R>::noconst_parray_t 
rarray<T,R>::cptr() const 
{
    profileSay("rarray<T,R>::noconst_parray_t rarray<T,R>::cptr() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return const_cast<noconst_parray_t>(parray_);
}

template<typename T>
typename rarray<T,1>::noconst_parray_t 
rarray<T,1>::cptr() const 
{
    profileSay("rarray<T,1>::noconst_parray_t rarray<T,1>::cptr() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return const_cast<noconst_parray_t>(parray_);
}

// ... for rarray_intermediate

template<typename T,int R> 
typename rarray_intermediate<T,R>::noconst_parray_t 
rarray_intermediate<T,R>::cptr() const 
{
    profileSay("rarray_intermediate<T,R>::noconst_parray_t rarray_intermediate<T,R>::cptr() const");
    return const_cast<noconst_parray_t>(parray_);
}

template<typename T> 
typename rarray_intermediate<T,1>::noconst_parray_t 
rarray_intermediate<T,1>::cptr() const 
{
    profileSay("rarray_intermediate<T,1>::noconst_parray_t rarray_intermediate<T,1>::cptr() const");
    return const_cast<noconst_parray_t>(parray_);
}

// rarray method to create a reference to this that treats elements as constant

template<typename T,int R>
rarray<const T,R>&  
rarray<T,R>::cref() const 
{
    profileSay("rarray<const T,R>& rarray<T,R>::cref() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return (rarray<const T,R>&)(*this);
}

template<typename T> 
rarray<const T,1>&  
rarray<T,1>::cref() const 
{
    profileSay("rarray<const T,1>& rarray<T,1>::cref() const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return (rarray<const T,1>&)(*this);
}

// ...for rarray_intermediate

template<typename T,int R> inline
rarray_intermediate<const T,R>& 
rarray_intermediate<T,R>::cref() const
{
    profileSay("rarray_intermediate<const T,R>& rarray_intermediate<T,R>::cref() const");
    return (rarray_intermediate<const T,R>&)(*this);
}

template<typename T> inline
rarray_intermediate<const T,1>& 
rarray_intermediate<T,1>::cref() const
{
    profileSay("rarray_intermediate<const T,1>& rarray_intermediate<T,1>::cref() const");
    return (rarray_intermediate<const T,1>&)(*this);
}

#ifndef SKIPINTERMEDIATE
template<typename T,int R> inline 
rarray_intermediate<const T,R-1> 
rarray<T,R>::operator[](int i) const
{
    profileSay("rarray_intermediate<const T,R-1> rarray<T,R>::operator[](int i) const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return rarray_intermediate<const T,R-1>(origin_, rcount_, extent_+1, parray_[i], parrayorigin_, manage_);
}
template<typename T,int R> inline 
rarray_intermediate<T,R-1> 
rarray<T,R>::operator[](int i)
{
    profileSay("rarray_intermediate<T,R-1> rarray<T,R>::operator[](int i)");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return rarray_intermediate<T,R-1>(origin_, rcount_, extent_+1, parray_[i], parrayorigin_, manage_);
}
#else
template<typename T,int R> inline 
rarray<T,R>::operator typename PointerArray<const T,R>::type () const 
{
    profileSay("rarray<T,R>::operator typename PointerArray<const T,R>::parray_t () const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
template<typename T,int R> inline 
rarray<T,R>::operator typename PointerArray<T,R>::type ()
{
    profileSay("rarray<T,R>::operator typename PointerArray<T,R>::type ()");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
#endif

#ifndef SKIPINTERMEDIATE
template<typename T> inline 
const T& rarray<T,1>::operator[](int i) const 
{
    profileSay("const T& rarray<T,1>::operator[](int i) const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}
template<typename T> inline 
T& rarray<T,1>::operator[](int i)
{
    profileSay("T& rarray<T,1>::operator[](int i)");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}
#else
template<typename T> inline 
rarray<T,1>::operator typename PointerArray<const T,1>::type () const 
{
    profileSay("rarray<T,1>::operator PointerArray<const T,1>::type () const");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
template<typename T> inline 
rarray<T,1>::operator typename PointerArray<T,1>::type ()
{
    profileSay("rarray<T,1>::operator typename PointerArray<T,1>::type ()");
    checkOrSay(origin_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
#endif

// rarray_intermediate methods for element access

template<typename T,int R> inline
rarray_intermediate<T,R-1> 
rarray_intermediate<T,R>::operator[](int i) 
{
    profileSay("rarray_intermediate<T,R-1> rarray_intermediate<T,R>::operator[](int i)");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");        
    return rarray_intermediate<T,R-1>(origin_, rcount_, extent_+1, parray_[i],parrayorigin_,manage_);
}

template<typename T> inline
T& rarray_intermediate<T,1>::operator[](int i) 
{
    profileSay("T& rarray_intermediate<T,1>::operator[](int i)");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}

// rarray method to assignment operators

template<typename T,int R>
rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R> &a) 
{
    profileSay("rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R>&a)");
    if (&a != this) {
        fini();
        init_shallow(a.origin_, a.rcount_, a.buffer_(), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
        manage_ = a.manage_;
    }
    return *this;
}

template<typename T>
rarray<T,1>& rarray<T,1>::operator=(const rarray<T,1> &a) 
{
    profileSay("rarray<T,1>& rarray<T,1>::operator=(const rarray<T,1>&a)");
    if (&a != this) {
        fini();
        init_shallow(a.origin_, a.rcount_, a.buffer_(), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
        manage_ = a.manage_;
    }
    return *this;
}

template<typename T,int R>
rarray<T,R>& rarray<T,R>::operator=(const rarray_intermediate<T,R> &a) 
{
    profileSay("rarray<T,R>& rarray<T,R>::operator=(const rarray_intermediate<T,R>&a)");
    fini();
    init_shallow(a.origin_, a.rcount_, base(a.parray_), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
    manage_ = a.manage_;
    return *this;
}

template<typename T>
rarray<T,1>& rarray<T,1>::operator=(const rarray_intermediate<T,1> &a) 
{
    profileSay("rarray<T,1>& rarray<T,1>::operator=(const rarray_intermediate<T,1>&a)");
    fini();
    init_shallow(a.origin_, a.rcount_, base(a.parray_), a.extent_, const_cast<char**>(a.parrayorigin_), a.parray_);
    manage_ = a.manage_;
    return *this;
}

// rarray private method to store dimension information and compute volume_

template<typename T,int R>
void rarray<T,R>::setn(const int* dim)
{
    profileSay("void rarray<T,R>::setn(const int*dim)");
    volume_ = 1;
    for (int i=0;i<R;i++) {
        extent_[i] = dim[i];
        volume_ *= dim[i];
    }   
}

template<typename T>
void rarray<T,1>::setn(const int* dim)
{
    profileSay("void rarray<T,1>::setn(const int*dim)");
    volume_ = 1;
    for (int i=0;i<1;i++) {
        extent_[i] = dim[i];
        volume_   *= dim[i];
    }   
}

// init functions 

template<typename T,int R>
void rarray<T,R>::init_shallow(T* const &  origin, 
                               int*        rcount, 
                               T*          buffer, 
                               const int*  dim,
                               char**      parrayorigin,
                               parray_t    parray)
{
    profileSay("void rarray<T,R>::init_shallow(T*const&origin,int*rcount,T*buffer,const int*dim,char**parrayorig,parray_t b)");
    checkOrSay(      origin != nullptr, "null pointer");
    checkOrSay(      buffer != nullptr, "null pointer");
    checkOrSay(         dim != nullptr, "null pointer");
    checkOrSay(parrayorigin != nullptr, "null pointer");
    checkOrSay(      parray != nullptr, "null pointer");
    rcount_ = rcount;
    origin_ = origin;
    //buffer_ = buffer;
    parray_ = parray;
    parrayorigin_ = parrayorigin;
    setn(dim);
    (*rcount_)++;
}

template<typename T,int R>
void rarray<T,R>::init_parray(T* const &  origin, 
                                T*          buffer, 
                                const int*  dim)
{
    profileSay("void rarray<T,R>::init_parray(T*const&orig,int*origrcount_,T*a,const int*dim)");
    checkOrSay(origin != nullptr, "null pointer");
    checkOrSay(buffer != nullptr, "null pointer");
    checkOrSay(   dim != nullptr, "null pointer");
    char** parrayorigin;
    parray_t parray = new_except_base(buffer, dim, parrayorigin);
    init_shallow(origin, new int(0), buffer, dim, parrayorigin, parray);
}

template<typename T,int R>
void rarray<T,R>::init_data(const int* dim, int dimtot)
{
    profileSay("void rarray<T,R>::init_data(const int*dim,int dimtot)");
    checkOrSay( dim != nullptr, "null pointer");
    checkOrSay( dimtot >= 0, "negative number of elements");
    T* buffer = new T[dimtot];
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = true;
}

template<typename T>
void rarray<T,1>::init_shallow(T* const &  origin, 
                               int*        rcount, 
                               T*          buffer, 
                               const int*  dim,
                               char**      parrayorigin,
                               parray_t  parray)
{
    profileSay("void rarray<T,1>::init_shallow(T*const&origin,int*rcount_,T*buffer,const int*dim,char**parrayorigin,parray_t parray)");
    checkOrSay(        origin != nullptr, "null pointer");
    checkOrSay(        buffer != nullptr, "null pointer");
    checkOrSay(           dim != nullptr, "null pointer");
    checkOrSay(parrayorigin != nullptr, "null pointer");
    checkOrSay(      parray != nullptr, "null pointer");
    rcount_ = rcount;
    origin_ = origin;
    //buffer_ = buffer;
    parray_ = parray;
    parrayorigin_ = parrayorigin;
    setn(dim);
    (*rcount_)++;
}

template<typename T>
void rarray<T,1>::init_parray(T* const &  origin, 
                                T*          buffer, 
                                const int*  dim)
{
    profileSay("void rarray<T,1>::init_parray(T*const&origin,T*buffer,const int*dim)");
    checkOrSay(origin != nullptr, "null pointer");
    checkOrSay(buffer != nullptr, "null pointer");
    checkOrSay(   dim != nullptr, "null pointer");
    char** parrayorigin;
    parray_t parray = new_except_base(buffer, dim, parrayorigin);
    init_shallow(origin, new int(0), buffer, dim, parrayorigin, parray);
}

template<typename T>
void rarray<T,1>::init_data(const int* dim, int dimtot)
{
    profileSay("void rarray<T,1>::init_data(const int*dim,int dimtot)");
    checkOrSay( dim != nullptr, "null pointer");
    checkOrSay( dimtot >= 0, "negative number of elements");
    T* buffer = new T[dimtot];
    init_parray(buffer, get_pointer(buffer), dim);
    manage_ = true;
}

//  rarray private cleanup routine

template<typename T,int R>
void rarray<T,R>::fini() 
{
    profileSay("void rarray<T,R>::fini()");
    if (origin_!=nullptr) {
        (*rcount_)--;
        if (*rcount_==0) {
            delete[] parrayorigin_;
            if (manage_) delete[] origin_;
            delete rcount_;
        }
    }
    rcount_ = nullptr;
    origin_ = nullptr;
    //buffer_ = nullptr;
}

template<typename T>
void rarray<T,1>::fini() 
{
    profileSay("void rarray<T,1>::fini()");
    if (origin_!=nullptr) {
        (*rcount_)--;
        if (*rcount_==0) {
            if (manage_) delete[] origin_;
            delete rcount_;
        }
    }
    rcount_ = nullptr;
    origin_ = nullptr;
    //buffer_ = nullptr;
}

// rarray private method to allocate the chain of pointers, except the base

template<typename T,int R>
typename rarray<T,R>::parray_t 
rarray<T,R>::new_except_base(T* buffer, const int* dim, char**& parrayorigin) 
{
    profileSay("rarray<T,R>::parray_t rarray<T,R>::new_except_base(T* buffer,const int*dim,char**&parrayorigin)");

    if (R>1) {

        typedef typename Unconst<T>::type* bufstart_t;
        
        bufstart_t bufstart = const_cast<bufstart_t>(buffer);
        int        nalloc   = 0;
        char**     result;
        
        for (int i=R-1; i--; )
            nalloc = dim[i]*(1+nalloc);
        
        char**  palloc = new char*[nalloc];        
        int     volume_   = 1;
        char*** ptr    = reinterpret_cast<char***>(&result);
        
        for (int i=0; i<R-1; i++) {
            
            for (int j=0; j<volume_; j++)
                ptr[j] = palloc + j*dim[i];
            
            volume_   *= dim[i];
            if (volume_==0) {
                break;
            } else {
                ptr     = reinterpret_cast<char***>(*ptr);
                palloc += volume_;
            }
        }
        
        for (int j=0; j<volume_; j++)
            ptr[j] = reinterpret_cast<char**>(bufstart + j*dim[R-1]);
        
        parrayorigin = result;
        return reinterpret_cast<parray_t>(result);
        
    } else {
        
        parrayorigin = reinterpret_cast<char**>(buffer);
        return reinterpret_cast<parray_t>(buffer);
        
    }
}

template<typename T>
typename rarray<T,1>::parray_t 
rarray<T,1>::new_except_base(T* buffer, const int* dim, char**& parrayorigin) 
{
    profileSay("rarray<T,1>::parray_t rarray<T,1>::new_except_base(T* buffer, const int*dim,char**&parrayorigin)");
    parrayorigin = reinterpret_cast<char**>(const_cast<typename Unconst<T>::type*>(buffer));
    return reinterpret_cast<parray_t>(buffer);
}

// rarray private method to find base of a chain of pointers

template<typename T,int R>
T* rarray<T,R>::base(parray_t parray) 
{
    profileSay("T* rarray<T,R>::base(parray_t parray)");
    const char* result1 = reinterpret_cast<const char*>(parray);
    char* result2 = const_cast<char*>(result1);
    char** result = reinterpret_cast<char**>(result2);    
    for (int i=0; i < R-1; i++)
        result = reinterpret_cast<char**>(*result);
    return reinterpret_cast<T*>(result);
}

template<typename T>
T* rarray<T,1>::base(parray_t parray)
{
    profileSay("T* rarray<T,1>::base(parray_t parray)");
    const char*  result1 = reinterpret_cast<const char*>(parray);
          char*  result2 = const_cast<char*>(result1);
          char** result  = reinterpret_cast<char**>(result2);
    return reinterpret_cast<T*>(result);
}

// function to get some automatic pointer conversion working with templates:
template<typename T> T* get_pointer(T*t) 
{
    profileSay("T* get_pointer(T*t)");
    return t;
}

// get rid of the macros
#undef profileSay
#undef checkOrSay
// 

#endif

// Local Variables:
// mode: C++
// End:
