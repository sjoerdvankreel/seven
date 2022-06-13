#ifndef SVN_VST_BASE_SDK_EDITOR_HPP
#define SVN_VST_BASE_SDK_EDITOR_HPP

#include <vstgui/plugin-bindings/vst3editor.h>
#include <vector>

namespace svn::vst::base {

class editor: 
public VSTGUI::VST3Editor
{
  using CView = VSTGUI::CView;
  using CFrame = VSTGUI::CFrame;
  using CControl = VSTGUI::CControl;
  using UTF8StringPtr = VSTGUI::UTF8StringPtr;
  using EditController = Steinberg::Vst::EditController;
  std::vector<std::vector<CControl*>> _controls;

public:
  void onViewAdded(CFrame* frame, CView* view) override;
  void onViewRemoved(CFrame* frame, CView* view) override;
  editor(EditController* controller, UTF8StringPtr template_name, 
    UTF8StringPtr xml_file, std::size_t param_count);
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_EDITOR_HPP
