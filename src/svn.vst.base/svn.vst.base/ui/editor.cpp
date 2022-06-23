#include <svn.vst.base/ui/editor.hpp>
#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <svn.vst.base/support/support.hpp>

#include <cassert>
#include <algorithm>

using namespace VSTGUI;
using namespace svn::base;

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

// Set initial visibility when the plugin ui is opened.
bool PLUGIN_API
editor::open(void* parent, const PlatformType& type)
{
  if (!VST3Editor::open(parent, type)) return false;
  static_cast<svn::vst::base::controller*>(this->controller)->sync_ui_parameters();
  return true;
}

void 
editor::onViewAdded(CFrame* frame, CView* view)
{
  VST3Editor::onViewAdded(frame, view);

  // Keep track of graphs.
  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if (graph != nullptr)
    _graphs.push_back(graph);
    
  // Keep track of controls by tag.
  auto* controller = static_cast<svn::vst::base::controller*>(getController());
  CControl* control = dynamic_cast<CControl*>(view);
  if (control != nullptr && control->getTag() >= 0)
  {
    // Control loses its value if removed and re-added.
    _controls[control->getTag()].push_back(control);
    param_value val = controller->state()[control->getTag()];
    control->setValue(base_to_vst_normalized(_topology, control->getTag(), val));
  }
}
 
void 
editor::onViewRemoved(CFrame* frame, CView* view)
{
  // Keep track of graphs.
  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if (graph != nullptr)
  {
    auto it = std::find(_graphs.begin(), _graphs.end(), graph);
    assert(it != _graphs.end());
    _graphs.erase(it);
  }

  // Keep track of controls by tag.
  CControl* control = dynamic_cast<CControl*>(view);
  if (control != nullptr && control->getTag() >= 0)
    for(std::size_t param = 0; param < _controls.size(); param++)
      for (std::size_t control = 0; control < _controls[param].size(); control++)
        if(_controls[param][control] == view)
          _controls[param].erase(_controls[param].begin() + control);

  VST3Editor::onViewRemoved(frame, view);
}
    
void               
editor::controllerEndEdit(ParamID tag)
{
  // Update graphs where needed.  
  for (std::size_t i = 0; i < _graphs.size(); i++)
    if (_graphs[i]->processor()->needs_repaint(tag))
      _graphs[i]->setDirty(true);
       
  auto const& dependents = _topology->ui.param_dependencies[tag];
  for(std::size_t d = 0; d < dependents.size(); d++)
  {                 
    CView* visible_view = nullptr;
    CViewContainer* parent_view = nullptr;

    for(std::size_t c = 0; c < _controls[dependents[d]].size(); c++)
    {
      // Visible if all dependencies are met.
      bool visible = true;
      auto const& param = _topology->params[dependents[d]];
      for (std::int32_t i = 0; i < param.descriptor->ui.relevance_count; i++)
      {
        auto controller = static_cast<svn::vst::base::controller*>(getController());
        std::int32_t part_index = _topology->parts[param.part_index].type_index;
        std::int32_t part_type = _topology->parts[param.part_index].descriptor->type;
        std::int32_t offset = _topology->param_bounds[part_type][part_index];
        std::int32_t that_param = param.descriptor->ui.relevance[i].if_param;
        double normalized = controller->getParamNormalized(that_param + offset);
        std::int32_t value = vst_normalized_to_base(_topology, that_param + offset, normalized).discrete;
        visible &= _topology->params[dependents[d]].descriptor->ui.relevance[i].if_value == value;
      }

      // Show/hide dependent controls.
      CView* container = _controls[dependents[d]][c]->getParentView();
      container->setVisible(visible);
      if(visible) 
      {
        assert(visible_view == nullptr || visible_view == container);
        visible_view = container;
      }
      assert(parent_view == nullptr || parent_view == container->getParentView());
      parent_view = container->getParentView()->asViewContainer();
    }
             
    // Move to front for hittesting.
    if(parent_view != nullptr && visible_view != nullptr)
      parent_view->changeViewZOrder(visible_view, parent_view->getNbViews() - 1);
  }
}

} // namespace svn::vst::base