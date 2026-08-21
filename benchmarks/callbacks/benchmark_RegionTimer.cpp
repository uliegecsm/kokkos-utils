#include "Kokkos_Core.hpp"

#include "kokkos-utils/callbacks/EventNameMatcher.hpp"
#include "kokkos-utils/callbacks/Manager.hpp"
#include "kokkos-utils/callbacks/SequenceOfRegionTimerListener.hpp"

#include "kokkos-utils/tests/scoped/callbacks/Manager.hpp"

#include "tests/Benchmarking.hpp"

/**
 * @addtogroup unitbenchmarks
 *
 * Timing a sequence of push/pop regions efficiently
 * -------------------------------------------------
 *
 * This group of benchmarks is used to check which strategy is best to time a sequence of
 * push/pop regions, *i.e.* using either of:
 *  - @ref callbacks::Manager with as many registered @ref RegionTimerListener as there are regions to time
 *  - @ref callbacks::Manager with a single registered @ref SequenceOfRegionTimer
 *  - a single @ref SequenceOfRegionTimerListener registered to @c Kokkos::Tools manually, *i.e.* bypassing
 *    @ref callbacks::Manager
 *
 * The conclusion seems to be that using @ref SequenceOfRegionTimerListener is much better than registering
 * several @ref RegionTimerListener. However, using @ref SequenceOfRegionTimerListener through @ref callbacks::Manager
 * instead of registering it manually has little impact, the latter being a few nanoseconds more efficient.
 *
 * We've also added 2 interesting cases.
 *
 * The first one can be used to measure the overhead of using @c Kokkos::Tools::pushRegion and
 * @c Kokkos::Tools::popRegion when the associated callbacks are not set (*i.e.* their value is @c nullptr).
 * It is usually very small (dozens of nanoseconds).
 *
 * The second one can be used to measure the overhead of using a timer similar to
 * @ref Kokkos::utils::timer::Timer<void>. It starts and stops the timer and asks for the elapsed time
 * in between the two events.
 * Very surprinsgly, the time it takes to perform the sequence start-stop-elapsed can be huge and noisy on some
 * machines (several thousands of nanoseconds) and very small and stable on others (dozens of nanoseconds).
 * Even more surprising is that some machines therefore report elapsed times in the order of thousands of
 * nanoseconds, while others report a handful of nanoseconds.
 * Thus, on some machines, we have to be careful with using the timers because:
 *  - they might have huge overhead
 *  - they might not be able to report very tiny elapsed time
 * The main take away message is that whenever possible, use only one sequence of start-stop-elapsed
 * for several runs of your benchmark to minimize the impact of timing, as follows:
 *  @code
 *  timer_t timer;
 *  timer.start();
 *  for(size_t irep = 0; irep < nreps; ++irep) function();
 *  timer.stop();
 *  @endcode
 *
 * @note Some machines might blacklist the TSC counter as unstable, and resort to less
 *       efficient clock sources. See also:
 *        - https://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/.
 *        - https://blog.trailofbits.com/2019/10/03/tsc-frequency-for-all-better-profiling-and-benchmarking/
 */

using execution_space = Kokkos::DefaultExecutionSpace;

#define REGION_LABEL "This is a quite long region name, hopefully it's not using SBO at all."

namespace Kokkos::utils::benchmarks::callbacks
{
struct MyWorkload
{
    void execute(const ::benchmark::State& state) const
    {
        Kokkos::Tools::pushRegion(REGION_LABEL);
        Kokkos::Tools::popRegion();

        Kokkos::Tools::pushRegion("region B");
        Kokkos::Tools::popRegion();

        //! Add some regions that won't match, it's just noise.
        using range_t = decltype(state.range(0));
        for(range_t irep = 0; irep < state.range(0); ++irep) {
            Kokkos::Tools::pushRegion("region " + std::to_string(irep));
            Kokkos::Tools::popRegion();
        }

        Kokkos::Tools::pushRegion("region C");
        Kokkos::Tools::popRegion();

        Kokkos::Tools::pushRegion("region D");
        Kokkos::Tools::popRegion();
    }
};

template <typename T>
class Fixture : public ::benchmark::Fixture
{
public:
    using impl_t = T;

public:
    FIXME_PARTIAL_OVERRIDE_WARNING_NVCC(TearDown)
    FIXME_PARTIAL_OVERRIDE_WARNING_NVCC(SetUp)

    void SetUp(const ::benchmark::State& state) override {
        impl.emplace(state);
    }

    void TearDown(const ::benchmark::State&) override {
        impl.reset();
    }

protected:
    std::optional<impl_t> impl = std::nullopt;
};

class SequenceOfRegionTimerListenerBenchmarkImpl : public Kokkos::utils::tests::scoped::callbacks::Manager
{
public:
    using event_matcher_t  = Kokkos::utils::callbacks::EventNameMatcher;
    using sequence_t       = Kokkos::utils::callbacks::SequenceOfRegionTimerListener<event_matcher_t>;
    using region_matcher_t = typename sequence_t::matcher_t;

public:
    static void push(const char* name) {
        sequence->operator()(Kokkos::utils::callbacks::PushRegionEvent{.name = name});
    }
    static void pop() {
        sequence->operator()(Kokkos::utils::callbacks::PopRegionEvent{});
    }

    explicit SequenceOfRegionTimerListenerBenchmarkImpl(const ::benchmark::State& state)
    {
        sequence = std::make_shared<sequence_t>(
            region_matcher_t{{{REGION_LABEL}}},
            region_matcher_t{{{"region B"}}},
            region_matcher_t{{{"region C"}}},
            region_matcher_t{{{"region D"}}}
        );

        if(state.range(1)) {
            Kokkos::utils::callbacks::Manager::register_listener(sequence);
        } else {
            Kokkos::Tools::Experimental::set_push_region_callback(SequenceOfRegionTimerListenerBenchmarkImpl::push);
            Kokkos::Tools::Experimental::set_pop_region_callback (SequenceOfRegionTimerListenerBenchmarkImpl::pop);
        }
    }

    ~SequenceOfRegionTimerListenerBenchmarkImpl() {
        if(sequence.use_count() == 2) {
            Kokkos::utils::callbacks::Manager::unregister_listener(sequence.get());
        } else {
            Kokkos::Tools::Experimental::set_push_region_callback(nullptr);
            Kokkos::Tools::Experimental::set_pop_region_callback (nullptr);
        }
        sequence = nullptr;
    }

    //! Reset timers and run @ref MyFixture::run.
    void run(::benchmark::State& state)
    {
        sequence->reset();

        workload.execute(state);

        if(!sequence->all_matched())
            throw std::runtime_error("At least one timer is not matched.");
    }

private:
    MyWorkload workload {};
    static inline std::shared_ptr<sequence_t> sequence = nullptr;
};

class ManagerBenchmarkImpl : public Kokkos::utils::tests::scoped::callbacks::Manager
{
public:
    using event_matcher_t = Kokkos::utils::callbacks::EventNameMatcher;
    using region_timer_t  = Kokkos::utils::callbacks::RegionTimerListener<event_matcher_t>;

public:
    explicit ManagerBenchmarkImpl(const ::benchmark::State& state)
    {
        for(const auto& name : {REGION_LABEL, "region B", "region C", "region D"})
        {
            auto timer = std::make_shared<region_timer_t>(name);
            Kokkos::utils::callbacks::Manager::register_listener(timer);
            timers.push_back(std::move(timer));
        }
    }

    ~ManagerBenchmarkImpl()
    {
        for(const auto& timer : timers)
            Kokkos::utils::callbacks::Manager::unregister_listener(timer.get());
    }

    //! Reset timers and run @ref MyFixture::run.
    void run(::benchmark::State& state)
    {
        for(auto& timer : timers)
            timer->reset();

        workload.execute(state);

        for(const auto& timer : timers)
            if(!timer->closed())
                throw std::runtime_error(std::string("At least one timer is not closed: ") + timer->matcher.matcher.matcher.name);
    }

private:
    std::vector<std::shared_ptr<region_timer_t>> timers;
    MyWorkload workload {};
};

class OverheadBenchmarkImpl
{
public:
    static void push(const char*) {}
    static void pop(){}

    explicit OverheadBenchmarkImpl(const ::benchmark::State&)
    {
        Kokkos::Tools::Experimental::set_push_region_callback(OverheadBenchmarkImpl::push);
        Kokkos::Tools::Experimental::set_pop_region_callback (OverheadBenchmarkImpl::pop);
    }

    ~OverheadBenchmarkImpl()
    {
        Kokkos::Tools::Experimental::set_push_region_callback(nullptr);
        Kokkos::Tools::Experimental::set_pop_region_callback (nullptr);
    }

    void run_push_pop() const
    {
        Kokkos::Tools::pushRegion(REGION_LABEL);
        Kokkos::Tools::popRegion();
    }

    auto run_timer()
    {
        timer.start();
        timer.stop();
        return timer.template duration<Kokkos::utils::timer::nanoseconds>();
    }

private:
    Kokkos::utils::timer::Timer<void> timer{};
};

using SequenceOfRegionTimerListenerBenchmark = Fixture<SequenceOfRegionTimerListenerBenchmarkImpl>;

BENCHMARK_DEFINE_F(SequenceOfRegionTimerListenerBenchmark, time_sequence_of_regions)(::benchmark::State& state) {
    for (auto sample : state) {
        this->impl->run(state);
    }
}
BENCHMARK_REGISTER_F(SequenceOfRegionTimerListenerBenchmark, time_sequence_of_regions)
    ->ArgNames({"repeat", "use-manager"})
    ->Args({  0, true})->Args({  0, false})
    ->Args({ 25, true})->Args({ 25, false})
    ->Args({250, true})->Args({250, false});

using ManagerBenchmark = Fixture<ManagerBenchmarkImpl>;

BENCHMARK_DEFINE_F(ManagerBenchmark, time_sequence_of_regions)(::benchmark::State& state) {
    for (auto sample : state) {
        this->impl->run(state);
    }
}
BENCHMARK_REGISTER_F(ManagerBenchmark, time_sequence_of_regions)
    ->ArgName("repeat")
    ->Arg(0)->Arg(25)->Arg(250);

using OverheadBenchmark = Fixture<OverheadBenchmarkImpl>;

BENCHMARK_DEFINE_F(OverheadBenchmark, push_pop)(::benchmark::State& state) {
    for (auto sample : state) {
        this->impl->run_push_pop();
    }
}
BENCHMARK_REGISTER_F(OverheadBenchmark, push_pop);

/**
 * This benchmark reports 3 measurements:
 *  1. The time it takes to run the start-stop-elapsed sequence.
 *  2. The mean of the reading of elapsed time (through a custom counter).
 *  3. The standard deviation of this reading (through a custom counter as well).
 */
BENCHMARK_DEFINE_F(OverheadBenchmark, timer)(::benchmark::State& state)
{
    const auto num_iters = std::distance(state.begin(), state.end());

    std::vector<double> reported_elapsed(num_iters, 0.);

    size_t iter = 0;

    for (auto sample : state) {
        reported_elapsed.at(iter++) = this->impl->run_timer().count();
    }
    const double reported_elapsed_mean = std::accumulate(reported_elapsed.cbegin(), reported_elapsed.cend(), double(0.), std::plus{}) / num_iters;
    const double reported_elapsed_std  = std::sqrt(std::accumulate(reported_elapsed.cbegin(), reported_elapsed.cend(), double(0.), [&](const double current, const double value){
        return current + std::pow(value - reported_elapsed_mean, 2);
    }) / num_iters);

    state.counters["reported-mean"] = ::benchmark::Counter(reported_elapsed_mean, ::benchmark::Counter::Flags::kDefaults);
    state.counters["reported-std" ] = ::benchmark::Counter(reported_elapsed_std,  ::benchmark::Counter::Flags::kDefaults);
}
BENCHMARK_REGISTER_F(OverheadBenchmark, timer);

} // namespace Kokkos::utils::benchmarks::callbacks

int main(int argc, char** argv)
{
    ::benchmark::MaybeReenterWithoutASLR(argc, argv);

    //! Instruct the tools to avoid global fences if possible.
    Kokkos::Tools::Experimental::set_request_tool_settings_callback(
        [](const uint32_t, Kokkos::Tools::Experimental::ToolSettings* settings) {
            settings->requires_global_fencing = false;
        });

    Kokkos::initialize(argc, argv);

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    Kokkos::finalize();

    return EXIT_SUCCESS;
}
