#ifndef KOKKOS_UTILS_CALLBACKS_BEGINENDTIMERLISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_BEGINENDTIMERLISTENER_HPP

#include "kokkos-utils/callbacks/ConjunctionMatcher.hpp"
#include "kokkos-utils/callbacks/EnqueuedEventTimer.hpp"
#include "kokkos-utils/callbacks/EventBeginEndIdMatcher.hpp"
#include "kokkos-utils/callbacks/EventQueueMatcher.hpp"
#include "kokkos-utils/callbacks/TimerListener.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Timer listener that is well-suited for timing the execution of a parallel
 *        workload enqueued on a particular execution space instance.
 *
 * This specialization is particularly well-suited for timing a @c Kokkos parallel for, parallel
 * reduce, parallel scan, or deep copy operation. In such a situation, the execution space
 * instance passed to this class's constructor should be the same as the one passed to the workload.
 */
template <Matcher MatcherType, Event BeginEventType, Event EndEventType, Kokkos::utils::concepts::ExecutionSpace Exec> requires MatcherFor<MatcherType, BeginEventType>
struct BeginEndTimerListener : public TimerListener<EventBeginEndIdMatcher<BeginEventType, ConjunctionMatcher<EventQueueMatcher<Exec>, MatcherType>>, EnqueuedEventTimer<Exec>>
{
    using queue_matcher_t = EventQueueMatcher<Exec>;
    using matcher_t       = EventBeginEndIdMatcher<BeginEventType, ConjunctionMatcher<queue_matcher_t, MatcherType>>;
    using timer_t         = EnqueuedEventTimer<Exec>;
    using base_t          = TimerListener<matcher_t, timer_t>;

    template <typename T>
    BeginEndTimerListener(T&& matcher, const Exec& exec) : base_t{
        matcher_t{.matcher = ConjunctionMatcher{queue_matcher_t{.exec = exec}, MatcherType{std::forward<T>(matcher)}}},
        timer_t{.exec = exec}
    } {}
};

//! Timer listener for a parallel for that matches @p MatcherType.
template <MatcherFor<BeginParallelForEvent> MatcherType, Kokkos::utils::concepts::ExecutionSpace Exec>
using ParallelForTimerListener = BeginEndTimerListener<MatcherType, BeginParallelForEvent, EndParallelForEvent, Exec>;

//! Timer listener for a parallel reduce that matches @p MatcherType.
template <MatcherFor<BeginParallelReduceEvent> MatcherType, Kokkos::utils::concepts::ExecutionSpace Exec>
using ParallelReduceTimerListener = BeginEndTimerListener<MatcherType, BeginParallelReduceEvent, EndParallelReduceEvent, Exec>;

//! Timer listener for a parallel scan that matches @p MatcherType.
template <MatcherFor<BeginParallelScanEvent> MatcherType, Kokkos::utils::concepts::ExecutionSpace Exec>
using ParallelScanTimerListener = BeginEndTimerListener<MatcherType, BeginParallelScanEvent, EndParallelScanEvent, Exec>;

//! Timer listener for a deep copy that matches @p MatcherType.
template <MatcherFor<BeginDeepCopyEvent> MatcherType, Kokkos::utils::concepts::ExecutionSpace Exec>
using DeepCopyTimerListener = BeginEndTimerListener<MatcherType, BeginDeepCopyEvent, EndDeepCopyEvent, Exec>;

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_BEGINENDTIMERLISTENER_HPP
