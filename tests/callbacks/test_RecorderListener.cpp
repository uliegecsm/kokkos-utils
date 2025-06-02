#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/EventInProfileSectionMatcher.hpp"
#include "kokkos-utils/callbacks/EventRegexMatcher.hpp"
#include "kokkos-utils/callbacks/Helpers.hpp"
#include "kokkos-utils/callbacks/RecorderListener.hpp"
#include "kokkos-utils/impl/type_traits.hpp"
#include "kokkos-utils/tests/scoped/ExecutionSpace.hpp"
#include "kokkos-utils/tests/scoped/callbacks/Manager.hpp"

#include "tests/callbacks/Helpers.hpp"
#include "tests/callbacks/TestWorkload.hpp"

/**
 * @addtogroup unittests
 *
 * @c Kokkos callback recorder listener
 * ------------------------------------
 *
 * This group of tests check the behavior of @ref Kokkos::utils::callbacks::RecorderListener.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

//! Listener to record events that occur in a profile section.
using event_in_profile_section_recorder_t = RecorderListener<EventInProfileSectionMatcher<EventRegexMatcher>, EventTypeList>;

class RecorderListenerTest : public ::testing::Test,
                             public scoped::callbacks::Manager,
                             public scoped::ExecutionSpace<execution_space>
{};

//! @test Check traits of @ref Kokkos::utils::callbacks::RecorderListener.
TEST(RecorderListener, traits)
{
    //! Check that @ref event_in_profile_section_recorder_t is a listener for any event.
    static_assert(std::same_as<
        listener_event_type_list_t<event_in_profile_section_recorder_t>,
        EventTypeList
    >);

    static_assert(Listener   <event_in_profile_section_recorder_t>);
    static_assert(ListenerFor<event_in_profile_section_recorder_t, EventTypeList>);
    static_assert(ListenerFor<event_in_profile_section_recorder_t, PushRegionEvent, PopRegionEvent>);
}

template <Event EvenType>
class RecorderListenerSingleEventTypeTest : public ::testing::Test,
                                            public scoped::callbacks::Manager {};

TYPED_TEST_SUITE(RecorderListenerSingleEventTypeTest, EventTestTypes);

//! @test Ensure that @ref Kokkos::utils::callbacks::RecorderListener can be used with any event type from @ref Kokkos::utils::callbacks::EventTypeList.
TYPED_TEST(RecorderListenerSingleEventTypeTest, record)
{
    using recorder_listener_t = RecorderListener<TypeParam>;
    static_assert(std::same_as<typename recorder_listener_t::event_type_list_t, Kokkos::Impl::type_list<TypeParam>>);
    static_assert(std::same_as<typename recorder_listener_t::matcher_t, AnyEventMatcher>);

    const auto recorder = std::make_shared<recorder_listener_t>();

    Manager::  register_listener(recorder);
    Manager::unregister_listener(recorder.get());

    ASSERT_EQ(recorder->recorded_events.size(), 0);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::RecorderListener.
TEST_F(RecorderListenerTest, recorded_events)
{
    EventInProfileSectionMatcher matcher{.matcher = EventRegexMatcher{.regex = std::regex("profile section")}};
    const auto recorder = std::make_shared<event_in_profile_section_recorder_t>(std::move(matcher));

    const auto any_event_recorder = std::make_shared<RecorderListener<EventTypeList>>();

    Kokkos::utils::callbacks::Manager::register_listener(recorder);
    Kokkos::utils::callbacks::Manager::register_listener(any_event_recorder);

    MyWorkload<execution_space>{}.execute(exec);

    //! Check the recorded events.
    ASSERT_THAT(
        recorder->recorded_events,
        ::testing::Contains(ABeginParallelForEventWithName(::testing::StrEq("computation - level 0 - pfor")))
    );

    ASSERT_THAT(
        recorder->recorded_events,
        ContainsInOrder<typename decltype(recorder->recorded_events)::value_type>(
            APushRegionEventWithName(::testing::StrEq("computation - level 0")),
            ABeginParallelForEventWithName(::testing::StrEq("computation - level 0 - pfor")),
            AEndParallelForEvent(),
            ABeginFenceEventWithName(::testing::StrEq("computation - level 0 - fence after pfor")),
            AEndFenceEvent(),
            AProfileEventWithName(::testing::StrEq("buried marker")),
            APushRegionEventWithName(::testing::StrEq("computation - level 1")),
            ABeginParallelReduceEventWithName(::testing::StrEq("computation - level 1 - preduce on default exec")),
            AEndParallelReduceEvent(),
            ABeginFenceEventWithName(::testing::StrEq("Kokkos::parallel_reduce: fence due to result being value, not view")),
            AEndFenceEvent(),
            APopRegionEvent(),
            APopRegionEvent()
        )
    );

    ASSERT_GT(any_event_recorder->recorded_events.size(), recorder->recorded_events.size());

    const auto fence_event_outside_profile_section = ABeginFenceEventWithName(::testing::StrEq("other fence after stopping the profile section"));

    ASSERT_THAT(
        recorder->recorded_events,
        ::testing::Not(::testing::Contains(fence_event_outside_profile_section))
    );

    ASSERT_THAT(
        any_event_recorder->recorded_events,
        ::testing::Contains(fence_event_outside_profile_section)
    );

    const auto output = [&recorder] () -> std::string {
        std::ostringstream oss;
        recorder->report(oss);
        return oss.str();
    }();

    ASSERT_THAT(output, ::testing::HasSubstr("computation - level 0 - pfor"));
    ASSERT_THAT(output, ::testing::HasSubstr("computation - level 0 - fence after pfor"));

    Kokkos::utils::callbacks::Manager::unregister_listener(recorder.get());
    Kokkos::utils::callbacks::Manager::unregister_listener(any_event_recorder.get());
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::RecorderListener<MatcherType, EventTypes...>::record.
TEST_F(RecorderListenerTest, record)
{
    ASSERT_THAT(
        RecorderListener<EventTypeList>::record([&exec = this->exec]{ MyWorkload<execution_space>{}.execute(exec); }),
        ::testing::Contains(
            ABeginParallelForEvent(
                ::testing::Field(&BeginParallelForEvent::name,   ::testing::StrEq("computation - level 0 - pfor")),
                ::testing::Field(&BeginParallelForEvent::dev_id, ::testing::Eq(Kokkos::Tools::Experimental::device_id(this->exec)))
            )
        )
    );

    //! Shorter version using @c ::testing::FieldsAre and @c ::testing::_ matchers.
    ASSERT_THAT(
        RecorderListener<EventTypeList>::record([&exec = this->exec]{ MyWorkload<execution_space>{}.execute(exec); }),
        ::testing::Contains(
            ABeginParallelForEvent(
                ::testing::FieldsAre(::testing::StrEq("computation - level 0 - pfor"), ::testing::Eq(Kokkos::Tools::Experimental::device_id(this->exec)), ::testing::_)
            )
        )
    );
}

//! Listener to record fence events.
using fence_finder_t = RecorderListener<EventRegexMatcher, BeginFenceEvent>;

//! @test Check traits of @ref fence_finder_t.
TEST(FenceFinder, traits)
{
    //! Check that @ref fence_finder_t is a listener for @ref Kokkos::utils::callbacks::BeginFenceEvent.
    static_assert(std::same_as<
        listener_event_type_list_t<fence_finder_t>,
        Kokkos::Impl::type_list<BeginFenceEvent>
    >);

    static_assert(Listener<fence_finder_t>);
}

//! @test Check the behavior of @ref fence_finder_t.
TEST_F_WITH_CB_MGR(FenceFinderTest, recorded_events)
{
    const auto exec = Kokkos::Experimental::partition_space(execution_space{}, 1)[0];

    const auto fence_finder = std::make_shared<fence_finder_t>(EventRegexMatcher(std::regex("computation - level 0 - fence after pfor")));

    Kokkos::utils::callbacks::Manager::register_listener(fence_finder);

    MyWorkload<execution_space>{}.execute(exec);

    //! Check the recorded events.
    ASSERT_THAT(
        fence_finder->recorded_events,
        ::testing::Contains(
            ABeginFenceEvent(
                ::testing::Field(&BeginFenceEvent::name,   ::testing::StrEq("computation - level 0 - fence after pfor")),
                ::testing::Field(&BeginFenceEvent::dev_id, ::testing::Eq(Kokkos::Tools::Experimental::device_id(exec)))
            )
        )
    );

    Kokkos::utils::callbacks::Manager::unregister_listener(fence_finder.get());
}

} // namespace Kokkos::utils::tests::callbacks
