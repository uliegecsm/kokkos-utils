#ifndef KOKKOS_UTILS_CALLBACKS_MATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_MATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

namespace impl
{

/**
 * Helper struct needed for the implementation of concepts and type traits such as:
 *  - @ref Kokkos::utils::callbacks::matcher_event_type_list_t
 *  - @ref ListenerFor
 */
template <typename Callable>
struct IsMatcherFor
{
    template <Event EventType>
    using type = std::is_invocable_r<bool, Callable, const EventType&>;
};

} // namespace impl

/**
 * @brief @p Callable is a matcher if it is invocable with at least one event type
 *        from @ref Kokkos::utils::callbacks::EventTypeList passed by @c const reference and returns @c bool.
 */
template <typename Callable>
concept Matcher = Kokkos::utils::impl::type_list_any_v<impl::IsMatcherFor<Callable>::template type, EventTypeList>;

//! Check that @p Callable is a matcher for each event in @p EventTypes.
template <typename Callable, typename... EventTypes>
concept MatcherFor = Kokkos::utils::impl::type_list_all_v<impl::IsMatcherFor<Callable>::template type, Kokkos::utils::impl::make_type_list_t<EventTypes...>>;

//! Type list holding the event types that @p Callable can be a matcher for.
template <typename Callable>
using matcher_event_type_list_t = Kokkos::Impl::filter_type_list_t<impl::IsMatcherFor<Callable>::template type, EventTypeList>;

//! Matcher that returns @c true for any event.
struct AnyEventMatcher
{
    template <Event EventType>
    constexpr bool operator()(const EventType&) const { return true; }
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_MATCHER_HPP
