#include <svn.vst.synth/synth.hpp>
#include <svn.synth/dsp/synthesizer.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.vst.base/support/bootstrap.hpp>

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

namespace svn::vst::synth {
#if NDEBUG
static Steinberg::FUID const processor_id(0xE576A3A3, 0xB01240E0, 0x9FB6D08A, 0x3F4538AD);
static Steinberg::FUID const controller_id(0xF0A5357D, 0x1EA74ABC, 0x903F3616, 0x496F1000);
#else
static Steinberg::FUID const processor_id(0xFBFCEDA8, 0x782047CE, 0xA12E8A8C, 0x8C3407E9);
static Steinberg::FUID const controller_id(0x57068B2B, 0x63374143, 0x85FA79D9, 0xAC8A38A5);
#endif // NDEBUG
} // namespace svn::vst::synth

namespace svn::vst::base { 
 
using namespace svn::base;
using namespace svn::synth;
using namespace svn::vst::synth;

Steinberg::FUID 
init_get_controller_id()
{ return synth::controller_id; } 

topology_info
init_create_topology()
{ 
  return topology_info::create(
    part_descriptors, part_type::count, 
    synth_polyphony, synth_max_ui_height);
}
 
std::unique_ptr<svn::base::audio_processor>
init_create_audio_processor(
  topology_info const* topology, float sample_rate,
  std::int32_t max_sample_count, param_value* state)
{ return std::make_unique<synthesizer>(topology, sample_rate, max_sample_count, state); }

} // namespace svn::vst::base

BEGIN_FACTORY_DEF(SVN_VST_SYNTH_COMPANY_NAME, SVN_VST_SYNTH_COMPANY_WEB, SVN_VST_SYNTH_COMPANY_MAIL)
  DEF_CLASS2(INLINE_UID_FROM_FUID(svn::vst::synth::processor_id), 
    PClassInfo::kManyInstances, kVstAudioEffectClass, SVN_VST_SYNTH_NAME, 
    Steinberg::Vst::kDistributable, SVN_VST_SYNTH_CATEGORY, SVN_VST_SYNTH_VERSION, 
    kVstVersionString, svn::vst::base::processor_factory)
  DEF_CLASS2(INLINE_UID_FROM_FUID(svn::vst::synth::controller_id), 
    PClassInfo::kManyInstances, kVstComponentControllerClass, 
    SVN_VST_SYNTH_CONTROLLER_NAME, 0, "", SVN_VST_SYNTH_VERSION,
    kVstVersionString, svn::vst::base::controller_factory)
END_FACTORY
