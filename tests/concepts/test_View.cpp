#include "gtest/gtest.h"

#include "kokkos-utils/concepts/View.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * **Concepts related to @c Kokkos::View**
 *
 * This group of tests check the behavior of our concepts related to @c Kokkos::View.
 */

namespace Kokkos::utils::tests::concepts
{

//! @test Check that @ref Kokkos::utils::concepts::View works as expected.
TEST(concepts, View)
{
    using view_1d_t = Kokkos::View<int[5]   , execution_space>;
    using view_2d_t = Kokkos::View<int[5][5], execution_space>;

    static_assert(Kokkos::utils::concepts::View<view_1d_t>);
    static_assert(Kokkos::utils::concepts::View<view_2d_t>);
}

} // namespace Kokkos::utils::tests::concepts
