#ifndef SVN_VST_BASE_SUPPORT_VST_INIT_HPP
#define SVN_VST_BASE_SUPPORT_VST_INIT_HPP

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <memory>

namespace svn::vst::base {

extern std::unique_ptr<svn::base::audio_processor>
create_audio_processor(
  svn::base::runtime_topology const* topology, 
  Steinberg::Vst::ProcessSetup const& setup);

extern std::unique_ptr<svn::base::runtime_topology>
create_topology();

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SUPPORT_VST_INIT_HPP