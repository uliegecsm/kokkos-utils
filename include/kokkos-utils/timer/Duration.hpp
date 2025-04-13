#ifndef KOKKOS_UTILS_TIMER_DURATION_HPP
#define KOKKOS_UTILS_TIMER_DURATION_HPP

#include <chrono>

namespace Kokkos::utils::timer
{

//! @name Helper types to represent time durations.
///@{
//! Similar to @c std::chrono::microseconds, but using @c double instead of an integer type to represent the tick count.
using microseconds = std::chrono::duration<double, std::micro>;

//! Similar to @c std::chrono::milliseconds, but using @c double instead of an integer type to represent the tick count.
using milliseconds = std::chrono::duration<double, std::milli>;

//! Similar to @c std::chrono::seconds, but using @c double instead of an integer type to represent the tick count.
using seconds = std::chrono::duration<double, std::ratio<1, 1>>;
///@}

} // namespace Kokkos::utils::timer

#endif // KOKKOS_UTILS_TIMER_DURATION_HPP
