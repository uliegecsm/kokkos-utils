#ifndef KOKKOS_UTILS_CALLBACKS_LISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_LISTENER_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

namespace impl
{

/**
 * Helper struct needed for the implementation of concepts and type traits such as:
 *  - @ref Kokkos::utils::callbacks::listener_event_type_list_t
 *  - @ref Kokkos::utils::callbacks::ListenerFor
 */
template <typename Callable>
struct IsListenerFor
{
    template <Event EventType>
    using type = std::is_invocable_r<void, Callable, const EventType&>;
};

template <typename T>
concept has_event_type_list = requires { typename T::event_type_list_t; };

template <typename Callable>
struct ListenerEventTypeList
{
    using type = Kokkos::Impl::filter_type_list_t<
        impl::IsListenerFor<Callable>::template type, EventTypeList>;
};

template <has_event_type_list Callable>
struct ListenerEventTypeList<Callable>
{
    using type = typename Callable::event_type_list_t;
};

/**
 * A @p Callable can be treated *implicitly* as a listener if it is invocable with at least one event type
 * from @ref Kokkos::utils::callbacks::EventTypeList passed by @c const reference and returns @c void.
 */
template <typename Callable>
concept ImplicitListener = Kokkos::utils::impl::type_list_any_v<impl::IsListenerFor<Callable>::template type, EventTypeList>;

/**
 * A @p Callable can be treated as a listener if it models @ref Kokkos::utils::callbacks::impl::has_event_type_list
 * and it is callable for each of the event types in the list passed by @c const reference and returns @c void.
 */
template <typename Callable>
concept ExplicitListener = has_event_type_list<Callable> &&
    Kokkos::utils::impl::type_list_all_v<
        impl::IsListenerFor<Callable>::template type,
        typename Callable::event_type_list_t
    >;
} // namespace impl

/**
 * A @p Callable is a listener if it satisfies either @ref Kokkos::utils::callbacks::impl::ExplicitListener
 * or @ref Kokkos::utils::callbacks::impl::ImplicitListener.
 */
template <typename Callable>
concept Listener = impl::ExplicitListener<Callable> || impl::ImplicitListener<Callable>;

//! Check that @p Callable is a listener for each event in @p EventTypes.
template <typename Callable, typename... EventTypes>
concept ListenerFor = Kokkos::utils::impl::type_list_all_v<impl::IsListenerFor<Callable>::template type, EventTypes...>;

//! Type list holding the event types that @p Callable can be a listener for.
template <typename Callable>
using listener_event_type_list_t = typename impl::ListenerEventTypeList<Callable>::type;

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_LISTENER_HPP
