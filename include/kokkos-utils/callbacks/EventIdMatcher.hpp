#ifndef KOKKOS_UTILS_CALLBACKS_EVENTIDMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTIDMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

//! Match an event whose @c event_id is @ref event_id.
struct EventIdMatcher
{
    template <IndexedEvent EventType>
    bool operator()(const EventType& event) const {
        return event.event_id == event_id;
    }

    EventTraits::event_id_t event_id = EventTraits::invalid_event_id;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTIDMATCHER_HPP
