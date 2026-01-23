#ifndef KOKKOS_UTILS_CALLBACKS_CONJUNCTIONMATCHER_HPP
#define KOKKOS_UTILS_CALLBACKS_CONJUNCTIONMATCHER_HPP

#include "kokkos-utils/callbacks/Events.hpp"
#include "kokkos-utils/callbacks/Matcher.hpp"

namespace Kokkos::utils::callbacks
{

/**
 * @brief Conjunction of matchers that is @c true only if all @ref matchers agree.
 *
 * @warning It is short-circuiting.
 */
template <Matcher... MatcherTypes> requires (sizeof...(MatcherTypes) > 1)
struct ConjunctionMatcher
{
    template <typename... Args> requires (sizeof...(Args) == sizeof...(MatcherTypes))
    explicit ConjunctionMatcher(Args&&... args) : matchers(std::forward<Args>(args)...) {}

    template <Event EventType> requires (MatcherFor<MatcherTypes, EventType> && ...)
    bool operator()(const EventType& event)
    {
        return std::apply([&event] (MatcherTypes&... matchers_) {
                return (matchers_(event) && ...);
        }, matchers);
    }

    std::tuple<MatcherTypes...> matchers;
};

template <typename... MatcherTypes>
ConjunctionMatcher(MatcherTypes&&...) -> ConjunctionMatcher<MatcherTypes...>;

} // namespace Kokkos::utils::callbacks

#endif // KOKKOS_UTILS_CALLBACKS_CONJUNCTIONMATCHER_HPP
