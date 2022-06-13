#ifndef SVN_VST_BASE_SDK_EDITOR_HPP
#define SVN_VST_BASE_SDK_EDITOR_HPP

#include <svn.base/runtime/runtime_topology.hpp>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <vector>

namespace svn::vst::base {

class editor: 
public VSTGUI::VST3Editor
{
  using CView = VSTGUI::CView;
  using CFrame = VSTGUI::CFrame;
  using CControl = VSTGUI::CControl;
  using ParamID = Steinberg::Vst::ParamID;
  using UTF8StringPtr = VSTGUI::UTF8StringPtr;
  using EditController = Steinberg::Vst::EditController;
  std::vector<std::vector<CControl*>> _controls;
  svn::base::runtime_topology const* const _topology;

public:
  void onViewAdded(CFrame* frame, CView* view) override;
  void onViewRemoved(CFrame* frame, CView* view) override;
  void controllerEndEdit(ParamID tag, std::int32_t value);
  editor(EditController* controller, UTF8StringPtr template_name,
    UTF8StringPtr xml_file, svn::base::runtime_topology const* topology);
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_EDITOR_HPP
