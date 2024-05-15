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

#---- Documentation.
add_subdirectory(docs)
