
#include "rarray.h"
#include <cassert>

int main () 
{
    // Create a 3D array that is 3 x 4 x 2
    int dim[] = {3,4,2};
    rarray<double,3> A(dim);

    // Assign values to the elements
    int values = 0;
    const rarray<const int,1> getdim(A.extents(),3);
    for(int i = 0; i != getdim[0]; ++i) 
        for(int j = 0; j != getdim[1]; ++j)
            for(int k = 0; k != getdim[2]; ++k)
                A[i][j][k] = values++;
    
    // Verify values
    int verify = 0;
    for(int i = 0; i != 3; ++i) 
        for(int j = 0; j != 4; ++j)
            for(int k = 0; k != 2; ++k)
                assert(A[i][j][k] == verify++);
    
    // int num = 0;
    // for (A::element_iterator i = A.begin(); i != A.end(); ++i)
    //     *i = num++;

    // for (A::subarray_iterator i = A.beginsubarray(); i != A.endsubarray(); ++i)
    //     for ((*i)::subarray_iterator j = (*i).beginsubarray(); j != (*i).endsubarray(); ++j)
    //         for ((*j)::subarray_iterator k = (*j).beginsubarray(); k != (*j).endsubarray(); ++k)
    //             *k = num++;

    return 0;
}

// things perhaps to add?
//
// types:
//
//   value_type              rarray<T,R-1> or T&
//   size_type               int
//   difference_type         int
//   iterator                T*
//   const_iterator          const T*
//   reverse_iterator        backwards_pointer<T*>
//   const_reverse_iterator  backwards_pointer<T*>
//   element                 T
