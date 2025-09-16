FROM fedora:41

RUN dnf -y update && \
    dnf -y install \
        @development-tools \
        cmake \
        ninja-build \
        git \
        python3 \
        python3-pip \
        curl \
        ccache \
        clang clang-tools-extra \
        which \
    && dnf clean all

RUN git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh

ENV PATH="/opt/vcpkg:${PATH}"

ENV CCACHE_DIR=/root/.ccache
ENV CCACHE_MAXSIZE=1G

WORKDIR /workspace
