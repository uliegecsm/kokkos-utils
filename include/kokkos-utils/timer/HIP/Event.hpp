#ifndef KOKKOS_UTILS_HIP_TIMER_EVENT_HPP
#define KOKKOS_UTILS_HIP_TIMER_EVENT_HPP

#include "Kokkos_Core.hpp"

namespace Kokkos::utils::timer
{

template <>
struct Event<Kokkos::HIP>
{
    using impl_event_t = hipEvent_t;

    impl_event_t event = nullptr;

    Event() { KOKKOS_IMPL_HIP_SAFE_CALL(hipEventCreate(&event)); }

    ~Event() { KOKKOS_IMPL_HIP_SAFE_CALL(hipEventDestroy(event)); }

    void record(const Kokkos::HIP& space) { KOKKOS_IMPL_HIP_SAFE_CALL(hipEventRecord(event, space.hip_stream())); }

    template <typename Duration = milliseconds>
    Duration duration(Event& other) {
        return std::chrono::duration_cast<Duration>(std::chrono::duration<float, std::milli>(elapsed(other)));
    }

private:
    /**
     * @brief Measure the elapsed time in milliseconds.
     *
     * As specified in https://rocm.docs.amd.com/projects/HIP/en/develop/doxygen/html/group___event.html.
     */
    float elapsed(Event& other)
    {
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventSynchronize(other.event));
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventSynchronize(event));

        float elapsed_time;
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventElapsedTime(&elapsed_time, event, other.event));

        return elapsed_time;
    }

};

} // Kokkos::utils::timer

#endif // KOKKOS_UTILS_HIP_TIMER_EVENT_HPP
