#ifndef KOKKOS_UTILS_HIP_TIMER_EVENT_HPP
#define KOKKOS_UTILS_HIP_TIMER_EVENT_HPP

#include "Kokkos_Core.hpp"

namespace Kokkos::utils::timer
{

/**
 * @brief Specialization for @c Kokkos::HIP that uses @c hipEvent_t.
 *
 * @note It must properly manage @ref event.
 */
template <>
struct Event<Kokkos::HIP>
{
    //! To be used for the custom deleter of @ref event.
    struct EventDeleter
    {
        void operator()(ihipEvent_t* ptr) const {
            KOKKOS_IMPL_HIP_SAFE_CALL(hipEventDestroy(ptr));
        }
    };

    using impl_event_t    = hipEvent_t;
    using event_storage_t = std::unique_ptr<ihipEvent_t, EventDeleter>;

    static_assert(std::same_as<typename event_storage_t::pointer, impl_event_t>);

    event_storage_t event = nullptr;

    Event() {
        impl_event_t tmp = nullptr;
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventCreate(&tmp));
        event.reset(tmp);
    }

    void record(const Kokkos::HIP& space) { KOKKOS_IMPL_HIP_SAFE_CALL(hipEventRecord(event.get(), space.hip_stream())); }

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
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventSynchronize(other.event.get()));
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventSynchronize(event.get()));

        float elapsed_time;
        KOKKOS_IMPL_HIP_SAFE_CALL(hipEventElapsedTime(&elapsed_time, event.get(), other.event.get()));

        return elapsed_time;
    }

};

} // Kokkos::utils::timer

#endif // KOKKOS_UTILS_HIP_TIMER_EVENT_HPP
