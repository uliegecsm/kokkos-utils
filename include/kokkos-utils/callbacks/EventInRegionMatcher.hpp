#ifndef KOKKOS_UTILS_CALLBACKS_EVENTINREGIONMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTINREGIONMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Matcher to select events that occur within a region.
 *
 * The region is selected by @ref matcher.
 *
 * Any event occuring between the matching @ref PushRegionEvent and its
 * corresponding @ref PopRegionEvent will match.
 */
template <typename MatcherType> requires MatcherFor<MatcherType, PushRegionEvent>
struct EventInRegionMatcher
{
    static constexpr uint32_t invalid_nested_level = Kokkos::Experimental::finite_max_v<uint32_t>;

    //! If the @p event matches @ref matcher, @ref matching is set to @c true.
    bool operator()(const PushRegionEvent& event)
    {
        if (this->matching)
        {
            ++this->nested_level;
            return true;
        }
        else if (matcher(event))
        {
            this->nested_level = 1;
            this->matching     = true;
        }

        return false;
    }

    //! Decrement @ref nested_level if we are still matching.
    bool operator()(const PopRegionEvent&)
    {
        if (this->matching)
        {
            --this->nested_level;

            if (this->nested_level == 0)
                this->matching = false;
        }

        return this->matching;
    }

    template <Event EventType>
    bool operator()(const EventType&) const {
        return matching;
    }

    MatcherType matcher {};
    bool matching = false;
    uint32_t nested_level = invalid_nested_level;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTINREGIONMATCHER_HPP
