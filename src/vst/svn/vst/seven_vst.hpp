#ifndef SVN_VST_SEVEN_VST_HPP
#define SVN_VST_SEVEN_VST_HPP

#include <pluginterfaces/base/funknown.h>

#define SVN_VST_PLUGIN_VERSION "1.0.0.0"

#if NDEBUG
#define SVN_VST_PLUGIN_NAME "Seven"
#define SVN_VST_ORIGINAL_FILENAME "seven.vst3"
#define SVN_VST_CONTROLLER_NAME "SevenController"
#else
#define SVN_VST_PLUGIN_NAME "Seven (Debug)"
#define SVN_VST_ORIGINAL_FILENAME "seven.debug.vst3"
#define SVN_VST_CONTROLLER_NAME "SevenController (Debug)"
#endif

namespace svn::vst {

#if NDEBUG
static Steinberg::FUID const processor_id(0xE576A3A3, 0xB01240E0, 0x9FB6D08A, 0x3F4538AD);
static Steinberg::FUID const controller_id(0xF0A5357D, 0x1EA74ABC, 0x903F3616, 0x496F1000);
#else
static Steinberg::FUID const processor_id(0xFBFCEDA8, 0x782047CE, 0xA12E8A8C, 0x8C3407E9);
static Steinberg::FUID const controller_id(0x57068B2B, 0x63374143, 0x85FA79D9, 0xAC8A38A5);
#endif

} // namespace svn::vst
#endif // SVN_VST_SEVEN_VST_HPP