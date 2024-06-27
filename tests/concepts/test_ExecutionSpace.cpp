#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/concepts/ExecutionSpace.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * **Concepts related to @ref Kokkos execution space**
 *
 * This group of tests check the behavior of our concepts related to @ref Kokkos execution space.
 */

namespace Kokkos::utils::tests::concepts
{

//! @test Check that @ref Kokkos::utils::concepts::ExecutionSpace works as expected.
TEST(concepts, ExecutionSpace)
{
    using Kokkos::utils::concepts::ExecutionSpace;

    static_assert(  ExecutionSpace<typename execution_space::execution_space>);
    static_assert(! ExecutionSpace<typename execution_space::memory_space>);
    static_assert(! ExecutionSpace<Kokkos::Device<typename execution_space::execution_space, typename execution_space::memory_space>>);
}

} // namespace Kokkos::utils::tests::concepts
