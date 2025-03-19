set -ex

CMAKE_PRESET=$1
CMAKE_INSTALL_PREFIX=/opt/this-is-our-install-folder-for-test-purposes

echo "> Running install test for CMake preset ${CMAKE_PRESET}. Installation is expected to happen in ${CMAKE_INSTALL_PREFIX}."

cmake --install build-with-${CMAKE_PRESET} --prefix ${CMAKE_INSTALL_PREFIX}

test -d ${CMAKE_INSTALL_PREFIX}

test -f ${CMAKE_INSTALL_PREFIX}/include/kokkos-utils/view/slice.hpp
test -f ${CMAKE_INSTALL_PREFIX}/lib/cmake/KokkosUtils/KokkosUtilsConfig.cmake

echo "> Install test success."
