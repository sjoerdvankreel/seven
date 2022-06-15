#ifndef SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP
#define SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP

#include <pluginterfaces/base/funknown.h>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/topology/topology_info.hpp>
#include <svn.vst.base/sdk/processor.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <pluginterfaces/base/ftypes.h>
#include <public.sdk/source/main/pluginfactory.h>
#include <cstdint>

extern "C" {

// These must be provided by the plugin project.
extern Steinberg::FUID const svn_vst_processor_id;
extern Steinberg::FUID const svn_vst_controller_id;
extern svn::base::topology_info const* svn_vst_create_topology();
extern svn::base::audio_processor* svn_vst_create_audio_processor(
  svn::base::topology_info const* topology, float sample_rate, 
  std::int32_t max_sample_count, svn::base::param_value* state);

// These are provided by vst.base and must be referenced by the plugin project.
SMTG_EXPORT_SYMBOL svn::base::topology_info const* svn_vst_get_topology();
inline Steinberg::FUnknown* svn_vst_controller_factory(void* context)
{ return static_cast<Steinberg::Vst::IEditController*>(new svn::vst::base::controller(svn_vst_get_topology())); }
inline Steinberg::FUnknown* svn_vst_processor_factory(void* context)
{ return static_cast<Steinberg::Vst::IAudioProcessor*>(new svn::vst::base::processor(svn_vst_get_topology(), svn_vst_controller_id)); }

} // extern "C"
#endif // SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP