#ifndef KOKKOS_UTILS_CALLBACKS_RECORDERLISTENER_HPP
#define KOKKOS_UTILS_CALLBACKS_RECORDERLISTENER_HPP

#include <deque>
#include <functional>
#include <variant>

#include "kokkos-utils/callbacks/Listener.hpp"
#include "kokkos-utils/callbacks/Manager.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

template <typename...>
class RecorderListener;

/**
 * @brief Listener for recording @ref Kokkos profiling callback calls.
 *
 * Records the @ref Kokkos profiling callback call that are represented by one
 * of the types @p EventTypes... and satisfy @p Matcher. The recorded events
 * are pushed back into a container in the order they are received.
 */
template <typename MatcherType, Event... EventTypes> requires MatcherFor<MatcherType, EventTypes...>
class RecorderListener<MatcherType, EventTypes...>
{
public:
    using matcher_t         = MatcherType;
    using event_type_list_t = Kokkos::Impl::type_list<EventTypes...>;

public:
    RecorderListener() = default;

    template <typename T> requires std::same_as<std::remove_cvref_t<T>, MatcherType>
    explicit RecorderListener(T&& matcher_)
      : matcher(std::forward<T>(matcher_)) {}

    template <EventOneOf<event_type_list_t> EventType>
    void operator()(const EventType& event) {
        if (matcher(event)) this->recorded_events.push_back(event);
    }

    /**
     * @brief Report the recorded events.
     *
     * The output is formatted as:
     *     - <Name of event type 0>: {<Description of event type 0>}
     *     - <Name of event type 1>: {<Description of event type 1>}
     *     - ...
     */
    void report(std::ostream& out) const
    {
        out << "Number of events recorded: " << recorded_events.size() << std::endl;
        for (const auto& recorded_event : recorded_events) {
            std::visit([&out] (const auto& arg) { out << "- " << arg << std::endl; }, recorded_event);
        }
    }

    //! Returns the events of the types @p EventTypes... that occur during the execution of @p callable.
    template <typename Callable>
    static auto record(Callable&& callable)
    {
        const auto recorder_listener = std::make_shared<RecorderListener>();

        Manager::register_listener(recorder_listener);

        std::invoke(std::forward<Callable>(callable));

        Manager::unregister_listener(recorder_listener.get());

        return std::move(recorder_listener->recorded_events);
    }

public:
    /**
     * @note We use a @c std::deque because it can automatically expand when needed (whereas
     *       @c std::vector requires a reallocation and a copy).
     */
    std::deque<std::variant<EventTypes...>> recorded_events {};

private:
    MatcherType matcher {};
};

template <typename MatcherType, Event... EventTypes>
class RecorderListener<MatcherType, Kokkos::Impl::type_list<EventTypes...>> : public RecorderListener<MatcherType, EventTypes...> {
    using RecorderListener<MatcherType, EventTypes...>::RecorderListener;
};

template <Event... EventTypes>
class RecorderListener<EventTypes...> : public RecorderListener<AnyEventMatcher, EventTypes...> {};

template <Event... EventTypes>
class RecorderListener<Kokkos::Impl::type_list<EventTypes...>> : public RecorderListener<EventTypes...> {};

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_RECORDERLISTENER_HPP
