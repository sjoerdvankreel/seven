#ifndef SVN_VST_BASE_SUPPORT_VST_INIT_HPP
#define SVN_VST_BASE_SUPPORT_VST_INIT_HPP

#include <pluginterfaces/base/funknown.h>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/support/param_value.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

#include <memory>
#include <cstdint>

namespace svn::vst::base {

Steinberg::FUnknown*
processor_factory(void* context);
Steinberg::FUnknown*
controller_factory(void* context);

extern Steinberg::FUID 
init_get_controller_id();
extern std::unique_ptr<svn::base::runtime_topology>
init_create_topology();
extern std::unique_ptr<svn::base::audio_processor>
init_create_audio_processor(
  svn::base::runtime_topology const* topology, float sample_rate, 
  std::int32_t max_sample_count, svn::base::param_value* state);

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SUPPORT_VST_INIT_HPP