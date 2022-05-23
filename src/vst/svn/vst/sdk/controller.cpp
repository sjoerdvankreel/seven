#include <svn/vst/support/ids.hpp>
#include <svn/vst/support/param.hpp>
#include <svn/vst/support/io_stream.hpp>
#include <svn/vst/sdk/controller.hpp>

#include <svn/support/io_stream.hpp>
#include <svn/support/topo_rt.hpp>
#include <svn/support/topo_static.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>

#include <vector>
#include <cstring>
#include <cstdint>

using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace Svn::Vst {   

tresult PLUGIN_API 
Controller::initialize(FUnknown* context)
{
  StringListParameter* listParameter = nullptr;
	tresult result = EditController::initialize(context);
	if(result != kResultTrue) return result;

  for (std::int32_t p = 0; p < svn::synth_part_count; p++)
  {
    auto const& part = svn::synth_parts[p];
    addUnit(new Unit(part.name.c_str(), p + 1, kRootUnitId));
  }

  for(std::int32_t p = 0; p < svn::synth_param_count; p++)
  {
    auto const& param = svn::synth_params[p].info;
    wchar_t const* detail = svn::synth_params[p].detail.c_str();
    std::int32_t part_index = svn::synth_params[p].part_index + 1;
    switch (param->type)
    {
    case svn::param_type::real:
      parameters.addParameter(
        detail, param->unit, 0L, param->default_.real,
        ParameterInfo::kCanAutomate, p, part_index, param->item.name);
      break;
    case svn::param_type::list:
      listParameter = new StringListParameter(
        detail, p, param->unit,
        ParameterInfo::kCanAutomate | ParameterInfo::kIsList, 
        part_index, param->item.name);
      for(std::int32_t i = 0; i <= param->max.discrete; i++)
        listParameter->appendString(param->items[i].detail);
      parameters.addParameter(listParameter);
      break;
    default:
      parameters.addParameter(new RangeParameter(
        detail, p, param->unit,
        param->min.discrete, param->max.discrete, param->default_.discrete,
        param->max.discrete - param->min.discrete, 
        ParameterInfo::kCanAutomate, part_index, param->item.name));
      break;
    }
  }
	return kResultTrue;
}

tresult PLUGIN_API 
Controller::setComponentState(IBStream* state)
{
  svn::param_value value;
  std::vector<svn::param_value> values(svn::synth_param_count, value);
  
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  IOStream stream(&streamer);
  if(!svn::io_stream::load(stream, values.data())) return kResultFalse;

  for(std::int32_t p = 0; p < svn::synth_param_count; p++)
    if(svn::synth_params[p].info->type == svn::param_type::real)
      setParamNormalized(p, values[p].real);
    else
      setParamNormalized(p, paramNormalizeDiscrete(p, values[p].discrete));
  return kResultOk;
}

} // namespace Svn::Vst
