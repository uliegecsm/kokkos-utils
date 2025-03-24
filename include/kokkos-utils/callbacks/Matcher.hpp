#ifndef KOKKOS_UTILS_CALLBACKS_MATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_MATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

namespace impl
{

//! Helper struct needed for the implementation of the @ref Matcher concept.
template <typename, typename>
struct IsMatcherFor;

template <typename Callable, typename... Events> requires (sizeof...(Events) > 0)
struct IsMatcherFor<Callable, Kokkos::Impl::type_list<Events...>>
{
    static constexpr bool value = std::conjunction_v<std::is_invocable_r<bool, Callable, const Events&>...>;
};

} // namespace impl

/**
 * @brief Concept that models that a callable object is a matcher for the event types in @p EventTypeSubList
 *        if it is invocable with each event type passed by @c const reference and returns a @c bool.
 */
template <typename Callable, typename EventTypeSubList>
concept Matcher = impl::IsMatcherFor<Callable, EventTypeSubList>::value;

//! Matcher that returns @c true for any event.
struct AnyEventMatcher
{
    template <Event EventType>
    constexpr bool operator()(const EventType&) const { return true; }
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_MATCHER_HPP
