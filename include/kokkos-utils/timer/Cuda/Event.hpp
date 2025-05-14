#ifndef KOKKOS_UTILS_CUDA_TIMER_EVENT_HPP
#define KOKKOS_UTILS_CUDA_TIMER_EVENT_HPP

#include "Kokkos_Core.hpp"

namespace Kokkos::utils::timer
{

/**
 * @brief Specialization for @c Kokkos::Cuda that uses @c cudaEvent_t.
 *
 * @note It must properly manage @ref event.
 */
template <>
struct Event<Kokkos::Cuda>
{
    //! To be used for the custom deleter of @ref event.
    struct EventDeleter
    {
        void operator()(CUevent_st* ptr) const {
            KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventDestroy(ptr));
        }
    };

    using impl_event_t    = cudaEvent_t;
    using event_storage_t = std::unique_ptr<CUevent_st, EventDeleter>;

    static_assert(std::same_as<typename event_storage_t::pointer, impl_event_t>);

    event_storage_t event = nullptr;

    Event() {
        impl_event_t tmp = nullptr;
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventCreate(&tmp));
        event.reset(tmp);
    }

    //! Record this event in the execution space instance @p exec.
    void record(const Kokkos::Cuda& exec) { KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventRecord(event.get(), exec.cuda_stream())); }

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
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventSynchronize(other.event.get()));
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventSynchronize(event.get()));

        float elapsed_time;
        KOKKOS_IMPL_CUDA_SAFE_CALL(cudaEventElapsedTime(&elapsed_time, event.get(), other.event.get()));

        return elapsed_time;
    }

};

} // Kokkos::utils::timer

#endif // KOKKOS_UTILS_CUDA_TIMER_EVENT_HPP
