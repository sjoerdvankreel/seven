#include <svn/vst/support/ids.hpp>
#include <svn/vst/support/param.hpp>
#include <svn/vst/sdk/controller.hpp>
#include <svn/support/topo_rt.hpp>
#include <svn/support/topo_static.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <cstring>
#include <cstdint>

using namespace Steinberg;
using namespace Steinberg::Vst;                  

namespace Svn::Vst {   

tresult PLUGIN_API 
Controller::initialize(FUnknown* context)
{
	tresult result = EditController::initialize(context);
	if(result != kResultTrue) return result;
  for(std::int32_t p = 0; p < svn::synth_param_count; p++)
  {
    auto const& param = svn::synth_params[p].info;
    parameters.addParameter(
      param->item.detail, param->unit,
      paramStepCount(p), paramNormalizeDefault(p),
      ParameterInfo::kCanAutomate, -1L, 0L,
      param->item.name);
  }
	return kResultTrue;
}

tresult PLUGIN_API 
Controller::setComponentState(IBStream* state)
{
  float value;
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  for (std::size_t i = 0; i < svn::synth_params::all.size(); i++)
    if (!streamer.readFloat(value))
      return kResultFalse;
    else 
      setParamNormalized(svn::synth_params::all[i].id, value);
  return kResultOk;
}

} // namespace Svn::Vst
