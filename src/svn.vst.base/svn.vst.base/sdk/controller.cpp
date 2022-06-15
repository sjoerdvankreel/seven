#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <svn.vst.base/sdk/editor.hpp>
#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <svn.vst.base/support/vst_io_stream.hpp>

#include <vector>
#include <cassert>
#include <cstring>
#include <cstdint>

using namespace VSTGUI;
using namespace svn::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace svn::vst::base {   

controller::
controller(svn::base::topology_info const* topology):
_topology(topology)
{ assert(topology != nullptr); }

void
controller::sync_dependent_parameters()
{
  // Updates visibility of dependent parameters.
  for (std::size_t p = 0; p < _topology->params.size(); p++)
    endEdit(static_cast<std::int32_t>(p));
}

IPlugView* PLUGIN_API 
controller::createView(char const* name)
{
  if(ConstString(name) != ViewType::kEditor) return nullptr;
  return _editor = new editor(this, "view", "controller.uidesc", _topology);
}

tresult PLUGIN_API 
controller::setComponentState(IBStream* state)
{
  param_value value;
  std::vector<param_value> values(_topology->input_param_count, value);
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.load(*_topology, values.data())) return kResultFalse;

  for(std::int32_t p = 0; p < _topology->input_param_count; p++)
    if(_topology->params[p].descriptor->type == param_type::real)
      setParamNormalized(p, values[p].real);
    else
      setParamNormalized(p, parameter::discrete_to_vst_normalized(
        *_topology->params[p].descriptor, values[p].discrete));
  sync_dependent_parameters();
  return kResultOk;
}

tresult PLUGIN_API
controller::initialize(FUnknown* context)
{
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultTrue) return result;
  for (std::size_t p = 0; p < _topology->parts.size(); p++)
    addUnit(new Unit(_topology->parts[p].runtime_name.c_str(), p + 1, kRootUnitId));
  for (std::size_t p = 0; p < _topology->params.size(); p++)
  {
    part_info const* part = &_topology->parts[_topology->params[p].part_index];
    parameters.addParameter(new parameter(p, part, &_topology->params[p]));
  }
  return kResultTrue;
}
 
tresult
controller::endEdit(ParamID tag)
{
  if (_editor == nullptr) return EditControllerEx1::endEdit(tag);
  if (_topology->ui.param_dependencies[tag].size() == 0) return EditControllerEx1::endEdit(tag);
  double normalized = getParamNormalized(tag);
  auto const& descriptor = *_topology->params[tag].descriptor;
  std::int32_t value = parameter::vst_normalized_to_discrete(descriptor, normalized);
  _editor->controllerEndEdit(tag, value);
  return EditControllerEx1::endEdit(tag);
}

} // namespace svn::vst::base