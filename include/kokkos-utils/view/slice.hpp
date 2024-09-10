#ifndef KOKKOS_UTILS_VIEW_SLICE_HPP
#define KOKKOS_UTILS_VIEW_SLICE_HPP

#include "kokkos-utils/concepts/View.hpp"

/**
 * @file
 *
 * This file is a collection of helper functions similar to what can be found in
 * the standard @c mdspan header, related to retrieving slices of a @c Kokkos::View.
 *
 * References:
 *  - https://en.cppreference.com/w/cpp/container/mdspan
 *  - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html
 */

namespace Kokkos::utils::view
{

namespace impl
{

template <typename ViewType, typename... Indices, size_t... AllsIndices>
KOKKOS_FUNCTION
constexpr auto slice(ViewType&& view, Indices&&... indices, std::index_sequence<AllsIndices...>)
{
    return Kokkos::subview(
        std::forward<ViewType>(view),
        std::forward<Indices>(indices)...,
        std::get<AllsIndices>(std::array<Kokkos::ALL_t, sizeof...(AllsIndices)>{})...
    );
}

} // namespace impl

/**
 * @brief Get a subview, given the first @p indices. The rest is filled with @c Kokkos::ALL.
 *
 * @note Though similar to @c std::submdspan, we think it it OK not to provide the remaining
 *       slice specifiers if they are all @c Kokkos::ALL iif the user specifies what is the expected rank
 *       of the input view. This ensures that the user will not face unexpected behaviors.
 *       Therefore, we allow the following:
 *       @code
 *       auto sliced = slice<4>(view_of_rank_4, 1, 2);
 *       @endcode
 *       which is strictly equivalent to:
 *       @code
 *       auto sliced = subview(view_of_rank_4, 1, 2, Kokkos::ALL, Kokkos::ALL);
 *       @endcode
 */
template <size_t Rank, typename ViewType, typename... Indices>
requires concepts::ViewOfRank<std::remove_reference_t<ViewType>, Rank>
KOKKOS_FUNCTION
constexpr auto slice(ViewType&& view, Indices&&... indices)
{
    constexpr auto size = Rank - sizeof...(Indices);
    return impl::slice<ViewType, Indices...>(
        std::forward<ViewType>(view),
        std::forward<Indices>(indices)...,
        std::make_index_sequence<size>{}
    );
}

//! @overload
template <typename ViewType, typename... Indices>
requires concepts::ViewOfRank<std::remove_reference_t<ViewType>, sizeof...(Indices)>
KOKKOS_FUNCTION
constexpr auto slice(ViewType&& view, Indices&&... indices)
{
    return slice<std::remove_reference_t<ViewType>::rank()>(
        std::forward<ViewType>(view),
        std::forward<Indices>(indices)...
    );
}

} // namespace Kokkos::utils::view

#endif // KOKKOS_UTILS_VIEW_SLICE_HPP
