#!/usr/bin/env bash
set -e

if command -v choco &> /dev/null; then
    choco install -y ninja cmake
fi

if [ ! -d "vcpkg" ]; then
    git clone https://github.com/microsoft/vcpkg.git vcpkg
fi

if [ ! -f "vcpkg/vcpkg" ] && [ ! -f "vcpkg/vcpkg.exe" ]; then
    ./vcpkg/bootstrap-vcpkg.sh || ./vcpkg/bootstrap-vcpkg.bat
fi

./vcpkg/vcpkg install --triplet x64-windows --manifest
