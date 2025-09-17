#!/usr/bin/env bash
set -e

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    if command -v choco &> /dev/null; then
        choco install -y ninja cmake
    fi
fi

if [ ! -d "vcpkg" ]; then
    git clone https://github.com/microsoft/vcpkg.git vcpkg
fi

if [[ "$OSTYPE" == "linux-gnu"* || "$OSTYPE" == "darwin"* ]]; then
    ./vcpkg/bootstrap-vcpkg.sh
elif [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "win32" ]]; then
    ./vcpkg/bootstrap-vcpkg.bat
fi

./vcpkg/vcpkg install
