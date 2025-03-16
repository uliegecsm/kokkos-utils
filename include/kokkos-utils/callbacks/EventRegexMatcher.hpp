#ifndef KOKKOS_UTILS_CALLBACKS_EVENTREGEXMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTREGEXMATCHER_HPP

#include <regex>

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

//! Matcher to select events whose name matches a regular expression.
struct EventRegexMatcher
{
    template <NamedEvent EventType>
    bool operator()(const EventType& event) const {
        return std::regex_search(event.name, this->regex);
    }

    template <DataEvent EventType>
    bool operator()(const EventType& event) const {
        return std::regex_search(event.alloc.name, this->regex);
    }

    std::regex regex;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTREGEXMATCHER_HPP
