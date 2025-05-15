#ifndef KOKKOS_UTILS_CALLBACKS_EVENTIDMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTIDMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/concepts/ExecutionSpace.hpp"

namespace Kokkos::utils::callbacks
{

//! Match an event whose @c event_id is @ref event_id.
struct EventIdMatcher
{
    using event_id_t = uint64_t;

    static constexpr event_id_t invalid_event_id = Kokkos::Experimental::finite_max_v<event_id_t>;

    template <IndexedEvent EventType>
    bool operator()(const EventType& event) const {
        return event.event_id == event_id;
    }

    event_id_t event_id = invalid_event_id;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTIDMATCHER_HPP
