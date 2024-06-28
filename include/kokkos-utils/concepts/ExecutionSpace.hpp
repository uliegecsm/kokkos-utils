#ifndef KOKKOS_UTILS_CONCEPTS_EXECUTIONSPACE_HPP
#define KOKKOS_UTILS_CONCEPTS_EXECUTIONSPACE_HPP

#include "Kokkos_Concepts.hpp"

namespace Kokkos::utils::concepts
{

//! Specify that a type is a @ref Kokkos execution space.
template <typename T>
concept ExecutionSpace = Kokkos::is_execution_space_v<T>;

} // namespace Kokkos::utils::concepts

#endif // KOKKOS_UTILS_CONCEPTS_EXECUTIONSPACE_HPP
