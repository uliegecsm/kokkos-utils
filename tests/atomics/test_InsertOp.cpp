#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/atomics/InsertOp.hpp"

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

//! @test Check that @ref Kokkos::utils::atomics::InsertMin works as expected.
TEST(atomics, InsertMin)
{
    using view_t = Kokkos::View<int*, execution_space>;

    constexpr int trials = 150;

    const execution_space space {};

    const view_t data(Kokkos::view_alloc(space, "data"), 2);

    const utils::atomics::InsertMin insert_min {};

    Kokkos::parallel_for(
        "atomics::InsertMin",
        Kokkos::RangePolicy<execution_space>(space, 0, trials),
        KOKKOS_LAMBDA(const int trial)
        {
            insert_min.op(data, 0, trials - trial - 2);
            insert_min.op(data, 1, 2);
        }
    );
    space.fence();

    const auto mirror = Kokkos::create_mirror_view_and_copy(Kokkos::DefaultHostExecutionSpace{}, data);

    ASSERT_EQ(mirror(0), -1);
    ASSERT_EQ(mirror(1),  0);
}

} // namespace Kokkos::utils::tests::atomics
