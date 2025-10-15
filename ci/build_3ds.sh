#!/usr/bin/env bash
set -e
BUILD_DIR=build

rm -rf build
# tex3ds -i assets/assets.t3s -d romfs/assets.d -o romfs/assets.t3x
cmake -DCMAKE_TOOLCHAIN_FILE=cmake-support/DevkitArm3DS.cmake -S . -B build
cmake --build $BUILD_DIR
