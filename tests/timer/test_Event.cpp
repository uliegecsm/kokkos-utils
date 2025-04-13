#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/timer/Duration.hpp"
#include "kokkos-utils/timer/Event.hpp"

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Event
 * -----
 *
 * This group of tests check the behavior of @ref Kokkos::utils::timer::Event.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::timer
{

using namespace Kokkos::utils::timer;

using event_t = Event<execution_space>;

//! @test Check the type of the event used by the primary template and the device specializations.
TEST(Event, impl_event_type)
{
    using expt_impl_event_t =
#if defined(KOKKOS_ENABLE_CUDA)
    cudaEvent_t;
#elif defined(KOKKOS_ENABLE_HIP)
    hipEvent_t;
#else
    std::chrono::steady_clock::time_point;
#endif

    static_assert(std::same_as<typename event_t::impl_event_t, expt_impl_event_t>);
}

//! @test Check the traits of the helper types @ref Kokkos::utils::timer::milliseconds and @ref Kokkos::utils::timer::seconds.
TEST(Event, duration_instances)
{
    static_assert(std::same_as<typename std::chrono::microseconds::period, std::ratio<1, 1000000>>);
    static_assert(std::same_as<typename              microseconds::period, std::ratio<1, 1000000>>);
    static_assert(std::same_as<typename              microseconds::rep,    double>);

    static_assert(std::same_as<typename std::chrono::milliseconds::period, std::ratio<1, 1000>>);
    static_assert(std::same_as<typename              milliseconds::period, std::ratio<1, 1000>>);
    static_assert(std::same_as<typename              milliseconds::rep,    double>);

    static_assert(std::same_as<typename std::chrono::seconds::period, std::ratio<1, 1>>);
    static_assert(std::same_as<typename              seconds::period, std::ratio<1, 1>>);
    static_assert(std::same_as<typename              seconds::rep,    double>);
}

//! @test Basic test of @ref Kokkos::utils::timer::Event::duration.
TEST(Event, duration)
{
    const execution_space exec {};

    event_t begin, end;

    begin.record(exec);

    end.record(exec);

    ASSERT_GE(begin.duration<milliseconds>(end).count(), 0.);
}

} // namespace Kokkos::utils::tests::timer
