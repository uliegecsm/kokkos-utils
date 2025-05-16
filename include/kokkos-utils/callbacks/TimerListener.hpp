#ifndef KOKKOS_UTILS_CALLBACKS_TIMERLISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_TIMERLISTENER_HPP

#include "kokkos-utils/callbacks/Listener.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

template <typename TimerType>
concept Startable = requires (TimerType& timer) {
    { timer.start() } -> std::same_as<void>;
};

template <typename TimerType, typename EventType>
concept StartableWithEvent = requires (TimerType& timer, const EventType& event) {
    { timer.start(event) } -> std::same_as<void>;
};

template <typename TimerType>
concept Stoppable = requires (TimerType& timer) {
    { timer.stop() } -> std::same_as<void>;
};

template <typename TimerType, typename EventType>
concept StoppableWithEvent = requires (TimerType& timer, const EventType& event) {
    { timer.stop(event) } -> std::same_as<void>;
};

/**
 * @brief Listener that starts a timer when a begin event is received and stops it
 *        when a corresponding end event is received.
 *
 * The begin event is selected by requiring it to be a match for @p BeginEndMatcherType
 * while no begin event has been matched before. The end event is selected by requiring
 * it to be a match for @p BeginEndMatcherType after a begin event has been matched.
 *
 * The listener can be reset by using the member function @ref reset. After calling
 * @ref reset, a new begin event can be matched.
 */
template <
    Matcher BeginEndMatcherType,
    typename TimerType
>
struct TimerListener
{
    template <Event EventType> requires MatcherFor<BeginEndMatcherType, EventType>
    void operator()(const EventType& event)
    {
        if(closed()) return;

        const bool matching = matcher(event);

        //! Start when the event matches while no begin event has been matched before.
        if(!this->matched_begin && matching)
        {
            this->matched_begin = true;

            if constexpr (Startable<TimerType>) {
                this->timer.start();
            } else if constexpr (StartableWithEvent<TimerType, EventType>) {
                this->timer.start(event);
            }
        }
        //! Stop when the event matches after a begin event has been matched.
        else if(this->matched_begin && ! this->matched_end && matching)
        {
            this->matched_end = true;

            if constexpr (Stoppable<TimerType>) {
                this->timer.stop();
            } else if constexpr (StoppableWithEvent<TimerType, EventType>) {
                this->timer.stop(event);
            }
        }
    }

    //! When the listener has matched the begin, but not yet the end event, it is said to be "connected".
    bool connected() const { return matched_begin && !matched_end; }

    //! When the listener has matched both the begin and end events, it is said to be "closed".
    bool closed() const { return matched_begin && matched_end; }

    //! Reset the listener so that it can be reused to match a begin event again.
    void reset() {
        matched_begin = matched_end = false;
    }

    BeginEndMatcherType matcher {};
    TimerType timer {};
    bool matched_begin = false;
    bool matched_end   = false;
};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_TIMERLISTENER_HPP
