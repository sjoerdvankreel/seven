#ifndef SVN_VST_BASE_UI_EDITOR_HPP
#define SVN_VST_BASE_UI_EDITOR_HPP

#include <svn.base/topology/topology_info.hpp>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <svn.vst.base/ui/graph_plot.hpp>
#include <vector>

namespace svn::vst::base {

// Vst3 editor with basic support for declarative parameter 
// visibility. See param_ui_descriptor.relevant_if_param.
// Json files (.uidesc) are generated by ui generator project.
// Note: this class breaks the vst3 gui editor. Remove 
// onViewAdded/onViewRemoved/controllerEndEdit if you need
// to play around with the editor. Conditional visibility 
// obviously won't work then.
class editor: 
public VSTGUI::VST3Editor
{
  using CView = VSTGUI::CView;
  using CFrame = VSTGUI::CFrame;
  using CControl = VSTGUI::CControl;
  using ParamID = Steinberg::Vst::ParamID;
  using PlatformType = VSTGUI::PlatformType;
  using UTF8StringPtr = VSTGUI::UTF8StringPtr;
  using EditController = Steinberg::Vst::EditController;
  
  std::vector<graph_plot*> _graphs;
  std::vector<std::vector<CControl*>> _controls;
  svn::base::topology_info const* const _topology;

public:
  // Keeping track of graphs and controls by tag for fast access.
  void onViewAdded(CFrame* frame, CView* view) override;
  void onViewRemoved(CFrame* frame, CView* view) override;

  // Update visibility of dependent views and rerender graphs.
  void controllerEndEdit(ParamID tag);
  bool PLUGIN_API open(void* parent, const PlatformType& type) override;

  editor(EditController* controller, UTF8StringPtr template_name,
    UTF8StringPtr xml_file, svn::base::topology_info const* topology);
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_EDITOR_HPP