#ifndef KOKKOS_UTILS_CALLBACKS_LISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_LISTENER_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

namespace impl
{

//! Helper struct needed for the implementation of @ref Kokkos::utils::callbacks::listener_event_type_list_t.
template <typename Callable>
struct IsListenerFor
{
    template <Event EventType>
    using type = std::is_invocable_r<void, Callable, const EventType&>;
};

} // namespace impl

//! Type list holding the event types that a callable object can be a listener for.
template <typename Callable>
using listener_event_type_list_t = Kokkos::Impl::filter_type_list_t<impl::IsListenerFor<Callable>::template type, EventTypeList>;

/**
 * @brief Concept that models that a callable object to be registered as a listener by @ref Kokkos::utils::callbacks::Manager must
 *        have a non-empty list of event types that it can be a listener for.
 */
template <typename Callable>
concept Listener = ( ! std::same_as<listener_event_type_list_t<Callable>, Kokkos::Impl::type_list<>>);

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_LISTENER_HPP
