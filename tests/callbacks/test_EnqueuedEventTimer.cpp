#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/EnqueuedEventTimer.hpp"
#include "kokkos-utils/callbacks/EnqueuedEventWithLaunchTimer.hpp"
#include "kokkos-utils/timer/Duration.hpp"

#include "tests/IgnoreWarnings.hpp"

/**
 * @addtogroup unittests
 *
 * Enqueued event timer
 * --------------------
 *
 * This group of tests check the behavior of @c Kokkos::utils::callbacks::EnqueuedEventTimer
 * and related utilities.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

template <typename TimerType>
struct TimerTest : public ::testing::Test
{
public:
    void SetUp() override {
        this->exec = Kokkos::Experimental::partition_space(execution_space{}, 1)[0];
        this->dev_id = Kokkos::Tools::Experimental::device_id(this->exec);
        this->timer = TimerType{this->exec};
    }

protected:
    execution_space exec {};
    uint32_t dev_id = 0;
    TimerType timer {};
};

struct EnqueuedEventTimerTest : public TimerTest<EnqueuedEventTimer<execution_space>> {};

//! @test Check the duration reported by @ref Kokkos::utils::callbacks::EnqueuedEventTimer.
TEST_F(EnqueuedEventTimerTest, duration)
{
    this->timer.start(BeginParallelForEvent{.name = "my-name", .dev_id = this->dev_id, .event_id = 1});

    this->timer.stop(EndParallelForEvent{.event_id = 1});

    const auto duration = this->timer.template duration<Kokkos::utils::timer::milliseconds>();

    //! Check that the duration is greater than 0.
    ASSERT_GE(duration.count(), 0.);
}

struct EnqueuedEventWithLaunchTimerTest : public TimerTest<EnqueuedEventWithLaunchTimer<execution_space>> {};

//! @test Check the enqueued event and launch durations reported by @ref Kokkos::utils::callbacks::EnqueuedEventWithLaunchTimer.
TEST_F(EnqueuedEventWithLaunchTimerTest, duration)
{
    using unit_t = Kokkos::utils::timer::microseconds;

    constexpr unit_t sleep_for_h{5.};

    Kokkos::utils::timer::Timer<void>            timer_outer_h {};
    Kokkos::utils::timer::Timer<execution_space> timer_inner_d {};

    timer_outer_h.start();

    this->timer.start(BeginParallelForEvent{.name = "my-name", .dev_id = this->dev_id, .event_id = 1});

    std::this_thread::sleep_for(sleep_for_h);

    timer_inner_d.start(this->exec);

    timer_inner_d.stop(this->exec);

    this->timer.stop(EndParallelForEvent{.event_id = 1});

    const auto duration = this->timer.template duration<unit_t>();

    timer_outer_h.stop();

    const auto launch_duration = this->timer.template launch<unit_t>();

    const auto outer_duration = timer_outer_h.template duration<unit_t>();
    const auto inner_duration = timer_inner_d.template duration<unit_t>();

    //! Check that the duration is greater than 0. and smaller than the duration of the outer timer.
    ASSERT_GE(duration, unit_t{0.});
    ASSERT_GE(duration, inner_duration);
    ASSERT_LE(duration, outer_duration);

    //! Check that the launch duration is greater than 0. and smaller than the duration of the outer timer.
    ASSERT_GE(launch_duration, sleep_for_h);
    ASSERT_LE(launch_duration, outer_duration);
}

using TimerTypes = ::testing::Types<
    EnqueuedEventTimer<execution_space>,
    EnqueuedEventWithLaunchTimer<execution_space>
>;

TYPED_TEST_SUITE(TimerTest, TimerTypes);

/**
 * @test Check that the @c start method of @ref Kokkos::utils::callbacks::EnqueuedEventTimer
 *       and @ref Kokkos::utils::callbacks::EnqueuedEventWithLaunchTimer aborts when called
 *       on a wrongly enqueued event.
 */
TYPED_TEST(TimerTest, start_aborts_for_wrongly_enqueued_event)
{
PRAGMA_DIAGNOSTIC_PUSH
PRAGMA_DIAGNOSTIC_IGNORED("-Wswitch-default")
    ASSERT_DEATH({
        this->timer.start(BeginParallelForEvent{.name = "my-name", .dev_id =
            this->dev_id + 1, .event_id = 1});
    }, "EnqueuedEventTimer cannot be started for a wrongly enqueued event.");
PRAGMA_DIAGNOSTIC_POP
}

} // namespace Kokkos::utils::tests::callbacks
