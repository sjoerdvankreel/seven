#ifndef SVN_VST_BASE_SDK_CONTROLLER_HPP
#define SVN_VST_BASE_SDK_CONTROLLER_HPP

#include <vstgui/plugin-bindings/vst3editor.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <svn.vst.base/ui/editor.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <vector>

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
  // Separate copy of the parameter state used for graphs.
  std::vector<svn::base::param_value> _state;

private:
  void update_state(std::int32_t param);

public:
  tresult endEdit(ParamID tag) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  IPlugView* PLUGIN_API createView(char const* name) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;
  svn::base::param_value const* state() const { return _state.data(); }

  // Update the editor ui if dependent params change.
  void sync_ui_parameters();
  controller(svn::base::topology_info const* topology) : 
  _topology(topology), _state(topology->params.size()) {}
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_CONTROLLER_HPP
