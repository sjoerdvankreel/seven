#include <svn.vst.base/sdk/editor.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <cassert>

using namespace VSTGUI;

namespace svn::vst::base {   

editor::
editor(EditController* controller, UTF8StringPtr template_name,
  UTF8StringPtr xml_file, svn::base::runtime_topology const* topology):
VST3Editor(controller, template_name, xml_file),
_topology(topology),
_controls(topology->params.size())
{
  assert(topology != nullptr);
  assert(xml_file != nullptr);
  assert(controller != nullptr);
  assert(template_name != nullptr);
}

void 
editor::onViewAdded(CFrame* frame, CView* view)
{
  VST3Editor::onViewAdded(frame, view);
  CControl* control = dynamic_cast<CControl*>(view);
  if(control == nullptr || control->getTag() < 0) return;
  _controls[control->getTag()].push_back(control);
}

void 
editor::onViewRemoved(CFrame* frame, CView* view)
{
  CControl* control = dynamic_cast<CControl*>(view);
  if (control == nullptr || control->getTag() < 0) return;
  for(std::size_t param = 0; param < _controls.size(); param++)
    for (std::size_t control = 0; control < _controls[param].size(); control++)
      if(_controls[param][control] == view)
        _controls[param].erase(_controls[param].begin() + control);
  VST3Editor::onViewRemoved(frame, view);
}

bool PLUGIN_API
editor::open(void* parent, const PlatformType& type)
{
  if (!VST3Editor::open(parent, type)) return false;
  static_cast<svn::vst::base::controller*>(this->controller)->sync_dependent_parameters();
  return true;
}
    
void               
editor::controllerEndEdit(ParamID tag, std::int32_t value)
{ 
  // Rearrange control order to have visible parameter on top.
  CView* visible_view = nullptr;
  CViewContainer* parent_view = nullptr;
  std::vector<CView*> invisible_views;
  auto const& dependents = _topology->ui_param_dependencies[tag];
  for(std::size_t d = 0; d < dependents.size(); d++)
    for(std::size_t c = 0; c < _controls[dependents[d]].size(); c++)
    {
      CView* container = _controls[dependents[d]][c]->getParentView();
      bool visible = _topology->params[dependents[d]].descriptor->ui_relevant_if_value == value;
      container->setVisible(visible);
      if(visible) assert(visible_view == nullptr), visible_view = container;
      else invisible_views.push_back(container);
      assert(parent_view == nullptr || parent_view == container->getParentView());
      parent_view = container->getParentView()->asViewContainer();
    }
  if (visible_view != nullptr)
    parent_view->removeView(visible_view, false);
  for(std::size_t i = 0; i < invisible_views.size(); i++)
  {
    parent_view->removeView(invisible_views[i], false);
    parent_view->addView(invisible_views[i]);
  }
  if (visible_view != nullptr)
    parent_view->addView(visible_view);
}

} // namespace svn::vst::base