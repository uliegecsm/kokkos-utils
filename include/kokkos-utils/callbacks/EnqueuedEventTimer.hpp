#ifndef KOKKOS_UTILS_CALLBACKS_ENQUEUEDEVENTTIMER_HPP
#define KOKKOS_UTILS_CALLBACKS_ENQUEUEDEVENTTIMER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/concepts/ExecutionSpace.hpp"
#include "kokkos-utils/timer/Duration.hpp"
#include "kokkos-utils/timer/Timer.hpp"

namespace Kokkos::utils::callbacks
{

//! Timer for events that are enqueued on @ref exec.
template <Kokkos::utils::concepts::ExecutionSpace Exec>
struct EnqueuedEventTimer
{
    //! Start the timer. The event must be on @ref exec.
    template <EnqueuedEvent EventType>
    void start(const EventType& event)
    {
        if(event.dev_id != dev_id)
            Kokkos::abort("EnqueuedEventTimer cannot be started for a wrongly enqueued event.");

        timer.start(exec);
    }

    //! Stop the timer.
    template <Event EventType>
    void stop(const EventType&) {
        timer.stop(exec);
    }

    template <typename Duration = Kokkos::utils::timer::milliseconds>
    Duration duration() { return timer.template duration<Duration>(); }

    Exec exec;
    uint32_t dev_id = Kokkos::Tools::Experimental::device_id(exec);
    Kokkos::utils::timer::Timer<Exec> timer {};
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_ENQUEUEDEVENTTIMER_HPP
