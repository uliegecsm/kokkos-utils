#include "gtest/gtest.h"

#include "Kokkos_Core.hpp"

#include "kokkos-utils/tests/scoped/ExecutionSpace.hpp"
#include "kokkos-utils/timer/Duration.hpp"
#include "kokkos-utils/timer/Event.hpp"

/**
 * @file
 *
 * @addtogroup unittests
 *
 * Event
 * -----
 *
 * This group of tests check the behavior of @ref Kokkos::utils::timer::Event.
 */

using execution_space = Kokkos::DefaultExecutionSpace;

namespace Kokkos::utils::tests::timer
{

using namespace Kokkos::utils::timer;

template <typename T>
struct EventTest : public ::testing::Test,
                   public scoped::ExecutionSpace<execution_space>
{};

using EventTypes = ::testing::Types<
    Event<void>,
    Event<execution_space>
>;

TYPED_TEST_SUITE(EventTest, EventTypes);

//! @test Check the type of the event used by the primary template and the device specializations.
TYPED_TEST(EventTest, impl_event_type)
{
    using expt_impl_event_t = std::conditional_t<
        std::same_as<TypeParam, Event<void>>,
        std::chrono::steady_clock::time_point,
#if defined(KOKKOS_ENABLE_CUDA)
        cudaEvent_t
#elif defined(KOKKOS_ENABLE_HIP)
        hipEvent_t
#else
    std::chrono::steady_clock::time_point
#endif
    >;

    static_assert(std::same_as<typename TypeParam::impl_event_t, expt_impl_event_t>);
}

//! @test Check the traits of the helper types @ref Kokkos::utils::timer::milliseconds and @ref Kokkos::utils::timer::seconds.
TEST(Event, duration_instances)
{
    static_assert(std::same_as<typename std::chrono::microseconds::period, std::ratio<1, 1000000>>);
    static_assert(std::same_as<typename              microseconds::period, std::ratio<1, 1000000>>);
    static_assert(std::same_as<typename              microseconds::rep,    double>);

    static_assert(std::same_as<typename std::chrono::milliseconds::period, std::ratio<1, 1000>>);
    static_assert(std::same_as<typename              milliseconds::period, std::ratio<1, 1000>>);
    static_assert(std::same_as<typename              milliseconds::rep,    double>);

    static_assert(std::same_as<typename std::chrono::seconds::period, std::ratio<1, 1>>);
    static_assert(std::same_as<typename              seconds::period, std::ratio<1, 1>>);
    static_assert(std::same_as<typename              seconds::rep,    double>);
}

//! @test Basic test of @ref Kokkos::utils::timer::Event::duration.
TYPED_TEST(EventTest, duration)
{
    TypeParam begin, end;

    if constexpr (std::same_as<TypeParam, void>)
    {
        begin.record();
        end.record();
    }
    else
    {
        begin.record(this->exec);
        end.record(this->exec);
    }

    ASSERT_GE(begin.template duration<milliseconds>(end).count(), 0.);
}

//! @test Ensure that @ref Kokkos::utils::timer::Event can be destroyed while recording.
TYPED_TEST(EventTest, destroyed_while_recording)
{
    std::optional<TypeParam> event(std::in_place);

    if constexpr (std::same_as<TypeParam, void>) {
        event->record();
    } else {
        event->record(this->exec);
    }

    event.reset();
}

/**
 * @test Ensure that @ref Kokkos::utils::timer::Event is properly movable.
 *
 * This is particularly critical for specializations for @c Kokkos::Cuda and @c Kokkos::HIP
 * that need to properly deal with the management of @c cudaEvent_t and @c hipEvent_t, respectively.
 */
TYPED_TEST(EventTest, movable)
{
    static_assert(std::movable<TypeParam>);

    TypeParam begin, end;

    if constexpr (std::same_as<TypeParam, void>)
    {
        begin.record();
        end.record();
    }
    else
    {
        begin.record(this->exec);
        end.record(this->exec);
    }

    TypeParam moved(std::move(end)); // NOLINT(misc-const-correctness,performance-move-const-arg)

    ASSERT_GE(begin.template duration<milliseconds>(moved).count(), 0.);
}

} // namespace Kokkos::utils::tests::timer
