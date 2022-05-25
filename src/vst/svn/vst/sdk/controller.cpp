#include <svn/vst/support/ids.hpp>
#include <svn/vst/support/io_stream.hpp>
#include <svn/vst/sdk/parameter.hpp>
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
	tresult result = EditController::initialize(context);
	if(result != kResultTrue) return result;

  for (std::int32_t p = 0; p < svn::synth_part_count; p++)
  {
    auto const& part = svn::synth_parts[p];
    addUnit(new Unit(part.name.c_str(), p + 1, kRootUnitId));
  }

  for(std::int32_t p = 0; p < svn::synth_param_count; p++)
    parameters.addParameter(new Parameter(p, &svn::synth_params[p]));

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
      setParamNormalized(p, Parameter::fromDiscrete(*svn::synth_params[p].info, values[p].discrete));
  return kResultOk;
}

} // namespace Svn::Vst
