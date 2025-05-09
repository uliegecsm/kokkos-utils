#ifndef KOKKOS_UTILS_CALLBACKS_EVENTBEGINENDEVENTIDMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTBEGINENDEVENTIDMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Match a begin event and its corresponding end event based on their event ID.
 *
 * The @ref matcher can be used to match a specific begin event of type @p BeginEventType.
 */
template <Matcher MatcherBeginEventType, BeginEvent BeginEventType, EndEvent EndEventType> requires MatcherFor<MatcherBeginEventType, BeginEventType>
struct EventBeginEndEventIdMatcher
{
    static constexpr auto invalid_event_id = Kokkos::Experimental::finite_max_v<uint64_t>;

    bool operator()(const BeginEventType& event)
    {
        if(matcher(event))
        {
            if (event_id != invalid_event_id) Kokkos::abort("EventBeginEndEventIdMatcher cannot match a begin event twice.");
            event_id = event.event_id;
            return true;
        }
        return false;
    }

    bool operator()(const EndEventType& event) const {
        return event.event_id == event_id;
    }

    MatcherBeginEventType matcher;
    uint64_t event_id = invalid_event_id;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTBEGINENDEVENTIDMATCHER_HPP
