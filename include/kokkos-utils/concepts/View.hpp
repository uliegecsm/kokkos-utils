#ifndef KOKKOS_UTILS_CONCEPTS_VIEW_HPP
#define KOKKOS_UTILS_CONCEPTS_VIEW_HPP

#include <concepts>

#include "Kokkos_View.hpp"

#include "kokkos-utils/impl/type_traits.hpp"

namespace Kokkos::utils::concepts
{

//! Specify that a type is a @c Kokkos::View.
template <typename T>
concept View = Kokkos::is_view_v<T>;

//! Specify that a type is a @c Kokkos::View of given rank @p Rank.
template <typename T, std::size_t Rank>
concept ViewOfRank = View<T> && T::rank == Rank;

//! Specify that a type is a modifiable @c Kokkos::View.
template <typename T>
concept ModifiableView = View<T> && ! std::is_const_v<typename T::value_type>;

//! Specify that a type is a @c Kokkos::View with value type @p ValueType.
template <typename T, typename ValueType>
concept ViewOf = View<T> && std::same_as<typename T::value_type, ValueType>;

//! Specify that a type is a @c Kokkos::View, whose value type is an instance of a given class template @p U.
template <typename T, template <typename...> class U>
concept ViewOfInstanceOf = View<T> && impl::InstanceOf<typename T::value_type, U>;

} // namespace Kokkos::utils::concepts

#endif // KOKKOS_UTILS_CONCEPTS_VIEW_HPP
