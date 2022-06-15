#ifndef SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP
#define SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP

#include <pluginterfaces/base/funknown.h>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <memory>
#include <cstdint>

namespace svn::vst::base {

Steinberg::FUnknown*
processor_factory(void* context);
Steinberg::FUnknown*
controller_factory(void* context);

extern Steinberg::FUID 
init_get_controller_id();
extern svn::base::topology_info init_create_topology();
extern std::unique_ptr<svn::base::audio_processor>
init_create_audio_processor(
  svn::base::topology_info const* topology, float sample_rate,
  std::int32_t max_sample_count, svn::base::param_value* state);

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SUPPORT_BOOTSTRAP_HPP