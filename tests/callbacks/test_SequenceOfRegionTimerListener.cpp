#include "gmock/gmock.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/EventNameMatcher.hpp"
#include "kokkos-utils/callbacks/SequenceOfRegionTimerListener.hpp"

#include "kokkos-utils/tests/scoped/callbacks/Manager.hpp"

/**
 * @addtogroup unittests
 *
 * Listener for timing a sequence of regions
 * -----------------------------------------
 *
 * This group of tests check the behavior of @ref Kokkos::utils::callbacks::SequenceOfRegionTimerListener.
 *
 * Tests can be found in @ref tests/callbacks/test_SequenceOfRegionTimerListener.cpp.
 */

namespace Kokkos::utils::tests::callbacks
{

using namespace Kokkos::utils::callbacks;

class SequenceOfRegionTimerListenerTest : public ::testing::Test,
                                          public scoped::callbacks::Manager
{};

/**
 * @test Check that @ref SequenceOfRegionTimerListener can be used in a loop
 *       with calls to @ref SequenceOfRegionTimerListener::reset.
 */
TEST_F(SequenceOfRegionTimerListenerTest, reset)
{
    using event_matcher_t  = EventNameMatcher;
    using sequence_t       = SequenceOfRegionTimerListener<event_matcher_t>;
    using region_timer_t   = typename sequence_t::region_timer_t;
    using region_matcher_t = typename region_timer_t::matcher_t;
    using unit_t           = Kokkos::utils::timer::nanoseconds;

    constexpr unit_t sleep_for{500.};

    const auto seq = std::make_shared<sequence_t>(
        region_matcher_t{{{"region A"}}},
        region_matcher_t{{{"region B"}}},
        region_matcher_t{{{"region C"}}}
    );

    Kokkos::utils::callbacks::Manager::register_listener(seq);

    for(unsigned short int irep = 0; irep < 3; ++irep)
    {
        #define DO_A_REGION(_name_)                           \
            Kokkos::Timer region_##_name_##_outer;            \
            region_##_name_##_outer.reset();                  \
            Kokkos::Profiling::pushRegion("region " #_name_); \
            std::this_thread::sleep_for(sleep_for);           \
            Kokkos::Profiling::popRegion();                   \
            const auto region_##_name_##_elapsed = region_##_name_##_outer.seconds();

        DO_A_REGION(A)
        DO_A_REGION(B)
        DO_A_REGION(C)

        #undef DO_A_REGION

        #define CHECK_TIMER(_index_, _name_)                                                   \
            {                                                                                  \
                      auto& timer    = seq->timers.at(_index_);                                \
                const auto  duration = timer.template duration<unit_t>();                      \
                ASSERT_GT(duration, sleep_for) << #_name_;                                     \
                ASSERT_LT(duration, Kokkos::utils::timer::seconds{region_##_name_##_elapsed}); \
            }

        CHECK_TIMER(0, A)
        CHECK_TIMER(1, B)
        CHECK_TIMER(2, C)

        #undef CHECK_TIMER

        seq->reset();
    }

    Kokkos::utils::callbacks::Manager::unregister_listener(seq.get());
}

} // namespace Kokkos::utils::tests::callbacks
