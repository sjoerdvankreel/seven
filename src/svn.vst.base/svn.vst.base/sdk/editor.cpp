#include <svn.vst.base/sdk/editor.hpp>
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

void
editor::controllerEndEdit(ParamID tag, std::int32_t value)
{
  for(std::size_t control = 0; control < _controls[tag].size(); control++)
      _controls[tag][control]->setVisible(_topology->params[tag].descriptor->ui_relevant_if_value == value);
}

} // namespace svn::vst::base