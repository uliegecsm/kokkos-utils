#--- Find Doxygen.
if(NOT TARGET Doxygen::doxygen)
    find_package(
        Doxygen
        ${DOXYGEN_VERSION}
        REQUIRED
    )
endif()

#---- Our 'homepage' is the README file.
set(HOMEPAGE_MD ${CMAKE_SOURCE_DIR}/README.md)

#---- Doxygen settings.
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${HOMEPAGE_MD})
set(DOXYGEN_TIMESTAMP YES)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_SOURCE_BROWSER YES)

#---- Add Doxygen as a target.
#     See also https://cmake.org/cmake/help/latest/module/FindDoxygen.html.
doxygen_add_docs(
    docs
    ${PROJECT_SOURCE_DIR}
)
