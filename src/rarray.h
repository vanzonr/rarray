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
//    structure. With rarrar, one can write
//
//       rarray<double,2> a(100,100);
//       ...
//       a[4][2] = ...
//
//    A bit more verbose then I'd like, but no more than needed within
//    the constraints of the C++ language.
//
//    The compatibility requirement with pointer-to-pointer structures
//    is achieve by allocating a pointer-to-pointer structure. This
//    accounts for most of the memory overhead of using rarray.
//
// 2. To be as fast as pointer-to-pointer structures.
//
// 3. Having rarrays know their sizes, so that can be passed to
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
//    reference would make a shallow, reference counted copy. This can
//    be preferable if the array is to be used in a reshaped way
//    inside the function. For deep-copies, there is a copy member
//    function.
//
// 4. Enabling interplay with libraries: this is achieved by
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
// 5. Optionally allowing bounds checking, triggered by defining the
//    BOUNDSCHECK compiler constant.
//
// 6. Avoiding a lot of cluttered sematics around const with pointer
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
// (c) 2013-2014 Ramses van Zon - SciNet/University of Toronto 
//

#ifndef RARRAY_H
#define RARRAY_H

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

// DEFINE INTERNAL TYPES


// DEFINITION struct PointerArray<T,R>

// PointerArray: a struct to recursively generates types T**... and
// T*const*... as typedefs called noconst_parray_t and
// pointer_array_t inside the struct PointerArray<T,R>.
// 
// More specifically:
//
//   The type 
//      PointerArray<T,1>::type
//   is equal to the type
//      T*
//   ie., the pointer type for a one dimensional c array, 
//   Similarly, the type 
//      PointerArray<T,2>::type
//   is equal to the type
//      T*const*
//   which is the pointer type for a two-dimensional c array.
//   The reason for the const is that the shape of this array is supposed to
//   remain at its fixed rectangular form. 
//   Following the same pattern, the type
//      PointerArray<T,3>::type 
//   is equal to
//      T*const*const*, 
//   the appropriate pointer for a 3d c array, etc.
//
//   Because one encounters non-const correct pointer-to-pointers
//   often (when allocating the pointer-to-pointer structure by hand,
//   one has little choice but to use these), the same struct
//   recursion defined PointerArray<T,R> as T****, i.e.,
// 
//      PointerArray<T,1>::noconst_type = T* 
//      PointerArray<T,2>::noconst_type = T**
//      ...
//
// Recursive definition of PointerArray<T,R> in terms of PointerArray<T,R-1>:
template<typename T,int R> 
struct PointerArray {
    typedef typename PointerArray<T,R-1>::type const*    type;     // const shape
    typedef typename PointerArray<T,R-1>::noconst_type*  noconst_type;// non-const variant
};
// End recursion by specifically defining the R=1 case:
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

// END DEFINITION struct PointerArray<T,R>


// DEFINITION struct unconst<T>

// Define a struct to strip constness from a type:
template<typename T> 
struct Unconst { 
    typedef T type; // non-stripping, intended for typed without const
};
// Override this non-const-stripped type with a stripped one which applies for types with const only.
template<typename T> 
struct Unconst<const T> {  
    typedef T type; // note that the const is gone
};

// END DEFINITION struct Unconst<T>


// forward definition of the class for intermediate bracket expressions:

template<typename T,int R> class rarray_intermediate;

//------------------------------------------------//
//                                                //
//               I N T E R F A C E                //
//                                                //
//------------------------------------------------//


// DEFINITION class rarray<T,R>

template<typename T,int R> 
class rarray {

  public: 
    typedef T*        iterator;                                        // iterator type
    typedef const T*  const_iterator;                                  // iterator type for constant access
    typedef int       difference_type;                                 // difference type for indices
    typedef int       size_type;                                       // type of indices
    typedef typename PointerArray<T,R>::type         parray_t;         // shorthand for T*const*const*...
    typedef typename PointerArray<T,R>::noconst_type noconst_parray_t; // shorthand for T***...

    rarray();                                                          // constructor leaving rarray uninitialized 
    rarray(int n0, int n1);                                            // constructor creating own buffer for R=2
    rarray(int n0, int n1, int n2);                                    // constructor creating own buffer for R=3
    rarray(int n0, int n1, int n2, int n3);                            // constructor creating own buffer for R=4
    rarray(int n0, int n1, int n2, int n3, int n4);                    // constructor creating own buffer for R=5
    rarray(int n0, int n1, int n2, int n3, int n4, int n5);            // constructor creating own buffer for R=6
    rarray(const int* extent);                                         // constructor creating own buffer for any R (the only way for R>6)
    rarray(T* buffer, int n0, int n1);                                 // constructor from an existing buffer for R=2
    rarray(T* buffer, int n0, int n1, int n2);                         // constructor from an existing buffer for R=3
    rarray(T* buffer, int n0, int n1, int n2, int n3);                 // constructor from an existing buffer for R=4
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4);         // constructor from an existing buffer for R=5
    rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5); // constructor from an existing buffer for R=6
    rarray(T* buffer, const int* extent);                              // constructor from an existing buffer for any R (the only way for R>6)
    rarray(const rarray<T,R> &a);                                      // copy constructor
    rarray(const rarray_intermediate<T,R> &a);                         // copy constructor
    rarray<T,R>& operator=(const rarray<T,R> &a);                      // assignment operator
    rarray<T,R>& operator=(const rarray_intermediate<T,R> &a);         // assignment operator
    ~rarray();                                                         // destructor
    void clear();                                                      // make undefined
    void reshape(int n0, int n1);                                      // reshape without changing the underlying buffer for R=2
    void reshape(int n0, int n1, int n2);                              // reshape without changing the underlying buffer for R=3
    void reshape(int n0, int n1, int n2, int n3);                      // reshape without changing the underlying buffer for R=4
    void reshape(int n0, int n1, int n2, int n3, int n4);              // reshape without changing the underlying buffer for R=5
    void reshape(int n0, int n1, int n2, int n3, int n4, int n5);      // reshape without changing the underlying buffer for R=6
    void reshape(const int* extent);                                   // reshape without changing the underlying buffer for any R (the only way for R>6)

    bool                is_clear()           const;                    // check if uninitialized
    rarray<T,R>         copy()               const;                    // return a copy
    int                 extent(int i)        const;                    // retrieve array size in dimension i
    const int*          extents()            const;                    // retrieve array sizes in all dimensions
    int                 size()               const;                    // retrieve the total number of elements  
    T*                  data();                                        // return a T* to the internal data
    const T*            data()               const;                    // return a T* to the internal data
    parray_t            ptr_array()          const;                    // return a T*const*.. acting similarly to this rarray when using []:
    noconst_parray_t    noconst_ptr_array()  const;                    // return a T**.. acting similarly to this rarray when using []:    
    rarray<const T,R>&  const_ref()          const;                    // create a reference to this that treats elements as constant:

    // access elements 
   #ifndef SKIPINTERMEDIATE
    // through intermediate object:
    rarray_intermediate<T,R-1> operator[](int i);
    rarray_intermediate<const T,R-1> operator[](int i) const;
   #else
    // or if SKIPINTERMEDIATE is set, by implicit case to a T**..:
    operator typename PointerArray<T,R>::type (); 
    operator typename PointerArray<const T,R>::type () const; 
   #endif

  private:
    parray_t  parray_;                                                 // start of the array of pointers
    int*      extent_;                                                 // array of number of elements in each dimension
    bool      ismine_;                                                 // does the container own the data buffer?
    bool      entire_;                                                 // true if not a subarray
    int*      rcount_;                                                 // reference count for the array if not a subarray

    T*   get_buffer() const;                                           // get start of current contiguous buffer
    void init_shallow(parray_t parray, const int* extent, bool entire, int* rcount); // setup new rarray object
    void init_shallow(parray_t parray, const int* extent);             // setup new rarray object
    void init_parray(T* buffer, const int* extent);                    // setup new rarray object
    void init_data(const int* extent, int extenttot);                  // setup new rarray object
    void fini();                                                       // cleanup routine
    static parray_t new_except_base(T* buffer, const int* extent);     // allocate the chain of pointers, except the base
    static T*       base(parray_t parray);                             // find base of a chain of pointers:

    friend class rarray_intermediate<T,R>;

}; // end definition rarray<T,R>

// Define rarray<T,1> to stop the recursion in operator[]:

template<typename T> 
class rarray<T,1> {

  public:
    typedef T*        iterator;                                        // iterator type
    typedef const T*  const_iterator;                                  // iterator type for constant access
    typedef int       difference_type;                                 // difference type for indices
    typedef int       size_type;                                       // type of indices
    typedef typename PointerArray<T,1>::type         parray_t;         // conforming shorthand for T*
    typedef typename PointerArray<T,1>::noconst_type noconst_parray_t; // conforming shorthand for T*

    rarray();                                                          // constructor leaving object uninitialized
    rarray(int n0);                                                    // constructor creating own buffer
    rarray(const int* extent);                                         // constructor creating own buffer from pointer, for conformity
    rarray(T* buffer, int n0);                                         // constructor from an existing buffer:
    rarray(T* buffer, const int* extent);                              // constructor from an existing buffer for conformity    
    rarray(const rarray<T,1> &a);                                      // copy constructor
    rarray(const rarray_intermediate<T,1> &a);                         // copy constructor    
    rarray<T,1>& operator=(const rarray<T,1> &a);                      // assignment operator
    rarray<T,1>& operator=(const rarray_intermediate<T,1> &a);         // assignment operator
    ~rarray();                                                         // destructor
    void clear();                                                      // make uninitialized again
    void reshape(int n0);                                              // to change shape (only shrinking is defined)
    void reshape(const int* extent);                                   // for conformity

    bool                is_clear()           const;                    // check if uninitialized
    rarray<T,1>         copy()               const;                    // return a copy
    int                 extent(int i)        const;                    // retrieve array size in dimension i
    const int*          extents()            const;                    // retrieve array sizes in all dimensions
    int                 size()               const;                    // retrieve the total number of elements
    T*                  data();                                        // return T* to the internal data
    const T*            data()               const;                    // return T* to the internal data
    parray_t            ptr_array()          const;                    // return T*const*... acting similarly to this rarray when using []
    noconst_parray_t    noconst_ptr_array()  const;                    // return  T**... acting similarly to this rarray when using []
    rarray<const T,1>&  const_ref()          const;                    // create reference to this that treats elements as constant

    // access elements through intermediate object:
   #ifndef SKIPINTERMEDIATE
    // through a T& pointer a the element:
    T& operator[](int i);
    const T& operator[](int i) const;
   #else
    // through implicit conversion to a pointer:
    operator typename PointerArray<T,1>::type ();
    operator typename PointerArray<const T,1>::type () const;
   #endif

  private:
    parray_t  parray_;                                                 // start of the array of pointers
    int*      extent_;                                                 // array of number of elements in each dimension
    bool      ismine_;                                                 // does the container own the data buffer?
    bool      entire_;                                                 // true if not a subarray
    int*      rcount_;                                                 // reference count for the array if not a subarray

    T*   get_buffer() const;                                           // get start of current contiguous buffer  
    void init_shallow(parray_t parray, const int* extent, bool entire, int* rcount); // setup new rarray object
    void init_shallow(parray_t parray, const int* extent);             // setup new rarray object
    void init_parray(T* buffer, const int* extent);                    // setup new rarray object
    void init_data(const int* extent, int extenttot);                  // setup new rarray object
    void fini() ;                                                      // cleanup routine
    static parray_t new_except_base(T* buffer, const int* extent);     // allocate the chain of pointers, except the base
    static T* base(parray_t parray) ;                                  // find base of a chain of pointers

    friend class rarray_intermediate<T,1>;

}; // end definition rarray<T,1>

// END DEFINITION class rarray<T,R>


// DEFINITION class rarray_intermediate <T,R>

// rarray_intermediate class: interface like rarray without assignment. Really
// only a reference to one, used in intermediate expressions produced
// by square brackets.

template<typename T,int R> 
class rarray_intermediate {

  public:
    typedef T*        iterator;                                        // iterator type
    typedef const T*  const_iterator;                                  // iterator type for constant access
    typedef int       difference_type;                                 // difference type for indices
    typedef int       size_type;                                       // type of indices
    typedef typename PointerArray<T,R>::type         parray_t;         // shorthand for T*const*const*...
    typedef typename PointerArray<T,R>::noconst_type noconst_parray_t; // shorthand for T***...
    
    int                             extent(int i)        const;        // retrieve array size in dimension i
    const int*                      extents()            const;        // retrieve array sizes in all dimensions    
    int                             size()               const;        // retrieve the total number of elements
    T*                              data()               const;        // return T* to the internal pointer to the data
    parray_t                        ptr_array()          const;        // return T*const*.. acting similarly to this rarray when using []
    noconst_parray_t                noconst_ptr_array()  const;        // return T**.. acting similarly to this rarray when using []
    rarray_intermediate<const T,R>& const_ref()          const;        // create a reference to this that treats elements as constant
    rarray_intermediate<T,R-1>      operator[](int i)    const;        // element access

  protected:
    parray_t   const  parray_;                                         // start of the pointer array
    const int* const  extent_;                                         // number of elements in each dimension

    rarray_intermediate(parray_t parray, const int* extent);           // constructor, called by
    friend class rarray<T,R>;                                          // constructor can only
    friend class rarray<T,R+1>;                                        // be called by these
    friend class rarray_intermediate<T,R+1>;                           // classes.
    friend class rarray<typename Unconst<T>::type,R+1>;

}; // end definition rarray_intermediate<T,R>

// Define rarray_intermediate<T,1> to stop the recursion in operator[]:

template<typename T> class rarray_intermediate<T,1> {

  public:
    typedef T*        iterator;                                        // iterator type
    typedef const T*  const_iterator;                                  // iterator type for constant access
    typedef int       difference_type;                                 // difference type for indices
    typedef int       size_type;                                       // type of indices
    typedef typename PointerArray<T,1>::type         parray_t;         // conforming shorthand for T*
    typedef typename PointerArray<T,1>::noconst_type noconst_parray_t; // conforming shorthand for T*

    int                             extent(int i)        const;        // retrieve array size in dimension i
    const int*                      extents()            const;        // retrieve array sizes in all dimensions
    int                             size()               const;        // retrieve the total number of elements
    T*                              data()               const;        // return T* to the internal pointer to the data
    parray_t                        ptr_array()          const;        // return T*const*.. acting similarly to this rarray when using []
    noconst_parray_t                noconst_ptr_array()  const;        // return T**.. acting similarly to this rarray when using []
    rarray_intermediate<const T,1>& const_ref()          const;        // create a reference to this that treats elements as constant
    T&                              operator[](int i)    const;        // element access

  protected:
    parray_t   const   parray_;                                        // start of the pointer array
    const int* const   extent_;                                        // number of elements in each dimension

    rarray_intermediate(parray_t parray, const int* extent);           // constructor    
    friend class rarray<T,1>; 
    friend class rarray<T,2>;
    friend class rarray_intermediate<T,2>;
    friend class rarray<typename Unconst<T>::type,2>;

};  // end of class template definition of rarray_intermediate<T,1>.

// END DEFINITION class rarray_intermediate <T,R>

#ifdef TRACETEST
    #define profileSay(a) std::cerr << "PROFILE " << __FILE__ << '@' << __LINE__<< ":\t" << a << std::endl;
#else
    #define profileSay(a) 
#endif

#ifdef BOUNDSCHECK
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

// DEFINITION OF EXTENT AND RARRAY

#define EXTENT(A,I) extent_given_byte_size(A,I,sizeof(A))
#define RARRAY(A)   make_rarray_given_byte_size(A,sizeof(A))

template<typename T>
int extent_given_byte_size(T a[], int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(T[],int,int)");
    checkOrSay(i>=0 and i<1, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(T);
        default: return 1;
    }
}

template<typename T,int Z>
int extent_given_byte_size(T a[][Z], int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(T[][Z],int,int)");
    checkOrSay(i>=0 and i<2, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(T)/Z;
        case 1:  return Z;
        default: return 1;
    }
}

template<typename T,int Y,int Z>
int extent_given_byte_size(T a[][Y][Z], int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(T[][Y][Z],int,int)");
    checkOrSay(i>=0 and i<3, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(T)/Z/Y;
        case 1:  return Y;
        case 2:  return Z;
        default: return 1;
    }
}

template<typename T,int X,int Y,int Z>
int extent_given_byte_size(T a[][X][Y][Z], int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(T[][X][Y][Z],int,int)");
    checkOrSay(i>=0 and i<4, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(T)/X/Z/Y;
        case 1:  return X;
        case 2:  return Y;
        case 3:  return Z;
        default: return 1;
    }
}

template<typename T,int W,int X,int Y,int Z>
int extent_given_byte_size(T a[][W][X][Y][Z], int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(T[][W][X][Y][Z],int,int)");
    checkOrSay(i>=0 and i<5, "wrong dimension");
    switch (i) {
        case 0:  return byte_size/sizeof(T)/W/X/Z/Y;
        case 1:  return W;
        case 2:  return X;
        case 3:  return Y;
        case 4:  return Z;
        default: return 1;
    }
}

template<typename T,int V,int W,int X,int Y,int Z>
int extent_given_byte_size(T a[][V][W][X][Y][Z], int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(T[][V][W][X][Y][Z],int,int)");
    checkOrSay(i>=0 and i<6, "wrong dimension");
    switch (i) {
       case 0:  return byte_size/sizeof(T)/V/W/X/Z/Y;
       case 1:  return V;
       case 2:  return W;
       case 3:  return X;
       case 4:  return Y;
       case 5:  return Z;
       default: return 1;
    }
}

template<typename T,int R>
int extent_given_byte_size(const rarray<T,R>& a, int i, int byte_size) 
{
    profileSay("int extent_given_byte_size(const rarray<T,R>&,int,int)");
    return a.extent(i);
}

template<typename T>
rarray<T,1> make_rarray_given_byte_size(T a[], int byte_size) 
{
    profileSay("rarray<T,1> make_rarray_given_byte_size(T[],int)");
    const int z = byte_size/sizeof(T);
    return rarray<T,1>(a,z);
}

template<typename T,int Z>
rarray<T,2> make_rarray_given_byte_size(T a[][Z], int byte_size) 
{
    profileSay("rarray<T,2> make_rarray_given_byte_size(T[][Z],int)");
    const int y = byte_size/sizeof(T)/Z;
    return rarray<T,2>(*a,y,Z);
}

template<typename T,int Y,int Z>
rarray<T,3> make_rarray_given_byte_size(T a[][Y][Z], int byte_size) 
{
    profileSay("rarray<T,3> make_rarray_given_byte_size(T[][Y][Z],int)");
    const int x = byte_size/sizeof(T)/Z/Y;
    return rarray<T,3>(**a,x,Y,Z);
}

template<typename T,int X,int Y,int Z>
rarray<T,4> make_rarray_given_byte_size(T a[][X][Y][Z], int byte_size) 
{
    profileSay("rarray<T,4> make_rarray_given_byte_size(T[][X][Y][Z],int)");
    const int w = byte_size/sizeof(T)/X/Z/Y;
    return rarray<T,4>(***a,w,X,Y,Z);
}

template<typename T,int W,int X,int Y,int Z>
rarray<T,5> make_rarray_given_byte_size(T a[][W][X][Y][Z], int byte_size) 
{
    profileSay("rarray<T,5> make_rarray_given_byte_size(T[][W][X][Y][Z],int)");
    const int v = byte_size/sizeof(T)/W/X/Z/Y;
    return rarray<T,5>(****a,v,W,X,Y,Z);
}

template<typename T,int V,int W,int X,int Y,int Z>
rarray<T,6> make_rarray_given_byte_size(T a[][V][W][X][Y][Z], int byte_size) 
{
    profileSay("rarray<T,6> make_rarray_given_byte_size(T[][V][W][X][Y][Z],int)");
    const int u = byte_size/sizeof(T)/V/W/X/Z/Y;
    return rarray<T,6>(*****a,u,V,W,X,Y,Z);
}

template<typename T,int R>
rarray<T,R> make_rarray_given_byte_size(rarray<T,R> a, int byte_size) 
{
    profileSay("rarray<T,R> make_rarray_given_byte_size(rarray<T,R>,int)");
    return a;
}

// output streaming operators

template<typename T,int R>
std::ostream& operator<<(std::ostream &o, const rarray<T,R>& r)
{
    profileSay("std::ostream& operator<<(std::ostream&,const rarray<T,R>&)");
    if (not r.is_clear()) {
        o << '{';
        for (int i=0; i<r.extent(0); i++)  {
            if (i>0)
                o << ',';
            o << r[i];
        }
        o << '}';
    } else {
        for (int i=0; i<R; i++) 
            o << '{';
        for (int i=0; i<R; i++) 
            o << '}';
    }
    return o;
}

template<typename T>
std::ostream& operator<<(std::ostream &o, const rarray<T,1>& r)
{
    profileSay("std::ostream& operator<<(std::ostream&,const rarray<T,1>&)");
    if (not r.is_clear()) {
        o << '{';
        for (int i=0; i<r.extent(0); i++) {
            if (i) o << ',';
            std::stringstream strstr;
            std::string result;
            strstr << r[i];
            result = strstr.str();
            if (result.find_first_of("{,}#") != std::string::npos)
                o << '#' << result.size() << ':';
            o << result;
        }
        o << '}';
    } else 
        o << "{}";
    return o;
}

template<typename T,int R>
std::ostream& operator<<(std::ostream &o, const rarray_intermediate<T,R>& r)
{
    profileSay("std::ostream& operator<<(std::ostream&,const rarray_intermediate<T,R>&)");
    o << '{' << r[0];
    for (int i=1; i<r.extent(0); i++) 
        o << ',' << r[i];
    o << '}';
    return o;
}

template<typename T>
std::ostream& operator<<(std::ostream &o, const rarray_intermediate<T,1>& r)
{
    profileSay("std::ostream& operator<<(std::ostream&,const rarray_intermediate<T,1>&)");
    o << '{';
    for (int i=0; i<r.extent(0); i++) {
        if (i) o << ',';
        std::stringstream strstr;
        std::string result;
        strstr << r[i];
        result = strstr.str();
        if (result.find_first_of("{,}#") != std::string::npos)
            o << '#' << result.size() << ':';
        o << result;
    }
    o << '}';
    return o;
}

template<typename T, int R>
struct deref 
{
    static T& access(typename PointerArray<T,R>::type p, const int* indices) 
    {
        return deref<T,R-1>::access(p[indices[0]-1], indices+1);
    }
};

template<typename T>
struct deref<T,1> 
{
    static T& access(typename PointerArray<T,1>::type p, const int* indices) 
    {
        return p[indices[0]-1];
    }
};

template<typename T, int R> 
void read_and_parse_extent(std::istream &in, int* extents, typename PointerArray<T,R>::type p = 0)
{
    using std::string;
    size_t init_file_ptr = in.tellg();
    try {
        int current_extents[R] = {0};
        for (int i=0; i<R; i++) {
            current_extents[i] = 1;
            if (in.get() != '{') 
                throw std::istream::failure("Format error");
        }
        int current_depth = R-1; // start scanning the deepest level
        while (current_depth>=0) {
            if (current_depth==R-1) {
                char    lastchar;
                string  word = "";
                do {
                    lastchar = in.get();
                    if (lastchar != ',' and lastchar != '}')
                        word += lastchar;
                    if (word == "#") {
                        word="";
                        string skipstr;
                        do {
                            skipstr += (lastchar = in.get());
                        } while (lastchar!=':');
                        int skip = atoi(skipstr.c_str());
                        for (int i=0; i<skip; i++) 
                            word += in.get();
                        lastchar = in.get();
                    }
                    if (lastchar == ',') {
                        if (p) {
                            std::stringstream ss(word);
                            ss >> deref<T,R>::access(p, current_extents);
                        }
                        word="";
                        current_extents[current_depth]++;
                    }
                } while (lastchar != '}');
                if (p) {
                    std::stringstream ss(word);
                    ss >> deref<T,R>::access(p, current_extents);
                }
                if (extents[current_depth] < current_extents[current_depth])
                    extents[current_depth] = current_extents[current_depth];
                current_depth--;
            } else {
                switch (in.get()) {
                  case ',':
                    current_extents[current_depth]++;
                    break;
                  case '{':
                    current_depth++;
                    current_extents[current_depth] = 1;
                    break;
                  case '}':
                    if (extents[current_depth] < current_extents[current_depth])
                        extents[current_depth] = current_extents[current_depth];
                    current_depth--;
                    break;
                  default:
                    throw std::istream::failure("Format error");
                }
            }
        }    
    }
    catch (std::istream::failure& e) {
        in.seekg(init_file_ptr, in.beg);// upon failure, undo characters read in
        throw e;                        // and pass on the error
    }
    if (p==0)
        in.seekg(init_file_ptr, in.beg);
}

template<typename T,int R>
std::istream& operator>>(std::istream &i, rarray<T,R>& r)
{
    profileSay("std::istream& operator>>(std::istream&,rarray<T,R>&)");
    int extent[R] = {0};
    read_and_parse_extent<T,R>(i, extent);
    r = rarray<T,R>(extent);
    read_and_parse_extent<T,R>(i, extent, r.ptr_array());
    return i;
}


//------------------------------------------------//
//                                                //
//          I M P L E M E N T A T I O N           //
//                                                //
//------------------------------------------------//

//----------------------------------------------------------------------//

// rarray<T,R> and rarray_intermediate<T,R> constructors:

// Constructors creating their own buffer:

// constructor for an undefined array:
template<typename T, int R>
rarray<T,R>::rarray() 
  : parray_(nullptr), 
    extent_(nullptr),
    ismine_(false),
    entire_(false),
    rcount_(nullptr)
{
    // constructor
    profileSay("rarray<T,R>::rarray()");
}

// R=1 case of the same
template<typename T>
rarray<T,1>::rarray() 
  : parray_(nullptr), 
    extent_(nullptr),
    ismine_(false),
    entire_(false),
    rcount_(nullptr)
{
    // constructor
    profileSay("rarray<T,1>::rarray()");
}

// Constructors that allocate memory

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1) //for R=2
{
    // constructor
    profileSay("rarray<T,R>::rarray(int, int)");
    checkOrSay(R==2, "wrong rank in constructor");
    checkOrSay(n0!=0 and n1!=0, "zero extents in constructor not allowed");
    const int extent[] = {n0,n1};
    init_data(extent, n0*n1);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2) //for R=3
{
    // constructor
    profileSay("rarray<T,R>::rarray(int, int, int)");
    checkOrSay(R==3, "wrong rank in constructor");
    checkOrSay(n0!=0 and n1!=0 and n2!=0, "zero extents in constructor not allowed");
    const int extent[] = {n0,n1,n2};
    init_data(extent, n0*n1*n2);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2, int n3) //for R=4
{
    // constructor
    profileSay("rarray<T,R>::rarray(int, int, int, int)");
    checkOrSay(R==4, "wrong rank in constructor");
    checkOrSay(n0!=0 and n1!=0 and n2!=0 and n3!=0, "zero extents in constructor not allowed");
    const int extent[] = {n0,n1,n2,n3};
    init_data(extent, n0*n1*n2*n3);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4) //for R=5
{
    // constructor
    profileSay("rarray<T,R>::rarray(int, int, int, int, int)");
    checkOrSay(R==5, "wrong rank in constructor");
    checkOrSay(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0, "zero extents in constructor not allowed");
    const int extent[] = {n0,n1,n2,n3,n4};
    init_data(extent, n0*n1*n2*n3*n4);
}

template<typename T,int R>
rarray<T,R>::rarray(int n0, int n1, int n2, int n3, int n4, int n5) //for R=6
{
    // constructor
    profileSay("rarray<T,R>::rarray(int, int, int, int, int, int)");
    checkOrSay(R==6, "wrong rank in constructor");
    checkOrSay(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0, "zero extents in constructor not allowed");
    const int extent[] = {n0,n1,n2,n3,n4,n5};
    init_data(extent, n0*n1*n2*n3*n4*n5);
}

template<typename T,int R>
rarray<T,R>::rarray(const int* extent) //for any R (the only way for R>6)
{
    // constructor
    profileSay("rarray<T,R>::rarray(const int*)");
    int volume = 1;
    for (int i=0; i<R; i++)
        volume *= extent[i];
    checkOrSay(volume!=0,"zero extents in constructor not allowed");
    init_data(extent, volume);
}

template<typename T>
rarray<T,1>::rarray(int n0)
{
    // constructor
    profileSay("rarray<T,1>::rarray(int)");
    init_data(&n0, n0);
}

template<typename T>
rarray<T,1>::rarray(const int* extent) //for uniformity
{
    // constructor
    profileSay("rarray<T,1>::rarray(const int*)");
    init_data(extent, extent[0]);
}

// from existing buffers:

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, int n0, int n1) //for R=2
{
    // constructor
    profileSay("rarray<T,R>::rarray(T*, int, int)");
    checkOrSay(R==2, "wrong rank in constructor");
    const int extent[] = {n0,n1};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2) //for R=3
{
    // constructor
    profileSay("rarray<T,R>::rarray(T*, int, int, int)");
    checkOrSay(R==3, "wrong rank in constructor");
    const int extent[] = {n0,n1,n2};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3) //for R=4
{
    // constructor
    profileSay("rarray<T,R>::rarray(T*, int, int, int, int)");
    checkOrSay(R==4, "wrong rank in constructor");
    const int extent[] = {n0,n1,n2,n3};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4) //for R=5
{
    // constructor
    profileSay("rarray<T,R>::rarray(T*, int, int, int, int, int)");
    checkOrSay(R==5, "wrong rank in constructor");
    const int extent[] = {n0,n1,n2,n3,n4};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, int n0, int n1, int n2, int n3, int n4, int n5) //for R=6
{
    // constructor
    profileSay("rarray<T,R>::rarray(T*, int, int, int, int, int, int)");
    checkOrSay(R==6, "wrong rank in constructor");
    const int extent[] = {n0,n1,n2,n3,n4,n5};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R>
rarray<T,R>::rarray(T* buffer, const int* extent) //for any I (the only way for R>6)
{
    // constructor
    profileSay("rarray<T,R>::rarray(T*, const int*)");
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T>
rarray<T,1>::rarray(T* buffer, int n0)
{
    // constructor
    profileSay("rarray<T,1>::rarray(T*, int)");
    const int extent[] = {n0};
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T>
rarray<T,1>::rarray(T* buffer, const int* extent) //for any I (the only way for R>6)
{
    // constructor
    profileSay("rarray<T,1>::rarray(T*, const int*)");
    init_parray(buffer, extent);
    ismine_ = false;
}

template<typename T,int R> 
rarray_intermediate<T,R>::rarray_intermediate(parray_t parray, const int* extent)
  : parray_(parray),
    extent_(extent) 
{
    // constructor
    profileSay("rarray_intermediate<T,R>::rarray_intermediate(parray_t, const int*)");
}

template<typename T> 
rarray_intermediate<T,1>::rarray_intermediate(parray_t parray, const int* extent)
  : parray_(parray),
    extent_(extent)
{
    // constructor
    profileSay("rarray_intermediate<T,1>::rarray_intermediate(parray_t, const int*)");
}

// rarray<T,R> destructor (rarray_intermediate has none)

template<typename T,int R>
rarray<T,R>::~rarray()
{
    // destructor
    profileSay("rarray<T,R>::~rarray()");
    fini();
}

template<typename T>
rarray<T,1>::~rarray()
{
    // destructor
    profileSay("rarray<T,1>::~rarray()");
    fini();
}

// public cleanup routine
template<typename T,int R>
void rarray<T,R>::clear()
{
    profileSay("void rarray<T,R>::clear()");
    fini();
}

template<typename T>
void rarray<T,1>::clear()
{
    profileSay("void rarray<T,1>::clear()");
    fini();
}

// check if uninitialized
template<typename T,int R>
bool rarray<T,R>::is_clear() const
{
    profileSay("bool rarray<T,R>::is_clear()");
    return parray_ == nullptr;
}

template<typename T>
bool rarray<T,1>::is_clear() const
{
    profileSay("bool rarray<T,1>::is_clear()");
    return parray_ == nullptr;
}

// rarray<T,R> and rarray_intermediate<T,R> copy constructors

template<typename T,int R>
rarray<T,R>::rarray(const rarray<T,R> &a) 
{
    // copy constructor
    profileSay("rarray<T,R>::rarray(const rarray<T,R>&)");
    extent_ = a.extent_;
    init_shallow(a.parray_, a.extent_, a.entire_, a.rcount_);
    ismine_ = a.ismine_;
}

template<typename T,int R>
rarray<T,R>::rarray(const rarray_intermediate<T,R> &a) 
{
    // copy constructor
    profileSay("rarray<T,R>::rarray(const rarray_intermediate<T,R>&)");
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_);
}

template<typename T>
rarray<T,1>::rarray(const rarray<T,1> &a) 
{
    // copy constructor
    profileSay("rarray<T,1>::rarray(const rarray<T,1>&)"); 
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_, a.entire_, a.rcount_);
    ismine_ = a.ismine_;
}

template<typename T>
rarray<T,1>::rarray(const rarray_intermediate<T,1> &a) 
{
    // copy constructor
    profileSay("rarray<T,1>::rarray(const rarray_intermediate<T,1>&)");
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_);
}

// create a copy

template<typename T, int R>
rarray<T,R> rarray<T,R>::copy() const
{
    profileSay("rarray<T,R> rarray<T,R>::copy() const");
    if (parray_ != nullptr) { 
        // if initialized
        rarray<T,R> result(extent_);
        T* bufbegin = get_buffer();
        T* bufend = bufbegin+size();
        T* newbuf = result.get_buffer();
        std::copy(bufbegin, bufend, newbuf);
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
    if (parray_ != nullptr) { // if initialized
        //copy    
        rarray<T,1> result(extent_);
        T* bufbegin = get_buffer();
        T* bufend = bufbegin+size();
        T* newbuf = result.get_buffer();
        std::copy(bufbegin, bufend, newbuf);
        return result;
    } else {
        // else return uninitialized copy
        return rarray();
    }
}
// rarray method to retrieve array size in dimension i

template<typename T,int R>
int rarray<T,R>::extent(int i) const 
{
    profileSay("int rarray<T,R>::extent(int) const");
    checkOrSay(i >=0 and i < R, "wrong dimension");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return extent_[i];
}

template<typename T>
int rarray<T,1>::extent(int i) const 
{
    profileSay("int rarray<T,1>::extent(int) const");
    checkOrSay(i >=0 and i < 1, "wrong dimension");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return extent_[i];
}

template<typename T,int R> 
int rarray_intermediate<T,R>::extent(int i) const 
{
    profileSay("int rarray_intermediate<T,R>::extent(int) const");
    checkOrSay(i >=0 and i < R, "wrong dimension");  
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");      
    return extent_[i];
}

template<typename T> 
int rarray_intermediate<T,1>::extent(int i) const 
{
    profileSay("int rarray_intermediate<T,1>::extent(int) const");
    checkOrSay(i >=0 and i < 1, "wrong dimension");   
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");     
    return extent_[i];
}

// rarray<T,R> and rarray_intermediate<T,R> methods to retrieve array sizes in
// all dimensions

template<typename T,int R>
const int* rarray<T,R>::extents() const 
{
    profileSay("const int* rarray<T,R>::extents() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return extent_;
}

template<typename T>
const int* rarray<T,1>::extents() const 
{
    profileSay("const int* rarray<T,1>::extents() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
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
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    // multiplying the sizes in all dimensions
    int result = 1;        
    for (int i=0; i<R; i++)
        result *= extent_[i];
    return result;
}

template<typename T>
int rarray<T,1>::size() const 
{ 
    profileSay("int rarray<T,1>::size() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    // multiplying the sizes in all dimensions
    int result = 1;        
    for (int i=0; i<1; i++)
        result *= extent_[i];
    return result;
}

// ...for rarray_intermediate

template<typename T,int R> 
int rarray_intermediate<T,R>::size() const 
{
    profileSay("int rarray_intermediate<T,R>::size() const");
    // multiplying the sizes in all dimensions
    int result = 1;       
    for (int i=0; i<R; i++)
        result *= extent_[i];
    return result;
}

template<typename T> 
int rarray_intermediate<T,1>::size() const 
{ 
    profileSay("int rarray_intermediate<T,1>::size() const");
    // multiplying the sizes in all dimensions
    int result = 1;        
    for (int i=0; i<1; i++)
        result *= extent_[i];
    return result;
}

// rarray method to return pointer of type T* to the internal data

template<typename T,int R>
T* rarray<T,R>::data()
{
    profileSay("T* rarray<T,R>::data()");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return get_buffer();
}

template<typename T,int R>
const T* rarray<T,R>::data() const 
{
    profileSay("const T* rarray<T,R>::data() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return get_buffer();
}

template<typename T>
T* rarray<T,1>::data()
{
    profileSay("T* rarray<T,1>::data()");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return get_buffer();
}

template<typename T>
const T* rarray<T,1>::data() const 
{
    profileSay("const T* rarray<T,1>::data() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return get_buffer();
}

// ...for rarray_intermediate

template<typename T,int R> 
T* rarray_intermediate<T,R>::data() const
{
    profileSay("T* rarray_intermediate<T,R>::data()");
    return rarray<T,R>::base(parray_);
}

template<typename T> 
T* rarray_intermediate<T,1>::data() const
{
    profileSay("T* rarray_intermediate<T,1>::data()");
    return rarray<T,1>::base(parray_);
}

// rarray method to return T*const*.. acting similarly to this rarray
// when using []:

template<typename T,int R> 
typename rarray<T,R>::parray_t 
rarray<T,R>::ptr_array() const 
{
    profileSay("rarray<T,R>::parray_t rarray<T,R>::ptr_array() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}

template<typename T> 
typename rarray<T,1>::parray_t 
rarray<T,1>::ptr_array() const 
{
    profileSay("rarray<T,1>::parray_t rarray<T,1>::ptr_array() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}

// ...for rarray_intermediate

template<typename T,int R> 
typename rarray_intermediate<T,R>::parray_t 
rarray_intermediate<T,R>::ptr_array() const 
{
    profileSay("rarray_intermediate<T,R>::parray_t rarray_intermediate<T,R>::ptr_array() const");
    return parray_;
}
    
template<typename T> 
typename rarray_intermediate<T,1>::parray_t 
rarray_intermediate<T,1>::ptr_array() const 
{
    profileSay("rarray_intermediate<T,1>::parray_t rarray_intermediate<T,1>::ptr_array() const");
    return parray_;
}

// rarray method to return  T**.. acting similarly to this rarray when using []

template<typename T,int R>
typename rarray<T,R>::noconst_parray_t 
rarray<T,R>::noconst_ptr_array() const 
{
    profileSay("rarray<T,R>::noconst_parray_t rarray<T,R>::noconst_ptr_array() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return const_cast<noconst_parray_t>(parray_);
}

template<typename T>
typename rarray<T,1>::noconst_parray_t 
rarray<T,1>::noconst_ptr_array() const 
{
    profileSay("rarray<T,1>::noconst_parray_t rarray<T,1>::noconst_ptr_array() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return const_cast<noconst_parray_t>(parray_);
}

// ... for rarray_intermediate

template<typename T,int R> 
typename rarray_intermediate<T,R>::noconst_parray_t 
rarray_intermediate<T,R>::noconst_ptr_array() const 
{
    profileSay("rarray_intermediate<T,R>::noconst_parray_t rarray_intermediate<T,R>::noconst_ptr_array() const");
    return const_cast<noconst_parray_t>(parray_);
}

template<typename T> 
typename rarray_intermediate<T,1>::noconst_parray_t 
rarray_intermediate<T,1>::noconst_ptr_array() const 
{
    profileSay("rarray_intermediate<T,1>::noconst_parray_t rarray_intermediate<T,1>::noconst_ptr_array() const");
    return const_cast<noconst_parray_t>(parray_);
}

// rarray method to create a reference to this that treats elements as constant

template<typename T,int R>
rarray<const T,R>&  
rarray<T,R>::const_ref() const 
{
    profileSay("rarray<const T,R>& rarray<T,R>::const_ref() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return (rarray<const T,R>&)(*this);
}

template<typename T> 
rarray<const T,1>&  
rarray<T,1>::const_ref() const 
{
    profileSay("rarray<const T,1>& rarray<T,1>::const_ref() const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return (rarray<const T,1>&)(*this);
}

// ...for rarray_intermediate

template<typename T,int R>
rarray_intermediate<const T,R>& 
rarray_intermediate<T,R>::const_ref() const
{
    profileSay("rarray_intermediate<const T,R>& rarray_intermediate<T,R>::const_ref() const");
    return (rarray_intermediate<const T,R>&)(*this);
}

template<typename T>
rarray_intermediate<const T,1>& 
rarray_intermediate<T,1>::const_ref() const
{
    profileSay("rarray_intermediate<const T,1>& rarray_intermediate<T,1>::const_ref() const");
    return (rarray_intermediate<const T,1>&)(*this);
}

#ifndef SKIPINTERMEDIATE
template<typename T,int R> 
rarray_intermediate<const T,R-1> 
rarray<T,R>::operator[](int i) const
{
    profileSay("rarray_intermediate<const T,R-1> rarray<T,R>::operator[](int) const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return rarray_intermediate<const T,R-1>(parray_[i], extent_+1);
}
template<typename T,int R> 
rarray_intermediate<T,R-1> 
rarray<T,R>::operator[](int i)
{
    profileSay("rarray_intermediate<T,R-1> rarray<T,R>::operator[](int)");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return rarray_intermediate<T,R-1>(parray_[i], extent_+1);
}
#else
template<typename T,int R> 
rarray<T,R>::operator typename PointerArray<const T,R>::type () const 
{
    profileSay("rarray<T,R>::operator typename PointerArray<const T,R>::parray_t () const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
template<typename T,int R> 
rarray<T,R>::operator typename PointerArray<T,R>::type ()
{
    profileSay("rarray<T,R>::operator typename PointerArray<T,R>::type ()");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
#endif

#ifndef SKIPINTERMEDIATE
template<typename T> 
const T& rarray<T,1>::operator[](int i) const 
{
    profileSay("const T& rarray<T,1>::operator[](int) const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}
template<typename T> 
T& rarray<T,1>::operator[](int i)
{
    profileSay("T& rarray<T,1>::operator[](int)");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}
#else
template<typename T> 
rarray<T,1>::operator typename PointerArray<const T,1>::type () const 
{
    profileSay("rarray<T,1>::operator PointerArray<const T,1>::type () const");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
template<typename T> 
rarray<T,1>::operator typename PointerArray<T,1>::type ()
{
    profileSay("rarray<T,1>::operator typename PointerArray<T,1>::type ()");
    checkOrSay(parray_!=nullptr, "attempt at using uninitialized rarray");
    return parray_;
}
#endif

// rarray_intermediate methods for element access

template<typename T,int R>
rarray_intermediate<T,R-1> 
rarray_intermediate<T,R>::operator[](int i) const
{
    profileSay("rarray_intermediate<T,R-1> rarray_intermediate<T,R>::operator[](int)");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");        
    return rarray_intermediate<T,R-1>(parray_[i], extent_+1);
}

template<typename T>
T& rarray_intermediate<T,1>::operator[](int i) const
{
    profileSay("T& rarray_intermediate<T,1>::operator[](int)");
    checkOrSay(i >=0 and i < extent_[0], "wrong index");
    return parray_[i];
}

// rarray method to assignment operators

template<typename T,int R>
rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R> &a) 
{
    profileSay("rarray<T,R>& rarray<T,R>::operator=(const rarray<T,R>&)");
    if (&a != this) {
        fini();
        extent_ = const_cast<int*>(a.extent_);
        init_shallow(a.parray_, a.extent_, a.entire_, a.rcount_);
        ismine_ = a.ismine_;
    }
    return *this;
}

template<typename T>
rarray<T,1>& rarray<T,1>::operator=(const rarray<T,1> &a) 
{
    profileSay("rarray<T,1>& rarray<T,1>::operator=(const rarray<T,1>&)");
    if (&a != this) {
        fini();
        extent_ = const_cast<int*>(a.extent_);
        init_shallow(a.parray_, a.extent_, a.entire_, a.rcount_);
        ismine_ = a.ismine_;
    }
    return *this;
}

template<typename T,int R>
rarray<T,R>& rarray<T,R>::operator=(const rarray_intermediate<T,R> &a) 
{
    profileSay("rarray<T,R>& rarray<T,R>::operator=(const rarray_intermediate<T,R>&)");
    fini();
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_);
    return *this;
}

template<typename T>
rarray<T,1>& rarray<T,1>::operator=(const rarray_intermediate<T,1> &a) 
{
    profileSay("rarray<T,1>& rarray<T,1>::operator=(const rarray_intermediate<T,1>&)");
    fini();
    extent_ = const_cast<int*>(a.extent_);
    init_shallow(a.parray_, a.extent_);
    return *this;
}

// rarray private method to get start of current contiguous buffer

template<typename T,int R>
T* rarray<T,R>::get_buffer() const 
{ 
    profileSay("T* rarray<T,R>::get_buffer()");
    return base(parray_); 
}

template<typename T>
T* rarray<T,1>::get_buffer() const 
{ 
    profileSay("T* rarray<T,1>::get_buffer()");
    return base(parray_); 
}

// init functions 

template<typename T,int R>
void rarray<T,R>::init_shallow(parray_t    parray,
                               const int*  extent,
                               bool        entire,
                               int*        rcount)
{
    profileSay("void rarray<T,R>::init_shallow(parray_t, const int*, bool, int*)");
    checkOrSay(      extent != nullptr, "null pointer");
    checkOrSay(      parray != nullptr, "null pointer");
    checkOrSay(base(parray) != nullptr, "null pointer");
    rcount_ = rcount;
    parray_ = parray;
    entire_ = entire;
    for (int i=0;i<R;i++)
        extent_[i] = extent[i];
    if (entire_)
        (*rcount_)++;
}

template<typename T,int R>
void rarray<T,R>::init_shallow(parray_t parray, const int* extent)
{
    profileSay("void rarray<T,R>::init_shallow(parray_t, const int*)");
    checkOrSay(      extent != nullptr, "null pointer");
    checkOrSay(      parray != nullptr, "null pointer");
    checkOrSay(base(parray) != nullptr, "null pointer");
    parray_ = parray;
    rcount_ = nullptr;
    entire_ = false;
    for (int i=0;i<R;i++)
        extent_[i] = extent[i];
}

template<typename T,int R>
void rarray<T,R>::init_parray(T* buffer, const int* extent)
{
    profileSay("void rarray<T,R>::init_parray(T*, const int*)");
    checkOrSay(buffer != nullptr, "null pointer");
    checkOrSay(extent != nullptr, "null pointer");
    parray_t parray = new_except_base(buffer, extent);
    extent_ = new int[R];
    init_shallow(parray, extent, true, new int(0));
}

template<typename T,int R>
void rarray<T,R>::init_data(const int* extent, int extenttot)
{
    profileSay("void rarray<T,R>::init_data(const int*, int)");
    checkOrSay( extent != nullptr, "null pointer");
    checkOrSay( extenttot >= 0, "negative number of elements");
    T* buffer = new T[extenttot];
    init_parray(buffer, extent);
    ismine_ = true;
}

template<typename T>
void rarray<T,1>::init_shallow(parray_t    parray,
                               const int*  extent,
                               bool        entire,
                               int*        rcount)
{
    profileSay("void rarray<T,1>::init_shallow(parray_t, const int*, bool, int*)");
    checkOrSay(      extent != nullptr, "null pointer");
    checkOrSay(      parray != nullptr, "null pointer");
    checkOrSay(base(parray) != nullptr, "null pointer");
    rcount_ = rcount;
    parray_ = parray;
    entire_ = entire;
    for (int i=0;i<1;i++)
        extent_[i] = extent[i];
    if (entire_ and rcount != nullptr)
        (*rcount_)++;
}

template<typename T>
void rarray<T,1>::init_shallow(parray_t parray, const int* extent)
{
    profileSay("void rarray<T,1>::init_shallow(parray_t, const int*)");
    checkOrSay(      extent != nullptr, "null pointer");
    checkOrSay(      parray != nullptr, "null pointer");
    checkOrSay(base(parray) != nullptr, "null pointer");
    parray_ = parray;
    rcount_ = nullptr;
    entire_ = false;
    for (int i=0;i<1;i++)
        extent_[i] = extent[i];
}

template<typename T>
void rarray<T,1>::init_parray(T* buffer, const int* extent)
{
    profileSay("void rarray<T,1>::init_parray(T*, const int*)");
    checkOrSay(buffer != nullptr, "null pointer");
    checkOrSay(extent != nullptr, "null pointer");
    extent_ = new int[1];
    parray_t parray = new_except_base(buffer, extent);
    init_shallow(parray, extent, true, new int(0));
}

template<typename T,int R>
void rarray<T,R>::reshape(const int* extent)
{
    profileSay("void rarray<T,R>::reshape(const int* extent)");
    checkOrSay(parray_ != nullptr and entire_ and *rcount_==1, "reshape not allowed on referenced object");
    int tot1 = 1, tot2 = 1;
    for (int i=0;i<R;i++) {
        tot1 *= extent_[i];
        tot2 *= extent[i];
    }
    checkOrSay(tot2 <= tot1, "reshaping beyond underlying memory buffer");
    if (parray_ != nullptr and entire_) { // cannot reshape subarrays
        if (*rcount_==1) {
            T* buffer = get_buffer();
            delete[] parray_;
            for (int i=0;i<R;i++)
                extent_[i] = extent[i];
            parray_ = new_except_base(buffer, extent_);
        } else { // if this has other references to it, create a new view
            T* buffer = get_buffer();    // get buffer address
            ismine_ = false;             // protect buffer from being deleted
            fini();                      // deallocate everything else
            init_parray(buffer, extent); // reallocate pointer arrays
        }
    }
}

template<typename T,int R>
void rarray<T,R>::reshape(int n0, int n1) //for R=2
{
    // constructor
    profileSay("void rarray<T,R>::reshape(int, int)");
    checkOrSay(R==2, "wrong rank in reshape");
    checkOrSay(n0!=0 and n1!=0, "zero extents not allowed");
    const int extent[] = {n0,n1};
    reshape(extent);
}

template<typename T,int R>
void rarray<T,R>::reshape(int n0, int n1, int n2) //for R=3
{
    // constructor
    profileSay("void rarray<T,R>::reshape(int, int, int)");
    checkOrSay(R==3, "wrong rank in reshape");
    checkOrSay(n0!=0 and n1!=0 and n2!=0, "zero extents not allowed");
    const int extent[] = {n0,n1,n2};
    reshape(extent);
}

template<typename T,int R>
void rarray<T,R>::reshape(int n0, int n1, int n2, int n3) //for R=4
{
    // constructor
    profileSay("void rarray<T,R>::reshape(int, int, int, int)");
    checkOrSay(R==4, "wrong rank in reshape");
    checkOrSay(n0!=0 and n1!=0 and n2!=0 and n3!=0, "zero extents not allowed");
    const int extent[] = {n0,n1,n2,n3};
    reshape(extent);
}

template<typename T,int R>
void rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4) //for R=5
{
    // constructor
    profileSay("void rarray<T,R>::reshape(int, int, int, int, int)");
    checkOrSay(R==5, "wrong rank in reshape");
    checkOrSay(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0, "zero extents not allowed");
    const int extent[] = {n0,n1,n2,n3,n4};
    reshape(extent);
}

template<typename T,int R>
void rarray<T,R>::reshape(int n0, int n1, int n2, int n3, int n4, int n5) //for R=6
{
    // constructor
    profileSay("void rarray<T,R>::reshape(int, int, int, int, int, int)");
    checkOrSay(R==6, "wrong rank in reshape");
    checkOrSay(n0!=0 and n1!=0 and n2!=0 and n3!=0 and n4!=0 and n5!=0, "zero extents not allowed");
    const int extent[] = {n0,n1,n2,n3,n4,n5};
    reshape(extent);
}

template<typename T>
void rarray<T,1>::init_data(const int* extent, int extenttot)
{
    profileSay("void rarray<T,1>::init_data(const int*, int)");
    checkOrSay(extent != nullptr, "null pointer");
    checkOrSay(extenttot >= 0, "negative number of elements");
    T* buffer = new T[extenttot];
    init_parray(buffer, extent);
    ismine_ = true;
}

template<typename T>
void rarray<T,1>::reshape(const int* extent)
{
    profileSay("void rarray<T,1>::reshape(const int* extent)");
    checkOrSay(parray_ != nullptr and entire_ and *rcount_==1, "reshape not allowed on referenced object");
    checkOrSay(*extent <= *extent_, "reshaping beyond underlying memory buffer");
    if (parray_ != nullptr and entire_) {
        if (*rcount_==1) {
            *extent_ = *extent;
        } else {
            T* buffer = get_buffer();    // get buffer address        
            ismine_ = false;             // protect buffer from being deleted
            fini();                      // deallocate everything else
            init_parray(buffer, extent); // reallocate pointer arrays
        }
    }
}

template<typename T>
void rarray<T,1>::reshape(int n0) //for R=1
{
    // constructor
    profileSay("void rarray<T,1>::reshape(int)");
    checkOrSay(R==1, "wrong rank in reshape");
    checkOrSay(n0!=0, "zero extents not allowed");
    const int extent[] = {n0};
    reshape(extent);
}

//  rarray private cleanup routine

template<typename T,int R>
void rarray<T,R>::fini() 
{
    profileSay("void rarray<T,R>::fini()");
    if (parray_ != nullptr and entire_) {
        (*rcount_)--;
        if (*rcount_==0) {
            if (ismine_) 
                delete[] get_buffer(); 
            delete[] parray_;
            delete[] extent_;
            delete rcount_;
        }
    }
    rcount_ = nullptr;
    parray_ = nullptr;
}

template<typename T>
void rarray<T,1>::fini() 
{
    profileSay("void rarray<T,1>::fini()");
    if (parray_ != nullptr and entire_) {
        (*rcount_)--;
        if (*rcount_==0) {
            if (ismine_)
                delete[] get_buffer();
            delete[] extent_;
            delete rcount_;
        }
    }
    rcount_ = nullptr;
    parray_ = nullptr;
}

// rarray private method to allocate the chain of pointers, except the base

template<typename T,int R>
typename rarray<T,R>::parray_t 
rarray<T,R>::new_except_base(T* buffer, const int* extent) 
{
    profileSay("parray_t rarray<T,R>::new_except_base(T*, const int*)");

    if (R>1) {

        typedef typename Unconst<T>::type* bufstart_t;
        
        bufstart_t bufstart = const_cast<bufstart_t>(buffer);
        int        nalloc   = 0;
        char**     result;
        
        for (int i=R-1; i--; )
            nalloc = extent[i]*(1+nalloc);
        
        char**  palloc = new char*[nalloc];        
        int     extenttot = 1;
        char*** ptr_array    = reinterpret_cast<char***>(&result);
        
        for (int i=0; i<R-1; i++) {
            
            for (int j=0; j<extenttot; j++)
                ptr_array[j] = palloc + j*extent[i];
            
            extenttot *= extent[i];
            if (extenttot==0) {
                break; // really should not be allowed and give an error
            } else {
                ptr_array     = reinterpret_cast<char***>(*ptr_array);
                palloc += extenttot;
            }
        }
        
        for (int j=0; j<extenttot; j++)
            ptr_array[j] = reinterpret_cast<char**>(bufstart + j*extent[R-1]);
        
        return reinterpret_cast<parray_t>(result);
        
    } else {
        
        return reinterpret_cast<parray_t>(buffer);
        
    }
}

template<typename T>
typename rarray<T,1>::parray_t 
rarray<T,1>::new_except_base(T* buffer, const int* extent) 
{
    profileSay("parray_t rarray<T,1>::new_except_base(T*, const int*)");
    return reinterpret_cast<parray_t>(buffer);
}

// rarray private method to find base of a chain of pointers

template<typename T,int R>
T* rarray<T,R>::base(parray_t parray) 
{
    profileSay("T* rarray<T,R>::base(parray_t)");
    if (parray != nullptr) {
        const char* result1 = reinterpret_cast<const char*>(parray);
        char* result2 = const_cast<char*>(result1);
        char** result = reinterpret_cast<char**>(result2);    
        for (int i=0; i < R-1; i++)
            result = reinterpret_cast<char**>(*result);
        return reinterpret_cast<T*>(result);
    } else
        return nullptr;
}

template<typename T>
T* rarray<T,1>::base(parray_t parray)
{
    profileSay("T* rarray<T,1>::base(parray_t)");
    const char*  result1 = reinterpret_cast<const char*>(parray);
          char*  result2 = const_cast<char*>(result1);
          char** result  = reinterpret_cast<char**>(result2);
    return reinterpret_cast<T*>(result);
}

// get rid of the macros
#undef profileSay
#undef checkOrSay
// 

#endif

// Local Variables:
// mode: C++
// End:
