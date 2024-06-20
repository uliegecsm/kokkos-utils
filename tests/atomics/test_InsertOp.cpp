#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/atomics/InsertOp.hpp"
#include "kokkos-utils/concepts/View.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * **Atomic insertion**
 *
 * This group of tests check the behavior of our atomic insert operators.
 */

namespace Kokkos::utils::tests::atomics
{

template <concepts::View view_t>
struct InsertMinTest
{
    view_t data;
    int trials;
    utils::atomics::InsertMin inserter {};

    template <typename Exec>
    void apply(const Exec& exec) const
    {
        Kokkos::parallel_for(
            "atomics::InsertMin",
            Kokkos::RangePolicy<execution_space>(exec, 0, trials),
            *this
        );
    }

    template <std::integral T>
    KOKKOS_FUNCTION
    void operator()(const T trial) const
    {
        inserter.op(data, 0, trials - trial - 2);
        inserter.op(data, 1, 2);
    }
};

//! @test Check that @ref Kokkos::utils::atomics::InsertMin works as expected.
TEST(atomics, InsertMin)
{
    using view_t = Kokkos::View<int*, execution_space>;

    constexpr int trials = 150;

    const execution_space exec {};

    const view_t data(Kokkos::view_alloc(exec, "data"), 2);

    InsertMinTest{.data = data, .trials = trials}.apply(exec);
    exec.fence();

    const auto mirror = Kokkos::create_mirror_view_and_copy(Kokkos::DefaultHostExecutionSpace{}, data);

    ASSERT_EQ(mirror(0), -1);
    ASSERT_EQ(mirror(1),  0);
}

} // namespace Kokkos::utils::tests::atomics
