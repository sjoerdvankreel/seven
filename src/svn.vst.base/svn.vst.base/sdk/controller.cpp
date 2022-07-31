#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>

#include <svn.base/dsp/support.hpp>
#include <svn.vst.base/ui/editor.hpp>
#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <svn.vst.base/support/support.hpp>
#include <svn.vst.base/support/io_stream.hpp>

#include <vector>
#include <cassert>
#include <cstring>
#include <cstdint>
   
using namespace VSTGUI;
using namespace svn::base;
using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace svn::vst::base {   

void
controller::sync_ui_parameters()
{
  // Updates visibility of dependent parameters and rendering of graphs.
  for (std::size_t p = 0; p < _topology->params.size(); p++)
    endEdit(static_cast<std::int32_t>(p));
}

tresult
controller::endEdit(ParamID tag)
{
  update_state(tag);
  if (_editor == nullptr) return EditControllerEx1::endEdit(tag);

  // Update visibility of dependent parameters and rerender graphs.
  _editor->controllerEndEdit(tag);
  return EditControllerEx1::endEdit(tag);
}

// Update private copy of the state in svn::base format, for easy access by graphs.
void
controller::update_state(std::int32_t param)
{
  double normalized = getParamNormalized(param);
  auto const& descriptor = *_topology->params[param].descriptor;
  _state[param] = vst_normalized_to_base(_topology, param, normalized);
}

IPlugView* PLUGIN_API
controller::createView(char const* name)
{
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  return _editor = new editor(this, "view", "controller.uidesc", _topology);
}   

tresult PLUGIN_API
controller::setComponentState(IBStream* state)
{
  param_value value;   
  if (state == nullptr) return kResultFalse;

  // Load state into temporary buffer.
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  std::vector<param_value> values(_topology->input_param_count, value);
  if (!stream.load(*_topology, values.data())) return kResultFalse;

  // SetParamNormalized() each value.
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    setParamNormalized(p, base_to_vst_normalized(_topology, p, values[p]));
    update_state(p);
  }

  // Set initial ui visibility state.
  sync_ui_parameters();
  return kResultOk;
}

tresult PLUGIN_API
controller::initialize(FUnknown* context)
{
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultTrue) return result;

  // Add parts as units.
  for (std::size_t p = 0; p < _topology->parts.size(); p++)
    addUnit(new Unit(_topology->parts[p].runtime_name.c_str(), static_cast<Steinberg::int32>(p + 1), kRootUnitId));
  // Add all runtime parameters.
  for (std::size_t p = 0; p < _topology->params.size(); p++)
  {
    part_info const* part = &_topology->parts[_topology->params[p].part_index];
    parameters.addParameter(new parameter(p, part, &_topology->params[p]));
  }
  return kResultTrue;
} 

} // namespace svn::vst::base