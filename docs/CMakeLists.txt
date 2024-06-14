#--- Find Doxygen.
string(JSON Doxygen_REQUIRED_VERSION GET "${VERSION_JSON}" dependencies doxygen value)

find_package(
    Doxygen
    ${Doxygen_REQUIRED_VERSION}
    REQUIRED
)

#---- Our 'homepage' is the README file.
set(HOMEPAGE_MD ${CMAKE_SOURCE_DIR}/README.md)

#---- Doxygen settings.
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${HOMEPAGE_MD})
set(DOXYGEN_TIMESTAMP YES)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_CURRENT_BINARY_DIR}/html/${CMAKE_PROJECT_NAME}.tag")

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
)
