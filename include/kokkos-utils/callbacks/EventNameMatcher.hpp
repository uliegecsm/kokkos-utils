#ifndef KOKKOS_UTILS_CALLBACKS_EVENTNAMEMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTNAMEMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

//! Matcher to select events whose name matches @ref name.
struct EventNameMatcher
{
    template <NamedEvent EventType>
    bool operator()(const EventType& event) const {
        return event.name == this->name;
    }

    template <DataEvent EventType>
    bool operator()(const EventType& event) const {
        return event.alloc.name == this->name;
    }

    std::string name;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTNAMEMATCHER_HPP
