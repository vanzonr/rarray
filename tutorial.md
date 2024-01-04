# Rarray Tutorial

One-dimensional arrays are a collection of elements that are all of
the same type and are arranged in a linear fashion, so each element
can be refered to with a single (integer) index.

Multidimensional arrays also have elements that are all of the same
type, but that are indexed using multiple indices.  The number of
indices is called the rank of the array.  Each of the indices is said
to be a dimension, so the rank can also be defined as the number of
dimensions.

E.g. a multidimensional array of rank two is a two-dimensional array.
It could be represented in tabular form and is therefore also sometimes
called a table or a matrix.  It has elements that are referenced
using two indices, which one may call the row and the column indices.

The possible values of the index of each dimension is independent of
the indices in the other dimensions, so multidimensional arrays
have a multidimensional rectangular shape.

*Note: The idea of elements arranged in a certain shape is a much more useful way to think about multidimensional arrays than the idea of arrays-of-arrays that one often sees in many programming contexts. For instance, a three-dimensional array that represents the temperature on a 3D grid of points could be viewed as arrays-of-arrays would be an array of layers, each layer as an array of linear subsets of grid points, but this division has no physical relevance.*

The rarray library provides dynamically allocatable multidimensional
arrays for C++ that can be shared among different parts of an
application.

## Getting Started

You can use the rarray library with "#include <rarray>".  Here,
rarray is its header file. In fact, this is the only thing you need to
do, as rarray is a header-only library.

## Creating Rarrays

The library provides a template class `rarray<T,R>`, where T is the
type of the elements are R is the rank.  To define a multidimensional
array, use rarray<T,R> as the typename with the elements' type
substituted for T and the required rank substituted for R.  For instance, the following defines a two-dimensional array of integers:
```cpp
rarray<double,2> a;
```
This array does not have a shape yet (or, said differently, its shape is 0x0). To define this array with a given shape, e.g., to make this a matrix with 4 rows and 5 columns, you should specify the extent of the array in each dimension:
```cpp
rarray<double,2> a(4, 5);
```
This works for rarrays of rank up to 11. For rarrays with higher rank, you have to pass an pointer to the array of extents.

Thw array `a` has a shape now, but its elements are not initialized.
Uninitialized values are bad practice, but there are too many ways to
do this initialization to be able to cover all of them during
construction of the rarray.  One should aim to initialize the elements
of an rarray in the code **shortly after** the definition of the
rarray.

One way to initialize the all the values of an rarray at once is with
the `fill` method.  Uniform initialization with the same value (e.g., one) can be done with the fill method with a single argument:
```cpp
rarray<double,2> a(4, 5);
a.fill(1.0);
```
To check that this indeed happened, one can printi the result.  Rarrays can be used with streams, so the following program:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> a(4, 5);
    a.fill(1.0);
    std::cout << a << '\n';
}
```
will print the array, in the following format:
```
{
{1,1,1,1,1},
{1,1,1,1,1},
{1,1,1,1,1},
{1,1,1,1,1}
}
```
This format with the curly braces is chosen as it is unique and can
therefore be correcly read in again by input streams.

A second way to set the values of an array at once is using a similar
curly braces format as the argument of the fill method:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> b(4, 5);
    b.fill({
        { 1.0,  2.0,  3.0,  4.0,  5.0},
        { 6.0,  7.0,  8.0,  9.0, 10.0},
        {11.0, 12.0, 13.0, 14.0, 15.0},
        {16.0, 17.0, 18.0, 19.0, 20.0}
        });
    std::cout << b << '\n';
}
```

Similar to how automatic multidimensional arrays are initialized in C,
if there are missing elements, those spots will be assigned the
default value corresponding to the type (which amounts to 0 for
numerical types).

Two other ways to deal with missing elements can be
specified as well.  One is to repeat the pattern that was given, e.g.
```cpp
rarray<double,2> matrix(4, 4);
matrix.fill({{1.0, 2.0}, {3.0, 4.0}}, ra::MISSING::REPEAT);
std::cout << matrix << '\n';
```
will give
```
{{1,2,1,2}
 {3,4,3,4},
 {1,2,1,2},
 {3,4,3,4}}
```
The other way to deal with missing elements is to keep what ever value was there before. E.g.
```cpp
rarray<double,2> matrix(4, 4);
matrix.fill(5.0);
matrix.fill({{1.0, 2.0}, {3.0, 4.0}}, ra::MISSING::SKIP);
std::cout << matrix << '\n';
```
will give
```cpp
{{1,2,5,5}
 {3,4,5,5},
 {5,5,5,5},
 {5,5,5,5}}
```
The default way of zeroing missing elements that one gets by no using
a `ra::MISSING` argument can also be established by
passing `ra::MISSING::DEFAULT` as the second argument to the
`fill` method.

The fill methods cannot change the shape of the rarray, but another
method, `form`, can.  The following code creates the same rarrays `a`
and `b` as above, but uses the `form` method:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> a;  // 2D arrays without size or shape.
    rarray<double,2> b;
    a.form(4, 5, 1.0);   // Form array with 4x5 elements, all set to 1.
    b.form({             // Form array that fits the given nested lists
        { 1.0,  2.0,  3.0,  4.0,  5.0},
        { 6.0,  7.0,  8.0,  9.0, 10.0},
        {11.0, 12.0, 13.0, 14.0, 15.0},
        {16.0, 17.0, 18.0, 19.0, 20.0}
        });
    std::cout << a << '\n'
              << b << '\n';
}
```

Similar to the situation with the `fill` method, if there are missing
elements, those spots will be assigned the default value corresponding
to the type (often 0). Thus, one can for instance create a 3x3 matrix
of zeros with
```
rarray<double,2> matrix;
matrix.form({{}, {}, {0.0, 0.0, 0.0}};
```
One can also specify other
ways to deal with missing elements.  E.g.
```
rarray<double,2> matrix;
matrix.form({{1.0, 2.0, 3.0}, {} ,{}, ra::MISSING::REPEAT};
```
creates an array with the value of 1 in the first column, 2 in the second column
and 3 in the third column, while
```
rarray<double,2> matrix;
matrix.form({{1.0, 1.0, 1.0}, {2.0}, {3.0}, ra::MISSING::REPEAT};
```
creates an array with 1s in the first row, 2s in the second row
and 3s in the third row. Because the size of the array is determined
from the nested expression, at least on row must be fully specified.

When using `ra::MISSING::SKIP` instead of `ra::MISSING::REPEAT`, the
missing elements are not initialized.

Finally, to undo any initialization, one can remove the data and shape
from an rarray with the `clear()` method.  This returns the rarray to
the state as if it were defined without any arguments.

## Rarray Indexing

If the number of values to set is large and not uniform, you will
likely have to set the elements one-by-one in a loop. Each element of
the array can be referenced using repeated square brackets (exactly as
native C-style arrays would be).  As is usual in C/C++, indexing is
zero based.  So `a[1][2]` is the element in the second row at the
third columm. The initialization above can thus
also be done as follows:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> a(4, 5);
    int k = 1;
    for (int i = 0; i < 4; i++) 
        for (int j = 0; j < 5; j++)
            a[i][j] = (double)(k++);
    std::cout << a << '\n';
}
```
We should note that when using a C++23 compliant compiler, rarray
supports the notation `a[i,j]` instead of `a[i][j]`, but because
generally, rarray only requires C++11 compliance, this tutorial will
use the repeat brackets notation.

There is another way to loop over the elements of an rarray using
range-based for.  Using a range-based for loop will loop over the
elements, so the same initialization can be written as follows:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> a(4, 5);
    int k = 1;
    for (auto& element : a)
        element = (double)(k++);
    std::cout << a << '\n';
}
```
Note that the range-based loop does not iterate over rows within which
one needs to iterate over elements of the row, rather it directly
iterates over the content of the array.

# Rarray Slicing

By design, the elements of an rarray are always stored contiguously in
memory (i.e., without gaps).  This ensures that they can be used in
calls to many numerical software libraries.  But this does restrict
the possibilities in which one can take subarrays of an rarray and
still store it in an rarray.

To understand what kind of slicing is and is not possible with
rarrays, it is useful to keep in mind that it stores elements in *row
major* format.  This means that elements with successive values of the
right-most index are contiguous in memory.  As a result, rarrays can
only be sliced in the first dimension.

An rarray has two slightly different slicing methods: `at` and `slice`.

## The at method

The `at` method takes a single index and produces an rarray of one
rank lower containing all elements that have that index for the first
dimension, while retaining the shape of the remaining dimensions.
E.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,3> a;
    a.form({
            {
             {1.0, 2.0},
             {3.0, 4.0}
            },
            {
             {5.0, 6.0},
             {7.0, 8.0}
            }
            {
             {9.0, 10.0},
             {11.0, 12.0}
            }
           });
    std::cout << a.at(1) << '\n';
}
```
prints:
```
{
{5,6},
{7,8}
}
```
The expression `a.at(i)` method is similar to `a[i]`,
but the former checks that `i` is a valid index and produces an
rarray, whereas `a[i]` is an internal intermediate expression.

## The slice method

This method allows to select successive indices for the first index.
It always results in an rarray of the same rank as the original
rarray. The `slice` method takes two indices as argument, one is the
starting value for the first index, and the next is the last value of
the first index plus one (the plus one is for consistency with other
parts of C++). E.g.,

```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,3> a;
    a.form({
            {
             {1.0, 2.0},
             {3.0, 4.0}
            },
            {
             {5.0, 6.0},
             {7.0, 8.0}
            }
            {
             {9.0, 10.0},
             {11.0, 12.0}
            }
           });
    std::cout << a.slice(1,3) << '\n';
}
```
prints:
```
{
{
{5,6},
{7,8}
},
{
{9,10},
{11,12}
}
}
```
Note that `a.slice(1,2)` would contain the same elements as `a.at(1)`,
but the former has an additional first dimension with extent 1
compared to the latter.

Let us mention once more that slicing in anything but the first index
is not possible because it would require non-contiguous memory access
of the elements.

# Rarray Copying and Reference Counting

The slices discussed above are not independent rarrays; they are just
*views* on the same rarray.  Rarrays have an internal counter called a
reference counter that stores how many views are in existance.  Only
when no references are left (i.e., views or the original rarray), then
any memory taken by the array will be released.

A similar thing happens when we use assignment of rarrays. E.g. in
```cpp
rarray<double,2> a(1000,1000);
rarray<double,2> b = a;
```
By the assignment, the rarray `b` is simply a reference to the same
data, with the same shape, as `a`.  This avoid unnecessary copying of
the data.  The following code demonstrates that `a` and `b` are the
same object:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> a(1000,1000);    
    rarray<double,2> b = a;
    a.fill(1.0);
    std::cout << a[500][500] << ',';
    b.fill(2.0);
    std::cout << a[500][500] << '\n';
}
```
which will print `1,2`, not `1,1`.

The same happens in function calls, with rarray parameters passed "by
value", e.g.
```cpp
void f(rarray<double> b) {...}
int main() {
   rarray<double,2> a(1000,1000);
   f(a);
}
```
would not copy the rarray's data into the argument b, but instead
create an additional reference to it.

Of course, for functions, we could also pass by reference using a C++
reference, with e.g. `void f(rarray<double> &b)`.  But that would not
increase the reference count, as `b` is literally the same as `a`.
This matters in some cases.  For instance, if the function `f` would
store a reference to `b` and keep this stored after the function
returns, then reference counting would protect this reference from
becoming a dangling reference when the original `a` gets destroyed or
`clear`-ed.  Another case where using the pass-by-value reference
counting helps is if the rarray needs to be reshaped inside the
function `f` (see below).

And then there are cases where counted references are not desired,
i.e., where we truly want an independent rarray `b` with elements
copied from `a`.  For those cases, there is the `copy` method, which
produces just such an independent copy. E.g:
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> a(1000,1000);    
    rarray<double,2> b = a.copy();
    a.fill(1.0);
    std::cout << a[500][500] << ',';
    b.fill(2.0);
    std::cout << a[500][500] << '\n';
}
```
which will print `1,1`, as the filling of `b` did not affect `a`.

# Rarray Properties

An rarray object has a number of methods to query its properties.

## rank

This method returns the rank of the rarray (eventhough this is already
encoded in its type). E.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,4> a(10, 10, 4, 4);
    std::cout << "rank = " << a.rank() << '\n';
}
```
prints "`rank = 4`".

## shape

This method returns the sizes in each dimension, e.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,4> a(10, 10, 4, 4);
    auto shape = a.shape();
    std::cout << "shape ="
    for (int i = 0; i < a.rank()) 
       std::cout << ' '  << shape[i];
}
```
prints "`shape = 10 10 4 4`".

This method returns a C pointer in the current version, but this may change in the future.

The shape method can be useful to define a second array with the same shapes as an existing rarray, without having to make a copy. E.g.
```cpp
rarray<double,4> a(10, 10, 4, 4); // our first rarray
rarray<double,4> b(a.shape()); // independent rarray with same shape as a
```

## extent

This method take an integer parameter `i` and returns the size of the `i`th dimension. E.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,4> a(10, 10, 4, 4);
    std::cout << "extent(i=0..3) ="
    for (int i = 0; i < a.rank()) 
       std::cout << ' '  << extent(i);
}
```
prints "`extent(i=0..3) = 10 10 4 4`".

Note that there is a size method as well (see below),

## size

This method returns the total number of elements in the rarray.  It is equal to the product of the extents. E.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,4> a(10, 10, 4, 4);
    std::cout << "size = " << a.size();
}
```
prints "`size = 1600`".

## data

This method returns a pointer to the first element of the rarray. Very
useful for interfacing with low-level routines or libraries with a
C-like API.

E.g. to write the content of a 10x10x4x4 array of zeros in binary format to a file "adump.bin", one could use the following code:
```cpp
#include <rarray>
#include <fstream>
int main() {
    rarray<double,4> a(10, 10, 4, 4);
    a.fill(0.0);
    ofstream f("adump.bin","wb");
    f.write((char*)a.data(), a.size()*sizeof(double));
    f.close();
}
```

## empty

This method checks if the rarray is empty. I.e., it returns true if
the rarray has not been initialized with dimensions, or has been
uninitialized with the clear method.

E.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,4> a; // no shape
    std::cout << "A:" << a.empty() << ' ';
    a.form(10,10,4,4, 0); // give shape and fill with zeros
    std::cout << "B:" << a.empty() << ' ';
    a.clear(); // uninitialze
    std::cout << "C:" << a.empty() << '\n';
}
```
prints "A:0 B:1 C:0".  Note that an array filled with zeros does not count as empty.

# Reshaping

To use the data in an rarray but access it in a different shape, one can use the `reshape` method.  E.g.
```cpp
rarray<double,4> a(10, 10, 4, 4);
a.fill(1.0);
a.reshape(5, 5, 8, 8);
```
This preserves the elements. Keep in mind that the elements are stored
contiguously in row-major order when figuring out which elements end
up where in the new shape.

The number of elements in the new shape should be the same as in the
old shape.  This way, `reshape` can guarantee not to allocate more
memory.  It is also possible to reshape ending up with less elements by
adding the ra::RESIZE::ALLOWED flag, e.g.
```cpp
a.reshape(5, 5, 5, 5, ra::RESIZE::ALLOWED);
```

If reference-counted copies of the reshaped array exist, these will
not be reshaped when the `reshape` method is used.  The reshape method
stores a new shape in the rarray, while the copies contain the old
shape.  The value of that data stored in those rarrays is still
shared, even though their shapes now differ.  This makes it possible
to resize an rarray inside a function without affecting external
copies, e.g.
```cpp
#include <rarray>
#include <iostream>
void printflat(rarray<double,2> a) {
   a.reshape(1, a.size());
   for (const auto& x : a.at(0))
      std::cout << x << ' ';
}
int main() {
   rarray<double,2> a;
   a.form({{1.0, 2.0}, {3.0, 4.0}});
   printflat(a);
   std::cout << '\n' << a << '\n';
}
```
which prints:
```
1 2 3 4
{
{1,2},
{3,4}
}
```
This is one situation where pass-by-reference of the rarray would not
work, as this would change the shape of the original rarray.

The `reshape` method cannot change the rank of the rarray.  To
reshape into a lower rank, one can combine `reshape` with the `at` method,
e.g. to flatten a 4D rarray, one could use the following code:
```cpp
rarray<double,4> a(10, 10, 4, 4);
a.fill(1.0);
a.reshape(1, 1, 1, a.size());
rarray<double,1> aflat = a.at(0).at(0).at(0);
```
Reshaping into a higher rank requires getting the pointer to the data out and using it in a new rarray, e.g. 
```cpp
rarray<double,1> aflat(1600);
aflat.fill(1.0);
rarray<double,4> a(aflat.data(), 10, 10, 4, 4);
```
Because this technique to reshape into a higher rank breaks the reference counting mechanism, only use it if you really need to.

# Iterating and index methods

Rarray supports iterators similar to standard C++ containers. However, these iterators traverse the elements of the rarray, in row-major order.  They do not iterate over the rows.

## begin/end

These standard iterator calls give the pointer to the first element and a pointer just passed the last, and can be used in the for-loop standard construction:
```cpp
rarray<double,4> a(10, 10, 4, 4);
a.fill(1.0);
int sum = 0;
for (auto iter = a.begin(); iter != a.end(); iter++)
   sum += *iter;
std::cout << "sum = " << sum << '\n';
```

It is due to these iterator method that the range-based for loop works:
```cpp
rarray<double,4> a(10, 10, 4, 4);
a.fill(1.0);
int sum = 0;
for (auto x : a)
   sum += a;
std::cout << "sum = " << sum << '\n';
```

## cbegin/cend

These are iterators explicitly to constant elements, and are usuful in
certain template programming situations.

## index

The index methods can get the indices associated with a element of the array. It takes a reference to the element and the dimension in which the index is required. E.g.
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,1> v;
    v.form({0.1,0.2,0.3,0.4});
    for (auto& x : v)
       std::cout << '[' << v.index(x,0) << "]=" << x << ' ';
}
```
prints '[0]=0.1 [1]=0.2 [2]=0.3 [3]=0.4'.

Another version of the index method takes only the element as a reference and returns the array of indices. For example,
```cpp
#include <rarray>
#include <iostream>
int main() {
    rarray<double,2> m;
    m.form({{0.1,0.2},{0.3,0.4}});
    for (auto& x : m) {
       auto indexpair = m.index(x);
       std::cout << '[' << indexpair[0] << ','
                 << indexpair[1] << "]="
                 << x << ' ';
}
```
prints '[0,0]=0.1 [0,1]=0.2 [1,0]=0.3 [1,1]=0.4'

The index methods are provided as a convenience, but require a
computation based on offsets, and are not exactly cheap.  If you
already know the indices of an elements in another way, using those
values would be recommended.

# Auxiliary functions

## rvector, rmatrix, rtensor

For convenience, rarray defines shortcut types for one-dimensional,
two dimensional and three dimensional arrays, called rvector, rmatrix
and rtensor, respectively. The following equivalences hold:
```cpp
rvector<T> = rarray<T,1>
rmatrix<T> = rarray<T,2>
rtensor<T> = rarray<T,3>
```
for any type `T`.

## xrange

The xrange function returns an iterable generator, which can produce
numbers from a starting point, with some step size, up to but not
including an end point.  In the simplest case, this can be used as a
counter, e.g.:
```cpp
#include <iostream>
#include <rarray>
int main() {
    for (int i: xrange(1000))
        std::cout << i << " ";
    std::cout << std::endl;
}
```
will print the numbers 0 to 999, which are generated in succession,
i.e., no integer array of size 1000 containing these numbers is ever
created. The return type of the xrange function is an
`ra::Xrange<T>` object, with `T` equal to `int`
in the above example.  This is an iterable object that can be used in
a range-based for.  It could also be used as the source of a copy, and
thus to initialize an rarray, e.g. you could get an array with values
`0,2,4,...,998`, as follows
```cpp
#include <rarray>
#include <algorithm>
int main() {
    ra::Xrange<int> in = xrange(0,1000,2);
    rvector<int> r(500);
    std::copy(in.begin(), in.end(), r.begin());
    ...
}
```
(replacing the type `ra::Xrange<int>` with `auto` would be a
good idea here.)

The general form of the range function is
```cpp
ra::Xrange<T> xrange(T endvalue);
ra::Xrange<T> xrange(T beginvalue, T endvalue, T stepsize=1);
```
In the first form, the begin value is 0 and the stepsize is 1. These
are template functions, where `T` can be any type that can be converted
to a double, although the most useful cases are those where T is an
integer.  In all cases, the first generated value is exactly
`beginvalue`, each subsequent value is one `stepsize` larger, and
the `endvalue` is the first value that is not generated (to be precise,
the number of numbers generated is
`ceil((beginvalue-endvalue)/stepsize)`, using floating point
division).

The case where `endvalue` is less than `beginvalue`, or
for which `stepsize` is negative, are undefined.

## linspace

The `linspace` function returns an rvector with a specified
number of points between two given values, inclusively.  The general
form of the function is
```cpp
rvector<T> linspace(T x1, T x2, int n, bool end_incl=true);
```
Here, `x1` is the first value, `x2` the
last value, and `n` is the number of values.  If
the latter is not given or is set to zero, the number of values is
such that the `stepsize` is as close to one as possible. If
`end_incl` is set to false, the generated values are such as if
the number of points is `n+1` but the last value is omitted.

For example, to create an rvector with 101 equally spaced
values between -1.0 and 1.0, one would do
```cpp
#include <rarray>
int main() {
    rvector<double> r = linspace(-1.0, 1.0, 101);
    ...
}
```
The first argument of linspace is allowed to be greater than the last,
in which case, decreasing values are generated.  The two arguments are
allowed to be equal as well, which generates a vector with all equal
values. In that case, `end_incl` can not be set to false. The
case where the number of points is 1 and `end_incl` is `false`, is
ill-defined.

Note that for integer types, using linspace without specifying the
number of points (i.e. `linspace(n1,n2)`) will give the same values as would be
generated by the xrange function without a stepsize and with the
endvalue one higher (i.e., `xrange(n1,n2+1)`).

# Usage with C++ STL

In many ways, rarrays act like containers and can be used with STL
methods.  Particular one-dimensional rarrays should act much like
std::vector's.  We already saw an example above using std::copy.  For
another example. this is how one could sort an rarray:
```cpp
#include <rarray>
#include <iostream>
#include <algorithm>
int main()
{
    rarray<double,1> a;
    a.form({4.0, 10.0 , 3.0, 1.0});
    std::sort(a.begin(), a.end());
    std::cout << a << '\n';
}
```

As this example shows (as did many examples above), rarrays can be
output as text using C++ STL streams. Similarly, they can be input as
text as well.

The output produced by the output streaming operator is like that of
automatic arrays initializers: Each dimension is started and ended by
`{` and `}` and components are comma separated.  Except for the inner
dimension, newlines are included in the output, but no spaces, and no
trailing newline.  E.g.:
```cpp
#include <iostream>
#include <rarray>
int main() {
    rarray<int,2> arr;
    arr.form({{1,2},{3,4},{5,6}});
    std::cout << arr;
}
```
will print the following:
```
{
{1,2},
{3,4},
{5,6}
}
```
Apart from inserting curly braces, commas, and newlines, the streaming
operators use the streaming operators of the elements' type. 

The streaming operators are designed such that the format that is
written out by the `<<` operator, should be readable by the `>>`
operator.  Without further formatting rules, reading would not be
unambiguous for some types, such as for `std::string` wich can contain
the syntactic elements `{`, `}`, `,` or `#`.  If these elements are
found in the output of an element, the element is prepended with a
string that encodes the string length of the output. This prepending
string starts with a `#` character, then the length of the
string output for the element (excluding the prepended part), followed
by a `:` character.

An exception to the `#` formatting rule exists for types that stream
out such that the first output character is `(` and the last is `)`,
with no other `)` characters intervening. Such output does not require
the `#` formatting. Complex numbers are a primary example of a type
with such output and input.

Here is an example of writing an rarray of strings to a file, and reading it back in:
```cpp
#include <rarray>
#include <string>
#include <iostream>
#include <fstream>
int main() {
    rvector<std::string> vout(3);
    vout.fill({"Hello,","world","{}"});
    std::ofstream fout("vectorofstrings.rat");
    fout << vout; 
    fout.close();
    std::ifstream fin("vectorofstrings.rat");
    rvector<std::string> vin;
    fin >> vin;
    fin.close();
    for (const auto& s: vin)
        std::cout << vin << '\n';
}
```
The actual content of the file vectorofstring.rat is
"`{#6:Hello,,world,#2:{}}`" (without the quotation marks).


Although text output of numerical data is inefficient and inexact,
better methods require either a proprietary binary format or the use
of an external library.  Having at least one output format readily
available can be quite convenient.

# Usage with C libraries

Commonly used software libraries in scientific computing that can work
with arrays, for instance, MPI, HDF5, FFTW, BLAS, do so by having (or
providing) a C interface.  Such an interface typically exposes
functions that take a pointer to the data, with the extents passed as
additional function arguments.  These bits of information are provided
by the `data` and `extent` methods described above.

To see how this may work, here is an example to call the matrix-matrix
multiplication routine from blas, using the cblas interface:
```cpp
#include <iostream>
#include <rarray>
#include <cblas.h>
void matrix_product(const rmatrix<double> A,
                    const rmatrix<double> B,
                    rmatrix<double> C)
{
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                C.extent(0), C.extent(1), A.extent(1), 1.0,
                A.data(), A.extent(1),
                B.data(), B.extent(1),
                1.0,
                C.data(), C.extent(1));
}
int main() {
    rmatrix<double> A, B;
    A.fill({ {1, -2, 3},
              2, -1, 0} };
    B.fill({ {-1, 3, -2, 1},
             {-2, 1, -3, 2},
             {-3, 2, -1, 3} } );
    rmatrix C(A.extent(0), B.extent(1));
    matrix_product(A, B, C);
    std::cout << "A=" << A << '\n'
              << "B=" << B << '\n' 
              << "C=A*B=" << C << '\n';
}
```

When working with complex data, an extra conversion may be
required. For instance, working with the library called FFTW3, the
following is a way to compute the fourier transform of a complex
rarray:
```cpp
#include <iostream>
#include <complex>
#include <rarray>
#include <fftw3.h>
int main() {
    int n = 4;
    rvector<std::complex<double>> a;
    a.fill({1.3+5.0i, 1.0, 5.0i, 2.0-5.0i});
    rvector<std::complex<double>> b(a.shape());
    fftw_plan plan = fftw_plan_dft_1d(a.size(),
                       (fftw_complex*)a.data(), (fftw_complex*)b.data(), 
                       FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    std::cout << "a=" << a << '\n' << "FT(a)=" << b << '\n';
}
```

# Further features

## Bypassing Reference Counting

It is possible to use an external, pre-allocated buffer, as follows:
```cpp
std::unique_ptr<float[]> pre_alloc_data(new float[256*256*256]); 
rarray<float,3> s(pre_alloc_data, 256, 256, 256);
```
Note that `s` will have dangling references (often leading to
"segmentation faults") if `pre_alloc_data` is deallocated while `s` has
not gone out of scope or has not yet been `clear`-ed.

## Comma Separated Assignment

In addition to the explicit assignment of each element and the fill and form methods, there is one more way to assign values to the elements of an rarray as a whole using a
comma separate form, as follows
```
rarray<double,2> matrix(3,3);
matrix = 1.0, 2.0, 3.0, 
         4.0, 5.0, 6.0, 
         7.0, 8.0, 9.0;
```
This initialization method is deprecated as it does not specify the shape and because its semantics clashes with the other assignment operator that creates a reference-counted shallow copy.

## Returning a rarray from a function

Because rarray implements move semantics, returning an rarray from a
function does not pose any problems.

Consider the function `zeros` used in `main()`:
```cpp
#include <rarray>
rarray<double,2> zeros(int n, int m) {
   rarray<double,2> r(n, m);
   r.fill(0.0);
   return r;
}
int main() {
   rarray<double,2> s = zeros(100, 100);
   return s[99][99];
}
```
In line 3, a rarray `r` is created, and filled, on line 4, with
zeros. On line 5, `r` gets moved out of the function and into `s`
using C++11's move semantics. Move semantics cause `r` inside the
function to be left in an empty state detached from `s`, so when the
function comes to an end, the data in `s` persists.

## Optional bounds checking

If the preprocessor constant `RA_BOUNDSCHECK` is defined, an
`out_of_bounds` exception is thrown if

  - an index is too small or too large;

  - the size of dimension is requested that does not exist (in a call to `extent(int i)`);

  - a constructor is called with a zero pointer for the buffer or for the shape array.

`RA_BOUNDSCHECK` can be defined by adding the `-DRA_BOUNDSCHECK`
argument to the compilation command, or by inserting `#define
RA_BOUNDSCHECK` before the `#include <rarray>` line in the source
code.

## Conversions

Rarray supports a lot of conversions to other, older common ways that multidimensional arrays may be used in C++.

### Converting automatic C-style arrays to rarrays

It is possible to convert C-style automatic arrays to rarrays if they
have a rank of at most 11.  The main convenience of this is that one
can write functions that take rarray argument(s) and pass automatic
arrays to them. For example:
```cpp
#include <iostream>
#include <rarray>
void print2d(const rarray<float,2> &s) {
    for (int i = 0; i < s.extent(0); i++) {
        for (int j = 0; j < s.extent(1); j++)
            std::cout << s[i][j] << ' ';
        std::cout << std::endl;
    }
}
int main() {
    float stackarray[4][4] = { { 1.0, 1.2, 1.4, 1.6},
                               { 2.0, 2.2, 2.4, 2.6},
                               { 3.0, 3.2, 3.4, 3.6},
                               { 4.0, 4.2, 4.4, 4.6} };
    // automatic conversion
    print2d(stackarray);
    // view using the same data
    auto a = rarray<float,2>(stackarray);
    print2d(a);
    // independent copy of the same data
    auto b = rarray<float,2>(stackarray).copy();
    print2d(b);
}
```

### Conversions for function arguments

A function might take a `rarray<const T,R>` parameter
if elements are not changed by it.  Although C++ cannot convert
template types with a `T` to ones with a `const T`
reference, the rarray library provides this conversion from
`rarray<T,R>` to a `rarray<const T,R>`. For example:
```
#include <rarray>
float add(const rarray<const float,2> &s) {
     float x = 0.0;
     for (int i = 0; i < s.extent(0); i++)
          for (int j = 0; j < s.extent(1); j++)
              x += s[i][j];
     return x;
}
int main() {
     rarray<float,2> s(40, 40); // note: not const!
     float z = add(s); // yet this works
}
```
One can also explicitly use the `const_ref` method to do this conversion.

Note: This works equally well when the function argument is passed by value.

## Conversion to raw pointers

Rarray objects are also easy to pass to functions from legacy that do
not use `rarray`s but pointers.  To avoid ambiguities, conversions of
a rarray to a pointer must be done using explicit methods.

There are two main ways that such functions expect a multidimensional
array to be passed: either as a pointer (a `T*`) to the first element
of the internal buffer composed of all elements, or as a
pointer-to-pointer structure (a `T**...`).  In the former case, it may
be important to remember that an rarray stores elements in row-major
format.

With the `const` keyword, the number of useful C++ forms for
multidimensional array arguments has grown to about six.  In the case
of a two-dimensional array these take the forms: `T*`, `const T*`,
`T*const*`, `const T*const*`, `T**`, and `const T**`.  Using the
rarray library, const-correct argument passing requires the `data` or
`ptr_array` method but non-const-correct argument passing will require
the `noconst_ptr_array` function, possibly combined with `const_ref`.

We will briefly looking at these cases separately now.

### Conversion to a T* or a const T*

A function may expect a multidimensional array to be passed as a
simple pointer to the first element, of the form `T*`, or of the form
`const T*`. This is the case for most C or fortran libraries, as
disussed above. A rarray object `s` of type `rarray<T,R>` can be
passed to these functions using the syntax `s.data()`, which yields a
`T*`. Examples:
```cpp
void fill1(float* a, int n1, int n2, float z) {
    for (int i=0; i<n1*n2; i++)
        a[i] = z;
}
int main() {
    rarray<float,2> s(40, 40);
    fill1(s.data(), s.extent(0), s.extent(1), 3.14);
}
```
```cpp
float add2(const float* a, int n1, int n2) {
    float x = 0.0;
    for (int i=0; i<n1*n2; i++)
        x += a[i];
    return x;
}
int main() {
    rarray<float,2> s(40, 40);
    float z = add2(s.data(), s.extent(0), s.extent(1));
}
```

C++ accepts a `float*` instead of a `const float*`, so `data()` could
be used in the latter example.

### Conversion to a T*const* or a const T*const* 

In `T*const*`, the middle const means that one cannot reassign the row
pointers.  The rarray classes can be converted to this type using the
`ptr_array()` method.  For higher dimensions, the repeated pointer
type that `ptr_array` returns generalizes to `T*const*const*`,
`T*const*const*const*`, etc.  Examples:
```cpp
void fill3(float*const* a, int n1, int n2, float z) {
    for (int i=0; i<n1; i++)
        for (int j=0; j<n2; j++)
            a[i][j] = z;
}
int main() {
    rarray<float,2> s(40, 40);
    fill3(s.ptr_array(), s.extent(0), s.extent(1), 3.14);
}
```
```cpp
float add4(const float*const* a, int n1, int n2) {
    float x = 0.0;
    for (int i=0; i<n1; i++)
        for (int j=0; j<n2; j++)
            x += a[i][j];
    return x;
}
int main() {
    rarray<float,2> s(40, 40);
    float z = add4(s.ptr_array(), 40, 40);
}
```

C++ accepts a `T*const*` where a `const T*const*` is expected, so here one can again use the method `ptr_array()`.

### Conversion to a T**

If one were to generating a `T**` from a rarray object, one could
change the internal structure of that rarray object through the double
pointer.  This is therefore considered not *const-correct*.  It is
however sometimes needed when using legacy code that expects such a
pointer, and for that reason, `rarray` has a function for it, called
`noconst_ptr_array`. Example:
```cpp
void fill5(float** a, int n1, int n2, float z) {
    for (int i=0; i<n1; i++)
        for (int j=0; j<n2; j++)
            a[i][j] = z;
}
int main() {
    rarray<float,2> s(40, 40);
    fill5(s.noconst_ptr_array(), s.extent(0), s.extent(1), 3.14);
}
```

### Conversion to a const T**

C++ does not allow conversion from `T**` to `const T**`.  To convert
to a `const T**`, one first needs to convert the `rarray<T,R>` to a
`rarray<const T,R>` using `const_ref()`, after which one can use the
`noconst_ptr_array` function, e.g.:
```cpp
float add6(const float** a, int n1, int n2) {
    float x = 0.0;
    for (int i=0; i<n1; i++)
        for (int j=0; j<n2; j++)
            x += a[i][j];
    return x;
}
int main() {
    rarray<float,2> s(40, 40);
    float z = add6(s.const_ref().noconst_ptr_array(), 40, 40);
}
```
