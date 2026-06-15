#ifndef KOKKOS_UTILS_CALLBACKS_EVENTREGIONMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_EVENTREGIONMATCHER_HPP

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
 *
 * Set @p TrueOnBoundary to @c true if it must return @c true for the opening @ref PushRegionEvent and
 * closing @ref PopRegionEvent and @c false otherwise.
 */
template <typename MatcherType, bool TrueOnBoundary = false> requires MatcherFor<MatcherType, PushRegionEvent>
struct EventRegionMatcher
{
    static constexpr uint32_t invalid_nested_level = Kokkos::Experimental::finite_max_v<uint32_t>;

    //! If the @p event matches @ref matcher, @ref matching is set to @c true.
    bool operator()(const PushRegionEvent& event)
    {
        if (this->matching)
        {
            ++this->nested_level;
            return ! TrueOnBoundary;
        }
        else if (matcher(event))
        {
            this->nested_level = 1;
            this->matching     = true;
            return TrueOnBoundary;
        }

        return TrueOnBoundary ? false : this->matching;
    }

    //! Decrement @ref nested_level if we are still matching.
    bool operator()(const PopRegionEvent&)
    {
        if (this->matching)
        {
            --this->nested_level;

            if (this->nested_level == 0)
            {
                this->matching = false;
                return TrueOnBoundary;
            }
        }

        return TrueOnBoundary ? false : this->matching;
    }

    template <Event EventType>
    bool operator()(const EventType&) const {
        return TrueOnBoundary ? false : this->matching;
    }

    //! The preprocessor condition is a workaround for an ICE in nvcc 12.8 with gcc 14.2.0.
#if defined(KOKKOS_ENABLE_CUDA) && (defined(KOKKOS_COMPILER_GNU) && KOKKOS_COMPILER_GNU == 1420) && (defined(KOKKOS_COMPILER_NVCC) && KOKKOS_COMPILER_NVCC == 1280)
    MatcherType matcher;
#else
    MatcherType matcher{};
#endif
    bool matching = false;
    uint32_t nested_level = invalid_nested_level;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_EVENTREGIONMATCHER_HPP
