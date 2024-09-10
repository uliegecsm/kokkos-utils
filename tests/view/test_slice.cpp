#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/view/extents.hpp"
#include "kokkos-utils/view/slice.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Get slices of a @c Kokkos::View
 * -------------------------------
 *
 * This group of tests check the behavior of our helpers related to @c Kokkos::View that can be
 * found in @ref slice.hpp.
 */

namespace Kokkos::utils::tests::view
{

#define CHECK_THAT(__slice__, __rank__, __extents__)       \
    {                                                      \
        const auto tmp = __slice__;                        \
        ASSERT_EQ(tmp.rank(), __rank__);                   \
        ASSERT_EQ(utils::view::extents(tmp), __extents__); \
    }

constexpr size_t dim_1 = 5, dim_2 = 3, dim_3 = 6, dim_4 = 9;

//! @test Check that @ref view::slice works as expected for a rank-1 @c Kokkos::View.
TEST(view, slice_rank_1)
{
    const Kokkos::View<double[dim_1], execution_space> view_1_static("rank-1 view with static extent");
    const Kokkos::View<double*      , execution_space> view_1_dynami("rank-1 view with dynamic extent", dim_1);

    CHECK_THAT(utils::view::slice<1>(view_1_static), 1, Kokkos::Array{dim_1});
    CHECK_THAT(utils::view::slice<1>(view_1_dynami), 1, Kokkos::Array{dim_1});

    CHECK_THAT(utils::view::slice(view_1_static, 0), 0, (Kokkos::Array<size_t, 0>{}));
    CHECK_THAT(utils::view::slice(view_1_dynami, 0), 0, (Kokkos::Array<size_t, 0>{}));
}

//! @test Check that @ref view::slice works as expected for a rank-2 @c Kokkos::View.
TEST(view, slice_rank_2)
{
    const Kokkos::View<double[dim_1][dim_2], execution_space> view_2_static("rank-2 view with static extents");
    const Kokkos::View<double**            , execution_space> view_2_dynami("rank-2 view with dynamic extents", dim_1, dim_2);
    const Kokkos::View<double*[dim_2]      , execution_space> view_2_mixed ("rank-2 view with mixed extents"  , dim_1);

    CHECK_THAT(utils::view::slice<2>(view_2_static, 0), 1, Kokkos::Array{dim_2});
    CHECK_THAT(utils::view::slice<2>(view_2_dynami, 0), 1, Kokkos::Array{dim_2});
    CHECK_THAT(utils::view::slice<2>(view_2_mixed , 0), 1, Kokkos::Array{dim_2});
}

//! @test Check that @ref view::slice works as expected for a rank-3 @c Kokkos::View.
TEST(view, slice_rank_3)
{
    const Kokkos::View<double[dim_1][dim_2][dim_3], execution_space> view_3_static("rank-3 view of static extents");
    const Kokkos::View<double***                  , execution_space> view_3_dynami("rank-3 view of dynamic extents", dim_1, dim_2, dim_3);
    const Kokkos::View<double*[dim_2][dim_3]      , execution_space> view_3_mixed ("rank-3 view of mixed extents"  , dim_1);

    CHECK_THAT(utils::view::slice<3>(view_3_static, 0), 2, (Kokkos::Array{dim_2, dim_3}));
    CHECK_THAT(utils::view::slice<3>(view_3_dynami, 0), 2, (Kokkos::Array{dim_2, dim_3}));
    CHECK_THAT(utils::view::slice<3>(view_3_mixed , 0), 2, (Kokkos::Array{dim_2, dim_3}));

    CHECK_THAT(utils::view::slice<3>(view_3_mixed , 0, Kokkos::ALL), 2, (Kokkos::Array{dim_2, dim_3}));
}

//! @test Check that @ref view::slice works as expected for a rank-4 @c Kokkos::View.
TEST(view, slice_rank_4)
{
    const Kokkos::View<double[dim_1][dim_2][dim_3][dim_4], execution_space> view_4_static("rank-4 view of static extents");

    CHECK_THAT(utils::view::slice<4>(view_4_static, 0, Kokkos::ALL), 3, (Kokkos::Array{dim_2, dim_3, dim_4}));

    CHECK_THAT(utils::view::slice(view_4_static, 0, 0, 0, 0), 0, (Kokkos::Array<size_t, 0>{}));

    CHECK_THAT(utils::view::slice<4>(view_4_static, Kokkos::make_pair(0, 2)), 4, (Kokkos::Array<size_t, 4>{2, dim_2, dim_3, dim_4}));
}

template <concepts::ViewOfRank<4> ViewType>
bool test_slice_on_device(const ViewType& view, const size_t expected_size)
{
    bool result = false;

    Kokkos::parallel_reduce(
        Kokkos::RangePolicy<execution_space>(0, 1),
        KOKKOS_LAMBDA(const int, bool& result)
        {
            const auto subview = Kokkos::subview      (view, 0, Kokkos::ALL, Kokkos::ALL, Kokkos::ALL);
            const auto slice   = utils::view::slice<4>(view, 0);
            result = (
                subview.size() == expected_size && subview.rank() == 3 &&
                slice  .size() == expected_size && slice  .rank() == 3 &&
                slice.extent(0) == dim_2 &&
                slice.extent(1) == dim_3 &&
                slice.extent(2) == dim_4
            );
        },
        Kokkos::LAnd<bool>(result)
    );

    return result;
}

//! @test Check that @ref view::slice works on device.
TEST(view, slice_on_device)
{
    const Kokkos::View<double[dim_1][dim_2][dim_3][dim_4], execution_space> view_4_static("rank-4 view of static extents");

    ASSERT_TRUE(test_slice_on_device(view_4_static, dim_2 * dim_3 * dim_4));
}

} // namespace Kokkos::utils::tests::view
