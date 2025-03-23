#!/bin/bash
# set -e

cmake_build () {
  ANDROID_ABI=$1
  mkdir -p build-$ANDROID_ABI
  cd build-$ANDROID_ABI
  cmake $GITHUB_WORKSPACE -DANDROID_PLATFORM=24 -DANDROID_ABI=$ANDROID_ABI -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_LATEST_HOME/build/cmake/android.toolchain.cmake
  cmake --build . --config Release --parallel 4
  cmake --build . --config Release --target install
}

cmake_build arm64-v8a
