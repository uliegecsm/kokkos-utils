#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

//! Initialize Google Test and @c Kokkos.
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    Kokkos::initialize(argc, argv);

    const auto result = RUN_ALL_TESTS();

    Kokkos::finalize();

    return result;
}