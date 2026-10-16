#include "gtest/gtest.h"

#include "Kokkos_Macros.hpp"
#ifdef KOKKOS_ENABLE_EXPERIMENTAL_CXX20_MODULES
import kokkos.core;
#else
#include "Kokkos_Core.hpp"
#endif

//! Initialize Google Test and @ref Kokkos.
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    Kokkos::initialize(argc, argv);

    const auto result = RUN_ALL_TESTS();

    Kokkos::finalize();

    return result;
}
