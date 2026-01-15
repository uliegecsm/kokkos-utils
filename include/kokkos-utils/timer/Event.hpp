#ifndef KOKKOS_UTILS_TIMER_EVENT_HPP
#define KOKKOS_UTILS_TIMER_EVENT_HPP

#include <chrono>

namespace Kokkos::utils::timer
{

/**
 * @brief Event.
 *
 * The instantiation for the type @c void concerns the case in which
 * the event is not related to a particular execution space. It is
 * implemented by using @c std::chrono::steady_clock::time_point.
 *
 * The instantiations for execution spaces default to an implementation
 * that is identical to the implementation for the instantiation for the
 * type @c void, except that the member function @c record takes an
 * execution space instance as argument. This argument is unused.
 *
 * The specializations for device execution spaces:
 *     - @c Cuda (@ref timer/Cuda/Event.hpp),
 *     - @c HIP (@ref timer/HIP/Event.hpp),
 * use the @c Cuda and @c HIP event management API. Note that the
 * @c cudaEventSynchronize and @c hipEventSynchronize functions used by
 * these specializations involve host-device synchronization.
 */
template <typename T>
requires Kokkos::ExecutionSpace<T> || std::is_void_v<T>
struct Event
{
    //! Let's choose a steady clock.
    using impl_clock_t = std::conditional_t<
        std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock,
        std::chrono::steady_clock
    >;

    using impl_event_t = typename impl_clock_t::time_point;

    impl_event_t event;

    //! Record this event.
    template <typename U = T>
    requires std::is_void_v<U>
    void record() {
        event = std::chrono::steady_clock::now();
    }

    template <typename Exec = T>
    requires Kokkos::ExecutionSpace<Exec>
    void record(const Exec&) {
        event = std::chrono::steady_clock::now();
    }

    //! Get a @p Duration object between this event and @p other.
    template <typename Duration>
    Duration duration(const Event& other) const {
        return std::chrono::duration_cast<Duration>(other.event - event);
    }
};

} // namespace Kokkos::utils::timer

#ifdef KOKKOS_ENABLE_CUDA
#include "kokkos-utils/timer/Cuda/Event.hpp"
#endif // KOKKOS_ENABLE_CUDA

#ifdef KOKKOS_ENABLE_HIP
#include "kokkos-utils/timer/HIP/Event.hpp"
#endif // KOKKOS_ENABLE_HIP

#endif // KOKKOS_UTILS_TIMER_EVENT_HPP
