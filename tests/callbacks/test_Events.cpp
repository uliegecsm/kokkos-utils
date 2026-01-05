#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "kokkos-utils/callbacks/Events.hpp"

#include "tests/callbacks/Helpers.hpp"

/**
 * @file
 *
 * @addtogroup unittests
 *
 * @c Kokkos callback events
 * -------------------------
 *
 * This group of tests check the behavior of the event types defined in @ref Events.hpp
 * associated with @c Kokkos profiling callbacks.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

//! @test Check that @ref Kokkos::utils::callbacks::BeginEvent appropriately constrains the begin event types.
TEST(EventTest, BeginEvent)
{
    static_assert(BeginEvent<BeginParallelForEvent>);
    static_assert(BeginEvent<BeginParallelReduceEvent>);
    static_assert(BeginEvent<BeginParallelScanEvent>);
    static_assert(BeginEvent<BeginFenceEvent>);

    static_assert( ! BeginEvent<EndParallelForEvent>);

    static_assert( ! BeginEvent<PushRegionEvent>);
}

//! @test Check that @ref Kokkos::utils::callbacks::EndEvent appropriately constrains the end event types.
TEST(EventTest, EndEvent)
{
    static_assert(EndEvent<EndParallelForEvent>);
    static_assert(EndEvent<EndParallelReduceEvent>);
    static_assert(EndEvent<EndParallelScanEvent>);
    static_assert(EndEvent<EndFenceEvent>);

    static_assert( ! EndEvent<BeginParallelForEvent>);

    static_assert( ! EndEvent<PopRegionEvent>);

    static_assert( ! EndEvent<StopProfileSectionEvent>);
}

//! @test Check that @ref Kokkos::utils::callbacks::DataEvent appropriately constrains the data event types.
TEST(EventTest, DataEvent)
{
    static_assert(DataEvent<AllocateDataEvent>);
    static_assert(DataEvent<DeallocateDataEvent>);

    static_assert( ! DataEvent<PopRegionEvent>);
}

/**
 * @test Check that @ref Kokkos::utils::callbacks::ProfileSectionManipulationEvent appropriately
 *       constrains the profile section manipulation event types.
 */
TEST(EventTest, ProfileSectionManipulationEvent)
{
    static_assert( ! ProfileSectionManipulationEvent<EndParallelForEvent>);

    static_assert(ProfileSectionManipulationEvent<StartProfileSectionEvent>);
    static_assert(ProfileSectionManipulationEvent<StopProfileSectionEvent>);
    static_assert(ProfileSectionManipulationEvent<DestroyProfileSectionEvent>);

    static_assert( ! ProfileSectionManipulationEvent<PushRegionEvent>);
    static_assert( ! ProfileSectionManipulationEvent<PopRegionEvent>);
}

/**
 * @test Check that @ref Kokkos::utils::callbacks::NamedEvent appropriately
 *       constrains the event types that have a @c name field.
 */
TEST(EventTest, NamedEvent)
{
    static_assert(NamedEvent<BeginParallelForEvent>);
    static_assert(NamedEvent<BeginParallelReduceEvent>);
    static_assert(NamedEvent<BeginParallelScanEvent>);
    static_assert(NamedEvent<BeginFenceEvent>);
    static_assert(NamedEvent<CreateProfileSectionEvent>);
    static_assert(NamedEvent<PushRegionEvent>);
    static_assert(NamedEvent<ProfileEvent>);

    static_assert( ! NamedEvent<PopRegionEvent>);
}

//! @test Check @ref Kokkos::utils::callbacks::get_name.
TEST(EventTest, name)
{
#define CHECK_GET_NAME(__type__) \
    static_assert(get_name<__type__>() == #__type__);

    CHECK_GET_NAME(BeginParallelForEvent)
    CHECK_GET_NAME(BeginParallelReduceEvent)
    CHECK_GET_NAME(BeginParallelScanEvent)
    CHECK_GET_NAME(DestroyProfileSectionEvent)
}

//! @test Check @ref Kokkos::utils::callbacks::paired_event_t.
TEST(EventTest, paired_event)
{
    static_assert(std::same_as<paired_event_t<BeginParallelForEvent>   ,  EndParallelForEvent>);
    static_assert(std::same_as<paired_event_t<BeginParallelReduceEvent>,  EndParallelReduceEvent>);
    static_assert(std::same_as<paired_event_t<BeginParallelScanEvent>,    EndParallelScanEvent>);
    static_assert(std::same_as<paired_event_t<BeginFenceEvent>,           EndFenceEvent>);
    static_assert(std::same_as<paired_event_t<AllocateDataEvent>,         DeallocateDataEvent>);
    static_assert(std::same_as<paired_event_t<BeginDeepCopyEvent>,        EndDeepCopyEvent>);
    static_assert(std::same_as<paired_event_t<CreateProfileSectionEvent>, DestroyProfileSectionEvent>);
    static_assert(std::same_as<paired_event_t<StartProfileSectionEvent>,  StopProfileSectionEvent>);
    static_assert(std::same_as<paired_event_t<PushRegionEvent>,           PopRegionEvent>);
}

template <Event>
struct EventTest;

template <BeginEvent EventType>
struct EventTest<EventType> : public ::testing::Test
{
    EventType event {.name = "my begin event", .dev_id = 1, .event_id = 2};
    std::string expt_descr = "{name = \"my begin event\", dev_id = 1, event_id = 2}";
};

template <EndEvent EventType>
struct EventTest<EventType> : public ::testing::Test
{
    EventType event {.event_id = 1};
    std::string expt_descr {"{event_id = 1}"};
};

template <DataEvent EventType>
struct EventTest<EventType> : public ::testing::Test
{
    EventType event {.alloc = {.kpsh = Kokkos::Profiling::make_space_handle("Host"), .name = "my data event", .ptr = reinterpret_cast<void*>(0x7ffdbc161a70), .size = 2}};
    std::string expt_descr {"{name = \"my data event\", space = \"Host\", ptr = 0x7ffdbc161a70, size = 2}"};
};

template <>
struct EventTest<BeginDeepCopyEvent> : public ::testing::Test
{
    BeginDeepCopyEvent event {
        .dst = {.kpsh = Kokkos::Profiling::make_space_handle("Cuda"), .name = "my destination", .ptr = reinterpret_cast<void*>(0x7ffdbc161a80), .size = 2},
        .src = {.kpsh = Kokkos::Profiling::make_space_handle("Host"), .name = "my source",      .ptr = reinterpret_cast<void*>(0x7ffdbc161a70), .size = 2}
    };
    std::string expt_descr {"{src = \"my source\" (Host, 0x7ffdbc161a70) -> dst = \"my destination\" (Cuda, 0x7ffdbc161a80) of size 2}"};
};

template <>
struct EventTest<EndDeepCopyEvent> : public ::testing::Test
{
    EndDeepCopyEvent event {};
    std::string expt_descr {"{}"};
};

template <>
struct EventTest<CreateProfileSectionEvent> : public ::testing::Test
{
    CreateProfileSectionEvent event {.name = "my profile section", .section_id = 1};
    std::string expt_descr {"{name = \"my profile section\", section_id = 1}"};
};

template <ProfileSectionManipulationEvent EventType>
struct EventTest<EventType> : public ::testing::Test
{
    EventType event {.section_id = 1};
    std::string expt_descr {"{section_id = 1}"};
};

template <>
struct EventTest<PushRegionEvent> : public ::testing::Test
{
    PushRegionEvent event {.name = "my push region event"};
    std::string expt_descr {"{name = \"my push region event\"}"};
};

template <>
struct EventTest<PopRegionEvent> : public ::testing::Test
{
    PopRegionEvent event {};
    std::string expt_descr {"{}"};
};

template <>
struct EventTest<ProfileEvent> : public ::testing::Test
{
    ProfileEvent event {.name = "my profile event"};
    std::string expt_descr {"{name = \"my profile event\"}"};
};

TYPED_TEST_SUITE(EventTest, EventTestTypes);

//! @test Check that @ref Kokkos::utils::callbacks::Event appropriately constrains all the event types.
TYPED_TEST(EventTest, concept)
{
    static_assert(Event<TypeParam>);
}

//! @test Check that an event is equal to itself.
TYPED_TEST(EventTest, comparison)
{
    ASSERT_EQ(this->event, this->event);
}

//! @test Check that each event type is an aggregate, default constructible, and movable.
TYPED_TEST(EventTest, traits)
{
    static_assert(std::is_aggregate_v<TypeParam>);
    static_assert(std::is_default_constructible_v<TypeParam>);
    static_assert(std::movable<TypeParam>);
}

//! @test Check that the event description is as expected.
TYPED_TEST(EventTest, description)
{
    ASSERT_THAT([&event = this->event]{ std::stringstream oss; oss << event; return oss.str(); }(), ::testing::StrEq(std::string(get_name<TypeParam>()) + ": " + this->expt_descr));
}

/**
 * @test Check that @ref Kokkos::utils::callbacks::get_callback_from_eventset and
 *       @ref Kokkos::utils::callbacks::get_callback_setter work as expected.
 */
TYPED_TEST(EventTest, get_and_set_callback_from_and_in_eventset)
{
    //! Type of the callback function pointer for the event type for which the test is instantiated.
    using callback_fptr_t = decltype(get_callback_from_eventset<TypeParam>(std::declval<Kokkos::Tools::Experimental::EventSet>()));

    //! Retrieve the eventset with the callback function pointers already set within @ref Kokkos.
    const auto context_callbacks = Kokkos::Tools::Experimental::get_callbacks();

    //! Retrieve from the eventset the callback function pointer for the event type for which the test is instantiated.
    const auto context_callback = get_callback_from_eventset<TypeParam>(context_callbacks);
    static_assert(std::same_as<std::remove_const_t<decltype(context_callback)>, callback_fptr_t>);

    /// Create a new function object that can act as a callback for the event type for which the test is instantiated. Note that
    /// the function object is created as a generic lambda with a templated call operator. This way, the compiler can instantiate
    /// a call operator with the appropriate arguments for the event type for which the test is instantiated.
    const auto callback_to_register = [] <typename... Args>(Args...) -> void {};
    static_assert(std::convertible_to<decltype(callback_to_register), callback_fptr_t>);

    //! Set the new callback in @c Kokkos (thus replacing the context callback).
    get_callback_setter<TypeParam>()(callback_to_register);

    //! Check that the callback was set in @c Kokkos as expected.
    ASSERT_EQ(get_callback_from_eventset<TypeParam>(Kokkos::Tools::Experimental::get_callbacks()), callback_to_register);

    //! Reset the context callback in @ref Kokkos.
    get_callback_setter<TypeParam>()(context_callback);
}

} // namespace Kokkos::utils::tests::callbacks
