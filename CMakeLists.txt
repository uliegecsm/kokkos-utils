cmake_minimum_required(VERSION 3.12)

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

#---- C++ standard that we need (at least).
set(CMAKE_CXX_STANDARD          20   CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED True CACHE BOOL "" FORCE)

#---- Find Kokkos.
find_package(
    Kokkos
    ${KOKKOS_VERSION}
    REQUIRED
)

#---- Find Google Test.
find_package(
    GTest
    ${GOOGLETEST_VERSION}
    CONFIG
    REQUIRED
)

#---- Build the kokkos::utils library.
add_library(kokkos_utils INTERFACE)
add_library(kokkos::utils ALIAS kokkos_utils)

target_sources(
    kokkos_utils
    INTERFACE
        FILE_SET concepts
            TYPE HEADERS
            BASE_DIRS include
            FILES include/concepts/View.hpp
)

target_include_directories(kokkos_utils INTERFACE "include")

target_compile_definitions(kokkos_utils INTERFACE KOKKOS_IMPL_PUBLIC_INCLUDE)

#---- Testing.
enable_testing()
add_subdirectory(tests)

#---- Documentation.
add_subdirectory(docs)
