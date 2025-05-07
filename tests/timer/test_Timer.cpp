#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/timer/Timer.hpp"

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Timer
 * -----
 *
 * This group of tests check the behavior of @ref Kokkos::utils::timer::Timer.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::timer
{

using namespace Kokkos::utils::timer;

using timer_t = Timer<execution_space>;

class TimerTest : public ::testing::Test
{
public:
    void SetUp() override {
        this->exec = Kokkos::Experimental::partition_space(execution_space{}, 1)[0];
    }

protected:
    execution_space exec {};
};

/**
 * @test Check that @ref Kokkos::utils::timer::Timer reports it's invalid when neither @ref Kokkos::utils::timer::Timer::start
 *       nor @ref Kokkos::utils::timer::Timer::stop has been called.
 */
TEST_F(TimerTest, invalid_if_not_started_and_not_stopped)
{
    const timer_t timer;
    ASSERT_FALSE(timer.is_valid());
}

/**
 * @test Check that @ref Kokkos::utils::timer::Timer reports it's invalid when @ref Kokkos::utils::timer::Timer::start has been called,
 *       but @ref Kokkos::utils::timer::Timer::stop has not.
 */
TEST_F(TimerTest, invalid_if_not_stopped)
{
    timer_t timer;
    timer.start(this->exec);
    ASSERT_FALSE(timer.is_valid());
}

/**
 * @test Check that @ref Kokkos::utils::timer::Timer reports it's invalid when @ref Kokkos::utils::timer::Timer::start has not been called,
 *       but @ref Kokkos::utils::timer::Timer::stop has.
 */
TEST_F(TimerTest, invalid_if_not_started)
{
    timer_t timer;
    timer.stop(this->exec);
    ASSERT_FALSE(timer.is_valid());
}

/**
 * @test Check that @ref Kokkos::utils::timer::Timer reports it's valid when @ref Kokkos::utils::timer::Timer::start and
 *       @ref Kokkos::utils::timer::Timer::stop have been called.
 */
TEST_F(TimerTest, valid_if_started_and_stopped)
{
    timer_t timer;
    timer.start(this->exec);
    timer.stop (this->exec);
    ASSERT_TRUE(timer.is_valid());
}

/**
 * @test Check @ref Kokkos::utils::timer::Timer by verifying that the elapsed time is less
 *       than the time measured by a @c Kokkos::Timer wrapped outside of it.
 */
TEST_F(TimerTest, start_stop_elapsed)
{
    using view_t = Kokkos::View<double*, execution_space>;

    Kokkos::Timer timer_external;
    timer_t timer;

    timer_external.reset();
    timer.start(this->exec);

    const view_t my_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, this->exec, "my view"), 10);
    Kokkos::deep_copy(exec, my_view, 1.0);

    timer.stop(this->exec);

    const auto elapsed = timer.duration<seconds>().count();

    /**
     * Fencing is not required if the elapsed time for the external timer is retrieved
     * after the one for the timer. Because retrieving the elapsed time from the timer
     * synchronizes the stop event.
     */
    const auto elapsed_external = timer_external.seconds();

    ASSERT_LE(elapsed, elapsed_external);
    ASSERT_GE(elapsed, 0.);
    ASSERT_GE(elapsed_external, 0.);
}

//! @test Check @ref Kokkos::utils::timer::Timer::duration.
TEST_F(TimerTest, duration)
{
    timer_t timer;

    timer.start(this->exec);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    timer.stop(this->exec);

    const double elapsed = timer.duration<milliseconds>().count();

    //! Check that the elapsed time is greater than the waiting time of 100 ms.
    ASSERT_GE(elapsed, 100.);

    //! Check the elapsed time in milliseconds.
    const auto duration_ms = timer.duration<milliseconds>();
    ASSERT_NEAR(duration_ms.count(), elapsed, /* abs_error */ 1e-3);

    //! Check the elapsed time in microseconds.
    const auto duration_us = timer.duration<microseconds>();
    ASSERT_NEAR(duration_us.count(), elapsed * 1e3, /* abs_error */ 1.);

    //! Check the elapsed time in seconds.
    const auto duration_se = timer.duration<seconds>();
    ASSERT_NEAR(duration_se.count(), elapsed / 1e3, /* abs_error */ 1e-6);

    //! Check consistency.
    ASSERT_LE(abs(duration_ms - duration_us), std::chrono::microseconds(1));
    ASSERT_LE(abs(duration_ms - duration_se), std::chrono::microseconds(1));

    //! Ensure the timer is effectively stopped.
    ASSERT_EQ(duration_ms, (timer.duration<milliseconds>()));
}

/**
 * @test Check that the same instance of @ref Kokkos::utils::timer::Timer can be reused multiple times.
 *
 * The following page on the @c Cuda event API
 * https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__EVENT.html#group__CUDART__EVENT_1gf4fcb74343aa689f4159791967868446
 * mentions that:
 *      @c cudaEventRecord can be called multiple times on the same event and will overwrite the previously captured state.
 *
 * The following page on the @c HIP even API
 * https://rocm.docs.amd.com/projects/HIP/en/docs-6.0.2/doxygen/html/group___event.html#ga553b6f7a8e7b7dd9536d8a64c24d7e29
 * mentions that:
 *      If @c hipEventRecord has been previously called on this event, then this call will overwrite any existing state in event.
 *
 * It also warns that:
 *      If this function is called on an event that is currently being recorded, results are undefined.
 *
 * This test helps us ensure that for a "start -> stop -> start -> stop -> ..." sequence, our timer is fine.
 */
 TEST_F(TimerTest, reuse)
{
    constexpr size_t nreps = 10;

    constexpr std::chrono::milliseconds wait(14);

    timer_t timer;

    for(size_t irep = 0; irep < nreps; ++irep)
    {
        const Kokkos::Timer timer_external;

        timer.start(this->exec);

        this->exec.fence("ensure that the event 'started'");

        std::this_thread::sleep_for(wait);

        timer.stop(this->exec);

        const auto elapsed = timer.duration<milliseconds>();

        const seconds elapsed_external(timer_external.seconds());

        /// Check that the elapsed time is greater than the waiting time,
        /// but smaller than our "external" timer. This ensures that the timer restarted from zero
        /// in the loop iteration.
        ASSERT_GE(elapsed, wait);
        ASSERT_LE(elapsed, elapsed_external);
    }
}

} // namespace Kokkos::utils::tests::timer
