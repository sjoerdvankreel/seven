#ifndef SVN_VST_BASE_SDK_CONTROLLER_HPP
#define SVN_VST_BASE_SDK_CONTROLLER_HPP

#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <svn.base/runtime/runtime_topology.hpp>

namespace svn::vst::base {

class controller: 
public Steinberg::Vst::EditControllerEx1
{
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  svn::base::runtime_topology const* const _topology;

public:
  controller(svn::base::runtime_topology const* topology);
  tresult PLUGIN_API initialize(FUnknown* context) override;
	tresult PLUGIN_API setComponentState(IBStream* state) override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_CONTROLLER_HPP
