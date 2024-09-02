#ifndef KOKKOS_UTILS_IMPL_TYPE_TRAITS_HPP
#define KOKKOS_UTILS_IMPL_TYPE_TRAITS_HPP

#include <type_traits>

/**
 * @file
 *
 * This file provides extensions to the @c type_traits standard header.
 *
 * References:
 *      * https://en.cppreference.com/w/cpp/header/type_traits
 */

namespace Kokkos::utils::impl
{

/**
 * @name Check if a type is an instantiation of a given class template.
 *
 * References:
 *      * https://indii.org/blog/is-type-instantiation-of-template/
 */
///@{
template <class T, template <typename...> class U>
inline constexpr bool is_instance_of_v = std::false_type{};

template <template <typename...> class U, typename... Vs>
inline constexpr bool is_instance_of_v<U<Vs...>, U> = std::true_type{};

template <typename T, template <typename...> class U>
concept InstanceOf = is_instance_of_v<T, U>;
///@}

} // namespace Kokkos::utils::impl

#endif // KOKKOS_UTILS_IMPL_TYPE_TRAITS_HPP
