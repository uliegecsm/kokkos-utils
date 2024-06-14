FROM ubuntu:24.04 as apt-requirements

# Install APT requirements.
RUN --mount=target=/requirements,type=bind,source=requirements <<EOF

    set -ex

    apt update

    apt --yes --no-install-recommends install $(grep -vE "^\s*#" /requirements/requirements.system.txt  | tr "\n" " ")

    apt clean && rm -rf /var/lib/apt/lists/*

EOF

# Compile Doxygen.
FROM apt-requirements as compile-doxygen

ARG DOXYGEN_VERSION

RUN <<EOF

    set -ex

    apt update

    apt --yes --no-install-recommends install flex bison python3

    git clone --depth=1 --branch Release_$(echo ${DOXYGEN_VERSION} | tr . _) https://github.com/doxygen/doxygen doxygen

    cd doxygen

    cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/opt/doxygen-${DOXYGEN_VERSION}

    cmake --build build -j4

    cmake --build build --target install

EOF

# Compile Google Test.
FROM apt-requirements as compile-google-test

ARG GOOGLETEST_VERSION

RUN <<EOF

    set -ex

    git clone --depth=1 --branch=v${GOOGLETEST_VERSION} https://github.com/google/googletest googletest

    cd googletest

    cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/opt/google-test-${GOOGLETEST_VERSION}

    cmake --build build -j4

    cmake --build build --target install

EOF

# Compile Kokkos.
FROM apt-requirements as compile-kokkos

ARG KOKKOS_VERSION
ARG KOKKOS_PRESET

RUN --mount=target=/cmake/presets,type=bind,source=cmake/presets <<EOF

    set -ex

    git clone --depth=1 --branch=${KOKKOS_VERSION} https://github.com/kokkos/kokkos kokkos

    cd kokkos

    cp /cmake/presets/kokkos.json CMakePresets.json

    cmake -S . --preset=${KOKKOS_PRESET} -DCMAKE_INSTALL_PREFIX=/opt/kokkos-${KOKKOS_VERSION}/${KOKKOS_PRESET}

    cmake --build --preset=${KOKKOS_PRESET} -j4

    cmake --build --preset=${KOKKOS_PRESET} --target=install

EOF

# Build the final image from previous stages.
FROM apt-requirements as final

ARG DOXYGEN_VERSION
ARG GOOGLETEST_VERSION
ARG KOKKOS_VERSION
ARG KOKKOS_PRESET

COPY --from=compile-doxygen     /opt/doxygen-${DOXYGEN_VERSION}                /opt/doxygen-${DOXYGEN_VERSION}
COPY --from=compile-google-test /opt/google-test-${GOOGLETEST_VERSION}         /opt/google-test-${GOOGLETEST_VERSION}
COPY --from=compile-kokkos      /opt/kokkos-${KOKKOS_VERSION}/${KOKKOS_PRESET} /opt/kokkos-${KOKKOS_VERSION}/${KOKKOS_PRESET}

ENV Doxygen_ROOT=/opt/doxygen-${DOXYGEN_VERSION}
ENV GTest_ROOT=/opt/google-test-${GOOGLETEST_VERSION}
ENV Kokkos_ROOT=/opt/kokkos-${KOKKOS_VERSION}/${KOKKOS_PRESET}
