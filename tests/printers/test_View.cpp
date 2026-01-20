#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/printers/View.hpp"
#include "kokkos-utils/tests/scoped/ExecutionSpace.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Print @c Kokkos::View
 * ---------------------
 *
 * This group of tests check the behavior of our printers for @c Kokkos::View that can be
 * found in @ref printers/View.hpp.
 */

namespace Kokkos::utils::tests::printers
{

using namespace Kokkos::utils::printers;

struct PrintersTest : public ::testing::Test,
                      public scoped::ExecutionSpace<execution_space>
{};

//! @test Test stream operator for a 0D @c Kokkos::View.
TEST_F(PrintersTest, Kokkos_View_0D)
{
    const Kokkos::View<double, execution_space> my_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, this->exec, "my nice 0D view"));
    Kokkos::deep_copy(this->exec, my_view, 5.52);
    this->exec.fence();

    std::ostringstream oss;
    oss << my_view;
    EXPECT_EQ(oss.str(), "[5.52]");
}

template <concepts::ViewOfRank<1> ViewType>
struct DeepAssign1D
{
    ViewType view;

    template <std::integral T>
    KOKKOS_FUNCTION void operator()(const T) const
    {
        view(0) = 1.5;
        view(1) = 96.;
        view(2) = 8.6;
        view(3) = 456.;
        view(4) = 456.15;
    }
};

//! @test Test stream operator for a 1D @c Kokkos::View.
TEST_F(PrintersTest, Kokkos_View_1D)
{
    const Kokkos::View<double*, execution_space> my_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, this->exec, "my nice 1D view"), 5);
    Kokkos::parallel_for(Kokkos::RangePolicy<execution_space>(this->exec, 0, 1), DeepAssign1D{my_view});
    this->exec.fence();

    std::ostringstream oss;
    oss << my_view;
    EXPECT_EQ(oss.str(), "[1.5, 96, 8.6, 456, 456.15]");
}

template <concepts::ViewOfRank<1> ViewType>
struct DeepAssign1DFormatting
{
    ViewType view;

    template <std::integral T>
    KOKKOS_FUNCTION void operator()(const T) const
    {
        view(0) = 1.5;
        view(1) = 96.;
        view(2) = 8.6;
        view(3) = 456.;
        view(4) = 456.15e15;
    }
};

//! @test Test stream operator for a 1D @c Kokkos::View when the parent stream has special formatting.
TEST_F(PrintersTest, Kokkos_View_1D_formatting)
{
    const Kokkos::View<double*, execution_space> my_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, this->exec, "my nice 1D view"), 5);
    Kokkos::parallel_for(Kokkos::RangePolicy<execution_space>(this->exec, 0, 1), DeepAssign1DFormatting{my_view});
    this->exec.fence();

    std::ostringstream oss;
    oss << std::scientific;
    oss.precision(12);
    oss << my_view;
    EXPECT_EQ(oss.str(), "[1.500000000000e+00, 9.600000000000e+01, 8.600000000000e+00, 4.560000000000e+02, 4.561500000000e+17]");
}

template <concepts::ViewOfRank<2> ViewType>
struct DeepAssign2D
{
    ViewType view;

    template <std::integral T>
    KOKKOS_FUNCTION void operator()(const T) const
    {
        view(0, 0) = 1.;
        view(0, 1) = 2.;
        view(1, 0) = 3.;
        view(1, 1) = 4.;
    }
};

//! @test Test stream operator for a 2D @c Kokkos::View.
TEST_F(PrintersTest, Kokkos_View_2D)
{
    const Kokkos::View<double**, execution_space> my_view(Kokkos::view_alloc(Kokkos::WithoutInitializing, this->exec, "my nice 2D view"), 2, 2);
    Kokkos::parallel_for(Kokkos::RangePolicy<execution_space>(this->exec, 0, 1), DeepAssign2D{my_view});
    this->exec.fence();

    std::ostringstream oss;
    oss << my_view;
    EXPECT_EQ(oss.str(),"[[1, 2], [3, 4]]");
}

} // namespace Kokkos::utils::tests::printers
