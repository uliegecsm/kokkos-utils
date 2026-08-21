set -ex

COMPILER_FAMILY=$1
KOKKOS_UTILS_VERSION=$2

CMAKE_INSTALL_PREFIX=/opt/this-is-our-install-folder-for-test-purposes

echo "> Running find package test with installation expected in ${CMAKE_INSTALL_PREFIX}."

WORK_DIR=`mktemp -d -p "$DIR"`

echo "> Working in ${WORK_DIR}."

cat << EOF > ${WORK_DIR}/CMakeLists.txt
cmake_minimum_required(VERSION 3.23)

project(test LANGUAGES CXX)

find_package(KokkosUtils CONFIG REQUIRED VERSION ${KOKKOS_UTILS_VERSION})

add_executable(test)
target_sources(test PRIVATE test.cpp)
target_link_libraries(test PRIVATE Kokkos::kokkosutils)
EOF

cat << EOF > ${WORK_DIR}/test.cpp
#include "Kokkos_Core.hpp"
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

cmake -S . -B build --warn-uninitialized -Werror=dev

cmake --build build -j4 --verbose

./build/test

echo "> Find package test success."
