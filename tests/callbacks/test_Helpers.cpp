#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "kokkos-utils/callbacks/Helpers.hpp"

/**
 * @file
 *
 * @addtogroup unittests
 *
 * @c Kokkos callback tests helpers
 * --------------------------------
 *
 * This group of tests check the behavior of the tests helpers implemented in @ref kokkos-utils/callbacks/Helpers.hpp.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

//! @test Check that @ref Kokkos::utils::tests::callbacks::ABeginParallelForEventWithName can describe itself.
TEST(ABeginParallelForEventWithName, CanDescribeItself)
{
    const auto matcher = ABeginParallelForEventWithName(::testing::StrEq("computation - level 0 - pfor"));
    ASSERT_EQ(::testing::DescribeMatcher<std::variant<BeginParallelForEvent>>(matcher), "is a variant<> with value of type 'Kokkos::utils::callbacks::BeginParallelForEvent' and the value is an object whose given field is equal to \"computation - level 0 - pfor\"");
}

//! @test Check that @ref Kokkos::utils::tests::callbacks::ContainsInOrder can describe itself.
TEST(ContainsInOrderMatcher, CanDescribeItself)
{
    const auto matcher = ContainsInOrder<int>(::testing::Eq(3), ::testing::Eq(4));
    ASSERT_EQ(::testing::DescribeMatcher<std::vector<int>>(matcher), "contains in order elements that match (is equal to 3, is equal to 4)");

    const auto matcher_not = ::testing::Not(ContainsInOrder<int>(::testing::Eq(3), ::testing::Eq(4)));
    ASSERT_EQ(::testing::DescribeMatcher<std::vector<int>>(matcher_not), "does not contain in order elements that match (is equal to 3, is equal to 4)");
}

//! @test Check that @ref Kokkos::utils::tests::callbacks::ContainsInOrder matches as expected.
TEST(ContainsInOrderMatcher, Matches)
{
    const std::vector<int> vec{1, 2, 3, 4};
    ASSERT_THAT(vec,                ContainsInOrder<int>());
    ASSERT_THAT(vec,                ContainsInOrder<int>(::testing::Eq(2), ::testing::Eq(4)));
    ASSERT_THAT(vec, ::testing::Not(ContainsInOrder<int>(::testing::Eq(2), ::testing::Eq(1))));
}

//! @test Check that @ref Kokkos::utils::callbacks::PartialMatcher works as expected for @ref Kokkos::utils::callbacks::AllocDescriptor.
TEST(PartialMatcher, AllocDescriptor)
{
    const AllocDescriptor event{.kpsh = {.name = "Cuda"}, .name = "my-label", .ptr = reinterpret_cast<void*>(0x7ffee2b9d8f0), .size = 128};

    const AllocDescriptor partial_match              {.kpsh = {.name = "Cuda"}, .name = "my-label", .size = 128};
    const AllocDescriptor partial_no_match_kpsh_name {.kpsh = {.name = "osef"}, .name = "my-label", .size = 128};
    const AllocDescriptor partial_no_match_name      {.kpsh = {.name = "Cuda"}, .name = "my-xxxxx", .size = 128};
    const AllocDescriptor partial_no_match_size      {.kpsh = {.name = "Cuda"}, .name = "my-label", .size = 129};

    ASSERT_THAT(event,                PartialMatcher<AllocDescriptor>{}(partial_match));
    ASSERT_THAT(event, ::testing::Not(PartialMatcher<AllocDescriptor>{}(partial_no_match_kpsh_name)));
    ASSERT_THAT(event, ::testing::Not(PartialMatcher<AllocDescriptor>{}(partial_no_match_name)));
    ASSERT_THAT(event, ::testing::Not(PartialMatcher<AllocDescriptor>{}(partial_no_match_size)));
}

//! @test Check that @ref Kokkos::utils::callbacks::PartialMatcher works as expected for @ref Kokkos::utils::callbacks::BeginDeepCopyEvent.
TEST(PartialMatcher, BeginDeepCopyEvent)
{
    const BeginDeepCopyEvent event {
        .dst = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-dst", .ptr = reinterpret_cast<void*>(0x7ffee2b9d8f0), .size = 128},
        .src = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-src", .ptr = reinterpret_cast<void*>(0x7ffee2b9d8f0), .size = 128}
    };

    const BeginDeepCopyEvent partial_match {
        .dst = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-dst", .size = 128},
        .src = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-src", .size = 128}
    };

    const BeginDeepCopyEvent partial_no_match_dst {
        .dst = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-xxx", .size = 128},
        .src = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-src", .size = 128}
    };

    const BeginDeepCopyEvent partial_no_match_src {
        .dst = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-dst", .size = 128},
        .src = AllocDescriptor{.kpsh = {.name = "Cuda"}, .name = "my-xxx", .size = 128}
    };

    ASSERT_THAT(event,                PartialMatcher<BeginDeepCopyEvent>{}(partial_match));
    ASSERT_THAT(event, ::testing::Not(PartialMatcher<BeginDeepCopyEvent>{}(partial_no_match_dst)));
    ASSERT_THAT(event, ::testing::Not(PartialMatcher<BeginDeepCopyEvent>{}(partial_no_match_src)));
}

} // namespace Kokkos::utils::tests::callbacks
