cmake_minimum_required(VERSION 3.23)

#---- Read the version file.
file(READ version.json VERSION_JSON)

string(JSON KOKKOS_UTILS_VERSION GET "${VERSION_JSON}" "kokkos-utils" "value")

string(JSON DOXYGEN_VERSION      GET "${VERSION_JSON}" dependencies doxygen    value)
string(JSON GOOGLETEST_VERSION   GET "${VERSION_JSON}" dependencies googletest value)
string(JSON KOKKOS_VERSION       GET "${VERSION_JSON}" dependencies kokkos     value)

#---- Define the project. It uses C++ only.
project(
    kokkos-utils
    VERSION ${KOKKOS_UTILS_VERSION}
    LANGUAGES CXX
)

#---- Options.
option(KOKKOSUTILS_ENABLE_TESTS "Enable testing" ON)
option(KOKKOSUTILS_ENABLE_DOC   "Enable documentation" ON)

#---- Find Kokkos.
#
# Currently, we require the Kokkos::kokkoscore target. Other targets will be added as needed.
if(NOT TARGET Kokkos::kokkoscore)
    find_package(
        Kokkos
        ${KOKKOS_VERSION}
        CONFIG
        REQUIRED
    )
endif()
if(NOT TARGET Kokkos::kokkoscore)
    message(FATAL_ERROR "Kokkos should define a Kokkos::kokkoscore target.")
endif()

#---- Find Google Test.
#
# Currently, we require the GTest::gtest_main target. Other targets will be added as needed.
if(NOT TARGET GTest::gtest_main)
    find_package(
        GTest
        ${GOOGLETEST_VERSION}
        CONFIG
        REQUIRED
    )
endif()
if(NOT TARGET GTest::gtest_main)
    message(FATAL_ERROR "Googletest should define a GTest::gtest_main target.")
endif()

#---- Documentation.
if(KOKKOSUTILS_ENABLE_DOC)
    add_subdirectory(docs)
endif()

#---- Build the Kokkos::utils library.
add_library(KokkosUtils INTERFACE)
add_library(Kokkos::utils ALIAS KokkosUtils)

target_sources(
    KokkosUtils
    INTERFACE
        FILE_SET concepts
        TYPE HEADERS
        BASE_DIRS include/kokkos-utils
        FILES include/kokkos-utils/concepts/View.hpp
)

target_include_directories(
    KokkosUtils
    INTERFACE
        "include/"
)

target_compile_features(
    KokkosUtils
    INTERFACE
        cxx_std_20
)

target_compile_definitions(
    KokkosUtils
    INTERFACE
        KOKKOS_IMPL_PUBLIC_INCLUDE
)

#---- Testing.
if(KOKKOSUTILS_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
