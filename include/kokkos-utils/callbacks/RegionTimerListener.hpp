#ifndef KOKKOS_UTILS_CALLBACKS_REGIONTIMERLISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_REGIONTIMERLISTENER_HPP

#include "kokkos-utils/callbacks/EventRegionMatcher.hpp"
#include "kokkos-utils/callbacks/EventTypeMatcher.hpp"
#include "kokkos-utils/callbacks/TimerListener.hpp"
#include "kokkos-utils/timer/Timer.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Timer listener for a profiling region that matches @p MatcherType.
 *
 * The @p TimerType defaults to @ref Kokkos::utils::timer::Timer<void> because a profiling
 * region typically is not itself related to an execution space instance.
 *
 * However, there can be use cases in which it may be of interest to associate the profiling
 * region with an execution space instance, such as when the profiling region surrounds a
 * group of parallel regions that enqueue work in the same execution space instance, or when
 * the profiling region surrounds a @c Kokkos::Experimental::Graph submission. In such cases,
 * @ref EnqueuedEventTimer can be used as the underlying timer.
 */
template <MatcherFor<PushRegionEvent> MatcherType, typename TimerType = Kokkos::utils::timer::Timer<void>>
struct RegionTimerListener : public TimerListener<EventTypeMatcher<EventRegionMatcher<MatcherType, true>, PushRegionEvent, PopRegionEvent>, TimerType>
{
    using region_matcher_t = EventRegionMatcher<MatcherType, true>;
    using matcher_t        = EventTypeMatcher<region_matcher_t, PushRegionEvent, PopRegionEvent>;
    using timer_t          = TimerType;
    using base_t           = TimerListener<matcher_t, timer_t>;

    template <typename T, typename U = TimerType>
    explicit RegionTimerListener(T&& matcher_, U&& timer_ = TimerType{}) : base_t{
        matcher_t{matcher_t{.matcher = {.matcher = {std::forward<T>(matcher_)}}}},
        timer_t{std::forward<U>(timer_)}
    } {}
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_REGIONTIMERLISTENER_HPP
