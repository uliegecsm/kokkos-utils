#ifndef KOKKOS_UTILS_TESTS_SCOPED_EXECUTIONSPACE_HPP
#define KOKKOS_UTILS_TESTS_SCOPED_EXECUTIONSPACE_HPP

namespace Kokkos::utils::tests::scoped
{

//! Create a new execution space instance with RAII semantics.
template <Kokkos::ExecutionSpace Exec>
struct [[nodiscard]] ExecutionSpace
{
    ExecutionSpace() : exec(Kokkos::Experimental::partition_space(Exec{}, 1)[0]) {}

    ExecutionSpace& operator=(const ExecutionSpace&) = delete;
    ExecutionSpace(const ExecutionSpace&)            = delete;

    Exec exec;
};

} // namespace Kokkos::utils::tests::scoped

#endif // KOKKOS_UTILS_TESTS_SCOPED_EXECUTIONSPACE_HPP
