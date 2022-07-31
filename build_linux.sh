#!/bin/bash
set -e

mkdir -p build
cd build
mkdir -p linux
cd linux
cmake ../../ -DVST_SDK_375_SRC_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk -DVST_SDK_375_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk/build/debug/lib/Debug -DVST_SDK_375_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk/build/release/lib/Release
cd ..
cd ..
