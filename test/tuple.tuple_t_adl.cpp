/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/tuple.hpp>
using namespace boost::hana;


template <bool b = false>
struct invalid { static_assert(b, "invalid must not be instantiated"); };

template <typename T> void adl(T) { }
template <typename ...T> void adl_pattern(_tuple_t<T...>) { }

int main() {
    // ADL kicks in but `invalid<>` must not instantiated
    adl(tuple_t<invalid<>>);
    adl_pattern(tuple_t<invalid<>>);
}
