#ifndef SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP
#define SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP

#include <pluginterfaces/base/funknown.h>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <memory>
#include <cstdint>

// These must be provided by the plugin project.
extern "C" {
extern Steinberg::FUID const svn_vst_processor_id;
extern Steinberg::FUID const svn_vst_controller_id;
extern svn::base::topology_info const* svn_vst_create_topology();
extern svn::base::audio_processor* svn_vst_create_audio_processor(
  svn::base::topology_info const* topology, float sample_rate, 
  std::int32_t max_sample_count, svn::base::param_value* state);
} // extern "C"

namespace svn::vst::base {

Steinberg::FUnknown*
processor_factory(void* context);
Steinberg::FUnknown*
controller_factory(void* context);

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP