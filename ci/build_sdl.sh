#!/usr/bin/env bash
set -e
BUILD_DIR=build

cmake -S . -B $BUILD_DIR -G Ninja -DCMAKE_BUILD_TYPE=Release -DRENDERER=SDL -DAUDIOPLAYER=SDL
cmake --build $BUILD_DIR --parallel
