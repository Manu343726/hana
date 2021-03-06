/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_TEST_LAWS_ORDERABLE_HPP
#define BOOST_HANA_TEST_LAWS_ORDERABLE_HPP

#include <boost/hana/assert.hpp>
#include <boost/hana/bool.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/operators.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/lazy.hpp>
#include <boost/hana/orderable.hpp>

#include <laws/base.hpp>


namespace boost { namespace hana { namespace test {
    template <typename T, typename = when<true>>
    struct TestOrderable : TestOrderable<T, laws> {
        using TestOrderable<T, laws>::TestOrderable;
    };

    template <typename T>
    struct TestOrderable<T, laws> {
        static_assert(_models<Orderable, T>{}, "");

        template <typename Xs>
        TestOrderable(Xs xs) {
            foreach2(xs, [](auto a, auto b) {

                // antisymmetry
                BOOST_HANA_CHECK(
                    hana::and_(hana::less_equal(a, b), hana::less_equal(b, a))
                        ^implies^ hana::equal(a, b)
                );

                // totality
                BOOST_HANA_CHECK(
                    hana::or_(hana::less_equal(a, b), hana::less_equal(b, a))
                );

                // other methods in terms of `less_equal`
                BOOST_HANA_CHECK(
                    hana::less(a, b) ^iff^ hana::not_(hana::less_equal(b, a))
                );

                BOOST_HANA_CHECK(
                    hana::greater(a, b) ^iff^ hana::less(b, a)
                );

                BOOST_HANA_CHECK(
                    hana::greater_equal(a, b) ^iff^ hana::not_(hana::less(a, b))
                );

                // less.than & al.
                BOOST_HANA_CHECK(less.than(a)(b) ^iff^ hana::less(b, a));
                BOOST_HANA_CHECK(greater.than(a)(b) ^iff^ hana::greater(b, a));
                BOOST_HANA_CHECK(less_equal.than(a)(b) ^iff^ hana::less_equal(b, a));
                BOOST_HANA_CHECK(greater_equal.than(a)(b) ^iff^ hana::greater_equal(b, a));

                // operators
                only_when_(has_operator<T, decltype(less)>,
                hana::lazy([](auto a, auto b) {
                    BOOST_HANA_CHECK(
                        hana::less(a, b) ^iff^ (a < b)
                    );
                })(a, b));

                only_when_(has_operator<T, decltype(less_equal)>,
                hana::lazy([](auto a, auto b) {
                    BOOST_HANA_CHECK(
                        hana::less_equal(a, b) ^iff^ (a <= b)
                    );
                })(a, b));

                only_when_(has_operator<T, decltype(greater_equal)>,
                hana::lazy([](auto a, auto b) {
                    BOOST_HANA_CHECK(
                        hana::greater_equal(a, b) ^iff^ (a >= b)
                    );
                })(a, b));

                only_when_(has_operator<T, decltype(greater)>,
                hana::lazy([](auto a, auto b) {
                    BOOST_HANA_CHECK(
                        hana::greater(a, b) ^iff^ (a > b)
                    );
                })(a, b));

                // ordering
                _injection<0> f{}; // test::_injection is also monotonic
                BOOST_HANA_CHECK(
                    hana::ordering(f)(a, b) ^iff^ hana::less(f(a), f(b))
                );
            });

            // transitivity
            foreach3(xs, [](auto a, auto b, auto c) {
                BOOST_HANA_CHECK(
                    hana::and_(hana::less_equal(a, b), hana::less_equal(b, c))
                        ^implies^ hana::less_equal(a, c)
                );
            });
        }
    };

    template <typename C>
    struct TestOrderable<C, when<_models<Constant, C>{}()>>
        : TestOrderable<C, laws>
    {
        template <typename Xs>
        TestOrderable(Xs xs) : TestOrderable<C, laws>{xs} {
            foreach2(xs, [](auto a, auto b) {

                BOOST_HANA_CHECK(
                    hana::value(hana::less(a, b)) ^iff^
                        hana::less(hana::value(a), hana::value(b))
                );

            });
        }
    };

    template <typename P>
    struct TestOrderable<P, when<_models<Product, P>{}()>>
        : TestOrderable<P, laws>
    {
        template <typename Products>
        TestOrderable(Products products)
            : TestOrderable<P, laws>{products}
        {
            foreach2(products, [](auto x, auto y) {
                BOOST_HANA_CHECK(
                    hana::less(x, y) ^iff^
                    hana::or_(
                        hana::less(hana::first(x), hana::first(y)),
                        hana::and_(
                            hana::equal(hana::first(x), hana::first(y)),
                            hana::less(hana::second(x), hana::second(y))
                        )
                    )
                );
            });
        }
    };

    template <typename S>
    struct TestOrderable<S, when<_models<Sequence, S>{}()>>
        : TestOrderable<S, laws>
    {
        struct invalid { };

        template <typename Xs>
        TestOrderable(Xs xs) : TestOrderable<S, laws>{xs} {
            constexpr auto list = make<S>;

            //////////////////////////////////////////////////////////////////
            // less
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(hana::not_(hana::less(
                list(),
                list()
            )));

            BOOST_HANA_CONSTANT_CHECK(hana::less(
                list(),
                list(invalid{})
            ));
            BOOST_HANA_CONSTANT_CHECK(hana::not_(hana::less(
                list(invalid{}),
                list()
            )));
            BOOST_HANA_CONSTANT_CHECK(hana::less(
                list(ct_ord<0>{}),
                list(ct_ord<7>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(hana::not_(hana::less(
                list(ct_ord<1>{}),
                list(ct_ord<0>{})
            )));

            BOOST_HANA_CONSTANT_CHECK(hana::not_(hana::less(
                list(ct_ord<0>{}, ct_ord<1>{}, ct_ord<8>{}),
                list(ct_ord<0>{}, ct_ord<1>{})
            )));

            BOOST_HANA_CONSTANT_CHECK(hana::less(
                list(ct_ord<0>{}, ct_ord<0>{}, ct_ord<8>{}),
                list(ct_ord<0>{}, ct_ord<1>{})
            ));
        }
    };
}}} // end namespace boost::hana::test

#endif // !BOOST_HANA_TEST_LAWS_ORDERABLE_HPP
