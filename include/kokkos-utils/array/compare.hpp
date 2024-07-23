#ifndef KOKKOS_UTILS_ARRAY_COMPARE_HPP
#define KOKKOS_UTILS_ARRAY_COMPARE_HPP

#include "Kokkos_Array.hpp"

/**
 * @file
 *
 * This file defines @c operator== for @c Kokkos::Array in the <tt>namespace Kokkos</tt>.
 *
 * This file can disappear when either https://github.com/kokkos/kokkos/pull/6599 or
 * https://github.com/kokkos/kokkos/pull/7148 is merged.
 */

namespace Kokkos
{
    //! Operator== for @c Kokkos::Array.
    template <
        typename LT, size_t LN, typename LP,
        typename RT, size_t RN, typename RP
    >
    KOKKOS_INLINE_FUNCTION constexpr bool operator==(
        const Array<LT, LN, LP>& lhs,
        const Array<RT, RN, RP>& rhs
    ) {
        if (lhs.size() != rhs.size()) return false;
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) return false;
        }
        return true;
    }

    //! Operator!= for @c Kokkos::Array.
    template <
        typename LT, size_t LN, typename LP,
        typename RT, size_t RN, typename RP
    >
    KOKKOS_INLINE_FUNCTION constexpr bool operator!=(
        const Array<LT, LN, LP>& lhs,
        const Array<RT, RN, RP>& rhs
    ) {
        return !(lhs == rhs);
    }

} // namespace Kokkos

#endif // KOKKOS_UTILS_ARRAY_COMPARE_HPP
