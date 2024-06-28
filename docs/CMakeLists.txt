#---- Find Doxygen.
string(JSON Doxygen_REQUIRED_VERSION GET "${VERSION_JSON}" dependencies doxygen value)

find_package(
    Doxygen
    ${Doxygen_REQUIRED_VERSION}
    REQUIRED
)

#---- Fetch Kokkos tag file. Note that the Kokkos project does not generate a tag file, but Trilinos does.
set(KOKKOS_TAG_FILE     "${CMAKE_CURRENT_BINARY_DIR}/Kokkos.tag")
set(KOKKOS_TAG_FILE_URL "https://docs.trilinos.org/dev/packages/common/tag_files/kokkos.tag")
message(STATUS "Fetching Kokkos tag file with ${KOKKOS_TAG_FILE_URL} and storing it to ${KOKKOS_TAG_FILE}.")
file(DOWNLOAD ${KOKKOS_TAG_FILE_URL} ${KOKKOS_TAG_FILE} STATUS fetch_kokkos_tag_file_status)
list(GET fetch_kokkos_tag_file_status 0 fetch_kokkos_tag_file_status_return_code)
if(NOT fetch_kokkos_tag_file_status_return_code EQUAL 0)
    message(FATAL_ERROR "An error occurred while fetching tag file for package Kokkos with ${KOKKOS_TAG_FILE_URL} (${fetch_kokkos_tag_file_status}).")
endif()

#---- Our 'homepage' is the README file.
set(HOMEPAGE_MD ${CMAKE_SOURCE_DIR}/README.md)

#---- Doxygen settings.
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${HOMEPAGE_MD})
set(DOXYGEN_TIMESTAMP YES)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_CURRENT_BINARY_DIR}/html/${CMAKE_PROJECT_NAME}.tag")
set(DOXYGEN_TAGFILES "${KOKKOS_TAG_FILE}")

#---- Extract files for documentation.
get_target_property(KokkosUtils_TARGET_FILES_FOR_DOC KokkosUtils INTERFACE_SOURCES)
get_property(KokkosUtils_FILES_FOR_DOC GLOBAL PROPERTY KokkosUtils_FILES_FOR_DOC)

#---- Add Doxygen as a target.
#     See also https://cmake.org/cmake/help/latest/module/FindDoxygen.html.
doxygen_add_docs(
    docs

    # Homepage
    ${HOMEPAGE_MD}

    # Headers from our KokkosUtils library target
    ${KokkosUtils_TARGET_FILES_FOR_DOC}

    # Files added to the global property KokkosUtils_FILES_FOR_DOC
    ${KokkosUtils_FILES_FOR_DOC}

    # Files NOT added to the global property KokkosUtils_FILES_FOR_DOC
    ${CMAKE_SOURCE_DIR}/docs/tests.dox
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
)
