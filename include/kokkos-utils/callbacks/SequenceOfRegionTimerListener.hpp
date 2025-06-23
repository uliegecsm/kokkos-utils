#ifndef KOKKOS_UTILS_CALLBACKS_SEQUENCEOFREGIONTIMERLISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_SEQUENCEOFREGIONTIMERLISTENER_HPP

#include "kokkos-utils/callbacks/RegionTimerListener.hpp"

namespace Kokkos::utils::callbacks
{
/**
 * @brief Time a sequence of push/pop regions.
 *
 * The regions of interest must not be interleaved.
 *
 * @note Overhead should be as low as possible. This is done by storing the @ref matchers
 *       in a list and looking only at the @ref current one. Already-matched and next @ref matchers
 *       will not be called.
 */
template <Matcher MatcherType>
struct SequenceOfRegionTimerListener
{
public:
    using region_timer_t = RegionTimerListener<MatcherType>;
    using matcher_t      = typename region_timer_t::matcher_t;
    using timer_t        = typename region_timer_t::timer_t;

    template <typename... Ms>
    explicit SequenceOfRegionTimerListener(Ms&&... matchers_)
        : matchers{std::forward<Ms>(matchers_)...},
          timers(matchers.size())
    {}

    void reset() { current = 0; }

    //! Returns @c true if all regions were correctly matched and timed.
    bool all_matched() const {
        return current == timers.size();
    }

    //! Deal with a @ref Kokkos::utils::callbacks::PushRegionEvent event. Only the @ref current timer is auditioned.
    void operator()(const PushRegionEvent& event)
    {
        if(all_matched()) return;

        const auto matched = matchers.at(current).operator()(event);

        if(matched)
            timers.at(current).start();
    }

    //! Deal with a @ref Kokkos::utils::callbacks::PopRegionEvent event. If the @ref current timer matches, @ref current is incremented.
    void operator()(const PopRegionEvent& event)
    {
        if(all_matched()) return;

        const auto matched = matchers.at(current).operator()(event);

        if(matched)
        {
            timers.at(current).stop();
            ++current;
        }
    }

public:
    std::vector<matcher_t> matchers;
    std::vector<timer_t>   timers;

private:
    size_t current = 0;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_SEQUENCEOFREGIONTIMERLISTENER_HPP
