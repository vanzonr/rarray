//
// rarray - Runtime arrays: template classes for pointer-based,
//          runtime, reference counted, multi-dimensional
//          arrays.  Documentation in rarraydoc.pdf
//
// Copyright (c) 2013-2025  Ramses van Zon
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

#ifndef RARRAY_H_
#define RARRAY_H_

#if __cplusplus >= 201103L

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <functional>
#include <string>
#include <initializer_list>
#include "versionheader.h"
#include "rarraymacros.h"
#include "rarraytypes.h"
#include "shared_buffer.h"
#include "shared_shape.h"

// Forward definitions of ra::rarray<T, R> and ra::CommaOp
namespace ra {

enum class RESIZE { NO, ALLOWED };
enum class MISSING { SKIP, DEFAULT, REPEAT };

namespace detail {
template<typename T> class CommaOp;
template<typename T, rank_type R, typename P> class Bracket;
template<typename T, rank_type R, typename P> class ConstBracket;
// TODO(geet): Each operator will create a subexpression of the Expr<...>,
// which we forward-define first Forward definitions to support array
// expressions // What type enumerates possible operators?
// using ExOp = int;
#define ExOp class   ///< not used
/// Not implemented
template<typename T, rank_type R, ExOp AOP, typename A1, typename A2, typename A3> class Expr;
// Helper routines to recursively determine the shape of nested
// initializer list used in rarray::assign(...).
template<typename T>
void init_shape_fill(unsigned N, size_type* sz, const T& l)
{}
template<typename U>
void init_shape_fill(unsigned N, size_type* sz,
                     const std::initializer_list<U>& l) {
    // U should be a type of N-1 nested initializer lists, so l is a
    // N-level nested initializer list. But the number of levels is
    // less, this will only fill upto the actual number of levels,
    // while if the number of levels is greater, only N levels will be
    // initialized.
    sz[0] = std::max(sz[0], static_cast<size_type>(l.size()));
    if (N >= 2)
        for (const auto& lsub : l)
            init_shape_fill(N - 1, sz + 1, lsub);
}
template<unsigned N, typename U>
auto init_shape(const std::initializer_list<U>& l) -> std::array<size_type, N> {
    std::array<size_type, N> sz{0};
    init_shape_fill(N,  &sz[0], l);
    return sz;
}
template<typename T>
struct init_list_prop {
    enum { rank = 0 };
    using  type = T;
};
template<typename U>
struct init_list_prop<std::initializer_list<U>> {
    enum { rank = init_list_prop<U>::rank + 1 };
    using  type = typename init_list_prop<U>::type;
};
}  // namespace detail

/// Class for multidimensional arrays
/// @tparam  T  the type of the elements (any type)
/// @tparam  R  the rank (a positive integer)
template<typename T, rank_type R>
class rarray {
 public:
    using value_type = T;
    using difference_type = ::ra::size_type;   // difference type for indices
    using size_type = ::ra::size_type;         // type of indices
    using iterator = T*;                       // iterator type
    using const_iterator = const T*;           // iterator type for constant access
    using parray_t = typename detail::PointerArray<T, R>::type;  // T*const*const*...
    using noconst_parray_t = typename detail::PointerArray<T, R>::noconst_type;  // T***...
    /// Default constructor; leaves rarray undefined
    inline rarray()
    : buffer_(), shape_() {}
    /// @name Constructors creating its own buffer for various R values
    /// @{
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline explicit rarray(size_type n0)
    : buffer_(n0),
      shape_({n0}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline rarray(size_type n0, size_type n1)
    : buffer_(n0*n1),
      shape_({n0, n1}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline rarray(size_type n0, size_type n1, size_type n2)
    : buffer_(n0*n1*n2),
      shape_({n0, n1, n2}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3)
    : buffer_(n0*n1*n2*n3),
      shape_({n0, n1, n2, n3}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4)
    : buffer_(n0*n1*n2*n3*n4),
      shape_({n0, n1, n2, n3, n4}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5)
    : buffer_(n0*n1*n2*n3*n4*n5),
      shape_({n0, n1, n2, n3, n4, n5}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6)
    : buffer_(n0*n1*n2*n3*n4*n5*n6),
      shape_({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7, size_type n8)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7, size_type n8, size_type n9)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin())
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline rarray(size_type n0, size_type n1, size_type n2, size_type n3, size_type n4,
                  size_type n5, size_type n6, size_type n7, size_type n8, size_type n9,
                  size_type n10)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin())
    {}
    explicit inline rarray(const size_type* anextent)
    : buffer_(std::accumulate(anextent, anextent+R, 1, std::multiplies<size_type>())),
      shape_(reinterpret_cast<const std::array<size_type, R>&>(*anextent), buffer_.begin())
    {}
    /// @}
    /// @name constructors from an existing buffer
    /// @{
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline rarray(T* buffer, size_type n0)
    : buffer_(n0, buffer),
      shape_({n0}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline rarray(T* buffer, size_type n0, size_type n1)
    : buffer_(n0*n1, buffer),
      shape_({n0, n1}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2)
    : buffer_(n0*n1*n2, buffer),
      shape_({n0, n1, n2}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3)
    : buffer_(n0*n1*n2*n3, buffer),
      shape_({n0, n1, n2, n3}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4)
    : buffer_(n0*n1*n2*n3*n4, buffer),
      shape_({n0, n1, n2, n3, n4}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5)
    : buffer_(n0*n1*n2*n3*n4*n5, buffer),
      shape_({n0, n1, n2, n3, n4, n5}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6)
    : buffer_(n0*n1*n2*n3*n4*n5*n6, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7, size_type n8)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7, size_type n8,
                  size_type n9)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer)
    {}
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline rarray(T* buffer, size_type n0, size_type n1, size_type n2, size_type n3,
                  size_type n4, size_type n5, size_type n6, size_type n7, size_type n8,
                  size_type n9, size_type n10)
    : buffer_(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10, buffer),
      shape_({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer)
    {}
    inline rarray(T* buffer, const size_type* anextent)
    : buffer_(std::accumulate(anextent, anextent+R, 1, std::multiplies<size_type>()), buffer),
      shape_(reinterpret_cast<const std::array<size_type, R>&>(*anextent), buffer)
    {}
    /// @}
    /// @name Constructors from automatic arrays
    /// @{
    template<std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 1>::type>
    inline explicit rarray(T (&a)[Z])
    : buffer_(Z, a),
      shape_({Z}, buffer_.begin())
    {}
    template<std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 2>::type>
    inline explicit rarray(T (&a)[Y][Z])
    : buffer_(Y*Z, *a),
      shape_({Y, Z}, buffer_.begin())
    {}
    template<std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 3>::type>
    inline explicit rarray(T (&a)[X][Y][Z])
    : buffer_(X*Y*Z, **a),
      shape_({X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 4>::type>
    inline explicit rarray(T (&a)[W][X][Y][Z])
    : buffer_(W*X*Y*Z, ***a),
      shape_({W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 5>::type>
    inline explicit rarray(T (&a)[V][W][X][Y][Z])
    : buffer_(V*W*X*Y*Z, ****a),
      shape_({V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 6>::type>
    inline explicit rarray(T (&a)[U][V][W][X][Y][Z])
    : buffer_(U*V*W*X*Y*Z,  *****a),
      shape_({U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t T_, std::size_t U, std::size_t V, std::size_t W, std::size_t X,
             std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 7>::type>
    inline explicit rarray(T (&a)[T_][U][V][W][X][Y][Z])
    : buffer_(T_*U*V*W*X*Y*Z,  ******a),
      shape_({T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t S, std::size_t T_, std::size_t U, std::size_t V, std::size_t W,
             std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 8>::type>
    inline explicit rarray(T (&a)[S][T_][U][V][W][X][Y][Z])
    : buffer_(S*T_*U*V*W*X*Y*Z,  *******a),
      shape_({S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t R_, std::size_t S, std::size_t T_, std::size_t U, std::size_t V,
             std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 9>::type>
    inline explicit rarray(T (&a)[R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(R_*S*T_*U*V*W*X*Y*Z,  ********a),
      shape_({R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_, std::size_t U,
             std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 10>::type>
    inline explicit rarray(T (&a)[Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(Q*R_*S*T_*U*V*W*X*Y*Z,  *********a),
      shape_({Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t P, std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_,
             std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 11>::type>
    inline explicit rarray(T (&a)[P][Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(P*Q*R_*S*T_*U*V*W*X*Y*Z,  **********a),
      shape_({P, Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    /// @}
    /// @name constructors from compound literal automatic arrays (non-standard c++ and may lead to dangling references).
    /// @{
    template<std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 1>::type>
    inline explicit rarray(T (&&a)[Z])
    : buffer_(Z, a),
      shape_({Z}, buffer_.begin())
    {}
    template<std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 2>::type>
    inline explicit rarray(T (&&a)[Y][Z])
    : buffer_(Y*Z, *a),
      shape_({Y, Z}, buffer_.begin())
    {}
    template<std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 3>::type>
    inline explicit rarray(T (&&a)[X][Y][Z])
    : buffer_(X*Y*Z, **a),
      shape_({X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 4>::type>
    inline explicit rarray(T (&&a)[W][X][Y][Z])
    : buffer_(W*X*Y*Z, ***a),
      shape_({W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 5>::type>
    inline explicit rarray(T (&&a)[V][W][X][Y][Z])
    : buffer_(V*W*X*Y*Z, ****a),
      shape_({V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 6>::type>
    inline explicit rarray(T (&&a)[U][V][W][X][Y][Z])
    : buffer_(U*V*W*X*Y*Z,  *****a),
      shape_({U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t T_, std::size_t U, std::size_t V, std::size_t W, std::size_t X,
             std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 7>::type>
    inline explicit rarray(T (&&a)[T_][U][V][W][X][Y][Z])
    : buffer_(T_*U*V*W*X*Y*Z,  ******a),
      shape_({T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t S, std::size_t T_, std::size_t U, std::size_t V, std::size_t W,
             std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 8>::type>
    inline explicit rarray(T (&&a)[S][T_][U][V][W][X][Y][Z])
    : buffer_(S*T_*U*V*W*X*Y*Z,  *******a),
      shape_({S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t R_, std::size_t S, std::size_t T_, std::size_t U, std::size_t V,
             std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 9>::type>
    inline explicit rarray(T (&&a)[R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(R_*S*T_*U*V*W*X*Y*Z,  ********a),
      shape_({R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_, std::size_t U,
             std::size_t V, std::size_t W, std::size_t X, std::size_t Y, std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 10>::type>
    inline explicit rarray(T (&&a)[Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(Q*R_*S*T_*U*V*W*X*Y*Z,  *********a),
      shape_({Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    template<std::size_t P, std::size_t Q, std::size_t R_, std::size_t S, std::size_t T_,
             std::size_t U, std::size_t V, std::size_t W, std::size_t X, std::size_t Y,
             std::size_t Z,
             rank_type R__=R, typename=typename std::enable_if<R__ == 11>::type>
    inline explicit rarray(T (&&a)[P][Q][R_][S][T_][U][V][W][X][Y][Z])
    : buffer_(P*Q*R_*S*T_*U*V*W*X*Y*Z,  **********a),
      shape_({P, Q, R_, S, T_, U, V, W, X, Y, Z}, buffer_.begin())
    {}
    /// @}
    /// Shallow/reference copy constructor.
    ///  @param[in]  a  the array to which to make this a reference (rarray<T,R>)
    RA_FORCE_inline rarray(const rarray<T, R> &a) noexcept
    : buffer_(a.buffer_),
      shape_(a.shape_)
    {}
    /// Shallow/reference assignment operator.
    ///  @param[in]  a  the array to which to make this a reference (rarray<T,R>)
    inline auto operator=(const rarray<T, R> &a) noexcept -> rarray& {
        buffer_ = a.buffer_;
        shape_ = a.shape_;
        return *this;
    }
    /// @name move constructor and move-assignment operator
    /// @{
    inline rarray(rarray<T, R>&& x) noexcept
    : buffer_(std::move(x.buffer_)),
      shape_(std::move(x.shape_))
    {}
    inline auto operator=(rarray<T, R>&& x) noexcept -> rarray& {
        buffer_ = std::move(x.buffer_);
        shape_ = std::move(x.shape_);
        return *this;
    }
    /// @}
    /// Comma separated element assignment (deprecated)
    ///  @param[in]  e  value for the first element (value_type)
    ///  @return  Intermediate object implementing the comma operator to mean assignment.
    inline auto operator=(const T& e) noexcept(RA_noboundscheck && std::is_nothrow_copy_constructible<T>()) -> detail::CommaOp<T> {
        // puts the first element in and prepares for more
        RA_CHECKORSAY(!empty(), "assignment to unsized array");
        RA_CHECKORSAY(size() > 0, "assignment with more elements than in array");
        T* first = &(buffer_[0]);
        if (size() > 0)
            *first = e;
        else
            return detail::CommaOp<T>(nullptr, nullptr);
        detail::CommaOp<T> co(first+1, first+size()-1);
        return co;
    }
    /// Destructor
    RA_FORCE_inline ~rarray() = default;
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline void fill(const detail::Expr<T, R, AOP, A1, A2, A3>& e);
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline void form(const detail::Expr<T, R, AOP, A1, A2, A3>& e);
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator+=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator-=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator*=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator/=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    /// Not implemented
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator%=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 1.
    ///  @param[in]  n0               new extent (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void reshape(size_type n0, RESIZE resize_policy = RESIZE::NO) {
        const size_type newsize = n0;
        if (size() == newsize) { 
            shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= n0) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 2.
    ///  @param[in]  n0               new extent in dimension 0 (size_type)
    ///  @param[in]  n1               new extent in dimension 1 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void reshape(size_type n0, size_type n1, RESIZE resize_policy = RESIZE::NO) {
        const size_type newsize = n0*n1;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 3.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 4.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 5.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 6.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  n5              new extent in dimension 5 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 7.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  n5              new extent in dimension 5 (size_type)
    ///  @param[in]  n6              new extent in dimension 6 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5*n6;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        }
        else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 8.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  n5              new extent in dimension 5 (size_type)
    ///  @param[in]  n6              new extent in dimension 6 (size_type)
    ///  @param[in]  n7              new extent in dimension 7 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize=n0*n1*n2*n3*n4*n5*n6*n7;
        if (size() == n0*n1*n2*n3*n4*n5*n6*n7) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        } else if  (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 9.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  n5              new extent in dimension 5 (size_type)
    ///  @param[in]  n6              new extent in dimension 6 (size_type)
    ///  @param[in]  n7              new extent in dimension 7 (size_type)
    ///  @param[in]  n8              new extent in dimension 8 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5*n6*n7*n8;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) { 
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 10.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  n5              new extent in dimension 5 (size_type)
    ///  @param[in]  n6              new extent in dimension 6 (size_type)
    ///  @param[in]  n7              new extent in dimension 7 (size_type)
    ///  @param[in]  n8              new extent in dimension 8 (size_type)
    ///  @param[in]  n9              new extent in dimension 9 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8, size_type n9,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize = n0*n1*n2*n3*n4*n5*n6*n7*n8*n9;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for rank 11.
    ///  @param[in]  n0              new extent in dimension 0 (size_type)
    ///  @param[in]  n1              new extent in dimension 1 (size_type)
    ///  @param[in]  n2              new extent in dimension 2 (size_type)
    ///  @param[in]  n3              new extent in dimension 3 (size_type)
    ///  @param[in]  n4              new extent in dimension 4 (size_type)
    ///  @param[in]  n5              new extent in dimension 5 (size_type)
    ///  @param[in]  n6              new extent in dimension 6 (size_type)
    ///  @param[in]  n7              new extent in dimension 7 (size_type)
    ///  @param[in]  n8              new extent in dimension 8 (size_type)
    ///  @param[in]  n9              new extent in dimension 9 (size_type)
    ///  @param[in]  n10             new extent in dimension 10 (size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8, size_type n9, size_type n10,
                        RESIZE resize_policy = RESIZE::NO)  {
        const size_type newsize =  n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10;
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Change the shape of the rarray while keeping the underlying data.
    /// The data in the underlying buffer will not change. Note that
    /// the rank will remain the same. If the \p resize_policy
    /// argument is used with a value of ra::RESIZE::ALLOWED, the new
    /// total size is allowed to be smaller than the previous size,
    /// otherwise the total size must remain the same.
    /// This is the overloaded method for any rank.
    ///  @param[in]  newshape        Array wih rank() elements (pointer to size_type)
    ///  @param[in]  resize_policy   ra::RESIZE::NO (default) or ra::RESIZE::ALLOWED
    inline void reshape(const size_type* newshape, RESIZE resize_policy = RESIZE::NO) {
        const size_type newsize = std::accumulate(newshape, newshape+R,
                                            1, std::multiplies<size_type>());
        if (size() == newsize) {
            shape_ = detail::shared_shape<T, R>((const std::array<size_type, R>&)(*newshape), buffer_.begin());
        } else if (resize_policy == RESIZE::ALLOWED && size() >= newsize) {
            buffer_ = buffer_.slice(0, newsize);
            shape_ = detail::shared_shape<T, R>((const std::array<size_type, R>&)(*newshape), buffer_.begin());
        } else {
            throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
        }
    }
    /// Create a deep, independent copy.
    /// @return Independent rarray with its own shape and data copied from the original.
    inline auto copy() const -> rarray {
        // return a copy
        rarray<T, R> clone;
        clone.buffer_ = buffer_.copy();
        clone.shape_ = shape_.copy();
        clone.shape_.relocate(clone.buffer_.begin());
        return clone;
    }
    /// Automatic conversion to an rarray with const elements.
    ///  @return  A const reference to *this reinterpreted as having const value_type.
    ///  @note  Only implemented if value_type is not already const.
    template<typename U = T,
             typename = typename std::enable_if<!std::is_const<U>::value>::type>
    inline operator const rarray<const U, R>&() const noexcept {
        return const_ref();
    }
    /// This method returns the number of dimensions of the rarray, a.k.a., its rank.
    ///  @return rank of the rarray (integer)
    constexpr static auto rank() noexcept -> int {
        return R;
    }
    /// Check if the rarray is undefined.
    /// @return  bool indicating if thie rarray has no shape, i.e. is undefined.
    inline auto empty() const noexcept -> bool {
        return buffer_.cbegin() == nullptr;
    }
    /// Retrieve the array size (a.k.a. extent) in a given dimension.
    ///  @param[in]  i  The dimension of which to give the extent (integer)
    ///  @return        The extent of the array in dimension \p i (size_type)
    inline auto extent(int i) const -> size_type {
        return shape_.extent(i);
    }
    /// Retrieve the sizes (a.k.a. extents) of the rarray in all dimensions.
    ///  @return  Array of extents (pointer to size_type)
    RA_FORCE_inline auto shape() const noexcept -> const size_type* {
        return &(shape_.extent()[0]);
    }
    /// Retrieve the number of elements. This is also the product of all extents.
    ///  @return  The total number of elements in the array (size_type)
    inline auto size() const noexcept -> size_type {
        return shape_.size();
    }
    /// Retrieve the address to the internal data.
    ///  @return  The internal data's address (pointer to value_type)
    inline auto data() noexcept -> T* {
        return buffer_.begin();
    }
    /// Retrieve the address to the internal data when this is const.
    ///  @return  The internal data's address (pointer to const value_type)
    inline auto data() const noexcept -> const T* {
        return buffer_.begin();
    }
    /// Retrieve nested pointer-to-pointer structure.
    ///  @return  A T*const*.. pointer acting similarly to this rarray when using square brackets.
    inline auto ptr_array() const noexcept -> parray_t {
        return shape_.ptrs();
    }
    /// Retrieve const-stripped nested pointer-to-pointer structure.
    ///  @return   A T**.. pointer acting similarly to this rarray when using square brackets.
    ///  @warning  Breaks const-correctness.
    inline auto noconst_ptr_array() const noexcept -> noconst_parray_t {
        return const_cast<noconst_parray_t>(shape_.ptrs());
    }
    /// Converts rarray to an rarray with const elements.
    ///  @return  A const reference to *this reinterpreted as having const value_type.
    inline auto const_ref() const noexcept -> const rarray<const T, R>& {
        return reinterpret_cast<const rarray<const T, R>&>(*this);
    }
    /// Make undefined.
    RA_FORCE_inline void clear() noexcept {
        shape_ = detail::shared_shape<T, R>();
        buffer_ = detail::shared_buffer<T>();
    }
    /// Fill with uniform value. Does not change the size nor shape.
    ///  @param  value  new value for all elements (type_value)
    inline void fill(const T& value) {
        buffer_.fill(value);
    }

 private:
    template<rank_type R_ = R,
             class = typename std::enable_if<R_ == 1>::type>
    inline void fill_g(std::initializer_list<T> list,
                       MISSING missing_policy) {
        auto filler = list.begin();
        for (size_type i = 0; i < extent(0); i++) {
            if (filler != list.end()) {
                buffer_[i] = *filler++;
                if (filler == list.end()) {
                    if (missing_policy == MISSING::REPEAT)
                        filler = list.begin();
                    else if (missing_policy == MISSING::SKIP)
                        break;
                }
            } else {
                buffer_[i] = T{};
            }
        }
    }
    template<typename U, rank_type R_ = R,
             class = typename std::enable_if<R_ != 1>::type>
    inline void fill_g(std::initializer_list<U> list,
                       MISSING missing_policy) {
        auto filler = list.begin();
        for (size_type i = 0; i < extent(0); i++) {
            if (filler != list.end()) {
                at(i).fill_g(*filler++, missing_policy);
                if (filler == list.end()) {
                    if (missing_policy == MISSING::REPEAT)
                        filler = list.begin();
                    else if (missing_policy == MISSING::SKIP)
                        break;
                }
            } else {
                at(i).fill(T{});
            }
        }
    }
    template<typename U>
    inline void form_g(std::initializer_list<U> list,
                         MISSING missing_policy) {
        std::array<size_type, R> newshape = detail::init_shape<R>(list);
        size_type newsize = std::accumulate(newshape.begin(), newshape.end(),
                                            1, std::multiplies<size_type>());
        buffer_ = detail::shared_buffer<T>(newsize);
        shape_ = detail::shared_shape<T, R>(newshape, buffer_.begin());
        fill_g(list, missing_policy);
    }

 public:
    /// @name Fill with repeating sequence (does not change the size nor shape)
    /// @{
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void fill(std::initializer_list<T> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<T>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void fill(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        fill_g(list, missing_policy);
    }
    /// @}
    /// @name form
    /// Form from initializer lists
    /// @{
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void form(std::initializer_list<T> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void form(std::initializer_list<std::initializer_list<T>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void form(std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<
                     std::initializer_list<T>>>>>>>>>>> list,
                     MISSING missing_policy = MISSING::DEFAULT) {
        form_g(list, missing_policy);
    }
    /// @}
    /// @name Form with uniform value (will set the size and shape)
    /// @{
    /// @brief Creates a newly allocated rarray filled with a given value
    /// @param n0... New dimensions of the rarray
    /// @param value Value to fill the rarray with
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void form(size_type n0, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0);
        shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void form(size_type n0, size_type n1, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1);
        shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void form(size_type n0, size_type n1, size_type n2, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8, size_type n9,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void form(size_type n0, size_type n1, size_type n2, size_type n3,
                     size_type n4, size_type n5, size_type n6, size_type n7,
                     size_type n8, size_type n9, size_type n10,
                     const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10},
                                            buffer_.begin());
        buffer_.fill(value);
    }
    ///@}
    /// @name iterators over the data
    ///@{
    inline auto begin() noexcept -> iterator {
        return buffer_.begin();
    }
    inline auto begin() const noexcept -> const_iterator {
        return buffer_.begin();
    }
    inline auto cbegin() const noexcept -> const_iterator {
        return buffer_.cbegin();
    }
    inline auto end() noexcept -> iterator {
        return buffer_.end();
    }
    inline auto end() const noexcept -> const_iterator {
        return buffer_.end();
    }
    inline auto cend() const noexcept -> const_iterator {
        return buffer_.cend();
    }
    ///@}
    /// Compute index of a reference inside an array.
    /// If \p a is an element in the array, get index in dimension \p i of that element;
    inline auto index(const T& a, int i) const -> size_type {
        // retrieve index in dimension i within *this of the element a
        std::ptrdiff_t linearindex = &a - &(buffer_[0]);
        if (linearindex < 0 || linearindex >= size())
            throw "element not in array";
        const size_type* extent_ = shape();
        for (int j = R-1; j > i; j--)
            linearindex /= extent_[j];
        return linearindex % extent_[i];
    }
    /// Compute index of a reference inside an array
    /// If \p iter points at an element in the array, get index in dimension \p i of that element;
    inline auto index(const iterator& iter, int i) const -> size_type {
        return index(*iter, i);
    }
    /// Compute index of a reference inside an array.
    /// If \p a is an element in the array, get the indices of that element;
    inline auto index(const T& a) const -> std::array<size_type, R> {
        std::array<size_type, R> ind;
        std::ptrdiff_t linearindex = &a - &(buffer_[0]);
        RA_CHECKORSAY(linearindex >= 0 && linearindex < size(), "element not in array");
        int j = R;
        const size_type* extent_ = shape();
        while (j-->0) {
            ind[j] = linearindex % extent_[j];
            linearindex /= extent_[j];
        }
        return ind;
    }
    /// Compute index of a reference inside an array.
    /// If \p i points at an element in the array, get the indices of that element.
    inline auto index(const iterator& i) const -> std::array<size_type, R> {
        return index(*i);
    }
    /// @name access subarrays with bounds checking
    /// @{
    template<class U = rarray<T, R-1>>
    RA_FORCE_inline auto at(size_type i)
    -> typename std::enable_if<R != 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        size_type stride = size()/extent(0);
        return ra::rarray<T, R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
    }
    template<class U = T&>
    RA_FORCE_inline auto at(size_type i)
    -> typename std::enable_if<R == 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        return shape_.ptrs()[i];
    }
    template<class U = const rarray<T, R-1>>
    RA_FORCE_inline auto at(size_type i) const
    -> typename std::enable_if<R != 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        size_type stride = size()/extent(0);
        return ra::rarray<T, R-1>(buffer_.slice(i*stride, (i+1)*stride), shape_.at(i));
    }
    template<class U = const T&>
    RA_FORCE_inline auto at(size_type i) const
    -> typename std::enable_if<R == 1, U>::type {
        if (i < 0 || i >= extent(0))
            throw std::out_of_range("rarray<T, R>::at");
        return shape_.ptrs()[i];
    }
    /// @}
    /// Allows to select successive indices for the first index.
    /// It always results in an rarray of the same rank as the original
    /// rarray.
    ///  @param[in]  beginindex  the starting value for the first index
    ///  @param[in]  endindexe   the last value of the first index plus one
    ///  @return an rarray which references the slice.
    RA_FORCE_inline auto slice(size_type beginindex, size_type endindex) -> rarray {
        if (beginindex < 0 || beginindex >= shape_.extent(0)
            || endindex < 0 || endindex > shape_.extent(0))
            throw std::out_of_range("rarray<T, R>::slice");
        size_type stride = size()/extent(0);
        return {buffer_.slice(beginindex*stride, endindex*stride),
                shape_.slice(beginindex, endindex)};
    }
    /// @name square bracket access
    /// @{
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i)
    -> typename std::enable_if<R == 1, U&>::type {
        RA_CHECKORSAY(i >= 0 && i < extent(0), "index out of range of array");
        return buffer_.begin()[i];
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i)
    -> typename std::enable_if<R != 1, typename detail::Bracket<U, R-1, rarray> >::type {
        return { *this, i, this->shape() };
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i) const
    -> typename std::enable_if<R == 1, const U&>::type {
        RA_CHECKORSAY(i >= 0 && i < extent(0), "index out of range of array");
        return buffer_.cbegin()[i];
    }
    template<class U = T>
    RA_FORCE_inline auto operator[](size_type i) const
    -> typename std::enable_if<R != 1, typename detail::ConstBracket<U, R-1, rarray>>::type {
        return { *this, i, this->shape() };
    }
    /// @}
    #if __cpp_multidimensional_subscript >= 202110L
    /// @name Allow c++23 multidimentional subscripts
    /// @{
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type i, Ts... args)
    -> typename std::enable_if<R == sizeof...(Ts)+1, T&>::type {
        return operator[](i)[args...];
    }
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type i, Ts... args) const
    -> typename std::enable_if<R == sizeof...(Ts)+1, const T&>::type {
        return operator[](i)[args...];
    }
    /// @}
    #endif
    // TODO(geet): for expressions
    /// Not implemented.
    RA_FORCE_inline auto leval(size_type i) const -> const T&;

 private:
    // allow *Bracket to access private_at
    friend class detail::Bracket<T, R-1, rarray<T, R>>;
    friend class detail::ConstBracket<T, R-1, rarray<T, R>>;
    RA_FORCE_inline auto private_at(size_type i) -> typename detail::PointerArray<T, R-1>::type {
        return shape_.ptrs()[i];
    }
    RA_FORCE_inline auto private_at(size_type i) const -> typename detail::PointerArray<T, R-1>::type {
        return shape_.ptrs()[i];
    }
    // allow rarray<T, R+1>::at(..) to create a proper rarray<T, R>
    // referencing data in current array
    friend class rarray<T, R+1>;
    inline rarray(detail::shared_buffer<T>&& abuffer, detail::shared_shape<T, R>&& ashape) :
        buffer_(std::forward<detail::shared_buffer<T>>(abuffer)),
        shape_(std::forward<detail::shared_shape<T, R>>(ashape))
    {}
    /// Meeded in rarray<T, R+1>::at(..) const.
    inline rarray(const detail::shared_buffer<T>&& abuffer, detail::shared_shape<T, R>&& ashape) :
        buffer_(std::forward<detail::shared_buffer<T>>(const_cast<detail::shared_buffer<T>&& >(abuffer))),
        shape_(std::forward<detail::shared_shape<T, R>>(ashape))
    {}
    detail::shared_buffer<T>  buffer_;  ///< contains the data
    detail::shared_shape<T, R> shape_;  ///< contains the extents and pointer-to-pointer
};  // end definition rarray<T, R>

namespace detail {

/// Class to facilitate assignment from a comma separated list.
template<typename T>
class CommaOp {
 public:
    /// Put the next number into the array.
    RA_FORCE_inline auto operator,(const T& e) -> CommaOp& {
        RA_CHECKORSAY(ptr_ != nullptr && last_ != nullptr, "invalid comma operator");
        RA_CHECKORSAY(ptr_ <= last_, "assignment with more elements than in array");
        if (ptr_ && ptr_ <= last_)
            *ptr_++ = e;
        return *this;
    }
 private:
    RA_FORCE_inline CommaOp(T* ptr, T* last) noexcept(RA_noboundscheck)
    : ptr_(ptr), last_(last) {
        RA_CHECKORSAY(ptr_ != nullptr && last_ != nullptr, "invalid comma operator");
    }
    T *ptr_;          ///< points to next element to be filled
    T * const last_;  ///< points to last element
    template<typename, int> friend class ra::rarray;
};

/// Class for repeated bracket access to elements
template<typename T, rank_type R, typename P>
class Bracket {
 private:
    P&               parent_;  ///< what array/array expression is being accessed
    size_type        index_;   ///< at what index
    const size_type* shape_;   ///< what is the shape of the result

 public:
    /// Implement square brackets to go to the next level.
    RA_FORCE_inline auto operator[](size_type nextindex) noexcept(RA_noboundscheck) -> Bracket<T, R-1, Bracket> {
        return { *this, nextindex, shape_ + 1 };
    }
    /// Implement implicit conversion to whatever parent.at() gives
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    Bracket(const Bracket&) = delete;
    Bracket(const Bracket&&) = delete;
    auto operator=(const Bracket&) -> Bracket& = delete;
    auto operator=(const Bracket&&) -> Bracket& = delete;
    ~Bracket() = default;

 private:
    RA_FORCE_inline Bracket(P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >=0 && index_ < shape_[0], "index out of range of array");
    }
    RA_FORCE_inline auto private_at(size_type nextindex) noexcept(RA_noboundscheck) -> decltype(parent_.private_at(index_)[nextindex]) {
        return parent_.private_at(index_)[nextindex];
    }
    template<typename, int> friend class ra::rarray;  ///< allow descending
    template<typename, int, typename> friend class detail::Bracket;  ///< allow descending
 public:
    RA_FORCE_inline auto at(size_type nextindex) -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
    #if __cpp_multidimensional_subscript >= 202110L
    /// Allow C++23 multidimensional subscripts
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type nextindex, Ts... args) -> typename std::enable_if<R == sizeof...(Ts)+1, T&>::type {
        return operator[](nextindex)[args...];
    }
    #endif
};

/// Class for repeated bracket access to elements (specialized for R==1)
template<typename T, typename P>
class Bracket<T, 1, P> {
 private:
    P&               parent_;  ///< what array/array expression is being accessed
    size_type        index_;   ///< at what index
    const size_type* shape_;   ///< what is the shape of the result

 public:
    /// Implement square brackets to go to the next level.
    RA_FORCE_inline auto operator[](size_type nextindex) noexcept(RA_noboundscheck) -> T& {
        RA_CHECKORSAY(nextindex >= 0 && nextindex < shape_[1], "index out of range of array");
        return parent_.private_at(index_)[nextindex];
    }
    /// Implement implicit conversion to whatever parent.at() gives.
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    Bracket(const Bracket&) = delete;
    Bracket(const Bracket&&) = delete;
    auto operator=(const Bracket&) -> Bracket& = delete;
    auto operator=(const Bracket&&) -> Bracket& = delete;
    ~Bracket() = default;

 private:
    RA_FORCE_inline Bracket(P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >= 0 && index_ < shape_[0], "index out of range of array");
    }
    template<typename, int> friend class ra::rarray;  // allow descending
    template<typename, int, typename> friend class detail::Bracket;  // allow descending
 public:
    /// Used for conversion to whatever parent.at() gives.
    RA_FORCE_inline auto at(size_type nextindex) -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
};

/// Class for repeated bracket access to const elements
template<typename T, rank_type R, typename P>
class ConstBracket {
 private:
    const P&       parent_;
    size_type      index_;

 public:
    RA_FORCE_inline auto operator[](size_type nextindex) const noexcept(RA_noboundscheck) -> ConstBracket<T, R-1, ConstBracket> {
        return { *this, nextindex, shape_ + 1 };
    }
    /// Implement implicit conversion to whatever parent.at() gives.
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    ConstBracket(const ConstBracket&) = delete;
    ConstBracket(const ConstBracket&&) = delete;
    auto operator=(const ConstBracket&) -> ConstBracket& = delete;
    auto operator=(const ConstBracket&&) -> ConstBracket& = delete;
    ~ConstBracket() = default;

 private:
    const size_type* shape_;
    RA_FORCE_inline ConstBracket(const P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >= 0 && index_ < shape_[0], "index out of range of array");
    }
    RA_FORCE_inline auto private_at(size_type nextindex) const noexcept(RA_noboundscheck) -> decltype(parent_.private_at(index_)[nextindex]) {
        return parent_.private_at(index_)[nextindex];
    }
    template<typename, int> friend class ra::rarray;  // allow descending
    template<typename, int, typename> friend class detail::ConstBracket;

 public:
    /// Used for conversion to whatever parent.at() gives.
    RA_FORCE_inline auto at(size_type nextindex) const -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
    #if __cpp_multidimensional_subscript >= 202110L
    /// Allow C++23 multidimensional subscripts
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type nextindex, Ts... args) -> typename std::enable_if<R == sizeof...(Ts)+1, const T&>::type {
        return operator[](nextindex)[args...];
    }
    #endif
};

/// Class for repeated bracket access to const elements, specialized for R==1
template<typename T, typename P>
class ConstBracket<T, 1, P> {
 private:
    const P& parent_;
    size_type  index_;

 public:
    RA_FORCE_inline auto operator[](size_type nextindex) const noexcept(RA_noboundscheck) -> const T& {
        RA_CHECKORSAY(nextindex >=0 && nextindex < shape_[1], "index out of range of array");
        return parent_.private_at(index_)[nextindex];
    }
    /// Implement implicit conversion to whatever parent.at() gives.
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        return parent_.at(index_);
    }
    ConstBracket(const ConstBracket&) = delete;
    ConstBracket(const ConstBracket&&) = delete;
    auto operator=(const ConstBracket&) -> ConstBracket& = delete;
    auto operator=(const ConstBracket&&) -> ConstBracket& = delete;
    ~ConstBracket() = default;

 private:
    const size_type* shape_;
    RA_FORCE_inline ConstBracket(const P& parent, size_type i, const size_type* shape) noexcept(RA_noboundscheck)
    : parent_(parent), index_(i), shape_(shape) {
        RA_CHECKORSAY(index_ >= 0 && index_ < shape_[0], "index out of range of array");
    }
    template<typename, int> friend class ra::rarray;  ///< allow descending
    template<typename, int, typename> friend class detail::ConstBracket;

 public:
    /// Used for conversion to whatever parent.at() gives.
    RA_FORCE_inline auto at(size_type nextindex) const -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
};

// generic version of make_rarray, but this cannot use type deduction

template<typename U>
auto make_rarray_g(std::initializer_list<U> list, ra::MISSING missing_policy) ->  rarray<typename ra::detail::init_list_prop<decltype(list)>::type,ra::detail::init_list_prop<decltype(list)>::rank>
{
    rarray<typename ra::detail::init_list_prop<decltype(list)>::type, ra::detail::init_list_prop<decltype(list)>::rank> a;
    a.form(list, missing_policy);
    return a;
}
    
}  // namespace detail

template<typename S>
inline auto linspace(S x1, S x2, size_type n = 0, bool end_incl = true) -> rarray<S, 1> {
    if (n == 0) {
        if (x2 > x1)
            n = static_cast<int>(x2 - x1 + end_incl);
        else
            n = static_cast<int>(x1 - x2 + end_incl);
    }
    rarray<S, 1> x(n);
    for (size_type i = 0; i < n; i++)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wconversion"
        x[i] = x1 + static_cast<S>(((x2-x1)*i)/(n-end_incl));
        #pragma GCC diagnostic pop
    if (end_incl)
        x[n-1] = x2;
    return x;
}

/// Helper class to generate a range of numbers
/// @tparam  T  should be an arithemetic type, i.e, an integer, float, or double.
template<class T>
class Xrange {
 private:
    struct const_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::input_iterator_tag;
        inline const_iterator(): i_(0), di_(1), b_(0) {}
        inline const_iterator(T i, T di, T b): i_(i), di_(di), b_(b) {}
        inline auto operator!=(const const_iterator& other) const -> bool {
            return i_ != other.i_;
        }
        inline auto operator==(const const_iterator& other) const -> bool {
            return i_ == other.i_ && di_ == other.di_ && b_ == other.b_;
        }
        inline auto operator++() -> const_iterator& {
            i_ += di_;
            if (di_ > 0 && i_ >= b_)
               i_ = b_;
            if (di_ < 0 && i_ <= b_)
               i_ = b_;
            return *this;
        }
        inline auto operator++(int) -> const_iterator {
            const const_iterator temp = *this;
            this->operator++();
            return temp;
        }
        inline auto operator*() const -> const T& {
            return i_;
        }
        T i_, di_, b_;
    };
    T a_, b_, d_;

 public:
    inline Xrange(T a, T b, T d)
    : a_(a), b_(a + static_cast<T>(static_cast<T>(std::ceil(static_cast<double>(b-a)/static_cast<double>(d)))*d)), d_(d)
    {}
    inline auto begin() const -> const_iterator {
        return const_iterator(a_, d_, b_);
    }
    inline auto end() const -> const_iterator {
        return const_iterator(b_, d_, b_);
    }
    inline auto size() const -> size_t  {
        return static_cast<size_t>((b_-a_)/d_);
    }
};

template<class T>
inline auto xrange(T end) -> Xrange<T> {
    return Xrange<T>(static_cast<T>(0), end, static_cast<T>(1));
}

template<class S, class T>
inline auto xrange(S begin, T end) -> Xrange<T> {
    return Xrange<T>(static_cast<T>(begin), end, static_cast<T>(1));
}

template<class S, class T, class U>
inline auto xrange(S begin, T end, U step) -> Xrange<T> {
    return Xrange<T>(static_cast<T>(begin), end, static_cast<T>(step));
}

template<typename A>
inline auto extent(const A &a, int i) -> size_type {
    switch (i) {
    case 0: return std::extent<A, 0>();
    case 1: return std::extent<A, 1>();
    case 2: return std::extent<A, 2>();
    case 3: return std::extent<A, 3>();
    case 4: return std::extent<A, 4>();
    case 5: return std::extent<A, 5>();
    case 6: return std::extent<A, 6>();
    case 7: return std::extent<A, 7>();
    case 8: return std::extent<A, 8>();
    case 9: return std::extent<A, 9>();
    case 10: return std::extent<A, 10>();
    default: throw std::out_of_range("ra::extent"); return 0;
    }
}

template<typename T, rank_type R>
inline auto extent(const rarray<T, R> &a, int i) -> size_type {
    return a.extent(i);
}

// specific versions of make_rarray for each rank, from 1 to 11, which can work with type deduction.

/// @name form
/// Functions to form new rarrays from nested initializer lists
/// @{
template<typename T>
auto make_rarray(std::initializer_list<T> list,
                 MISSING missing_policy = MISSING::DEFAULT) -> rarray<T,1>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<T>> list,
                 MISSING missing_policy = MISSING::DEFAULT) -> rarray<T,2>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,3>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,4>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,5>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,6>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,7>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,8>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,9>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,10>
{
    return detail::make_rarray_g(list, missing_policy);
}
template<typename T>
auto make_rarray(std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<
                 std::initializer_list<T>>>>>>>>>>> list,
                 MISSING missing_policy=MISSING::DEFAULT) -> rarray<T,11>
{
    return detail::make_rarray_g(list, missing_policy);
}
/// @}

/// @name form
/// Functions to form new rarrays from a shape and a fill value.
/// @{
template<typename T>
auto make_rarray(size_type n0, const T& fillvalue) -> rarray<T,1>
{
    rarray<T,1> result(n0);
    result.fill(fillvalue);
    return result;
}
template<typename T>
auto make_rarray(size_type n0, size_type n1, const T& fillvalue) -> rarray<T,2>
{
    rarray<T,2> result(n0,n1);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,const T& fillvalue) -> rarray<T,3>
{
    rarray<T,3> result(n0,n1,n2);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,const T& fillvalue) -> rarray<T,4>
{
    rarray<T,4> result(n0,n1,n2,n3);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,const T& fillvalue) -> rarray<T,5>
{
    rarray<T,5> result(n0,n1,n2,n3,n4);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,const T& fillvalue) -> rarray<T,6>
{
    rarray<T,6> result(n0,n1,n2,n3,n4,n5);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,const T& fillvalue) -> rarray<T,7>
{
    rarray<T,7> result(n0,n1,n2,n3,n4,n5,n6);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,const T& fillvalue) -> rarray<T,8>
{
    rarray<T,8> result(n0,n1,n2,n3,n4,n5,n6,n7);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,const T& fillvalue) -> rarray<T,9>
{
    rarray<T,9> result(n0,n1,n2,n3,n4,n5,n6,n7,n8);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,const T& fillvalue) -> rarray<T,10>
{
    rarray<T,10> result(n0,n1,n2,n3,n4,n5,n6,n7,n8,n9);
    result.fill(fillvalue);
    return result;
}

template<typename T>
auto make_rarray(size_type n0,size_type n1,size_type n2,size_type n3,size_type n4,size_type n5,size_type n6,size_type n7,size_type n8,size_type n9,size_type n10,const T& fillvalue) -> rarray<T,11>
{
    rarray<T,11> result(n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10);
    result.fill(fillvalue);
    return result;
}

}  // namespace ra

// make RARRAY and INDEX work for rarrays as well as automatic arrays:
namespace std {
template<typename T, ra::rank_type R>
struct remove_all_extents<ra::rarray<T, R>> {
    using type = T;
};
template<typename T, ra::rank_type R>
struct rank<ra::rarray<T, R>> {
    static const size_t value = R;
};
}  // namespace std

// Include I/O always (from version 2.3 onwards)
#include "rarrayio.h"

// Get rid of the macros
#include "rarraydelmacros.h"

// Global namespace stuff

// EXTENT, RARRAY and INDEX are deprecated
#define EXTENT(A, I)   ra::extent(A, I)
#define RARRAY(A)      rarray<typename std::remove_all_extents<decltype(A)>::type, std::rank<decltype(A)>::value>(A)
#define INDEX(A, X, I) RARRAY(A).index(X, I)

// for now:
using ra::rarray;
using ra::linspace;
using ra::xrange;
using ra::make_rarray;

// add rvector, rmatrix and rtensor shortcut types
template<typename T> using rvector = rarray<T, 1>;
template<typename T> using rmatrix = rarray<T, 2>;
template<typename T> using rtensor = rarray<T, 3>;

#else
#error "This file requires C++11 or newer support."
#endif

#endif  // RARRAY_H_
