#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tests/callbacks/Helpers.hpp"

/**
 * @file
 *
 * @addtogroup unittests
 *
 * @c Kokkos callback tests helpers
 * --------------------------------
 *
 * This group of tests check the behavior of the tests helpers implemented in @ref tests/callbacks/Helpers.hpp.
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

} // namespace Kokkos::utils::tests::callbacks
