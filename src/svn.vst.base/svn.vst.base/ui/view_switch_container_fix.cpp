#include <svn.vst.base/ui/editor.hpp>
#include <svn.vst.base/ui/view_switch_container_fix.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

using namespace VSTGUI;
using namespace VSTGUI::UIViewCreator;

namespace svn::vst::base {

CView* 
view_switch_container_fix_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ 
  auto result = new view_switch_container_fix();
  new view_switch_controller_fix(result, desc, desc->getController());
  return result;
}

bool 
view_switch_container_fix_creator::apply(
  CView* view, UIAttributes const& attrs, IUIDescription const* desc) const
{
  CPoint offset;
  if(!UIViewSwitchContainerCreator::apply(view, attrs, desc)) return false;
  return true;
}

void
view_switch_controller_fix::valueChanged(VSTGUI::CControl* control)
{
  UIDescriptionViewSwitchController::valueChanged(control);
  for(std::int32_t i = 0; i < 200; i++)
    dynamic_cast<editor*>(viewSwitch->getEditor())->controllerEndEdit(i);
}

} // namespace svn::vst::base