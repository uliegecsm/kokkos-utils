#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/view/extents.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Get extents of a @c Kokkos::View
 * --------------------------------
 *
 * This group of tests check the behavior of our helpers related to @c Kokkos::View that can be
 * found in @ref extents.hpp.
 */

namespace Kokkos::utils::tests::view
{

constexpr size_t dim_1 = 5, dim_2 = 3, dim_3 = 6, dim_4 = 9;

//! @test Check that @ref view::extents works as expected for a rank-0 @c Kokkos::View.
TEST(view, extents_rank_0)
{
    const Kokkos::View<double, execution_space> view_0("rank-0 view");

    static_assert(utils::view::extents(view_0) == Kokkos::Array<size_t, 0>{});
}

//! @test Check that @ref view::extents works as expected for a rank-1 @c Kokkos::View.
TEST(view, extents_rank_1)
{
    const Kokkos::View<double[dim_1], execution_space> view_1_static("rank-1 view with static extent");
    const Kokkos::View<double*      , execution_space> view_1_dynami("rank-1 view with dynamic extent", dim_1);

    ASSERT_EQ(utils::view::extents(view_1_static), Kokkos::Array{dim_1});
    ASSERT_EQ(utils::view::extents(view_1_dynami), Kokkos::Array{dim_1});
}

//! @test Check that @ref view::extents works as expected for a rank-2 @c Kokkos::View.
TEST(view, extents_rank_2)
{
    const Kokkos::View<double[dim_1][dim_2], execution_space> view_2_static("rank-2 view with static extents");
    const Kokkos::View<double**            , execution_space> view_2_dynami("rank-2 view with dynamic extents", dim_1, dim_2);
    const Kokkos::View<double*[dim_2]      , execution_space> view_2_mixed ("rank-2 view with mixed extents"  , dim_1);

    ASSERT_EQ(utils::view::extents(view_2_static), (Kokkos::Array{dim_1, dim_2}));
    ASSERT_EQ(utils::view::extents(view_2_dynami), (Kokkos::Array{dim_1, dim_2}));
    ASSERT_EQ(utils::view::extents(view_2_mixed ), (Kokkos::Array{dim_1, dim_2}));
}

//! @test Check that @ref view::extents works as expected for a rank-3 @c Kokkos::View.
TEST(view, extents_rank_3)
{
    const Kokkos::View<double[dim_1][dim_2][dim_3], execution_space> view_3_static("rank-3 view of static extents");
    const Kokkos::View<double***                  , execution_space> view_3_dynami("rank-3 view of dynamic extents", dim_1, dim_2, dim_3);
    const Kokkos::View<double*[dim_2][dim_3]      , execution_space> view_3_mixed ("rank-3 view of mixed extents"  , dim_1);

    ASSERT_EQ(utils::view::extents(view_3_static), (Kokkos::Array{dim_1, dim_2, dim_3}));
    ASSERT_EQ(utils::view::extents(view_3_dynami), (Kokkos::Array{dim_1, dim_2, dim_3}));
    ASSERT_EQ(utils::view::extents(view_3_mixed ), (Kokkos::Array{dim_1, dim_2, dim_3}));
}

//! @test Check that @ref view::extents works as expected for a rank-4 @c Kokkos::View.
TEST(view, extents_rank_4)
{
    const Kokkos::View<double[dim_1][dim_2][dim_3][dim_4], execution_space> view_4_static("rank-4 view of static extents");

    ASSERT_EQ(utils::view::extents(view_4_static), (Kokkos::Array{dim_1, dim_2, dim_3, dim_4}));
}

} // namespace Kokkos::utils::tests::view
