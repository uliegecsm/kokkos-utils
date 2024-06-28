#ifndef KOKKOS_UTILS_ATOMICS_INSERTOP_HPP
#define KOKKOS_UTILS_ATOMICS_INSERTOP_HPP

#include <concepts>

#include "kokkos-utils/concepts/View.hpp"

namespace Kokkos::utils::atomics
{

/**
 * @brief Insert an element in a view at a specific index using @c Kokkos::atomic_min.
 *
 * To be used with *e.g.* @ref Kokkos::UnorderedMap::insert.
 */
struct InsertMin
{
    template <concepts::View ViewType, std::integral IndexType, typename ValueType>
    KOKKOS_FUNCTION
    void op(const ViewType& values, const IndexType index, ValueType&& value) const {
        Kokkos::atomic_min(&values(index), std::forward<ValueType>(value));
    }
};

} // namespace Kokkos::utils::atomics

#endif // KOKKOS_UTILS_ATOMICS_INSERTOP_HPP
