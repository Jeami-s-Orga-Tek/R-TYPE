#!/usr/bin/env bash
set -e

if [[ "$RUNNER_OS" == "Linux" ]]; then
  sudo apt-get update && sudo apt-get install -y ninja-build cmake ccache
elif [[ "$RUNNER_OS" == "Windows" ]]; then
  choco install ninja cmake -y
fi

if [ ! -d "vcpkg" ]; then
  git clone https://github.com/microsoft/vcpkg.git
  ./vcpkg/bootstrap-vcpkg.sh
fi

./vcpkg/vcpkg install --clean-after-build
