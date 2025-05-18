#ifndef KOKKOS_UTILS_CALLBACKS_EVENTBEGINENDIDMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTBEGINENDIDMATCHER_HPP

#include "kokkos-utils/callbacks/EventIdMatcher.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Match a begin event and its corresponding end event based on their @c event_id.
 *
 * The @ref matcher can be used to match a specific begin event of type @p BeginEventType.
 *
 * @note The begin and end event types, constrained by the @ref BeginEvent and @ref EndEvent concepts,
 *       respectively, have an @c event_id member. For a given event of a begin event type, there is
 *       only one event of the corresponding end event type that will potentially have the same
 *       @c event_id as the begin event. For instance, given a @ref BeginParallelForEvent
 *       event, only one @ref EndParallelForEvent event should match.
 */
template <Matcher BeginMatcherType, BeginEvent BeginEventType> requires MatcherFor<BeginMatcherType, BeginEventType>
struct EventBeginEndIdMatcher
{
    using end_event_t = paired_event_t<BeginEventType>;

    bool operator()(const BeginEventType& event)
    {
        const bool matching = matcher(event);

        if(matching)
            matcher_end.event_id = event.event_id;

        return matching;
    }

    bool operator()(const end_event_t& event) const {
        return matcher_end(event);
    }

    BeginMatcherType matcher;
    EventIdMatcher   matcher_end {};
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTBEGINENDIDMATCHER_HPP
