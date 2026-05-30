#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/Helpers.hpp"
#include "kokkos-utils/callbacks/RecorderListener.hpp"
#include "kokkos-utils/tests/scoped/ExecutionSpace.hpp"
#include "kokkos-utils/tests/scoped/callbacks/Manager.hpp"

using execution_space = Kokkos::DefaultExecutionSpace;

/**
 * @file
 *
 * @addtogroup unittests
 *
 * ExecutionSpace storage
 * ----------------------
 *
 * This group of tests check the behavior of our @c Kokkos::utils::tests::scoped::ExecutionSpace that can be
 * found in @ref tests/scoped/ExecutionSpace.hpp.
 */

namespace Kokkos::utils::tests::tests::scoped
{

//! @test Check that @c Kokkos::utils::tests::scoped::ExecutionSpace fences on destruction.
TEST(tests_scoped_execution_space, fence_on_destruction)
{
    using namespace Kokkos::utils::callbacks;

    Manager::initialize();

    ASSERT_THAT(
        RecorderListener<BeginFenceEvent>::record([]{ const Kokkos::utils::tests::scoped::ExecutionSpace<execution_space> exec_storagge; }),
        ::testing::ElementsAre(
            ABeginFenceEventWithName(::testing::StrEq(Kokkos::utils::tests::scoped::ExecutionSpace<execution_space>::fence_label))
        )
    );

    Manager::finalize();
}

} // namespace Kokkos::utils::tests::tests::scoped
