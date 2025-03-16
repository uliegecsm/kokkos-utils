#ifndef KOKKOS_UTILS_CALLBACKS_EVENTINPROFILESECTIONREGEXMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTINPROFILESECTIONREGEXMATCHER_HPP

#include <regex>

#include "Kokkos_NumericTraits.hpp"

#include "kokkos-utils/callbacks/Events.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Matcher to select events that occur within a profile section.
 *
 * The profile section is selected by matching its name to a regular expression.
 */
struct EventInProfileSectionRegexMatcher
{
    static constexpr uint32_t invalid_section_id = Kokkos::Experimental::finite_max_v<uint32_t>;

    bool operator()(const CreateProfileSectionEvent& event)
    {
        if (std::regex_search(event.name, this->regex))
        {
            if (section_id != invalid_section_id) Kokkos::abort("EventInProfileSectionRegexMatcher cannot match a create event twice.");
            section_id = event.section_id;
        }
        return false;
    }

    bool operator()(const StartProfileSectionEvent& event)
    {
        if (event.section_id == section_id) recording = true;
        return false;
    }

    bool operator()(const StopProfileSectionEvent& event)
    {
        if (event.section_id == section_id) recording = false;
        return false;
    }

    template <Event EventType>
    bool operator()(const EventType& /* event */) const {
        return recording;
    }

    std::regex regex;
    uint32_t section_id = invalid_section_id;
    bool recording = false;
};

} // Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTINPROFILESECTIONREGEXMATCHER_HPP

