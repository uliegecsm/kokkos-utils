#ifndef KOKKOS_UTILS_CALLBACKS_EVENTTYPEMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTTYPEMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Restrict the event types that can be matched by @ref matcher to the subset @p EventTypes.
 *
 * @note The @p EventTypes are expected to be valid event types for @p MatcherType.
 */
template <Matcher MatcherType, Event... EventTypes> requires MatcherFor<MatcherType, EventTypes...>
struct EventTypeMatcher
{
    template <EventOneOf<EventTypes...> EventType> requires MatcherFor<MatcherType, EventType>
    bool operator()(const EventType& event) {
        return matcher(event);
    }

    MatcherType matcher;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTTYPEMATCHER_HPP
