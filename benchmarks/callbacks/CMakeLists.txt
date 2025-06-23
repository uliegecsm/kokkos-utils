add_executable(benchmarks_callbacks_RegionTimer)
target_sources(
    benchmarks_callbacks_RegionTimer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_RegionTimer.cpp
)
target_link_libraries(
    benchmarks_callbacks_RegionTimer
    PRIVATE
        benchmark::benchmark
        Kokkos::kokkoscore
        Kokkos::utils
)
add_test(
    NAME benchmarks_callbacks_RegionTimer
    COMMAND $<TARGET_FILE:benchmarks_callbacks_RegionTimer> --benchmark_min_time=1x
)
