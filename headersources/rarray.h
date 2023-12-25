//
// rarray - Runtime arrays: template classes for pointer-based,
//          runtime, reference counted, multi-dimensional
//          arrays.  Documentation in rarraydoc.pdf
//
// Copyright (c) 2013-2023  Ramses van Zon
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
#include "versionheader.h"
#include "rarraymacros.h"
#include "rarraytypes.h"
#include "shared_buffer.h"
#include "shared_shape.h"

// Forward definitions of ra::rarray<T, R> and ra::CommaOp
namespace ra {

enum class RESIZE { NO,  ALLOWED };

namespace detail {
template<typename T> class CommaOp;
template<typename T, rank_type R, typename P> class Bracket;
template<typename T, rank_type R, typename P> class ConstBracket;
// TODO(geet): Each operator will create a subexpression of the Expr<...>,
// which we forward-define first Forward definitions to support array
// expressions // What type enumerates possible operators?
// using ExOp = int;
#define ExOp class
template<typename T, rank_type R, ExOp AOP, typename A1, typename A2, typename A3> class Expr;
}  // namespace detail

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
    // constructor leaving rarray undefined
    inline rarray()
    : buffer_(), shape_() {}
    // constructors creating its own buffer for various R values
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
    // constructors from an existing buffer
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
    // constructors from automatic arrays
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
    // constructors from compound literal automatic arrays
    // (non-standard c++ and may lead to dangling references)
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
    // (shallow) copy constructor and assignment operator
    RA_FORCE_inline rarray(const rarray<T, R> &a) noexcept
    : buffer_(a.buffer_),
      shape_(a.shape_)
    {}
    inline auto operator=(const rarray<T, R> &a) noexcept -> rarray<T, R>& {
        buffer_ = a.buffer_;
        shape_ = a.shape_;
        return *this;
    }
    // move constructor and assignment operator
    inline rarray(rarray<T, R>&& x) noexcept
    : buffer_(std::move(x.buffer_)),
      shape_(std::move(x.shape_))
    {}
    inline auto operator=(rarray<T, R>&& x) noexcept -> rarray<T, R>& {
        buffer_ = std::move(x.buffer_);
        shape_ = std::move(x.shape_);
        return *this;
    }
    // Comma separated element assignment
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
    // destructor
    RA_FORCE_inline ~rarray() = default;
    // Will need constructor and assignment for expressions
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline explicit rarray(const detail::Expr<T, R, AOP, A1, A2, A3>& e);
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator+=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator-=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator*=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator/=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    template<ExOp AOP, typename A1, typename A2, typename A3> RA_FORCE_inline auto operator%=(const detail::Expr<T, R, AOP, A1, A2, A3>& e) -> rarray&;
    // reshape shallow copy keeping the underlying data
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void reshape(size_type n0, RESIZE resize_allowed = RESIZE::NO) {
        if ( (size() == n0) || (resize_allowed == RESIZE::ALLOWED && size() >= n0) )
            shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void reshape(size_type n0, size_type n1, RESIZE resize_allowed = RESIZE::NO) {
        if (size() == n0*n1
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1))
            shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4*n5
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4*n5))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4*n5*n6
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4*n5*n6))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4*n5*n6*n7
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4*n5*n6*n7))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                                + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8, size_type n9,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8*n9
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8*n9))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ")
                                    + std::string(__PRETTY_FUNCTION__));
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void reshape(size_type n0, size_type n1, size_type n2, size_type n3,
                        size_type n4, size_type n5, size_type n6, size_type n7,
                        size_type n8, size_type n9, size_type n10,
                        RESIZE resize_allowed = RESIZE::NO)  {
        if (size() == n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10
            || (resize_allowed == RESIZE::ALLOWED && size() >= n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10))
            shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10}, buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
    }
    inline void reshape(const size_type* newshape, RESIZE resize_allowed = RESIZE::NO) {
        size_type newsize = std::accumulate(newshape, newshape+R,
                                            1, std::multiplies<size_type>());
        if (size() == newsize
            || (resize_allowed == RESIZE::ALLOWED && size() >= newsize))
            shape_ = detail::shared_shape<T, R>((const std::array<size_type, R>&)(*newshape), buffer_.begin());
        else
            throw std::out_of_range(std::string("Incompatible dimensions in function ") + std::string(__PRETTY_FUNCTION__));
    }
    // create a deep copy
    inline auto copy() const -> rarray<T, R> {
        // return a copy
        rarray<T, R> clone;
        clone.buffer_ = buffer_.copy();
        clone.shape_ = shape_.copy();
        clone.shape_.relocate(clone.buffer_.begin());
        return clone;
    }
    // automatic conversion to const value_type, *if* it is not already const
    template<typename U = T,
             typename = typename std::enable_if<!std::is_const<U>::value>::type>
    inline operator const rarray<const U, R>&() const noexcept {
        return const_ref();
    }
    // properties
    constexpr auto rank() const noexcept -> int {
        return R;
    }
    // check if undefined
    inline auto empty() const noexcept -> bool {
        return buffer_.cbegin() == nullptr;
    }
    // retrieve array size in dimension i
    inline auto extent(int i) const -> size_type {
        return shape_.extent(i);
    }
    // retrieve array sizes in all dimensions
    RA_FORCE_inline auto shape() const noexcept -> const size_type* {
        return &(shape_.extent()[0]);
    }
    // retrieve the total number of elements
    inline auto size() const noexcept -> size_type {
        return shape_.size();
    }
    // return pointer to the internal data
    inline auto data() noexcept -> T* {
        return buffer_.begin();
    }
    // return a T* to the internal data
    inline auto data() const noexcept -> const T* {
        return buffer_.begin();
    }
    // return a T*const*.. acting similarly to this rarray when using []:
    inline auto ptr_array() const noexcept -> parray_t {
        return shape_.ptrs();
    }
    // return a T**.. acting similarly to this rarray when using []:
    inline auto noconst_ptr_array() const noexcept -> noconst_parray_t {
        return const_cast<noconst_parray_t>(shape_.ptrs());
    }
    // create a reference to this that treats elements as constant:
    inline auto const_ref() const noexcept -> const rarray<const T, R>& {
        return reinterpret_cast<const rarray<const T, R>&>(*this);
    }
    // modifiers
    // make undefined
    RA_FORCE_inline void clear() noexcept {
        shape_ = detail::shared_shape<T, R>();
        buffer_ = detail::shared_buffer<T>();
    }
    // fill with uniform value (does not change the size || shape)
    inline void fill(const T& value) {
        buffer_.fill(value);
    }
    // assign with uniform value (will set the size and shape)
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 1>::type>
    inline void assign(size_type n0, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0);
        shape_ = detail::shared_shape<T, R>({n0}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 2>::type>
    inline void assign(size_type n0, size_type n1, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1);
        shape_ = detail::shared_shape<T, R>({n0, n1}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 3>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 4>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 5>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 6>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 7>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 8>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 9>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 10>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8, size_type n9,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9}, buffer_.begin());
        buffer_.fill(value);
    }
    template<rank_type R_ = R, class = typename std::enable_if<R_ == 11>::type>
    inline void assign(size_type n0, size_type n1, size_type n2, size_type n3,
                       size_type n4, size_type n5, size_type n6, size_type n7,
                       size_type n8, size_type n9, size_type n10,
                       const T& value) {
        buffer_ = detail::shared_buffer<T>(n0*n1*n2*n3*n4*n5*n6*n7*n8*n9*n10);
        shape_ = detail::shared_shape<T, R>({n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10},
                                           buffer_.begin());
        buffer_.fill(value);
    }
    // iterators over the data
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
    // compute index of a reference inside an array
    // if a is an element in the array, get index in dimension i of that element
    // if i points at an element in the array, get index in dimension i of that element
    // if a is an element in the array, get the indices of that element
    // if i points at an element in the array, get the indices of that element
    inline auto index(const T& a, int i) const -> size_type {
        // retrieve index in dimension i within *this of the element a
        ptrdiff_t linearindex = &a - &(buffer_[0]);
        if (linearindex < 0 || linearindex >= size())
            throw "element not in array";
        const size_type* extent_ = shape();
        for (int j = R-1; j > i; j--)
            linearindex /= extent_[j];
        return linearindex % extent_[i];
    }
    inline auto index(const iterator& iter, int i) const -> size_type {
        return index(*iter, i);
    }
    inline auto index(const T& a) const -> std::array<size_type, R> {
        std::array<size_type, R> ind;
        ptrdiff_t linearindex = &a - &(buffer_[0]);
        RA_CHECKORSAY(linearindex >= 0 && linearindex < size(), "element not in array");
        int j = R;
        const size_type* extent_ = shape();
        while (j-->0) {
            ind[j] = linearindex % extent_[j];
            linearindex /= extent_[j];
        }
        return ind;
    }
    inline auto index(const iterator& i) const -> std::array<size_type, R> {
        return index(*i);
    }
    // old forms of index functions:
    inline auto index(const T& a, size_type* ind) const -> size_type* {
        ptrdiff_t linearindex = &a - &(buffer_[0]);
        RA_CHECKORSAY(linearindex >= 0 && linearindex < size(), "element not in array");
        int j = R;
        const size_type* extent_ = shape();
        while (j-->0) {
            ind[j] = linearindex % extent_[j];
            linearindex /= extent_[j];
        }
        return ind;
    }
    inline auto index(const iterator& i, size_type* ind) const -> size_type* {
        return index(*i, ind);
    }
    // access subarrays with bounds checking
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
    // for square bracket access:
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
    // allow c++23 multidimentional subscripts
    #if __cpp_multidimensional_subscript >= 202110L
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
    #endif
    // TODO(geet): for expressions
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
    // need the following for rarray<T, R+1>::at(..) const
    inline rarray(const detail::shared_buffer<T>&& abuffer, detail::shared_shape<T, R>&& ashape) :
        buffer_(std::forward<detail::shared_buffer<T>>(const_cast<detail::shared_buffer<T>&& >(abuffer))),
        shape_(std::forward<detail::shared_shape<T, R>>(ashape))
    {}
    detail::shared_buffer<T>  buffer_;
    detail::shared_shape<T, R> shape_;
};  // end definition rarray<T, R>

namespace detail {
// Class to facilitate assignment from a comma separated list
template<typename T>
class CommaOp {
 public:
    // put the next number into the array:
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
    T *ptr_;          // points to next element to be filled
    T * const last_;  // points to last element
    template<typename, int> friend class ra::rarray;
};

// Classes for repeated bracket access to elements
template<typename T, rank_type R, typename P>
class Bracket {
 private:
    P&               parent_;  // what array/array expression is being accessed
    size_type        index_;   // at what index
    const size_type* shape_;   // what is the shape of the result

 public:
    // implement square brackets to go to the next level:
    RA_FORCE_inline auto operator[](size_type nextindex) noexcept(RA_noboundscheck) -> Bracket<T, R-1, Bracket> {
        return { *this, nextindex, shape_ + 1 };
    }
    // implement implicit conversion to whatever parent.at() gives:
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
    template<typename, int> friend class ra::rarray;  // allow descending
    template<typename, int, typename> friend class detail::Bracket;  // allow descending
 public:
    RA_FORCE_inline auto at(size_type nextindex) -> decltype(parent_.at(index_).at(nextindex)) {
        return parent_.at(index_).at(nextindex);
    }
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type nextindex, Ts... args) -> typename std::enable_if<R == sizeof...(Ts)+1, T&>::type {
        return operator[](nextindex)[args...];
    }
    #endif
};

template<typename T, typename P>
class Bracket<T, 1, P> {
 private:
    P&               parent_;  // what array/array expression is being accessed
    size_type        index_;   // at what index
    const size_type* shape_;   // what is the shape of the result

 public:
    // implement square brackets to go to the next level:
    RA_FORCE_inline auto operator[](size_type nextindex) noexcept(RA_noboundscheck) -> T& {
        RA_CHECKORSAY(nextindex >= 0 && nextindex < shape_[1], "index out of range of array");
        return parent_.private_at(index_)[nextindex];
    }
    // implement implicit conversion to whatever parent.at() gives:
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
    RA_FORCE_inline auto at(size_type nextindex) -> decltype(parent_.at(index_).at(nextindex)) {
        // used for conversion to whatever parent.at() gives
        return parent_.at(index_).at(nextindex);
    }
};

template<typename T, rank_type R, typename P>
class ConstBracket {
 private:
    const P&       parent_;
    size_type      index_;

 public:
    RA_FORCE_inline auto operator[](size_type nextindex) const noexcept(RA_noboundscheck) -> ConstBracket<T, R-1, ConstBracket> {
        return { *this, nextindex, shape_ + 1 };
    }
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        // implement implicit conversion to whatever parent.at() gives
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
    RA_FORCE_inline auto at(size_type nextindex) const -> decltype(parent_.at(index_).at(nextindex)) {
        // used for conversion to whatever parent.at() gives
        return parent_.at(index_).at(nextindex);
    }
    // allow c++23 multidimentional subscripts
    #if __cpp_multidimensional_subscript >= 202110L
    template<typename... Ts>
    RA_FORCE_inline auto operator[](size_type nextindex, Ts... args) -> typename std::enable_if<R == sizeof...(Ts)+1, const T&>::type {
        return operator[](nextindex)[args...];
    }
    #endif
};

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
    RA_FORCE_inline operator decltype(parent_.at(index_)) () {
        // implement implicit conversion to whatever parent.at() gives
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
    template<typename, int> friend class ra::rarray;  // allow descending
    template<typename, int, typename> friend class detail::ConstBracket;

 public:
    RA_FORCE_inline auto at(size_type nextindex) const -> decltype(parent_.at(index_).at(nextindex)) {
        // used for conversion to whatever parent.at() gives
        return parent_.at(index_).at(nextindex);
    }
};

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

// T should be an arithemetic type, i.e, an integer, float || double.
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

// add rvector, rmatrix and rtensor shortcut types
template<typename T> using rvector = rarray<T, 1>;
template<typename T> using rmatrix = rarray<T, 2>;
template<typename T> using rtensor = rarray<T, 3>;

#else
#error "This file requires C++11 or newer support."
#endif

#endif  // RARRAY_H_
