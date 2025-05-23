#ifndef KOKKOS_UTILS_TESTS_HELPERS_HPP
#define KOKKOS_UTILS_TESTS_HELPERS_HPP

#include "kokkos-utils/concepts/ExecutionSpace.hpp"

namespace Kokkos::utils::tests::fixtures
{

//! Fixture class that creates a new execution space instance.
template <Kokkos::utils::concepts::ExecutionSpace Exec>
class ExecutionSpaceInstance
{
public:
    ExecutionSpaceInstance() : exec(Kokkos::Experimental::partition_space(Exec{}, 1)[0]) {}

protected:
    Exec exec;
};

} // namespace Kokkos::utils::tests

#endif // KOKKOS_UTILS_TESTS_HELPERS_HPP
