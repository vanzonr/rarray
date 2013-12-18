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
//    Relatedly, it should be allowed to use an external bufer, e.g.
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

////////  DEFINITION struct pchain<T,R>  ////////

// pchain: a struct to recursively generates types T**... and
// T*const*... as typedefs called cast_ptr_t and ptr_t inside the
// struct pchain<T,R>.
// 
// More specifically:
//
//   The type 
//      pchain<T,1>::ptr_t
//   is equal to the type
//      T*
//   ie., the pointer type for a one dimensional c array, 
//
//   The type 
//      pchain<T,2>::ptr_t
//   is equal to the type
//      T*const*
//   which is the pointer type for a two-dimensional c array.
//   The reason for the const is that the shape of this array is supposed to
//   remain at its fixed rectangular form. 
//
//   The type
//      pchain<T,3>::ptr_t 
//   is equal to
//      T*const*const*, 
//   the appropriate pointer for a 3d c array, 
//
//   etc.
//
//   Because one encounters non-const correct pointer-to-pointers
//   often (when allocating the pointer-to-pointer structure by hand,
//   one has little choice but to use these), the same struct
//   recursion defined pchain<T,R> as T****, i.e.,
// 
//      pchain<T,1>::cast_ptr_t = T* 
//      pchain<T,2>::cast_ptr_t = T**
//      pchain<T,3>::cast_ptr_t = T***
//      ...
//
// Recursive definition of pchain<T,R> in terms of pchain<T,R-1>:
template<typename T,int R> 
struct pchain {
    typedef typename pchain<T,R-1>::ptr_t const* ptr_t;     // const shape
    typedef typename pchain<T,R-1>::cast_ptr_t*  cast_ptr_t;// non-const variant
};
// Now we end the recursion by specifically defining the R=1 case:
template<typename T> 
struct pchain<T,1> {
    typedef T* ptr_t; // note there is not const: a const here would
                      // express that the elements of the
                      // multidimensional arrays are constant, not
                      // that its shape is constant. We do want to be
                      // able to have multidimensional arrays of
                      // constant shape but with modifiable elements,
                      // so const here would not be
                      // appropriate. Finally note that one can
                      // express non-modifiable element using
                      // pchain<const T,R>.
    typedef T* cast_ptr_t; // There would never have been a const here
};

////////  END DEFINITION struct pchain<T,R>  ////////


////////  DEFINITION struct unconst<T>  ////////

// Define a struct to strip constness from a type:
// Generically, do not strip a const:
template<typename T> 
struct unconst { 
    typedef T type; // non-stripping, intended for typed without const
};
// Override this non-const-stripped type with a stripped one using
// the following instantiation which applies for types with const only.
template<typename T> 
struct unconst<const T> {  
    typedef T type; // note that the const is gone
};

////////  END DEFINITION struct unconst<T>  ////////

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

    // chained pointer types:
    typedef typename pchain<T,R>::ptr_t      ptr_t;
    typedef typename pchain<T,R>::cast_ptr_t cast_ptr_t;

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
    rarray(T* buf, int n0, int n1);  // for R=2
    rarray(T* buf, int n0, int n1, int n2);  // for R=3
    rarray(T* buf, int n0, int n1, int n2, int n3);  // for R=4
    rarray(T* buf, int n0, int n1, int n2, int n3, int n4);  // for R=5
    rarray(T* buf, int n0, int n1, int n2, int n3, int n4, int n5);  // for R=6
    rarray(T* buf, const int* dim);  // for any R (the only way for R>6)

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
    inline ptr_t ptr() const; // as this is really just a nasty cast, keep const

    // return  T**.. acting similarly to this rarray when using []:
    cast_ptr_t cptr() const;  // as this is really just a nasty cast, keep const

    // create a reference to this that treats elements as constant:
    rarray<const T,R>&  cref() const;

    // access elements 
   #ifndef SKIPINTERMEDIATE
    // through intermediate object:
    inline rarray_intermediate<T,R-1> operator[](int i);
    inline rarray_intermediate<const T,R-1> operator[](int i) const;
   #else
    // or if SKIPINTERMEDIATE is set, by implicit case to a T**..:
    inline operator typename pchain<T,R>::ptr_t (); 
    inline operator typename pchain<const T,R>::ptr_t () const; 
   #endif

    // copy constructors:
    rarray(const rarray<T,R> &a);
    rarray(const rarray_intermediate<T,R> &a);

    // assignment operators:
    rarray<T,R>& operator=(const rarray<T,R> &a); 
    rarray<T,R>& operator=(const rarray_intermediate<T,R> &a);

  private:

    int*  originrefcount; // reference count for the original buffer, or norefcount
    T*    origin; // the original contiguous buffer
    T*    buffer; // start of current contiguous buffer(=origin unless sliced)
    int   n[R];   // array with the number of elements in each dimension
    int   ntot;   // total number of buffer elements
    ptr_t tnsr;   // start of the pointer-chain
    char**tnsrorigin; // original start of the pointer-chain
    bool  owned;  // does the container own the data/buffer

    // store dimension information and compute ntot:
    void setn(const int* dim); 

    // setup new rarray object:
    void init_shallow (T* const & orig, int* origrefcount, T* a, const int* dim, char** tnsrorig, ptr_t b);  
    void init_tnsr(T* const & orig, int* origrefcount, T* a, const int* dim);
    void init_data(const int* dim, int antot);

    // cleanup routine:
    void fini();

    // allocate the chain of pointers, except the base:
    static ptr_t new_except_base(T* buf, const int*  n, char**& torig);

    // find base of a chain of pointers:
    static T* base(ptr_t tnsr);

    friend class rarray_intermediate<T,R>;

}; // end definition rarray<T,R>

// Define rarray<T,1> to stop the recursion in operator[]:

template<typename T> 
class rarray<T,1> {

  public:

    // chained pointer types:
    typedef typename pchain<T,1>::ptr_t      ptr_t;
    typedef typename pchain<T,1>::cast_ptr_t cast_ptr_t;

    // constructors:
    rarray(); // uninitialized

    // creating their own buffer:
    rarray(int n0);
    rarray(const int* dim);  // for conformity

    // from an existing buffer:
    rarray(T* buf, int n0);
    rarray(T* buf, const int* dim);  // for conformity

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
    inline ptr_t ptr() const; // as this is really just a nasty cast, keep const

    // return  T**.. acting similarly to this rarray when using []:
    cast_ptr_t cptr() const;  // as this is really just a nasty cast, keep const

    // create a reference to this that treats elements as constant:
    rarray<const T,1>&  cref() const;

    // access elements through intermediate object:
   #ifndef SKIPINTERMEDIATE
    // through a T& pointer a the element:
    inline T& operator[](int i);
    inline const T& operator[](int i) const;
   #else
    // through implicit conversion to a pointer:
    inline operator typename pchain<T,1>::ptr_t ();
    inline operator typename pchain<const T,1>::ptr_t () const;
   #endif

    // copy constructors:
    rarray(const rarray<T,1> &a);
    rarray(const rarray_intermediate<T,1> &a);

    // assignment operators:
    rarray<T,1>& operator=(const rarray<T,1> &a);
    rarray<T,1>& operator=(const rarray_intermediate<T,1> &a) ;

  private:

    int*  originrefcount; // reference count for the original buffer, or norefcount
    T*    origin; // the original contiguous buffer
    T*    buffer; // start of current contiguous buffer(=origin unless sliced)
    int   n[1];   // array with the number of elements in each dimension
    int   ntot;   // total number of buffer elements
    ptr_t tnsr;   // start of the pointer-chain
    char**tnsrorigin; // original start of the pointer-chain
    bool  owned;  // does the container own the data/buffer

    // store dimension information and compute ntot:
    void setn(const int* dim); 

    // setup new rarray object:
    void init_shallow (T* const & orig, int* origrefcount, T* a, const int* dim, char** tnsrorig, ptr_t b);  
    void init_tnsr(T* const & orig, int* origrefcount, T* a, const int* dim);
    void init_data(const int* dim, int ntot);

    // cleanup routine:
    void fini() ;

    // allocate the chain of pointers, except the base:
    static ptr_t new_except_base(T* buf, const int* n, char**& torig);

    // find base of a chain of pointers:
    static T* base(ptr_t tnsr) ;

    friend class rarray_intermediate<T,1>;

}; // end definition rarray<T,1>

////////  END DEFINITION class rarray<T,R>  ////////

//////////////////////////////////////////////////////////////////////////////

////////  DEFINITION class rarray_intermediate <T,R>  ////////

// rarray_intermediate class: interface like rarray without assignment. Really
// only a reference to one, used in intermediate expressions produced
// by square brackets.

template<typename T,int R> class rarray_intermediate {

  public:

    // chained pointer types:
    typedef typename pchain<T,R>::ptr_t      ptr_t;
    typedef typename pchain<T,R>::cast_ptr_t cast_ptr_t;

    // retrieve array size in dimension i:
    inline int extent(int i) const;

    // retrieve array sizes in all dimensions:
    const int* extents() const; 

    // retrieve the total number of elements:
    int size() const;

    // return T* to the internal pointer to the data:
    T* data();

    // return T*const*.. acting similarly to this rarray when using []:
    inline ptr_t ptr() const; // as this is really just a nasty cast, keep const

    // return T**.. acting similarly to this rarray when using []:
    cast_ptr_t cptr() const; // as this is really just a nasty cast, keep const

    // create a reference to this that treats elements as constant:
    rarray_intermediate<const T,R>& cref() const; 

    // element access:
    inline rarray_intermediate<T,R-1> operator[](int i); 

  protected:

    int*       const&  originrefcount; // refcount of original contiguous buffer
    T*         const&  origin; // points to original contiguous buffer
    const int* const   n;      // number of elements in each dimension
    ptr_t      const   tnsr;   // start of the pointer-chain
    char**     const   tnsrorigin; // original start of the pointer-chain
    bool       const   owned;  // does the container own the data/buffer

    // constructor:
    inline rarray_intermediate(T*const& anorigin, int*const& anoriginrefcount, 
                               const int* an, ptr_t atnsr,
                               char** const atnsrorigin, bool anowned);

    // can be called only by:
    friend class rarray<T,R>; 
    friend class rarray<T,R+1>;
    friend class rarray_intermediate<T,R+1>;
    friend class rarray<typename unconst<T>::type,R+1>;

}; // end definition rarray_intermediate<T,R>

// Define rarray_intermediate<T,1> to stop the recursion in operator[]:

template<typename T> class rarray_intermediate<T,1> {

  public:

    // chained pointer types:
    typedef typename pchain<T,1>::ptr_t      ptr_t;
    typedef typename pchain<T,1>::cast_ptr_t cast_ptr_t;

    // retrieve array size in dimension i:
    int extent(int i) const;

    // retrieve array sizes in all dimensions:
    const int* extents() const; 

    // retrieve the total number of elements:
    int size() const; 

    // return T* to the internal pointer to the data:
    T* data();

    // return T*const*.. acting similarly to this rarray when using []:
    inline ptr_t ptr() const; // as this is really just a nasty cast, keep const

    // return T**.. acting similarly to this rarray when using []:
    cast_ptr_t cptr() const;  // as this is really just a nasty cast, keep const

    // create a reference to this that treats elements as constant:
    rarray_intermediate<const T,1>& cref() const;

    // element access:
    inline T& operator[](int i);

  protected:

    int*       const&  originrefcount; // refcount of original contiguous buffer
    T*         const&  origin; // points to original contiguous buffer
    const int* const   n;      // number of elements in each dimension
    ptr_t      const   tnsr;   // start of the pointer-chain
    char**     const   tnsrorigin; // original start of the pointer-chain
    bool       const   owned;  // does the container own the data/buffer

    // constructor:
    inline rarray_intermediate(T*const& anorigin, int*const& anoriginrefcount,
                               const int* an, ptr_t atnsr, 
                               char** const atnsrorigin, bool anowned);

    // can only be used by:
    friend class rarray<T,1>; 
    friend class rarray<T,2>;
    friend class rarray_intermediate<T,2>;
    friend class rarray<typename unconst<T>::type,2>;

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

#define norefcount nullptr

////////////////////////////////////////////////////////////////////////////

// rarray<T,R> and rarray_intermediate<T,R> constructors:

// ones creating their own buffer:

// constructor for an undefined array:
template<typename T, int R>
rarray<T,R>::rarray() 
: originrefcount(nullptr), origin(nullptr), buffer(nullptr)
{
    profileSay("rarray<T,R>::rarray()");
}

// R=1 case of the same
template<typename T>
rarray<T,1>::rarray() 
    : originrefcount(nullptr), origin(nullptr), buffer(nullptr), tnsrorigin(nullptr), owned(false)
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
rarray<T,R>::rarray(int n0,int n1,int n2)  // for R=3
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2)");
    checkOrSay(R==3, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2};
    init_data(dim, n0*n1*n2);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0,int n1,int n2,int n3)  // for R=4
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2,int n3)");
    checkOrSay(R==4, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3};
    init_data(dim, n0*n1*n2*n3);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0,int n1,int n2,int n3,int n4)  // for R=5
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2,int n3,int n4)");
    checkOrSay(R==5, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4};
    init_data(dim, n0*n1*n2*n3*n4);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0,int n1,int n2,int n3,int n4,int n5)  // for R=6
{
    profileSay("rarray<T,R>::rarray(int n0,int n1,int n2,int n3,int n4,int n5)");
    checkOrSay(R==6, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4,n5};
    init_data(dim, n0*n1*n2*n3*n4*n5);
}

template<typename T,int R>
rarray<T,R>::rarray(const int* dim)  // for any R (the only way for R>6)
{
    profileSay("rarray<T,R>::rarray(const int*)");
    int ntot = 1;
    for (int i=0; i<R; i++)
        ntot*=dim[i];
    init_data(dim, ntot);
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
    profileSay("rarray<T,1>::rarray(const int*)");
    init_data(dim, dim[0]);
}

// from existing buffers:

template<typename T,int R>
rarray<T,R>::rarray(T* buf, int n0, int n1)  // for R=2
{
    profileSay("rarray<T,R>::rarray(T*buf,int n0,int n1)");
    checkOrSay(R==2, "wrong rank in constructor");
    const int dim[] = {n0,n1};
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buf,int n0,int n1,int n2)  // for R=3
{
    profileSay("rarray<T,R>::rarray(T*buf,int n0,int n1,int n2)");
    checkOrSay(R==3, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2};
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buf,int n0,int n1,int n2,int n3)  // for R=4
{
    profileSay("rarray<T,R>::rarray(T*buf,int n0,int n1,int n2,int n3)");
    checkOrSay(R==4, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3};
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buf,int n0,int n1,int n2,int n3,int n4)  // for R=5
{
    profileSay("rarray<T,R>::rarray(T*buf,int n0,int n1,int n2,int n3,int n4)");
    checkOrSay(R==5, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4};
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buf,int n0,int n1,int n2,int n3,int n4,int n5)  // for R=6
{
    profileSay("rarray<T,R>::rarray(T*buf,int n0,int n1,int n2,int n3,int n4,int n5)");
    checkOrSay(R==6, "wrong rank in constructor");
    const int dim[] = {n0,n1,n2,n3,n4,n5};
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buf, const int* dim)  // for any I (the only way for R>6)
{
    profileSay("rarray<T,R>::rarray(T*buf,const int*dim)");
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T>
rarray<T,1>::rarray(T* buf, int n0)
{
    profileSay("rarray<T,1>::rarray(T*buf,int n0)");
    const int dim[] = {n0};
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T>
rarray<T,1>::rarray(T* buf, const int* dim)  // for any I (the only way for R>6)
{
    profileSay("rarray<T,1>::rarray(T*buf,const int*dim)");
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = false;
}

template<typename T,int R> inline 
rarray_intermediate<T,R>::rarray_intermediate(T*const&    anorigin, 
                                              int*const&  anoriginrefcount,
                                              const int*  an, 
                                              ptr_t       atnsr,
                                              char**const atnsrorigin, 
                                              bool        anowned)
  : origin(anorigin), 
    originrefcount(anoriginrefcount), 
    n(an),
    tnsr(atnsr),
    tnsrorigin(atnsrorigin),
    owned(anowned)
{
    profileSay("rarray_intermediate<T,R>::rarray_intermediate(T*const&anorigin,int*const&anoriginrefcount,const int*an,ptr_t atnsr,char**const atnsrorigin,bool anowned");
}

template<typename T> inline 
rarray_intermediate<T,1>::rarray_intermediate(T*const&    anorigin, 
                                              int*const&  anoriginrefcount,
                                              const int*  an, 
                                              ptr_t       atnsr,
                                              char**const atnsrorigin, 
                                              bool        anowned)
  : origin(anorigin), 
    originrefcount(anoriginrefcount), 
    n(an), 
    tnsr(atnsr),
    tnsrorigin(atnsrorigin),
    owned(anowned)
{
    profileSay("rarray_intermediate<T,1>::rarray_intermediate(T*const&anorigin,int*const&anoriginrefcount,const int*an,ptr_t atnsr");
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
    init_shallow(a.origin, a.originrefcount, a.buffer, a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
    owned = a.owned;
}

template<typename T,int R>
rarray<T,R>::rarray(const rarray_intermediate<T,R> &a) 
{
    profileSay("rarray<T,R>::rarray(const rarray_intermediate<T,R>&a)");
    init_shallow(a.origin, a.originrefcount, base(a.tnsr), a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
    owned = a.owned;
}

template<typename T>
rarray<T,1>::rarray(const rarray<T,1> &a) 
{
    profileSay("rarray<T,1>::rarray(const rarray<T,1>&a)"); 
    init_shallow(a.origin, a.originrefcount, a.buffer, a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
    owned = a.owned;
}

template<typename T>
rarray<T,1>::rarray(const rarray_intermediate<T,1> &a) 
{
    profileSay("rarray<T,1>::rarray(const rarray_intermediate<T,1>&a)");
    init_shallow(a.origin, a.originrefcount, base(a.tnsr), a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
    owned = a.owned;
}

// create a copy

template<typename T, int R>
rarray<T,R> rarray<T,R>::copy() const
{
    profileSay("rarray<T,R> rarray<T,R>::copy() const");
    if (origin != nullptr) { 
        // if initialized
        rarray<T,R> result(n);
        std::copy(buffer, buffer+ntot, result.buffer);
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
    if (origin != nullptr) { // if initialized
        //copy    
        rarray<T,1> result(n);
        std::copy(buffer, buffer+ntot, result.buffer);
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
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return n[i];
}

template<typename T>
int rarray<T,1>::extent(int i) const 
{
    profileSay("int rarray<T,1>::extent(int i) const");
    checkOrSay(i >=0 and i < 1, "wrong dimension");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return n[i];
}

template<typename T,int R> inline 
int rarray_intermediate<T,R>::extent(int i) const 
{
    profileSay("int rarray_intermediate<T,R>::extent(int i) const");
    checkOrSay(i >=0 and i < R, "wrong dimension");  
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");      
    return n[i];
}

template<typename T> inline 
int rarray_intermediate<T,1>::extent(int i) const 
{
    profileSay("int rarray_intermediate<T,1>::extent(int i) const");
    checkOrSay(i >=0 and i < 1, "wrong dimension");   
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");     
    return n[i];
}

// rarray<T,R> and rarray_intermediate<T,R> methods to retrieve array sizes in
// all dimensions

template<typename T,int R>
const int* rarray<T,R>::extents() const 
{
    profileSay("const int* rarray<T,R>::extents() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return n;
}

template<typename T>
const int* rarray<T,1>::extents() const 
{
    profileSay("const int* rarray<T,1>::extents() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return n;
}

// ...for rarray_intermediate

template<typename T,int R> 
const int* rarray_intermediate<T,R>::extents() const 
{
    profileSay("const int* rarray_intermediate<T,R>::extents() const");
    return n;
}

template<typename T> 
const int* rarray_intermediate<T,1>::extents() const 
{
    profileSay("const int* rarray_intermediate<T,1>::extents() const");
    return n;
}

// rarray method to retrieve the total number of elements

template<typename T,int R>
int rarray<T,R>::size() const 
{ 
    profileSay("int rarray<T,R>::size() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return ntot;
}

template<typename T>
int rarray<T,1>::size() const 
{ 
    profileSay("int rarray<T,1>::size() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return ntot;
}

// ...for rarray_intermediate

template<typename T,int R> 
int rarray_intermediate<T,R>::size() const 
{
    profileSay("int rarray_intermediate<T,R>::size() const");
    // multiplying the sizes in all dimensions
    int ntot = 1;       
    for (int i=0; i<R; i++)
        ntot *= n[i];
    return ntot;
}

template<typename T> 
int rarray_intermediate<T,1>::size() const 
{ 
    profileSay("int rarray_intermediate<T,1>::size() const");
    // multiplying the sizes in all dimensions
    int ntot = 1;        
    for (int i=0; i<1; i++)
        ntot *= n[i];
    return ntot;
}

// rarray method to return pointer of type T* to the internal data

template<typename T,int R>
T* rarray<T,R>::data()
{
    profileSay("T* rarray<T,R>::data()");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return buffer;
}

template<typename T,int R>
const T* rarray<T,R>::data() const 
{
    profileSay("const T* rarray<T,R>::data() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return buffer;
}

template<typename T>
T* rarray<T,1>::data()
{
    profileSay("T* rarray<T,1>::data()");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return buffer;
}

template<typename T>
const T* rarray<T,1>::data() const 
{
    profileSay("const T* rarray<T,1>::data() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return buffer;
}

// ...for rarray_intermediate

template<typename T,int R> 
T* rarray_intermediate<T,R>::data()
{
    profileSay("T* rarray_intermediate<T,R>::data()");
    return rarray<T,R>::base(tnsr);
}

template<typename T> 
T* rarray_intermediate<T,1>::data()
{
    profileSay("T* rarray_intermediate<T,1>::data()");
    return rarray<T,1>::base(tnsr);
}

// rarray method to return T*const*.. acting similarly to this rarray
// when using []:

template<typename T,int R> inline 
typename rarray<T,R>::ptr_t 
rarray<T,R>::ptr() const 
{
    profileSay("rarray<T,R>::ptr_t rarray<T,R>::ptr() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return tnsr;
}

template<typename T> inline 
typename rarray<T,1>::ptr_t 
rarray<T,1>::ptr() const 
{
    profileSay("rarray<T,1>::ptr_t rarray<T,1>::ptr() const");
    return tnsr;
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
}

// ...for rarray_intermediate

template<typename T,int R> inline 
typename rarray_intermediate<T,R>::ptr_t 
rarray_intermediate<T,R>::ptr() const 
{
    profileSay("rarray_intermediate<T,R>::ptr_t rarray_intermediate<T,R>::ptr() const");
    return tnsr;
}
    
template<typename T> inline 
typename rarray_intermediate<T,1>::ptr_t 
rarray_intermediate<T,1>::ptr() const 
{
    profileSay("rarray_intermediate<T,1>::ptr_t rarray_intermediate<T,1>::ptr() const");
    return tnsr;
}

// rarray method to return  T**.. acting similarly to this rarray when using []

template<typename T,int R>
typename rarray<T,R>::cast_ptr_t 
rarray<T,R>::cptr() const 
{
    profileSay("rarray<T,R>::cast_ptr_t rarray<T,R>::cptr() const");
    return const_cast<cast_ptr_t>(tnsr);
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
}

template<typename T>
typename rarray<T,1>::cast_ptr_t 
rarray<T,1>::cptr() const 
{
    profileSay("rarray<T,1>::cast_ptr_t rarray<T,1>::cptr() const");
    return const_cast<cast_ptr_t>(tnsr);
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
}

// ... for rarray_intermediate

template<typename T,int R> 
typename rarray_intermediate<T,R>::cast_ptr_t 
rarray_intermediate<T,R>::cptr() const 
{
    profileSay("rarray_intermediate<T,R>::cast_ptr_t rarray_intermediate<T,R>::cptr() const");
    return const_cast<cast_ptr_t>(tnsr);
}

template<typename T> 
typename rarray_intermediate<T,1>::cast_ptr_t 
rarray_intermediate<T,1>::cptr() const 
{
    profileSay("rarray_intermediate<T,1>::cast_ptr_t rarray_intermediate<T,1>::cptr() const");
    return const_cast<cast_ptr_t>(tnsr);
}

// rarray method to create a reference to this that treats elements as constant

template<typename T,int R>
rarray<const T,R>&  
rarray<T,R>::cref() const 
{
    profileSay("rarray<const T,R>& rarray<T,R>::cref() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return (rarray<const T,R>&)(*this);
}

template<typename T> 
rarray<const T,1>&  
rarray<T,1>::cref() const 
{
    profileSay("rarray<const T,1>& rarray<T,1>::cref() const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
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
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < n[0], "wrong index");
    return rarray_intermediate<const T,R-1>(origin, originrefcount, n+1, tnsr[i],tnsrorigin,owned);
}
template<typename T,int R> inline 
rarray_intermediate<T,R-1> 
rarray<T,R>::operator[](int i)
{
    profileSay("rarray_intermediate<T,R-1> rarray<T,R>::operator[](int i)");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < n[0], "wrong index");
    return rarray_intermediate<T,R-1>(origin, originrefcount, n+1, tnsr[i],tnsrorigin,owned);
}
#else
template<typename T,int R> inline 
rarray<T,R>::operator typename pchain<const T,R>::ptr_t () const 
{
    profileSay("rarray<T,R>::operator typename pchain<const T,R>::ptr_t () const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return tnsr;
}
template<typename T,int R> inline 
rarray<T,R>::operator typename pchain<T,R>::ptr_t ()
{
    profileSay("rarray<T,R>::operator typename pchain<T,R>::ptr_t ()");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return tnsr;
}
#endif

#ifndef SKIPINTERMEDIATE
template<typename T> inline 
const T& rarray<T,1>::operator[](int i) const 
{
    profileSay("const T& rarray<T,1>::operator[](int i) const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < n[0], "wrong index");
    return tnsr[i];
}
template<typename T> inline 
T& rarray<T,1>::operator[](int i)
{
    profileSay("T& rarray<T,1>::operator[](int i)");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < n[0], "wrong index");
    return tnsr[i];
}
#else
template<typename T> inline 
rarray<T,1>::operator typename pchain<const T,1>::ptr_t () const 
{
    profileSay("rarray<T,1>::operator pchain<const T,1>::ptr_t () const");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return tnsr;
}
template<typename T> inline 
rarray<T,1>::operator typename pchain<T,1>::ptr_t ()
{
    profileSay("rarray<T,1>::operator typename pchain<T,1>::ptr_t ()");
    checkOrSay(origin!=nullptr, "attempt at using uninitialized rarray");
    return tnsr;
}
#endif

// rarray_intermediate methods for element access

template<typename T,int R> inline
rarray_intermediate<T,R-1> 
rarray_intermediate<T,R>::operator[](int i) 
{
    profileSay("rarray_intermediate<T,R-1> rarray_intermediate<T,R>::operator[](int i)");
    checkOrSay(i >=0 and i < n[0], "wrong index");        
    return rarray_intermediate<T,R-1>(origin, originrefcount, n+1, tnsr[i],tnsrorigin,owned);
}

template<typename T> inline
T& rarray_intermediate<T,1>::operator[](int i) 
{
    profileSay("T& rarray_intermediate<T,1>::operator[](int i)");
    checkOrSay(i >=0 and i < n[0], "wrong index");
    return tnsr[i];
}

// rarray method to assignment operators

template<typename T,int R>
rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R> &a) 
{
    profileSay("rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R>&a)");
    if (&a != this) {
        fini();
        init_shallow(a.origin, a.originrefcount, a.buffer, a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
        owned = a.owned;
    }
    return *this;
}

template<typename T>
rarray<T,1>& rarray<T,1>::operator=(const rarray<T,1> &a) 
{
    profileSay("rarray<T,1>& rarray<T,1>::operator=(const rarray<T,1>&a)");
    if (&a != this) {
        fini();
        init_shallow(a.origin, a.originrefcount, a.buffer, a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
        owned = a.owned;
    }
    return *this;
}

template<typename T,int R>
rarray<T,R>& rarray<T,R>::operator=(const rarray_intermediate<T,R> &a) 
{
    profileSay("rarray<T,R>& rarray<T,R>::operator=(const rarray_intermediate<T,R>&a)");
    fini();
    init_shallow(a.origin, a.originrefcount, base(a.tnsr), a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
    owned = a.owned;
    return *this;
}

template<typename T>
rarray<T,1>& rarray<T,1>::operator=(const rarray_intermediate<T,1> &a) 
{
    profileSay("rarray<T,1>& rarray<T,1>::operator=(const rarray_intermediate<T,1>&a)");
    fini();
    init_shallow(a.origin, a.originrefcount, base(a.tnsr), a.n, const_cast<char**>(a.tnsrorigin), a.tnsr);
    owned = a.owned;
    return *this;
}

// rarray private method to store dimension information and compute ntot

template<typename T,int R>
void rarray<T,R>::setn(const int* dim)
{
    profileSay("void rarray<T,R>::setn(const int*dim)");
    ntot = 1;
    for (int i=0;i<R;i++) {
        n[i] = dim[i];
        ntot *= dim[i];
    }   
}

template<typename T>
void rarray<T,1>::setn(const int* dim)
{
    profileSay("void rarray<T,1>::setn(const int*dim)");
    ntot = 1;
    for (int i=0;i<1;i++) {
        n[i] = dim[i];
        ntot *= dim[i];
    }   
}

// init functions 

template<typename T,int R>
void rarray<T,R>::init_shallow(T* const &  orig, 
                               int*        origrefcount, 
                               T*          a, 
                               const int*  dim,
                               char**      tnsrorig,
                               ptr_t       b)
{
    profileSay("void rarray<T,R>::init_shallow(T*const&orig,int*origrefcount,T*a,const int*dim,char**tnsrorig,ptr_t b)");
    checkOrSay(    orig != nullptr, "null pointer");
    checkOrSay(       a != nullptr, "null pointer");
    checkOrSay(     dim != nullptr, "null pointer");
    checkOrSay(tnsrorig != nullptr, "null pointer");
    checkOrSay(       b != nullptr, "null pointer");
    originrefcount = origrefcount;
    if (originrefcount != norefcount) 
        (*originrefcount)++;
    origin = orig;
    buffer = a;
    setn(dim);
    tnsr = b;
    tnsrorigin = tnsrorig;
}

template<typename T,int R>
void rarray<T,R>::init_tnsr(T* const &  orig, 
                            int*        origrefcount, 
                            T*          a, 
                            const int*  dim)
{
    profileSay("void rarray<T,R>::init_tnsr(T*const&orig,int*origrefcount,T*a,const int*dim)");
    checkOrSay(orig != nullptr, "null pointer");
    checkOrSay(   a != nullptr, "null pointer");
    checkOrSay( dim != nullptr, "null pointer");
    if (origrefcount==norefcount)
        origrefcount=new int(0);
    char** tnsrorig;
    ptr_t tnsr = new_except_base(a, dim, tnsrorig);
    init_shallow(orig, origrefcount, a, dim, tnsrorig, tnsr);
}

template<typename T,int R>
void rarray<T,R>::init_data(const int* dim, int antot)
{
    profileSay("void rarray<T,R>::init_data(const int*dim,int antot)");
    checkOrSay( dim != nullptr, "null pointer");
    checkOrSay( antot >= 0, "negative number of elements");
    T* buf = new T[antot];
    init_tnsr(buf, norefcount, get_pointer(buf), dim);
    owned = true;
}

template<typename T>
void rarray<T,1>::init_shallow(T* const &  orig, 
                               int*        origrefcount, 
                               T*          a, 
                               const int*  dim,
                               char**      tnsrorig,
                               ptr_t       b)
{
    profileSay("void rarray<T,1>::init_shallow(T*const&orig,int*origrefcount,T*a,const int*dim,char**tnsrorig,ptr_t b)");
    checkOrSay(    orig != nullptr, "null pointer");
    checkOrSay(       a != nullptr, "null pointer");
    checkOrSay(     dim != nullptr, "null pointer");
    checkOrSay(tnsrorig != nullptr, "null pointer");
    checkOrSay(       b != nullptr, "null pointer");
    originrefcount = origrefcount;
    if (originrefcount != norefcount) 
        (*originrefcount)++;
    origin = orig;
    buffer = a;
    setn(dim);
    tnsr = b;
    tnsrorigin = tnsrorig;
}

template<typename T>
void rarray<T,1>::init_tnsr(T* const &  orig, 
                            int*        origrefcount, 
                            T*          a, 
                            const int*  dim)
{
    profileSay("void rarray<T,R>::init_tnsr(T*const&orig,int*origrefcount,T*a,const int*dim)");
    checkOrSay(orig != nullptr, "null pointer");
    checkOrSay(   a != nullptr, "null pointer");
    checkOrSay( dim != nullptr, "null pointer");
    char** tnsrorig;
    ptr_t tnsr = new_except_base(a, dim, tnsrorig);
    if (origrefcount==norefcount)
        origrefcount=new int(0);
    init_shallow(orig, origrefcount, a, dim, tnsrorig, tnsr);
}

template<typename T>
void rarray<T,1>::init_data(const int* dim, int antot)
{
    profileSay("void rarray<T,R>::init_data(const int*dim,int antot)");
    checkOrSay( dim != nullptr, "null pointer");
    checkOrSay( antot >= 0, "negative number of elements");
    T* buf = new T[antot];
    int* bufrefcount = new int(0);
    init_tnsr(buf, bufrefcount, get_pointer(buf), dim);
    owned = true;
}

//  rarray private cleanup routine

template<typename T,int R>
void rarray<T,R>::fini() 
{
    profileSay("void rarray<T,R>::fini()");
    if (origin!=nullptr) {
        if (originrefcount != norefcount) {
            (*originrefcount)--;
            if (*originrefcount==0) {
                delete[] tnsrorigin;
                if (owned) delete[] origin;
                delete originrefcount;
            }
        }
    }
    originrefcount = nullptr;
    origin = nullptr;
    buffer = nullptr;
}

template<typename T>
void rarray<T,1>::fini() 
{
    profileSay("void rarray<T,1>::fini()");
    if (origin!=nullptr) {
        if (originrefcount != norefcount) {
            (*originrefcount)--;
            if (*originrefcount==0) {
                if (owned) delete[] origin;
                delete originrefcount;
            }
        }
    }
    originrefcount = nullptr;
    origin = nullptr;
    buffer = nullptr;
}

// rarray private method to allocate the chain of pointers, except the base

template<typename T,int R>
typename rarray<T,R>::ptr_t 
rarray<T,R>::new_except_base(T* buf, const int* n, char**& torig) 
{
    profileSay("rarray<T,R>::ptr_t rarray<T,R>::new_except_base(T* buf, const int* n)");

    if (R>1) {

        typedef typename unconst<T>::type* bufstart_t;
        
        bufstart_t bufstart = const_cast<bufstart_t>(buf);
        int        nalloc   = 0;
        char**     result;
        
        for (int i=R-1; i--; )
            nalloc = n[i]*(1+nalloc);
        
        char**  palloc = new char*[nalloc];        
        int     ntot   = 1;
        char*** ptr    = reinterpret_cast<char***>(&result);
        
        for (int i=0; i<R-1; i++) {
            
            for (int j=0; j<ntot; j++)
                ptr[j] = palloc + j*n[i];
            
            ntot   *= n[i];
            if (ntot==0) {
                break;
            } else {
                ptr     = reinterpret_cast<char***>(*ptr);
                palloc += ntot;
            }
        }
        
        for (int j=0; j<ntot; j++)
            ptr[j] = reinterpret_cast<char**>(bufstart + j*n[R-1]);
        
        torig = result;
        return reinterpret_cast<ptr_t>(result);
        
    } else {
        
        torig = reinterpret_cast<char**>(buf);
        return reinterpret_cast<ptr_t>(buf);
        
    }
}

template<typename T>
typename rarray<T,1>::ptr_t 
rarray<T,1>::new_except_base(T* buf, const int* n, char**& torig) 
{
    profileSay("rarray<T,1>::ptr_t rarray<T,1>::new_except_base(T* buf, const int*n,char**&torig)");
    torig = reinterpret_cast<char**>(const_cast<typename unconst<T>::type*>(buf));
    return reinterpret_cast<ptr_t>(buf);
}

// rarray private method to find base of a chain of pointers

template<typename T,int R>
T* rarray<T,R>::base(ptr_t tnsr) 
{
    profileSay("T* rarray<T,R>::base(ptr_t tnsr)");
    const char* result1 = reinterpret_cast<const char*>(tnsr);
    char* result2 = const_cast<char*>(result1);
    char** result = reinterpret_cast<char**>(result2);    
    for (int i=0; i < R-1; i++)
        result = reinterpret_cast<char**>(*result);
    return reinterpret_cast<T*>(result);
}

template<typename T>
T* rarray<T,1>::base(ptr_t tnsr)
{
    profileSay("T* rarray<T,1>::base(ptr_t tnsr)");
    const char*  result1 = reinterpret_cast<const char*>(tnsr);
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
