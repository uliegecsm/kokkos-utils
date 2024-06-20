#include "gtest/gtest.h"

#include "kokkos-utils/concepts/DualView.hpp"
#include "kokkos-utils/concepts/View.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * **Concepts related to @c Kokkos::DualView**
 *
 * This group of tests check the behavior of our concepts related to @c Kokkos::DualView.
 */

namespace Kokkos::utils::tests::concepts
{

//! @test Check that @ref Kokkos::utils::concepts::DualView works as expected.
TEST(concepts, DualView)
{
    using dual_view_1d_t = Kokkos::DualView<int[5]   , execution_space>;
    using      view_1d_t = Kokkos::    View<int[5]   , execution_space>;
    using dual_view_2d_t = Kokkos::DualView<int[5][5], execution_space>;
    using      view_2d_t = Kokkos::    View<int[5][5], execution_space>;

    //! A @c Kokkos::DualView fulfills the concept.
    static_assert(Kokkos::utils::concepts::DualView<dual_view_1d_t>);
    static_assert(Kokkos::utils::concepts::DualView<dual_view_2d_t>);

    //! A @c Kokkos::View does not fulfill the concept.
    static_assert(! Kokkos::utils::concepts::DualView<view_1d_t>);
    static_assert(! Kokkos::utils::concepts::DualView<view_2d_t>);

    //! A @c Kokkos::DualView does not fulfill the @ref Kokkos::utils::concepts::View concept.
    static_assert(! Kokkos::utils::concepts::View<dual_view_1d_t>);
    static_assert(! Kokkos::utils::concepts::View<dual_view_2d_t>);
}

} // namespace Kokkos::utils::tests::concepts
