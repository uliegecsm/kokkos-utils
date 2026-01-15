#ifndef KOKKOS_UTILS_TESTS_CALLBACKS_TESTWORKLOAD_HPP
#define KOKKOS_UTILS_TESTS_CALLBACKS_TESTWORKLOAD_HPP

#include "Kokkos_Core.hpp"
#include "Kokkos_Profiling_ProfileSection.hpp"
#include "Kokkos_Profiling_ScopedRegion.hpp"

namespace Kokkos::utils::tests::callbacks
{

template <Kokkos::ExecutionSpace Exec>
struct MyWorkload
{
    struct MyFunctor
    {
        template <std::integral T>
        KOKKOS_FUNCTION
        void operator()(const T) const {}
    };

    void execute(const Exec& exec) const
    {
        Kokkos::Profiling::ProfilingSection profile_section("profile section");

        profile_section.start();

        {
            const Kokkos::Profiling::ScopedRegion guard_level_0("computation - level 0");

            Kokkos::parallel_for(
                "computation - level 0 - pfor",
                Kokkos::RangePolicy(exec, 0, 1),
                MyFunctor{}
            );
            exec.fence("computation - level 0 - fence after pfor");

            Kokkos::Profiling::markEvent("buried marker");

            {
                const Kokkos::Profiling::ScopedRegion guard_level_1("computation - level 1");

                // Another kernel, a parallel reduce on the default execution space instance.
                double my_result;
                Kokkos::parallel_reduce(
                    "computation - level 1 - preduce on default exec",
                    Kokkos::RangePolicy(0, 1),
                    KOKKOS_LAMBDA (const typename Exec::size_type, const double&) {},
                    my_result
                );
            }
        }

        profile_section.stop();

        exec.fence("other fence after stopping the profile section");
    }
};

} // namespace Kokkos::utils::tests::callbacks

#endif // KOKKOS_UTILS_TESTS_CALLBACKS_TESTWORKLOAD_HPP
