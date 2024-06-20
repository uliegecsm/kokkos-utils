#ifndef KOKKOS_UTILS_CONCEPTS_SPACE_HPP
#define KOKKOS_UTILS_CONCEPTS_SPACE_HPP

#include "Kokkos_Concepts.hpp"

namespace Kokkos::utils::concepts
{

//! Concept to specify that a type is a @c Kokkos space.
template <typename T>
concept Space = Kokkos::is_space<T>::value;

} // namespace Kokkos::utils::concepts

#endif // KOKKOS_UTILS_CONCEPTS_SPACE_HPP
