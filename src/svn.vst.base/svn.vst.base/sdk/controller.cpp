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
controller(svn::base::runtime_topology const* topology):
_topology(topology)
{ assert(topology != nullptr); }

IPlugView* PLUGIN_API 
controller::createView(char const* name)
{
  if(ConstString(name) != ViewType::kEditor) return nullptr;
  return new editor(this, "view", "controller.uidesc", _topology->params.size());
}

tresult 
controller::endEdit(ParamID tag)
{
  assert(_topology->ui_param_dependencies[tag].size() == 0);
  return EditControllerEx1::endEdit(tag);
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
    parameters.addParameter(new parameter(p, &_topology->params[p]));
  return kResultTrue;
}

} // namespace svn::vst::base