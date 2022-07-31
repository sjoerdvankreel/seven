#!/bin/bash
set -e

mkdir -p build
cd build
mkdir -p linux
cd linux

mkdir -p debug
cd debug
cmake ../../../ -DCMAKE_BUILD_TYPE=Debug -DVST_SDK_375_SRC_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk -DVST_SDK_375_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk/build/debug/lib/Debug -DVST_SDK_375_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk/build/release/lib/Release
cd ..

mkdir -p release
cmake ../../../ -DCMAKE_BUILD_TYPE=Debug -DVST_SDK_375_SRC_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk -DVST_SDK_375_BUILD_DEBUG_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk/build/debug/lib/Debug -DVST_SDK_375_BUILD_RELEASE_DIR=/media/sf_vst-sdk_3.7.5/linux/VST_SDK/vst3sdk/build/release/lib/Release
cd release
cd ..

cd ..
cd ..
