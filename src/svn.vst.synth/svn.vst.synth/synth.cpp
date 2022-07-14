#include <svn.vst.synth/synth.hpp>
#include <svn.synth/dsp/synthesizer.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.vst.base/support/bootstrap.hpp>

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace svn::base;
using namespace svn::synth;

// Bindings to vst base project.
extern "C" {
   
topology_info const*
svn_vst_create_topology()
{ return topology_info::create(
    part_descriptors, part_type::count, synth_polyphony, 
    synth_max_ui_height, synth_ui_start_color, synth_ui_end_color); 
}
      
audio_processor*
svn_vst_create_audio_processor(
  topology_info const* topology, float sample_rate, 
  std::int32_t max_sample_count, param_value* state)
{ return new synthesizer(topology, sample_rate, max_sample_count, state); }

Steinberg::FUID const svn_vst_processor_id(0xFBFCEDA8, 0x782047CE, 0xA12E8A8C, 0x8C3407E9);
Steinberg::FUID const svn_vst_controller_id(0x57068B2B, 0x63374143, 0x85FA79D9, 0xAC8A38A5);

} // extern "C"

// Bindings to vst3 sdk. 
BEGIN_FACTORY_DEF(SVN_VST_SYNTH_COMPANY_NAME, SVN_VST_SYNTH_COMPANY_WEB, SVN_VST_SYNTH_COMPANY_MAIL)
  DEF_CLASS2(INLINE_UID_FROM_FUID(svn_vst_processor_id), 
    PClassInfo::kManyInstances, kVstAudioEffectClass, SVN_VST_SYNTH_NAME, 
    Steinberg::Vst::kDistributable, SVN_VST_SYNTH_CATEGORY, SVN_VST_SYNTH_VERSION, 
    kVstVersionString, svn_vst_processor_factory)
  DEF_CLASS2(INLINE_UID_FROM_FUID(svn_vst_controller_id), 
    PClassInfo::kManyInstances, kVstComponentControllerClass, 
    SVN_VST_SYNTH_CONTROLLER_NAME, 0, "", SVN_VST_SYNTH_VERSION,
    kVstVersionString, svn_vst_controller_factory)
END_FACTORY