#!/bin/bash
set -e

mkdir -p build
cd build
mkdir -p linux
cd linux
cmake ../../ -DVST_SDK_375_SRC_DIR=/media/sf_vst-sdk_3.7.5/VST_SDK/vst3sdk -DVST_SDK_375_BUILD_DIR=/media/sf_vst-sdk_3.7.5/VST_SDK/vst3sdk/build
cd ..
cd ..
