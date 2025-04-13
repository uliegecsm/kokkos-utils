#ifndef KOKKOS_UTILS_CUDA_TIMER_EVENT_HPP
#define KOKKOS_UTILS_CUDA_TIMER_EVENT_HPP

#include "Kokkos_Core.hpp"

namespace Kokkos::utils::timer
{

template <>
struct Event<Kokkos::Cuda>
{
    using impl_event_t = cudaEvent_t;

    impl_event_t event = nullptr;

    Event() { KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventCreate(&event)); }

    ~Event() { KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventDestroy(event)); }

    void record(const Kokkos::Cuda& space) { KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventRecord(event, space.cuda_stream())); }

    template <typename Duration = milliseconds>
    Duration duration(Event& other) {
        return std::chrono::duration_cast<Duration>(std::chrono::duration<float, std::milli>(elapsed(other)));
    }

private:
    /**
     * @brief Measure the elapsed time in milliseconds.
     *
     * As specified in https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__EVENT.html.
     */
    float elapsed(Event& other)
    {
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventSynchronize(other.event));
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventSynchronize(event));

        float elapsed_time;
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventElapsedTime(&elapsed_time, event, other.event));

        return elapsed_time;
    }

};

} // Kokkos::utils::timer

#endif // KOKKOS_UTILS_CUDA_TIMER_EVENT_HPP
