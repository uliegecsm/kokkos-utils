#--- Find GTest.
string(JSON GTest_REQUIRED_VERSION GET "${VERSION_JSON}" dependencies googletest value)

find_package(
    GTest
    ${GTest_REQUIRED_VERSION}
    CONFIG
    REQUIRED
)

#--- Function to add a test.
function(add_one_test)

    cmake_parse_arguments(
        aot_args
        ""
        "TEST_NAME"
        ""
        ${ARGN}
    )

    # Set the test name, executable name and primary source file.
    set(TEST_NAME       "test_${aot_args_TEST_NAME}")
    set(EXECUTABLE_NAME "test_${aot_args_TEST_NAME}")
    cmake_path(SET SOURCE_FILE "test_${aot_args_TEST_NAME}.cpp")
    cmake_path(ABSOLUTE_PATH SOURCE_FILE)

    # Add source file to the documentation list.
    set_property(GLOBAL APPEND PROPERTY KokkosUtils_FILES_FOR_DOC ${SOURCE_FILE})

    # Create test executable.
    add_executable(${EXECUTABLE_NAME})

    # Add the source file to the executable.
    target_sources(
        ${EXECUTABLE_NAME}
        PRIVATE
            ${SOURCE_FILE}
    )

    # Link the executable to the library, GTest and Kokkos.
    target_link_libraries(
        ${EXECUTABLE_NAME}
        PRIVATE
            Kokkos::utils
            GTest::gtest_main
            Kokkos::kokkoscore
    )

    # Add the test to CTest.
    add_test(
        NAME    ${EXECUTABLE_NAME}
        COMMAND $<TARGET_FILE:${EXECUTABLE_NAME}>
    )

endfunction()

### TESTS : concepts ###
add_subdirectory(concepts)
