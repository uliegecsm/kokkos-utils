set -ex

COMPILER_FAMILY=$1
CXX_MODULES=$2
KOKKOS_UTILS_VERSION=$3

CMAKE_INSTALL_PREFIX=/opt/this-is-our-install-folder-for-test-purposes

echo "> Running find package test with installation expected in ${CMAKE_INSTALL_PREFIX}."

WORK_DIR=`mktemp -d -p "$DIR"`

echo "> Working in ${WORK_DIR}."

cat << EOF > ${WORK_DIR}/CMakeLists.txt
cmake_minimum_required(VERSION 3.23)

project(test LANGUAGES CXX)

find_package(KokkosUtils CONFIG REQUIRED VERSION ${KOKKOS_UTILS_VERSION})

# Once the CMake minimum required version is 3.28 or above, this policy
# is useless. Otherwise, it is necessary to support modules.
# See also:
#   * https://cmake.org/cmake/help/latest/policy/CMP0155.html
if(Kokkos_ENABLE_EXPERIMENTAL_CXX20_MODULES)
    cmake_policy(SET CMP0155 NEW)
endif()

add_executable(test)
target_sources(test PRIVATE test.cpp)
target_link_libraries(test PRIVATE Kokkos::kokkosutils)

# Even though the Kokkos::kokkoscore target modules may probably be inherited transitively
# through Kokkos::kokkosutils, it doesn't for now.
# See also:
#   * https://discourse.cmake.org/t/issues-with-transitive-usage-of-c-modules/15586
if(Kokkos_ENABLE_EXPERIMENTAL_CXX20_MODULES)
    target_link_libraries(test PRIVATE Kokkos::kokkoscore)
endif()
EOF

cat << EOF > ${WORK_DIR}/test.cpp
#include "Kokkos_Macros.hpp"
#ifdef KOKKOS_ENABLE_EXPERIMENTAL_CXX20_MODULES
import kokkos.core;
#else
#include "Kokkos_Core.hpp"
#endif
#include "kokkos-utils/concepts/Space.hpp"

int main()
{
    static_assert(Kokkos::utils::concepts::Space<Kokkos::DefaultExecutionSpace::memory_space>);
    return EXIT_SUCCESS;
}
EOF

cd $WORK_DIR

export KokkosUtils_ROOT=${CMAKE_INSTALL_PREFIX}
echo ${KokkosUtils_ROOT}

if [ "${COMPILER_FAMILY}" = "clang" ];then
    export CXX=clang++
elif [ "${COMPILER_FAMILY}" = "rocm" ];then
    export CXX=hipcc
elif [ "${COMPILER_FAMILY}" = "intel" ];then
    export CXX=icpx
fi

if [ "${CXX_MODULES}" = "ON" ]; then
    CMAKE_CONFIGURE="-GNinja"
else
    CMAKE_CONFIGURE=""
fi

cmake -S . -B build -LA ${CMAKE_CONFIGURE} --warn-uninitialized -Werror=dev

cmake --build build -j4 --verbose

./build/test

echo "> Find package test success."
