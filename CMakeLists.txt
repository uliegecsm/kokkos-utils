cmake_minimum_required(VERSION 3.23)

#---- Read the version file.
file(READ version.json VERSION_JSON)

#---- Get the version of kokkos-utils.
string(JSON KOKKOS_UTILS_VERSION GET "${VERSION_JSON}" "kokkos-utils" "value")

#---- Define the project. It uses C++ only.
project(
    kokkos-utils
    VERSION ${KOKKOS_UTILS_VERSION}
    LANGUAGES CXX
)

#---- Options.
option(KokkosUtils_ENABLE_TESTS      "Enable testing" ON)
option(KokkosUtils_ENABLE_DOC        "Enable documentation" ON)
option(KokkosUtils_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

#---- Enable further debugging by exporting compile commands.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#---- Global property that helps us keep track of files that will automatically be added
#     to our Doxygen documentation.
define_property(GLOBAL PROPERTY KokkosUtils_FILES_FOR_DOC
    BRIEF_DOCS "Files that will be added to the Doxygen documentation."
)

#---- Find Kokkos.
#
# Currently, we require the Kokkos::kokkoscore target. Other targets will be added as needed.
string(JSON Kokkos_REQUIRED_VERSION  GET "${VERSION_JSON}" dependencies kokkos value)

if(NOT TARGET Kokkos::kokkoscore)
    find_package(
        Kokkos
        ${Kokkos_REQUIRED_VERSION}
        CONFIG
        REQUIRED
    )
else()
    if(DEFINED Kokkos_VERSION)
        if(NOT Kokkos_VERSION VERSION_GREATER_EQUAL Kokkos_REQUIRED_VERSION)
            message(FATAL_ERROR "Kokkos target(s) already set, but version not at least ${Kokkos_REQUIRED_VERSION}.")
        endif()
    else()
        message(WARNING "Kokkos target(s) already set, but could not check version.")
    endif()
endif()

kokkos_check(DEVICES "${EXPECTED_Kokkos_DEVICES}")

#---- Build the Kokkos::utils library.
add_library(kokkosutils INTERFACE)
add_library(Kokkos::utils ALIAS kokkosutils)

target_sources(
    kokkosutils
    INTERFACE
        FILE_SET headers
        TYPE HEADERS
        BASE_DIRS include/kokkos-utils
        FILES
            include/kokkos-utils/atomics/InsertOp.hpp

            include/kokkos-utils/callbacks/EventInProfileSectionMatcher.hpp
            include/kokkos-utils/callbacks/EventInRegionMatcher.hpp
            include/kokkos-utils/callbacks/EventNameMatcher.hpp
            include/kokkos-utils/callbacks/EventRegexMatcher.hpp
            include/kokkos-utils/callbacks/Events.hpp
            include/kokkos-utils/callbacks/Helpers.hpp
            include/kokkos-utils/callbacks/Listener.hpp
            include/kokkos-utils/callbacks/Manager.hpp
            include/kokkos-utils/callbacks/Matcher.hpp
            include/kokkos-utils/callbacks/RecorderListener.hpp

            include/kokkos-utils/concepts/DualView.hpp
            include/kokkos-utils/concepts/ExecutionSpace.hpp
            include/kokkos-utils/concepts/MemorySpace.hpp
            include/kokkos-utils/concepts/Space.hpp
            include/kokkos-utils/concepts/View.hpp

            include/kokkos-utils/impl/type_list.hpp
            include/kokkos-utils/impl/type_traits.hpp

            include/kokkos-utils/timer/Duration.hpp
            include/kokkos-utils/timer/Event.hpp
            include/kokkos-utils/timer/Timer.hpp
            include/kokkos-utils/timer/Cuda/Event.hpp
            include/kokkos-utils/timer/HIP/Event.hpp

            include/kokkos-utils/view/extents.hpp
            include/kokkos-utils/view/slice.hpp
)

target_include_directories(
    kokkosutils
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
)

target_compile_features(
    kokkosutils
    INTERFACE
        cxx_std_20
)

target_compile_definitions(
    kokkosutils
    INTERFACE
        KOKKOS_IMPL_PUBLIC_INCLUDE
)

target_link_libraries(
    kokkosutils
    INTERFACE
        Kokkos::kokkoscore
)

#---- Setup clang-tidy if needed.
if(KokkosUtils_ENABLE_CLANG_TIDY)
    configure_file(${CMAKE_SOURCE_DIR}/.clang-tidy ${CMAKE_BINARY_DIR}/.clang-tidy @ONLY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy" "--config-file=${CMAKE_BINARY_DIR}/.clang-tidy")
endif()

#---- Install.
install(TARGETS kokkosutils EXPORT KokkosUtilsConfig FILE_SET headers DESTINATION include/kokkos-utils)
install(EXPORT KokkosUtilsConfig DESTINATION lib/cmake/KokkosUtils NAMESPACE Kokkos::)

#---- Testing.
if(KokkosUtils_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

#---- Documentation.
if(KokkosUtils_ENABLE_DOC)
    add_subdirectory(docs)
endif()
