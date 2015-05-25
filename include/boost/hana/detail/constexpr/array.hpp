/*!
@file
Defines `boost::hana::detail::constexpr_::array`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_DETAIL_CONSTEXPR_ARRAY_HPP
#define BOOST_HANA_DETAIL_CONSTEXPR_ARRAY_HPP

#include <boost/hana/detail/constexpr/algorithm.hpp>
#include <boost/hana/detail/std/size_t.hpp>
#include <boost/hana/functional/placeholder.hpp>


namespace boost { namespace hana { namespace detail {
namespace constexpr_ {
    template <typename N>
    constexpr N factorial(N n) {
        N result = 1;
        while (n != 0)
            result *= n--;
        return result;
    }

    //! @ingroup group-details
    //! A minimal `std::array` with better `constexpr` support.
    //!
    //! We also provide some algorithms from the `constexpr/algorithm.hpp`
    //! header as member functions to make them easier to use in constexpr
    //! contexts, since a `constexpr` `array` can't be mutated in place.
    template <typename T, detail::std::size_t Size>
    struct array {
        T elems_[Size > 0 ? Size : 1];

        constexpr T& operator[](detail::std::size_t n)
        { return elems_[n]; }

        constexpr T const& operator[](detail::std::size_t n) const
        { return elems_[n]; }

        constexpr detail::std::size_t size() const noexcept
        { return Size; }

        constexpr T* begin() noexcept             { return &elems_[0]; }
        constexpr T const* begin() const noexcept { return &elems_[0]; }
        constexpr T* end() noexcept               { return &elems_[Size]; }
        constexpr T const* end() const noexcept   { return &elems_[Size]; }

        // Algorithms from constexpr/algorithm.hpp
        constexpr array reverse() const {
            array result = *this;
            constexpr_::reverse(result.begin(), result.end());
            return result;
        }

        template <typename BinaryPred>
        constexpr auto permutations(BinaryPred pred) const {
            array<array<T, Size>, constexpr_::factorial(Size)> result{};
            auto out = result.begin();
            array copy = *this;

            do *out++ = copy;
            while (constexpr_::next_permutation(copy.begin(), copy.end(), pred));

            return result;
        }

        constexpr auto permutations() const
        { return this->permutations(hana::_ < hana::_); }


        template <typename BinaryPred>
        constexpr auto sort(BinaryPred pred) const {
            array result = *this;
            constexpr_::sort(result.begin(), result.end(), pred);
            return result;
        }

        constexpr auto sort() const
        { return this->sort(hana::_ < hana::_); }

        template <typename U>
        constexpr auto iota(U value) const {
            array result = *this;
            constexpr_::iota(result.begin(), result.end(), value);
            return result;
        }
    };

    template <typename T, detail::std::size_t M, typename U, detail::std::size_t N>
    constexpr bool operator==(array<T, M> a, array<U, N> b)
    { return M == N && constexpr_::equal(a.begin(), a.end(), b.begin(), b.end()); }

    template <typename T, detail::std::size_t M, typename U, detail::std::size_t N>
    constexpr bool operator<(array<T, M> a, array<U, N> b) {
        return M < N || constexpr_::lexicographical_compare(
                                    a.begin(), a.end(), b.begin(), b.end());
    }
} // end namespace constexpr_

using constexpr_::array;

}}} // end namespace boost::hana::detail

#endif // !BOOST_HANA_DETAIL_CONSTEXPR_ARRAY_HPP
