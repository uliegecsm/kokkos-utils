#ifndef KOKKOS_UTILS_CONCEPTS_DUALVIEW_HPP
#define KOKKOS_UTILS_CONCEPTS_DUALVIEW_HPP

#include "Kokkos_DualView.hpp"

namespace Kokkos::utils::concepts
{

//! Concept to specify that a type is a @c Kokkos::DualView.
template <typename T>
concept DualView = Kokkos::is_dual_view_v<T>;

} // namespace Kokkos::utils::concepts

#endif // KOKKOS_UTILS_CONCEPTS_DUALVIEW_HPP
