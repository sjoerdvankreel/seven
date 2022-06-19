#include <svn.vst.base/sdk/editor.hpp>
#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/sdk/controller.hpp>

#include <cassert>
#include <algorithm>

using namespace VSTGUI;

namespace svn::vst::base {   

editor::
editor(EditController* controller, UTF8StringPtr template_name,
  UTF8StringPtr xml_file, svn::base::topology_info const* topology):
VST3Editor(controller, template_name, xml_file),
_topology(topology), _controls(topology->params.size()), _graphs()
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
  if (control != nullptr && control->getTag() >= 0)
    _controls[control->getTag()].push_back(control);

  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if(graph != nullptr)
    _graphs.push_back(graph);
}
 
void 
editor::onViewRemoved(CFrame* frame, CView* view)
{
  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if (graph != nullptr)
  {
    auto it = std::find(_graphs.begin(), _graphs.end(), graph);
    assert(it != _graphs.end());
    _graphs.erase(it);
  }

  CControl* control = dynamic_cast<CControl*>(view);
  if (control != nullptr && control->getTag() >= 0)
    for(std::size_t param = 0; param < _controls.size(); param++)
      for (std::size_t control = 0; control < _controls[param].size(); control++)
        if(_controls[param][control] == view)
          _controls[param].erase(_controls[param].begin() + control);

  VST3Editor::onViewRemoved(frame, view);
}

// Set initial visibility when the plugin ui is opened.
bool PLUGIN_API
editor::open(void* parent, const PlatformType& type)
{
  if (!VST3Editor::open(parent, type)) return false;
  static_cast<svn::vst::base::controller*>(this->controller)->sync_dependent_parameters();
  return true;
}
    
void               
editor::controllerEndEdit(ParamID tag, double normalized)
{ 
  // Rearrange control order to have visible parameter on top.
  CView* visible_view = nullptr;
  CViewContainer* parent_view = nullptr;
  std::vector<CView*> invisible_views;
  auto const& dependents = _topology->ui.param_dependencies[tag];
  for(std::size_t d = 0; d < dependents.size(); d++)
    for(std::size_t c = 0; c < _controls[dependents[d]].size(); c++)
    {
      auto const& descriptor = *_topology->params[tag].descriptor;
      std::int32_t value = parameter::vst_normalized_to_discrete(descriptor, normalized);
      CView* container = _controls[dependents[d]][c]->getParentView();
      bool visible = _topology->params[dependents[d]].descriptor->ui.relevant_if_value == value;
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