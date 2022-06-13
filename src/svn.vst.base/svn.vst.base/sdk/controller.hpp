#ifndef SVN_VST_BASE_SDK_CONTROLLER_HPP
#define SVN_VST_BASE_SDK_CONTROLLER_HPP

#include <pluginterfaces/vst/vsttypes.h>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.vst.base/sdk/editor.hpp>

namespace svn::vst::base {

class controller: 
public Steinberg::Vst::EditControllerEx1, 
public VSTGUI::VST3EditorDelegate
{
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using IPlugView = Steinberg::IPlugView;
  using ParamID = Steinberg::Vst::ParamID;
  
  editor* _editor = nullptr;
  svn::base::runtime_topology const* const _topology;

public:
  controller(svn::base::runtime_topology const* topology);
  tresult endEdit(ParamID tag) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(char const* name) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_CONTROLLER_HPP
