#ifndef KOKKOS_UTILS_TIMER_EVENT_HPP
#define KOKKOS_UTILS_TIMER_EVENT_HPP

#include <chrono>

#include "kokkos-utils/concepts/ExecutionSpace.hpp"

namespace Kokkos::utils::timer
{

/**
 * @brief Event.
 *
 * This primary template is implemented using @c std::chrono::steady_clock::time_point.
 *
 * Using a CPU timer to time device kernel execution requires host-device
 * synchronization barriers.
 *
 * This class has specialisations for device backends:
 *     - @c Cuda (@ref timer/Cuda/Event.hpp),
 *     - @c HIP (@ref timer/HIP/Event.hpp),
 * which use @c Cuda and @c HIP events, respectively, and require
 * no explicit host-device synchronization barriers. Note that the
 * @c cudaEventSynchronize and @c hipEventSynchronize functions used by
 * these specialisations do involve a host-device synchronization barrier.
 */
template <Kokkos::utils::concepts::ExecutionSpace Exec>
struct Event
{
    using impl_event_t = std::chrono::steady_clock::time_point;

    impl_event_t event;

    //! Record this event in the execution space @p exec.
    void record(const Exec& /* exec */) {
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
