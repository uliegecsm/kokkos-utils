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
template <Kokkos::utils::concepts::ExecutionSpace Exec>
class Timer
{
public:
    using event_t = Event<Exec>;

public:
    //! Reset the timer.
    void reset(const Exec& exec)
    {
        tick.record(exec);
        started = true;
    }

    //! Stop the timer.
    void stop(const Exec& exec)
    {
        tock.record(exec);
        stopped = true;
    }

    //! Returns @c true if @ref reset and @ref stop have been called.
    bool is_valid() const { return started && stopped; }

    /**
     * Get a @c std::chrono::duration object representing the elapsed time.
     *
     * @note This function is not @c const because it calls @ref Event::duration,
     *       whose device specializations are not @c const.
     */
    template <typename Duration = milliseconds>
    Duration duration() { return tick.template duration<Duration>(tock); }

private:
    event_t tick {},         tock {};
    bool    started = false, stopped = false;
};

} // namespace Kokkos::utils::timer

#endif // KOKKOS_UTILS_TIMER_TIMER_HPP
