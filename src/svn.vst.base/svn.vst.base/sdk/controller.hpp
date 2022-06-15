#ifndef SVN_VST_BASE_SDK_CONTROLLER_HPP
#define SVN_VST_BASE_SDK_CONTROLLER_HPP

#include <vstgui/plugin-bindings/vst3editor.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <svn.base/topology/topology_info.hpp>
#include <svn.vst.base/sdk/editor.hpp>

namespace svn::vst::base {

// Vst edit controller dynamically generated from topology_info.
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
  svn::base::topology_info const* const _topology;

public:
  tresult endEdit(ParamID tag) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(char const* name) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;

  // Update the editor ui if dependent params change.
  void sync_dependent_parameters();
  controller(svn::base::topology_info const* topology) : _topology(topology) {}
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_CONTROLLER_HPP
