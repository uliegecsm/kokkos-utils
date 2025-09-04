#ifndef KOKKOS_UTILS_TESTS_SCOPED_CALLBACKS_MANAGER_HPP
#define KOKKOS_UTILS_TESTS_SCOPED_CALLBACKS_MANAGER_HPP

#include "kokkos-utils/callbacks/Manager.hpp"

namespace Kokkos::utils::tests::scoped::callbacks
{
//! Initializing and finalizing @ref Kokkos::utils::callbacks::Manager in a RAII manner.
struct [[nodiscard]] Manager
{
    Manager()  { Kokkos::utils::callbacks::Manager::initialize(); }
    ~Manager() { Kokkos::utils::callbacks::Manager::finalize(); }

    Manager(const Manager&)            = delete;
    Manager& operator=(const Manager&) = delete;
};

} // namespace Kokkos::utils::tests::scoped::callbacks

#endif // KOKKOS_UTILS_TESTS_SCOPED_CALLBACKS_MANAGER_HPP
