* Download and unpack vst375 sdk to /whatever/vstsdk.
* Note: seven cmake project statically links to the vst3 sdk, rather than being build as part of the sdk.
* Build vstsdk into /whatever/vstsdk/vst3sdk/build using cmake.
* Build seven into /whatever/seven/scratch using cmake -DVST_SDK_375_SRC_DIR=/whatever/vstsdk/vst3sdk -DVST_SDK_375_BUILD_DIR=/whatever/vstsdk/vst3sdk/build.
* Generate ui description from vst plugin dll (see uigen).