#include <tuple>
#include <vector>

#include "gtest/gtest.h"

#include "kokkos-utils/impl/type_traits.hpp"

/**
 * @addtogroup unittests
 *
 * Type traits
 * -----------
 *
 * This group of tests check the behavior of our utilities extending the standard @c type_traits.
 */

namespace Kokkos::utils::tests::impl
{

//! @test Check @ref Kokkos::utils::impl::is_instance_of_v.
TEST(impl, is_instance_of_v)
{
    using Kokkos::utils::impl::is_instance_of_v;

    static_assert(  is_instance_of_v<std::vector<int   >, std::vector>);
    static_assert(  is_instance_of_v<std::vector<double>, std::vector>);
    static_assert(! is_instance_of_v<std::tuple <double>, std::vector>);

    static_assert(  is_instance_of_v<std::tuple<int, double>, std::tuple>);
    static_assert(! is_instance_of_v<std::tuple<int, double>, std::vector>);
}

} // namespace Kokkos::utils::tests::impl
