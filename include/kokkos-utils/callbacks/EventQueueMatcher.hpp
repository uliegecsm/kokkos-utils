#ifndef KOKKOS_UTILS_CALLBACKS_EVENTQUEUEMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTQUEUEMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/concepts/ExecutionSpace.hpp"

namespace Kokkos::utils::callbacks
{

//! Match an event whose @c dev_id is the same as the one of @ref exec.
template <Kokkos::utils::concepts::ExecutionSpace Exec>
struct EventQueueMatcher
{
    template <EnqueuedEvent EventType>
    bool operator()(const EventType& event) const {
        return event.dev_id == dev_id;
    }

    Exec exec;
    uint32_t dev_id = Kokkos::Tools::Experimental::device_id(exec);
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTQUEUEMATCHER_HPP
