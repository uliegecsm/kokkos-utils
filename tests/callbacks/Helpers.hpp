#ifndef KOKKOS_UTILS_TESTS_CALLBACKS_HELPERS_HPP
#define KOKKOS_UTILS_TESTS_CALLBACKS_HELPERS_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::tests::callbacks
{
namespace impl
{
template <typename>
struct EventTestTypes;

template <typename... EventTypes>
struct EventTestTypes<Kokkos::Impl::type_list<EventTypes...>>
{
    using type = ::testing::Types<EventTypes...>;
};
} // namespace impl

//! Useful type for defining a typed test suite over all types in @ref Kokkos::utils::callbacks::EventTypeList.
using EventTestTypes = typename impl::EventTestTypes<Kokkos::utils::callbacks::EventTypeList>::type;

} // namespace Kokkos::utils::tests::callbacks

#endif // KOKKOS_UTILS_TESTS_CALLBACKS_HELPERS_HPP
