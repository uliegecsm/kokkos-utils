#ifndef KOKKOS_UTILS_TESTS_SCOPED_EXECUTIONSPACE_HPP
#define KOKKOS_UTILS_TESTS_SCOPED_EXECUTIONSPACE_HPP

#include "Kokkos_Core.hpp"

namespace Kokkos::utils::tests::scoped
{

//! Create a new execution space instance with RAII semantics.
template <Kokkos::ExecutionSpace Exec>
struct [[nodiscard]] ExecutionSpace
{
    ExecutionSpace() : exec(Kokkos::Experimental::partition_space(Exec{}, 1)[0]) {}
    ~ExecutionSpace() { exec.fence(fence_label); };

    ExecutionSpace& operator=(const ExecutionSpace&) = delete;
    ExecutionSpace(const ExecutionSpace&)            = delete;

    Exec exec;

    static inline std::string fence_label = std::format("{}: fence on destruction", Kokkos::Impl::TypeInfo<Exec>::name());
};

} // namespace Kokkos::utils::tests::scoped

#endif // KOKKOS_UTILS_TESTS_SCOPED_EXECUTIONSPACE_HPP
