#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/EnqueuedEventTimer.hpp"
#include "kokkos-utils/timer/Duration.hpp"

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

struct EnqueuedEventTimerTest : public ::testing::Test
{
public:
    using timer_t = EnqueuedEventTimer<execution_space>;

public:
    void SetUp() override {
        this->exec = Kokkos::Experimental::partition_space(execution_space{}, 1)[0];
        this->dev_id = Kokkos::Tools::Experimental::device_id(this->exec);
        this->timer = timer_t{this->exec};
    }

protected:
    execution_space exec {};
    uint32_t dev_id = 0;
    timer_t timer {};
};

//! @test Check @ref Kokkos::utils::callbacks::EnqueuedEventTimer.
TEST_F(EnqueuedEventTimerTest, duration)
{
    this->timer.start(BeginParallelForEvent{.name = "my-name", .dev_id = this->dev_id, .event_id = 1});

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    this->timer.stop(EndParallelForEvent{.event_id = 1});

    const double elapsed = timer.template duration<Kokkos::utils::timer::milliseconds>().count();

    //! Check that the elapsed time is greater than the waiting time of 100 ms.
    ASSERT_GE(elapsed, 100.);
}

/**
 * @test Check that @ref Kokkos::utils::callbacks::EnqueuedEventTimer::start aborts when called
 *       on a wrongly enqueued event.
 */
TEST_F(EnqueuedEventTimerTest, start_aborts_for_wrongly_enqueued_event)
{
    ASSERT_DEATH({
        this->timer.start(BeginParallelForEvent{.name = "my-name", .dev_id =
            this->dev_id + 1, .event_id = 1});
    }, "EnqueuedEventTimer cannot be started for a wrongly enqueued event.");
}

} // namespace Kokkos::utils::tests::callbacks
