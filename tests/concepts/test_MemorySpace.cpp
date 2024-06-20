#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/concepts/MemorySpace.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * **Concepts related to @c Kokkos memory space**
 *
 * This group of tests check the behavior of our concepts related to @c Kokkos memory space.
 */

namespace Kokkos::utils::tests::concepts
{

//! @test Check that @ref Kokkos::utils::concepts::MemorySpace works as expected.
TEST(concepts, MemorySpace)
{
    static_assert(! Kokkos::utils::concepts::MemorySpace<typename execution_space::execution_space>);
    static_assert(  Kokkos::utils::concepts::MemorySpace<typename execution_space::memory_space>);
    static_assert(! Kokkos::utils::concepts::MemorySpace<Kokkos::Device<typename execution_space::execution_space, typename execution_space::memory_space>>);
}

} // namespace Kokkos::utils::tests::concepts
