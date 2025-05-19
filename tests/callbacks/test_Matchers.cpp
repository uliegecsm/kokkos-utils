#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/impl/type_traits.hpp"

#include "kokkos-utils/callbacks/ConjunctionMatcher.hpp"
#include "kokkos-utils/callbacks/EventBeginEndIdMatcher.hpp"
#include "kokkos-utils/callbacks/EventIdMatcher.hpp"
#include "kokkos-utils/callbacks/EventInProfileSectionMatcher.hpp"
#include "kokkos-utils/callbacks/EventNameMatcher.hpp"
#include "kokkos-utils/callbacks/EventQueueMatcher.hpp"
#include "kokkos-utils/callbacks/EventRegexMatcher.hpp"
#include "kokkos-utils/callbacks/EventRegionMatcher.hpp"
#include "kokkos-utils/callbacks/EventTypeMatcher.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

/**
 * @addtogroup unittests
 *
 * @c Kokkos callback matchers
 * ---------------------------
 *
 * This group of tests check the behavior of the matchers that can act as predicates
 * for the events associated with @ref Kokkos profiling callbacks.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

//! List of event types that have a name.
using named_event_type_list_t = Kokkos::Impl::type_list<
    BeginParallelForEvent,
    BeginParallelReduceEvent,
    BeginParallelScanEvent,
    BeginFenceEvent,
    AllocateDataEvent,
    DeallocateDataEvent,
    CreateProfileSectionEvent,
    PushRegionEvent,
    ProfileEvent
>;

//! @test Check traits of @ref Kokkos::utils::callbacks::EventRegexMatcher.
TEST(EventRegexMatcher, traits)
{
    static_assert(Matcher   <EventRegexMatcher>);
    static_assert(MatcherFor<EventRegexMatcher, named_event_type_list_t>);
    static_assert(std::movable<EventRegexMatcher>);

    static_assert( ! MatcherFor<EventRegexMatcher, BeginDeepCopyEvent>);
}

//! @test Check that @ref Kokkos::utils::callbacks::EventRegexMatcher works as expected.
TEST(EventRegexMatcher, regex_matcher)
{
    const EventRegexMatcher matcher{{std::regex("buried-[a-z]+-to-time")}};

    ASSERT_TRUE( matcher(BeginParallelForEvent{.name = "buried-kernel-to-time", .event_id = 2}));
    ASSERT_FALSE(matcher(BeginParallelForEvent{.name = "not-this-other-kernel", .event_id = 2}));

    ASSERT_TRUE( matcher(AllocateDataEvent{.alloc = {.name = "buried-allocation-to-time"}}));
    ASSERT_FALSE(matcher(AllocateDataEvent{.alloc = {.name = "not-this-other-allocation"}}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventNameMatcher.
TEST(EventNameMatcher, traits)
{
    static_assert(Matcher     <EventNameMatcher>);
    static_assert(MatcherFor  <EventNameMatcher, named_event_type_list_t>);
    static_assert(std::movable<EventNameMatcher>);
}

//! @test Check that @ref Kokkos::utils::callbacks::EventNameMatcher
TEST(EventNameMatcher, by_name)
{
    const EventNameMatcher matcher{.name = "named-as-I-like-it"};

    ASSERT_FALSE(matcher(AllocateDataEvent{.alloc = {.name = "named-deep-copy"}}));
    ASSERT_TRUE (matcher(AllocateDataEvent{.alloc = {.name = "named-as-I-like-it"}}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventInProfileSectionMatcher.
TEST(EventInProfileSectionMatcher, traits)
{
    using matcher_t = EventInProfileSectionMatcher<EventRegexMatcher>;

    static_assert(Matcher     <matcher_t>);
    static_assert(MatcherFor  <matcher_t, EventTypeList>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventInProfileSectionMatcher.
TEST(EventInProfileSectionMatcher, in_profile_section_matcher)
{
    constexpr uint32_t section_id = 2;

    EventInProfileSectionMatcher<EventRegexMatcher> matcher{{{std::regex("buried-profile-section")}}};

    ASSERT_FALSE(matcher(CreateProfileSectionEvent{.name = "buried-profile-section", .section_id = section_id}));

    ASSERT_FALSE(matcher(AllocateDataEvent{})) << "Expecting not to record the event before starting the section.";

    ASSERT_FALSE(matcher(StartProfileSectionEvent{.section_id = section_id}));

    ASSERT_TRUE(matcher(AllocateDataEvent{})) << "Expecting to record the event inside the section.";

    ASSERT_FALSE(matcher(StopProfileSectionEvent{.section_id = section_id}));

    ASSERT_FALSE(matcher(AllocateDataEvent{})) << "Expecting to record the event after stopping the section.";

    ASSERT_FALSE(matcher(DestroyProfileSectionEvent{.section_id = section_id}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventRegionMatcher.
TEST(EventRegionMatcher, traits)
{
    using matcher_t = EventRegionMatcher<EventRegexMatcher>;

    static_assert(Matcher     <matcher_t>);
    static_assert(MatcherFor  <matcher_t, EventTypeList>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventRegionMatcher when we want to match events within a region.
TEST(EventRegionMatcher, within_region_matcher)
{
    EventRegionMatcher<EventRegexMatcher> matcher{{{std::regex("buried-region")}}};

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "not-the-one-we-want"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "buried-region"}));

    ASSERT_TRUE(matcher(AllocateDataEvent{}));

    ASSERT_TRUE(matcher(PushRegionEvent{.name = "nested-region"}));

    ASSERT_TRUE(matcher(AllocateDataEvent{}));

    ASSERT_TRUE(matcher(PopRegionEvent{}));

    ASSERT_FALSE(matcher(PopRegionEvent{}));
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventRegionMatcher when we want to match the opening and closing events.
TEST(EventRegionMatcher, boundary_region_matcher)
{
    EventRegionMatcher<EventNameMatcher, true> matcher{{"buried-region"}};

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "not-the-one-we-want"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_TRUE(matcher(PushRegionEvent{.name = "buried-region"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "nested-region"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PopRegionEvent{}));

    ASSERT_TRUE(matcher(PopRegionEvent{}));

    ASSERT_FALSE(matcher(BeginFenceEvent{}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::AnyEventMatcher.
TEST(AnyEventMatcher, traits)
{
    static_assert(Matcher     <AnyEventMatcher>);
    static_assert(MatcherFor  <AnyEventMatcher, EventTypeList>);
    static_assert(std::movable<AnyEventMatcher>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::AnyEventMatcher.
TEST(AnyEventMatcher, operator_parentheses)
{
    AnyEventMatcher matcher;

    Kokkos::utils::impl::for_each<EventTypeList>([&] <Event EventType>() {
        static_assert(matcher(EventType{}));
    });
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventTypeMatcher.
TEST(EventTypeMatcher, traits)
{
    using matcher_t = EventTypeMatcher<AnyEventMatcher, BeginFenceEvent, PushRegionEvent>;

    static_assert(Matcher     <matcher_t>);
    static_assert(MatcherFor  <matcher_t, BeginFenceEvent, PushRegionEvent>);
    static_assert(std::same_as<matcher_event_type_list_t<matcher_t>, Kokkos::Impl::type_list<BeginFenceEvent, PushRegionEvent>>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventTypeMatcher.
TEST(EventTypeMatcher, operator_parentheses)
{
    EventTypeMatcher<AnyEventMatcher, BeginFenceEvent, PushRegionEvent> matcher {};

    ASSERT_TRUE(matcher(PushRegionEvent{}));
    ASSERT_TRUE(matcher(BeginFenceEvent{}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventBeginEndIdMatcher.
TEST(EventBeginEndIdMatcher, traits)
{
    using matcher_t = EventBeginEndIdMatcher<BeginParallelForEvent, EventNameMatcher>;

    static_assert(Matcher     <matcher_t>);
    static_assert(MatcherFor  <matcher_t, BeginParallelForEvent, EndParallelForEvent>);
    static_assert(std::same_as<matcher_event_type_list_t<matcher_t>, Kokkos::Impl::type_list<BeginParallelForEvent, EndParallelForEvent>>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventBeginEndIdMatcher.
TEST(EventBeginEndIdMatcher, operator_parentheses)
{
    EventBeginEndIdMatcher<BeginParallelForEvent, EventNameMatcher> matcher {{"example-pfor"}};

    ASSERT_FALSE(matcher(BeginParallelForEvent {.name = "does-not-match"}));
    ASSERT_FALSE(matcher(EndParallelForEvent   {.event_id = 42}));
    ASSERT_TRUE (matcher(BeginParallelForEvent {.name = "example-pfor", .event_id = 666}));
    ASSERT_FALSE(matcher(EndParallelForEvent   {.event_id = 42}));
    ASSERT_TRUE (matcher(EndParallelForEvent   {.event_id = 666}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventQueueMatcher.
TEST(EventQueueMatcher, traits)
{
    using matcher_t = EventQueueMatcher<execution_space>;

    using enqueued_event_type_list_t = Kokkos::Impl::type_list<
        BeginParallelForEvent,
        BeginParallelReduceEvent,
        BeginParallelScanEvent,
        BeginFenceEvent
    >;

    static_assert(Matcher     <matcher_t>);
    static_assert(std::same_as<matcher_event_type_list_t<matcher_t>, enqueued_event_type_list_t>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventQueueMatcher.
TEST(EventQueueMatcher, operator_parentheses)
{
    const auto execs = Kokkos::Experimental::partition_space(execution_space{}, 1, 1);

    const auto dev_id_0 = Kokkos::Tools::Experimental::device_id(execs.at(0));
    const auto dev_id_1 = Kokkos::Tools::Experimental::device_id(execs.at(1));

    const EventQueueMatcher<execution_space> matcher {.exec = execs.at(0)};

    ASSERT_EQ  (matcher(BeginParallelForEvent{.name = "", .dev_id = dev_id_1}), dev_id_0 == dev_id_1);
    ASSERT_TRUE(matcher(BeginParallelForEvent{.name = "", .dev_id = dev_id_0}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::EventIdMatcher.
TEST(EventIdMatcher, traits)
{
    using matcher_t = EventIdMatcher;

    using indexed_event_type_list_t = Kokkos::Impl::type_list<
        BeginParallelForEvent,    EndParallelForEvent,
        BeginParallelReduceEvent, EndParallelReduceEvent,
        BeginParallelScanEvent,   EndParallelScanEvent,
        BeginFenceEvent,          EndFenceEvent
    >;

    static_assert(Matcher     <matcher_t>);
    static_assert(std::same_as<matcher_event_type_list_t<matcher_t>, indexed_event_type_list_t>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventIdMatcher.
TEST(EventIdMatcher, operator_parentheses)
{
    const EventIdMatcher matcher{.event_id = 42};

    ASSERT_TRUE (matcher(BeginParallelForEvent{.event_id =  42}));
    ASSERT_FALSE(matcher(BeginParallelForEvent{.event_id = 666}));
}

//! @test Check traits of @ref Kokkos::utils::callbacks::ConjunctionMatcher.
TEST(ConjunctionMatcher, traits)
{
    using matcher_t = ConjunctionMatcher<
        EventRegionMatcher<EventRegexMatcher>,
        EventRegionMatcher<AnyEventMatcher>
    >;

    static_assert(Matcher     <matcher_t>);
    static_assert(MatcherFor  <matcher_t, EventTypeList>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::ConjunctionMatcher.
TEST(ConjunctionMatcher, everyone_agrees)
{
    ConjunctionMatcher matcher{
        EventRegionMatcher<EventRegexMatcher>{{{std::regex("buried-region-one")}}},
        EventRegionMatcher<EventRegexMatcher>{{{std::regex("buried-region-two")}}}
    };

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "not-the-one-we-want"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "buried-region-one"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "buried-region-two"}));

    ASSERT_TRUE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PopRegionEvent{}));
}

/**
 * @test Check that @ref Kokkos::utils::callbacks::ConjunctionMatcher will only match for the subset of events that
 *       are common to all its sub-matchers.
 */
TEST(ConjunctionMatcher, different_event_type_sets)
{
    ConjunctionMatcher matcher{
        EventRegexMatcher{{std::regex("my-triggering-event")}},
        AnyEventMatcher{}
    };

    static_assert(std::same_as<
        matcher_event_type_list_t<EventRegexMatcher>,
        matcher_event_type_list_t<decltype(matcher)>
    >);

    ASSERT_FALSE(matcher(BeginFenceEvent{.name = "not-this-one"}));
    ASSERT_TRUE (matcher(BeginFenceEvent{.name = "my-triggering-event"}));
}

//! Helper matcher that records in @ref encountered all the events passed to it.
template <Matcher MatcherType>
struct RecordingMatcher
{
    bool operator()(const BeginFenceEvent& event)
    {
        encountered.push_back(event);
        return matcher(event);
    }

    MatcherType matcher;
    std::vector<BeginFenceEvent> encountered {};
};

/**
 * @test Check that @ref Kokkos::utils::callbacks::ConjunctionMatcher is short-circuiting, *i.e.* it will stop evaluating
 *       the sub-matchers as soon as one returns @c false.
 */
TEST(ConjunctionMatcher, order_of_evaluation)
{
    using matcher_t = RecordingMatcher<EventNameMatcher>;

    ConjunctionMatcher matcher{
        matcher_t{{"matcher-a"}},
        matcher_t{{"matcher-a"}},
        matcher_t{{"matcher-b"}},
        matcher_t{{"matcher-c"}}
    };

    ASSERT_FALSE(matcher(BeginFenceEvent{.name = "matcher-a", .event_id = 0}));
    ASSERT_FALSE(matcher(BeginFenceEvent{.name = "matcher-a", .event_id = 1}));
    ASSERT_FALSE(matcher(BeginFenceEvent{.name = "matcher-b", .event_id = 2}));
    ASSERT_FALSE(matcher(BeginFenceEvent{.name = "matcher-c", .event_id = 3}));

    //! The first matcher will be evaluated for all events.
    ASSERT_THAT(std::get<0>(matcher.matchers).encountered, ::testing::ElementsAre(
        BeginFenceEvent{.name = "matcher-a", .event_id = 0},
        BeginFenceEvent{.name = "matcher-a", .event_id = 1},
        BeginFenceEvent{.name = "matcher-b", .event_id = 2},
        BeginFenceEvent{.name = "matcher-c", .event_id = 3}
    ));

    //! The second matcher will be evaluated for all events for which the first matcher evaluated to @c true.
    ASSERT_THAT(std::get<1>(matcher.matchers).encountered, ::testing::ElementsAre(
        BeginFenceEvent{.name = "matcher-a", .event_id = 0},
        BeginFenceEvent{.name = "matcher-a", .event_id = 1}
    ));

    //! The third matcher will be evaluated for all events for which the first and the second matcher evaluated to @c true.
    ASSERT_THAT(std::get<2>(matcher.matchers).encountered, ::testing::ElementsAre(
        BeginFenceEvent{.name = "matcher-a", .event_id = 0},
        BeginFenceEvent{.name = "matcher-a", .event_id = 1}
    ));
    
    //! The forth matcher will be evaluated for all events for which the third matcher evaluated to @c true.
    ASSERT_THAT(std::get<3>(matcher.matchers).encountered, ::testing::IsEmpty());
}

} // namespace Kokkos::utils::tests::callbacks
