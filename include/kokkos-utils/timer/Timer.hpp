#ifndef KOKKOS_UTILS_TIMER_TIMER_HPP
#define KOKKOS_UTILS_TIMER_TIMER_HPP

#include "kokkos-utils/timer/Duration.hpp"
#include "kokkos-utils/timer/Event.hpp"

namespace Kokkos::utils::timer
{
/**
 * @brief Measure elapsed time between events.
 *
 * This class uses @ref Event to create, destroy, record, and compute
 * the elapsed time between events.
 */
template <typename T>
requires Kokkos::ExecutionSpace<T> || std::is_void_v<T>
class Timer
{
public:
    using event_t = Event<T>;

public:
    //! Returns @c true if @ref start and @ref stop have been called.
    bool is_valid() const { return tick.has_value() && tock.has_value(); }

    /**
     * Get a @c std::chrono::duration object representing the elapsed time.
     *
     * @note This function is not @c const because it calls @ref Event::duration,
     *       whose device specializations are not @c const.
     */
    template <typename Duration = milliseconds>
    Duration duration() { return tick->template duration<Duration>(*tock); }

    //! Start the timer.
    template <typename U = T>
    requires std::is_void_v<U>
    void start()
    {
        if(! tick.has_value()) tick.emplace();
        tick->record();
    }

    template <typename Exec = T>
    requires Kokkos::ExecutionSpace<Exec>
    void start(const Exec& exec)
    {
        if(! tick.has_value()) tick.emplace();
        tick->record(exec);
    }

    //! Stop the timer.
    template <typename U = T>
    requires std::is_void_v<U>
    void stop()
    {
        if(! tock.has_value()) tock.emplace();
        tock->record();
    }

    template <typename Exec = T>
    requires Kokkos::ExecutionSpace<Exec>
    void stop(const Exec& exec)
    {
        if(! tock.has_value()) tock.emplace();
        tock->record(exec);
    }

private:
    std::optional<event_t> tick = std::nullopt, tock = std::nullopt;
};

} // namespace Kokkos::utils::timer

#endif // KOKKOS_UTILS_TIMER_TIMER_HPP
