#include "gmock/gmock.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/BeginEndTimerListener.hpp"
#include "kokkos-utils/callbacks/EventNameMatcher.hpp"
#include "kokkos-utils/callbacks/EventRegexMatcher.hpp"
#include "kokkos-utils/callbacks/Helpers.hpp"
#include "kokkos-utils/callbacks/Manager.hpp"
#include "kokkos-utils/callbacks/RegionTimerListener.hpp"
#include "kokkos-utils/callbacks/TimerListener.hpp"
#include "kokkos-utils/tests/fixtures/ExecutionSpaceInstanceFixture.hpp"
#include "kokkos-utils/timer/Duration.hpp"

#include "tests/callbacks/TestWorkload.hpp"

/**
 * @addtogroup unittests
 *
 * @c Kokkos callback timer listener
 * ---------------------------------
 *
 * This group of tests check the behavior of @ref Kokkos::utils::callbacks::TimerListener
 * and related utilities.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

//! Listener to time parallel for regions whose name matches a regex.
using parallel_for_timer_t = ParallelForTimerListener<EventRegexMatcher, execution_space>;

class TimerListenerTest : public ManagerTestFixture,
                          public ExecutionSpaceInstanceFixture<execution_space>
{};

//! @test Check traits of @ref ParallelForTimerListener.
TEST(ParallelForTimerListener, traits)
{
    /// Check that @ref parallel_for_timer_t is a listener for @ref BeginParallelForEvent
    /// and @ref EndParallelForEvent.
    static_assert(std::same_as<
        listener_event_type_list_t<parallel_for_timer_t>,
        Kokkos::Impl::type_list<BeginParallelForEvent, EndParallelForEvent>
    >);

    static_assert(Listener<parallel_for_timer_t>);
}

/**
 * @test Check that @ref TimerListener reports it is not connected and not closed
 *       when neither the begin event nor the end event have been matched.
 */
TEST_F(TimerListenerTest, begin_not_matched_and_end_not_matched)
{
    const parallel_for_timer_t par_for_timer(std::regex("my-name"), exec);
    ASSERT_FALSE(par_for_timer.connected());
    ASSERT_FALSE(par_for_timer.closed());
}

/**
 * @test Check that @ref TimerListener reports it is connected but not closed
 *       when the begin event has been matched but the end event has not.
 */
TEST_F(TimerListenerTest, begin_matched_but_end_not_matched)
{
    parallel_for_timer_t par_for_timer(std::regex("my-name"), exec);
    par_for_timer(BeginParallelForEvent{
        .name = "my-name", .dev_id = Kokkos::Tools::Experimental::device_id(exec), .event_id = 1
    });
    ASSERT_TRUE(par_for_timer.connected());
    ASSERT_FALSE(par_for_timer.closed());
}

/**
 * @test Check that @ref TimerListener reports it is not connected but closed when both
 *       the begin event and the end event have been matched. Check that it can
 *       be reused after calling @c reset.
 */
 TEST_F(TimerListenerTest, begin_and_end_matched)
 {
    parallel_for_timer_t par_for_timer(std::regex("my-name"), exec);
    par_for_timer(BeginParallelForEvent{
        .name = "my-name", .dev_id = Kokkos::Tools::Experimental::device_id(exec), .event_id = 1
    });
    ASSERT_TRUE(par_for_timer.connected());
    par_for_timer(EndParallelForEvent{.event_id = 1});
    ASSERT_FALSE(par_for_timer.connected());
    ASSERT_TRUE(par_for_timer.closed());

    par_for_timer.reset();
    ASSERT_FALSE(par_for_timer.connected());
    ASSERT_FALSE(par_for_timer.closed());

    par_for_timer(BeginParallelForEvent{
        .name = "my-name", .dev_id = Kokkos::Tools::Experimental::device_id(exec), .event_id = 2
    });
    ASSERT_TRUE(par_for_timer.connected());
    ASSERT_FALSE(par_for_timer.closed());
}

/**
 * @test Check that @ref TimerListener reports it is not connected if the device id of the event
 *       does not match the device id of the execution space that it stores.
 */
TEST_F(TimerListenerTest, begin_not_matched_if_device_id_does_not_match)
{
    const auto other_exec = Kokkos::Experimental::partition_space(execution_space{}, 1)[0];
    const auto other_exec_dev_id = Kokkos::Tools::Experimental::device_id(other_exec);
    const auto       exec_dev_id = Kokkos::Tools::Experimental::device_id(      exec);

    parallel_for_timer_t par_for_timer(std::regex("my-name"), exec);
    par_for_timer(BeginParallelForEvent{
        .name = "my-name", .dev_id = other_exec_dev_id, .event_id = 1
    });
    ASSERT_EQ(par_for_timer.connected(), other_exec_dev_id == exec_dev_id) << "The event was enqueued on " << other_exec_dev_id << " and the timer should look at events on " << exec_dev_id << '.';
}

//! @test Check the behavior of @ref ParallelForTimerListener.
TEST_F(TimerListenerTest, parallel_for)
{
    const auto par_for_timer_matching = std::make_shared<parallel_for_timer_t>(
        std::regex("computation - level 0 - pfor"), exec
    );
    const auto par_for_timer_no_match = std::make_shared<parallel_for_timer_t>(
        std::regex("this-should-not-match"), exec
    );

    Manager::register_listener(par_for_timer_matching);
    Manager::register_listener(par_for_timer_no_match);

    MyWorkload<execution_space>{}.execute(exec);

    ASSERT_GT(par_for_timer_matching->timer.duration(), Kokkos::utils::timer::seconds(0));

    EXPECT_FALSE(par_for_timer_no_match->connected() || par_for_timer_no_match->closed())
        << "The listener should not be connected or closed because the events did not match.";

    Manager::unregister_listener(par_for_timer_matching.get());
    Manager::unregister_listener(par_for_timer_no_match.get());
}

//! @test Check the parallel for timer listener when used with @ref EnqueuedEventWithLaunchTimer.
TEST_F(TimerListenerTest, with_launch)
{
    using parallel_for_with_launch_timer_t = ParallelForWithLaunchTimerListener<EventNameMatcher, execution_space>;

    using unit_t = Kokkos::utils::timer::microseconds;
    
    Kokkos::utils::timer::Timer<void> timer_external;

    const auto par_for_timer = std::make_shared<parallel_for_with_launch_timer_t>(
        "computation - level 0 - pfor", exec
    );

    Manager::register_listener(par_for_timer);

    timer_external.start();

    MyWorkload<execution_space>{}.execute(exec);

    timer_external.stop();

    ASSERT_GT(par_for_timer->timer.duration(), unit_t{0.});
    ASSERT_GT(par_for_timer->timer.launch(),   unit_t{0.});
    ASSERT_LE(par_for_timer->timer.launch(), timer_external.duration());

    Manager::unregister_listener(par_for_timer.get());
}

//! Listener to time regions whose name matches a regex.
using region_timer_t = RegionTimerListener<EventRegexMatcher>;

//! @test Check traits of @ref RegionTimerListener.
TEST(RegionTimerListener, traits)
{
    /// Check that @ref region_timer_t is a listener for @ref PushRegionEvent
    /// and @ref PopRegionEvent.
    static_assert(std::same_as<
        listener_event_type_list_t<region_timer_t>,
        Kokkos::Impl::type_list<PushRegionEvent, PopRegionEvent>
    >);

    static_assert(Listener<region_timer_t>);
}

/**
 * @test Check the behavior of @ref RegionTimerListener.
 *
 * @note The assertion that the duration provided by the region timer is larger
 *       than the duration provided by the parallel for timer relies on the fact
 *       that in the test workload, the parallel for on the non-default execution
 *       space instance is followed by a fence and the parallel reduce on the
 *       default space instance reduces into a host space scalar, thus ensuring
 *       that this parallel reduce is blocking. In other words, for these reasons,
 *       we can use a region timer without execution space and without tool fences.
 */
TEST_F(TimerListenerTest, region)
{
    const auto par_for_timer = std::make_shared<parallel_for_timer_t>(
        std::regex("computation - level 0 - pfor"), exec
    );

    const auto region_timer = std::make_shared<region_timer_t>(
        std::regex("computation - level 0")
    );

    Manager::register_listener(par_for_timer);
    Manager::register_listener(region_timer);

    MyWorkload<execution_space>{}.execute(exec);

    ASSERT_GT(par_for_timer->timer.duration(), Kokkos::utils::timer::milliseconds{0.});
    ASSERT_LT(par_for_timer->timer.duration(), region_timer->timer.duration());

    Manager::unregister_listener(par_for_timer.get());
    Manager::unregister_listener(region_timer.get());
}

//! @test Check traits of @ref RegionTimerListener when used with @ref EnqueuedEventTimer.
TEST(RegionTimerListener, traits_when_used_with_enqueued_event_timer)
{
    using region_timer_with_enqueued_event_timer_t = RegionTimerListener<
        EventRegexMatcher,
        EnqueuedEventTimer<execution_space>
    >;

    static_assert(std::constructible_from<
        region_timer_with_enqueued_event_timer_t,
        std::regex, execution_space
    >);
}

} // namespace Kokkos::utils::tests::callbacks
