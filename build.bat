@echo off
mkdir build
cd build
cmake ../ -DVST_SDK_375_SRC_DIR=C:/repos/vst-sdk_3.7.5/VST_SDK/vst3sdk -DVST_SDK_375_BUILD_DIR=C:/repos/vst-sdk_3.7.5/VST_SDK/vst3sdk/build
cd ..
set /p wait=Press return...