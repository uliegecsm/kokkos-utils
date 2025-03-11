#ifndef KOKKOS_UTILS_TESTS_HELPERS_HPP
#define KOKKOS_UTILS_TESTS_HELPERS_HPP

#include "gmock/gmock.h"

#include <sstream>

namespace Kokkos::utils::tests
{

MATCHER_P(WhenInsertedIntoStream, matcher, "when inserted into stream " + ::testing::DescribeMatcher<std::string>(matcher, negation))
{
    std::ostringstream oss;
    oss << arg;
    return ::testing::ExplainMatchResult(matcher, oss.str(), result_listener);
}

} // namespace Kokkos::utils::tests

#endif // KOKKOS_UTILS_TESTS_HELPERS_HPP
