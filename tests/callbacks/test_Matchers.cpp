#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/impl/type_traits.hpp"

#include "kokkos-utils/callbacks/EventInProfileSectionMatcher.hpp"
#include "kokkos-utils/callbacks/EventInRegionMatcher.hpp"
#include "kokkos-utils/callbacks/EventNameMatcher.hpp"
#include "kokkos-utils/callbacks/EventRegexMatcher.hpp"
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
    const EventRegexMatcher matcher{.regex = std::regex("buried-[a-z]+-to-time")};

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

    EventInProfileSectionMatcher matcher{.matcher = EventRegexMatcher{.regex = std::regex("buried-profile-section")}};

    ASSERT_FALSE(matcher(CreateProfileSectionEvent{.name = "buried-profile-section", .section_id = section_id}));

    ASSERT_FALSE(matcher(AllocateDataEvent{})) << "Expecting not to record the event before starting the section.";

    ASSERT_FALSE(matcher(StartProfileSectionEvent{.section_id = section_id}));

    ASSERT_TRUE(matcher(AllocateDataEvent{})) << "Expecting to record the event inside the section.";

    ASSERT_FALSE(matcher(StopProfileSectionEvent{.section_id = section_id}));

    ASSERT_FALSE(matcher(AllocateDataEvent{})) << "Expecting to record the event after stopping the section.";

    ASSERT_FALSE(matcher(DestroyProfileSectionEvent{.section_id = section_id}));
}
//! @test Check traits of @ref Kokkos::utils::callbacks::EventInRegionMatcher.
TEST(EventInRegionMatcher, traits)
{
    using matcher_t = EventInRegionMatcher<EventRegexMatcher>;

    static_assert(Matcher     <matcher_t>);
    static_assert(MatcherFor  <matcher_t, EventTypeList>);
    static_assert(std::movable<matcher_t>);
}

//! @test Check the behavior of @ref Kokkos::utils::callbacks::EventInRegionMatcher.
TEST(EventInRegionMatcher, in_region_matcher)
{
    EventInRegionMatcher matcher(EventRegexMatcher{.regex = std::regex("buried-region")});

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "not-the-one-we-want"}));

    ASSERT_FALSE(matcher(AllocateDataEvent{}));

    ASSERT_FALSE(matcher(PushRegionEvent{.name = "buried-region"}));

    ASSERT_TRUE(matcher(AllocateDataEvent{}));

    ASSERT_TRUE(matcher(PushRegionEvent{.name = "nested-region"}));

    ASSERT_TRUE(matcher(AllocateDataEvent{}));

    ASSERT_TRUE(matcher(PopRegionEvent{}));

    ASSERT_FALSE(matcher(PopRegionEvent{}));
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

} // namespace Kokkos::utils::tests::callbacks
