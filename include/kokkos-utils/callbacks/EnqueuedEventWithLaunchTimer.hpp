#ifndef KOKKOS_UTILS_CALLBACKS_ENQUEUEDEVENTWITHLAUNCHTIMER_HPP
#define KOKKOS_UTILS_CALLBACKS_ENQUEUEDEVENTWITHLAUNCHTIMER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/timer/Duration.hpp"
#include "kokkos-utils/timer/Timer.hpp"

namespace Kokkos::utils::callbacks
{

//! Timer for events that are enqueued on @ref exec. This timer also measures the launch time.
template <Kokkos::ExecutionSpace Exec>
struct EnqueuedEventWithLaunchTimer
{
    //! Start the timer. The event must be on @ref exec.
    template <EnqueuedEvent EventType>
    void start(const EventType& event)
    {
        if(event.dev_id != dev_id)
            Kokkos::abort("EnqueuedEventTimer cannot be started for a wrongly enqueued event.");

        timer.start(exec);
        timer_launch.start();
    }

    //! Stop the timer.
    template <Event EventType>
    void stop(const EventType&)
    {
        timer_launch.stop();
        timer.stop(exec);
    }

    template <typename Duration = Kokkos::utils::timer::milliseconds>
    Duration duration() { return timer.template duration<Duration>(); }

    template <typename Duration = Kokkos::utils::timer::milliseconds>
    Duration launch() { return timer_launch.template duration<Duration>(); }

    Exec exec;
    uint32_t dev_id = Kokkos::Tools::Experimental::device_id(exec);
    Kokkos::utils::timer::Timer<Exec> timer {};
    Kokkos::utils::timer::Timer<void> timer_launch {};
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_ENQUEUEDEVENTWITHLAUNCHTIMER_HPP
