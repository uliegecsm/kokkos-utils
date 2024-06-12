#include "gtest/gtest.h"

#include "kokkos-utils/concepts/View.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

namespace tests::concepts
{

//! @test Check that @ref utils::View works as expected.
TEST(concepts, View)
{
    using view_1d_t = Kokkos::View<int[5]   , execution_space>;
    using view_2d_t = Kokkos::View<int[5][5], execution_space>;

    static_assert(Kokkos::utils::concepts::View<view_1d_t>);
    static_assert(Kokkos::utils::concepts::View<view_2d_t>);
}

} // namespace tests::concepts
