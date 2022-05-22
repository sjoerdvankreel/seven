#ifndef SVN_VST_SUPPORT_IDS_HPP
#define SVN_VST_SUPPORT_IDS_HPP

#include <pluginterfaces/base/funknown.h>

namespace Svn::Vst {

#if NDEBUG
static Steinberg::FUID const ProcessorId(0xE576A3A3, 0xB01240E0, 0x9FB6D08A, 0x3F4538AD);
static Steinberg::FUID const ControllerId(0xF0A5357D, 0x1EA74ABC, 0x903F3616, 0x496F1000);
#else
static Steinberg::FUID const ProcessorId(0xFBFCEDA8, 0x782047CE, 0xA12E8A8C, 0x8C3407E9);
static Steinberg::FUID const ControllerId(0x57068B2B, 0x63374143, 0x85FA79D9, 0xAC8A38A5);
#endif

} // namespace Svn::Vst
#endif // SVN_VST_SUPPORT_IDS_HPP