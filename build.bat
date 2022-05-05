@echo off
mkdir build
cd build
cmake ../ -DVST_SDK_374_SRC_DIR=C:/repos/VST_SDK/vst3sdk -DVST_SDK_374_BUILD_DIR=C:/repos/VST_SDK/vst3sdk/build
cd ..
set /p wait=Press return...