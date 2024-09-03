#ifndef KOKKOS_UTILS_IMPL_TYPE_LIST_HPP
#define KOKKOS_UTILS_IMPL_TYPE_LIST_HPP

#include "impl/Kokkos_Utilities.hpp"

/**
 * @file
 *
 * This file provides extensions to the @c Kokkos::Impl::type_list struct in @c Kokkos_Utilities.hpp.
 */

namespace Kokkos::utils::impl
{

//! @name Get the number of types in a @c Kokkos::Impl::type_list.
///@{
template <typename>
struct TypeListSize;

template <typename... Ts>
struct TypeListSize<Kokkos::Impl::type_list<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};

template <typename T>
inline constexpr size_t type_list_size_v = TypeListSize<T>::value;
///@}

//! @name Get the @p I th type in a @c Kokkos::Impl::type_list.
///@{
template <size_t, typename>
struct TypeListAt;

template <size_t I, typename Head, typename... Tail>
struct TypeListAt<I, Kokkos::Impl::type_list<Head, Tail...>>
  : TypeListAt<I - 1, Kokkos::Impl::type_list<Tail...>> {};

template <class Head, class... Tail>
struct TypeListAt<0, Kokkos::Impl::type_list<Head, Tail...>>
{
    using type = Head;
};

template <size_t I, typename T>
using type_list_at_t = typename TypeListAt<I, T>::type;
///@}

//! @name Transform a @c Kokkos::Impl::type_list.
///@{
template <template <typename> typename, typename>
struct TransformTypeList;

template <template <typename> typename TransformerType, typename... Ts>
struct TransformTypeList<TransformerType, Kokkos::Impl::type_list<Ts...>>
{
    using type = Kokkos::Impl::concat_type_list_t<Kokkos::Impl::type_list<typename TransformerType<Ts>::type...>>;
};

template <template <typename> typename TransformerType, typename T>
using transform_type_list_t = typename TransformTypeList<TransformerType, T>::type;
///@}

//! @name Convert a @c Kokkos::Impl::type_list into a @c std::tuple.
///@{
template <typename>
struct TypeListToTuple;

template <typename... Ts>
struct TypeListToTuple<Kokkos::Impl::type_list<Ts...>>
{
    using type = std::tuple<Ts...>;
};

template <typename T>
using type_list_to_tuple_t = typename TypeListToTuple<T>::type;
///@}

} // namespace Kokkos::utils::impl

#endif // KOKKOS_UTILS_IMPL_TYPE_LIST_HPP
