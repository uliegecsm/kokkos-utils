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
//! @name Check if a type is a @c Kokkos::Impl::type_list.
///@{
template <typename T>
struct is_type_list : public std::false_type {};

template <typename... T>
struct is_type_list<Kokkos::Impl::type_list<T...>> : public std::true_type {};

template <typename T>
inline constexpr bool is_type_list_v = is_type_list<T>::value;
///@}

//! @name Make a type list if it's not one already.
///@{
template <typename...>
struct make_type_list;

template <typename T> requires Kokkos::utils::impl::is_type_list_v<T>
struct make_type_list<T>
{
    using type = T;
};

template <typename T, typename... Ts>
struct make_type_list<T, Ts...>
{
    using type = Kokkos::Impl::type_list<T, Ts...>;
};

template <typename... Args>
using make_type_list_t = typename make_type_list<Args...>::type;
///@}

//! @name Get the number of types in a @c Kokkos::Impl::type_list.
///@{
template <typename>
struct TypeListSize;

template <typename... Ts>
struct TypeListSize<Kokkos::Impl::type_list<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};

template <typename T>
inline constexpr size_t type_list_size_v = TypeListSize<T>::value;
///@}

//! @name Whether a type is contained in a @c Kokkos::Impl::type_list.
///@{
template <typename, typename>
struct TypeListContains;

template <typename T, typename... Ts>
struct TypeListContains<T, Kokkos::Impl::type_list<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

template <typename T, typename S>
inline constexpr bool type_list_contains_v = TypeListContains<T, S>::value;
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

//! @name Get the index of a type in a @c Kokkos::Impl::type_list.
///@{
template <typename, typename>
struct TypeListIndex;

template <typename T, typename... Ts>
struct TypeListIndex<T, Kokkos::Impl::type_list<T, Ts...>>
  : std::integral_constant<size_t, 0> {};

template <typename T, typename Head, typename... Ts>
struct TypeListIndex<T, Kokkos::Impl::type_list<Head, Ts...>>
  : std::integral_constant<size_t, 1 + TypeListIndex<T, Kokkos::Impl::type_list<Ts...>>::value> {};

template <typename T, typename S>
inline constexpr size_t type_list_index_v = TypeListIndex<T, S>::value;
///@}

//! @name Check that a predicate is @c true for all types in a type list.
///@{
template <template <typename> class UnaryPred, class List>
struct type_list_all;

template <template <typename> class UnaryPred, class... Ts>
struct type_list_all<UnaryPred, Kokkos::Impl::type_list<Ts...>>
  : std::conjunction<UnaryPred<Ts>...> {};

template <template <typename> class UnaryPred, typename TypeList>
inline constexpr bool type_list_all_v = type_list_all<UnaryPred, TypeList>::value;
///@}

/**
 * @name Check that a predicate is @c true for at least one of the types in a type list.
 *
 * @note Compared to @c Kokkos::Impl::type_list_any, this version is short-circuiting.
 *       See also https://github.com/kokkos/kokkos/blob/db736f280b09ae5acaedab3b3ad6bc7d741e92bc/core/src/impl/Kokkos_Utilities.hpp#L171-L176.
 */
///@{
template <template <typename> class UnaryPred, class List>
struct type_list_any;

template <template <typename> class UnaryPred, class... Ts>
struct type_list_any<UnaryPred, Kokkos::Impl::type_list<Ts...>>
  : std::disjunction<UnaryPred<Ts>...> {};

template <template <typename> class UnaryPred, typename TypeList>
inline constexpr bool type_list_any_v = type_list_any<UnaryPred, TypeList>::value;
///@}

//! Calls the instantiation of the call operator of a callable object for each type in a @c Kokkos::Impl::type_list.
template <typename TypeList, typename Callable>
constexpr void for_each(Callable callable)
{
    [&] <size_t... Is>(std::index_sequence<Is...>) constexpr {
        (callable.template operator()<type_list_at_t<Is, TypeList>>(), ...);
    }(std::make_index_sequence<type_list_size_v<TypeList>>{});
}

} // namespace Kokkos::utils::impl

#endif // KOKKOS_UTILS_IMPL_TYPE_LIST_HPP
