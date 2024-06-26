#ifndef KOKKOS_UTILS_CONCEPTS_MEMORYSPACE_HPP
#define KOKKOS_UTILS_CONCEPTS_MEMORYSPACE_HPP

#include "Kokkos_Concepts.hpp"

namespace Kokkos::utils::concepts
{

//! Concept to specify that a type is a @c Kokkos memory space.
template <typename T>
concept MemorySpace = Kokkos::is_memory_space_v<T>;

} // namespace Kokkos::utils::concepts

#endif // KOKKOS_UTILS_CONCEPTS_MEMORYSPACE_HPP
