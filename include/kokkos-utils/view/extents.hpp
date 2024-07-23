#ifndef KOKKOS_UTILS_VIEW_EXTENTS_HPP
#define KOKKOS_UTILS_VIEW_EXTENTS_HPP

#include "kokkos-utils/concepts/View.hpp"

/**
 * @file
 *
 * This file is a collection of helper functions similar to what can be found in
 * the standard @c mdspan header, related to retrieving the extents of a @c Kokkos::View.
 */

namespace Kokkos::utils::view
{

namespace impl
{

//! Implementation of @ref Kokkos::utils::view::extents.
template <concepts::View ViewType, size_t... Ints>
KOKKOS_INLINE_FUNCTION
constexpr auto extents(const ViewType& view, std::index_sequence<Ints...>)
{
    Kokkos::Array<size_t, sizeof...(Ints)> extents {};
    ((extents[Ints] = view.extent(Ints)), ...);
    return extents;
}

} // namespace impl

//! Get all extents of @p view.
template <concepts::View ViewType>
KOKKOS_INLINE_FUNCTION
constexpr auto extents(const ViewType& view)
{
    constexpr auto rank = ViewType::rank;
    constexpr auto dims = std::make_index_sequence<rank>{};
    return impl::extents(view, dims);
}

} // namespace Kokkos::utils::view

#endif // KOKKOS_UTILS_VIEW_EXTENTS_HPP
