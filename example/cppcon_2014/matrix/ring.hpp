/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXAMPLE_CPPCON_2014_MATRIX_RING_HPP
#define BOOST_HANA_EXAMPLE_CPPCON_2014_MATRIX_RING_HPP

#include "matrix.hpp"

#include <boost/hana/detail/std/integral_constant.hpp>
#include <boost/hana/foldable.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/sequence.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/ring.hpp>
#include <boost/hana/tuple.hpp>

#include <utility>


namespace boost { namespace hana {
    template <unsigned R1, unsigned C1, unsigned R2, unsigned C2>
    struct mult_impl<cppcon::Matrix<R1, C1>, cppcon::Matrix<R2, C2>> {
        template <typename M1, typename M2>
        static constexpr decltype(auto) apply(M1&& m1, M2&& m2) {
            static_assert(C1 == R2,
                "wrong dimensions for matrix multiplication");
            auto cols = cppcon::columns(std::forward<M2>(m2));
            return unpack(
                transform(cppcon::rows(std::forward<M1>(m1)),
                    [&](auto&& row) -> decltype(auto) {
                        return zip.with(cppcon::detail::tuple_scalar_product,
                            repeat<Tuple>(std::forward<decltype(row)>(row), uint<R1>),
                            cols
                        );
                    }
                ),
                cppcon::matrix
            );
        }
    };

    template <unsigned R, unsigned C>
    struct one_impl<cppcon::Matrix<R, C>> {
        static constexpr decltype(auto) apply() {
            return unpack(range_c<unsigned, 0, R>, [](auto ...n) {
                return unpack(range_c<unsigned, 0, C>, [=](auto ...m) {
                    auto row = [=](auto n) {
                        return cppcon::row(if_(n == m, int_<1>, int_<0>)...);
                    };
                    return cppcon::matrix(row(n)...);
                });
            });
        }
    };
}}

#endif // !BOOST_HANA_EXAMPLE_CPPCON_2014_MATRIX_RING_HPP
