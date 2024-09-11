#include <tuple>

#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/concepts/View.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Concepts related to @c Kokkos::View
 * -----------------------------------
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

//! @test Check @ref Kokkos::utils::concepts::ViewOfInstanceOf.
TEST(concepts, ViewOfInstanceOf)
{
    using Kokkos::utils::concepts::ViewOfInstanceOf;

    static_assert(  ViewOfInstanceOf<Kokkos::View<std::tuple<int> , execution_space>, std::tuple>);
    static_assert(  ViewOfInstanceOf<Kokkos::View<std::tuple<int>*, execution_space>, std::tuple>);
    static_assert(! ViewOfInstanceOf<Kokkos::View<std::tuple<int>*, execution_space>, Kokkos::IndexType>);
}

//! @test Check @ref Kokkos::utils::concepts::ViewOf.
TEST(concepts, ViewOf)
{
    using Kokkos::utils::concepts::ViewOf;

    static_assert(  ViewOf<Kokkos::View<double       , execution_space>, double>);
    static_assert(  ViewOf<Kokkos::View<double[5]    , execution_space>, double>);
    static_assert(  ViewOf<Kokkos::View<double*      , execution_space>, double>);
    static_assert(  ViewOf<Kokkos::View<double**     , execution_space>, double>);
    static_assert(  ViewOf<Kokkos::View<const double*, execution_space>, const double>);
    static_assert(! ViewOf<Kokkos::View<const double*, execution_space>, double>);
    static_assert(! ViewOf<Kokkos::View<int          , execution_space>, double>);
    static_assert(! ViewOf<             double                         , double>);
}

//! @test Check the behavior of @ref Kokkos::utils::concepts::ViewOfRank.
TEST(concepts, ViewOfRank)
{
    using Kokkos::utils::concepts::ViewOfRank;

    using int_0d_view_t = Kokkos::View<int  , execution_space>;
    using int_1d_view_t = Kokkos::View<int* , execution_space>;
    using int_2d_view_t = Kokkos::View<int**, execution_space>;

    static_assert(  ViewOfRank<int_0d_view_t, 0>);
    static_assert(! ViewOfRank<int_0d_view_t, 1>);

    static_assert(  ViewOfRank<int_1d_view_t, 1>);
    static_assert(! ViewOfRank<int_1d_view_t, 2>);

    static_assert(! ViewOfRank<int_2d_view_t, 1>);
    static_assert(  ViewOfRank<int_2d_view_t, 2>);
}

//! @test Check the behavior of @ref Kokkos::utils::concepts::ModifiableView.
TEST(concepts, ModifiableView)
{
    using Kokkos::utils::concepts::ModifiableView;

    using       double_view_t = Kokkos::View<      double*, execution_space>;
    using const_double_view_t = Kokkos::View<const double*, execution_space>;

    static_assert(  ModifiableView<      double_view_t>);
    static_assert(! ModifiableView<const_double_view_t>);
}

} // namespace Kokkos::utils::tests::concepts
