add_executable(test_concepts_View test_View.cpp)
target_link_libraries(test_concepts_View PRIVATE kokkos::utils GTest::gtest_main Kokkos::kokkoscore)
add_test(NAME test_concepts_View COMMAND $<TARGET_FILE:test_concepts_View>)