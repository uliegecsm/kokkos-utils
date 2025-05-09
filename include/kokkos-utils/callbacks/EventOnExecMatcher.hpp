#ifndef KOKKOS_UTILS_CALLBACKS_EVENTONEXECMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTONEXECMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/concepts/ExecutionSpace.hpp"

namespace Kokkos::utils::callbacks
{

//! Match an event whose @c dev_id is the same as the one of @ref exec.
template <Matcher MatcherType, Kokkos::utils::concepts::ExecutionSpace Exec>
struct EventOnExecMatcher
{
    template <OnDeviceEvent EventType>
    bool operator()(const EventType& event)
    {
        if(event.dev_id == dev_id)
            return matcher(event);
        else
            return false;
    }

    MatcherType matcher;
    Exec exec;
    uint32_t dev_id = Kokkos::Tools::Experimental::device_id(exec);
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTONEXECMATCHER_HPP
