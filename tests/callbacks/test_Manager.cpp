#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/Helpers.hpp"
#include "kokkos-utils/impl/type_traits.hpp"
#include "kokkos-utils/tests/fixtures/ExecutionSpaceInstance.hpp"

#include "tests/callbacks/TestWorkload.hpp"

/**
 * @addtogroup unittests
 *
 * @c Kokkos callback manager
 * --------------------------
 *
 * This group of tests check the behavior of @ref Kokkos::utils::callbacks::Manager.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

class ManagerTest : public ManagerTestFixture,
                    public fixtures::ExecutionSpaceInstance<execution_space>
{};

//! @test Check properties of @ref Kokkos::utils::callbacks::Manager being a singleton class.
TEST(Manager, singleton_traits)
{
    static_assert( ! std::is_copy_constructible_v<Manager>);
    static_assert( ! std::is_copy_assignable_v   <Manager>);

    static_assert( ! std::is_move_constructible_v<Manager>);
    static_assert( ! std::is_move_assignable_v   <Manager>);
}

/**
 * @brief Listener that stores the events it receives.
 *
 * Only events of the types @p EventTypes... are handled. The events are stored
 * in the @ref events tuple. For each handled event type, this tuple holds a vector
 * in which the events of this type are stored in the order they are received.
 */
template <Event... EventTypes>
struct TesterListener
{
    template <Kokkos::utils::impl::IsTypeOneOf<EventTypes...> EventType>
    void operator()(const EventType& event) {
        std::get<std::vector<EventType>>(events).push_back(event);
    }

    std::tuple<std::vector<EventTypes>...> events {};
};

//! @test Check the behavior of @ref Kokkos::utils::callbacks::Manager with a single listener.
TEST_F(ManagerTest, dispatch_to_call_operators_of_single_listener)
{
    const auto tester_listener = std::make_shared<TesterListener<BeginParallelForEvent, EndParallelForEvent, BeginFenceEvent>>();

    Manager::register_listener(tester_listener);

    MyWorkload<execution_space>{}.execute(exec);

    Manager::unregister_listener(tester_listener.get());

    const auto& [begin_parallel_for_events, end_parallel_for_events, begin_fence_events] = tester_listener->events;

    //! There is one begin parallel for event.
    ASSERT_EQ(begin_parallel_for_events.size(), 1);
    ASSERT_EQ(begin_parallel_for_events[0].name, "computation - level 0 - pfor");

    //! There is one end parallel for event and its @c id matches the one of the begin parallel for event.
    ASSERT_EQ(end_parallel_for_events.size(), 1);
    ASSERT_EQ(begin_parallel_for_events[0].event_id, end_parallel_for_events[0].event_id);

    //! There are at least two begin fence events.
    ASSERT_GT(begin_fence_events.size(), 2);
    ASSERT_THAT(
        begin_fence_events,
        ::testing::Contains(::testing::Field(&BeginFenceEvent::name, ::testing::StrEq("computation - level 0 - fence after pfor")))
    );
    ASSERT_THAT(
        begin_fence_events,
        ::testing::Contains(::testing::Field(&BeginFenceEvent::name, ::testing::StrEq("other fence after stopping the profile section")))
    );
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::Manager with multiple listeners.
TEST_F(ManagerTest, dispatch_to_call_operators_of_multiple_listeners)
{
    const auto tester_listener_a = std::make_shared<TesterListener<BeginParallelForEvent>>();
    const auto tester_listener_b = std::make_shared<TesterListener<BeginParallelForEvent, EndParallelForEvent>>();

    Manager::register_listener(tester_listener_a);
    Manager::register_listener(tester_listener_b);

    MyWorkload<execution_space>{}.execute(exec);

    Manager::unregister_listener(tester_listener_a.get());
    Manager::unregister_listener(tester_listener_b.get());

    const auto& [begin_parallel_for_events_listener_a]                                     = tester_listener_a->events;
    const auto& [begin_parallel_for_events_listener_b, end_parallel_for_events_listener_b] = tester_listener_b->events;

    //! There is one begin parallel for event.
    ASSERT_EQ(begin_parallel_for_events_listener_a.size(), 1);
    ASSERT_EQ(begin_parallel_for_events_listener_a[0].name, "computation - level 0 - pfor");
    ASSERT_EQ(begin_parallel_for_events_listener_b.size(), 1);
    ASSERT_EQ(begin_parallel_for_events_listener_b[0].name, "computation - level 0 - pfor");

    //! There is one end parallel for event and its @c id matches the one of the begin parallel for event.
    ASSERT_EQ(end_parallel_for_events_listener_b.size(), 1);
    ASSERT_EQ(begin_parallel_for_events_listener_b[0].event_id, end_parallel_for_events_listener_b[0].event_id);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::Manager with a listener constructed from a lambda.
TEST_F(ManagerTest, listener_from_lambda)
{
    bool matched = false;

    using view_t = Kokkos::View<int, execution_space>;

    const view_t my_src_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, exec, "my rank-0 src view"));
    const view_t my_dst_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, exec, "my rank-0 dst view"));

    auto listener_handle = Manager::register_listener(
        [&](const BeginDeepCopyEvent& event) {
            if (event.dst.size == sizeof(int) && event.src.ptr == my_src_view.data()
                && event.dst.ptr == my_dst_view.data()) matched = true;
        }
    );

    Kokkos::deep_copy(exec, my_dst_view, my_src_view);

    ASSERT_TRUE(matched);

    Manager::unregister_listener(listener_handle);
}

} // namespace Kokkos::utils::tests::callbacks
