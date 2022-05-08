#include <svn/vst/seven_vst.hpp>
#include <svn/vst/seven_controller.hpp>
#include <svn/param/synth_params.hpp>
#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <cstring>

using namespace VSTGUI;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

FUnknown* 
SevenController::createInstance(void* context)
{ return static_cast<IEditController*>(new SevenController); }

tresult PLUGIN_API
SevenController::setComponentState(IBStream * state)
{
  float value;
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);

  if (!streamer.readFloat(value)) return kResultFalse;
  setParamNormalized(svn::param_id::unit1_gain, value);
  return kResultOk;
}

tresult PLUGIN_API 
SevenController::initialize(FUnknown* context)
{
	tresult result = EditController::initialize(context);
	if(result != kResultTrue) return result;
  for(std::size_t i = 0; i < svn::synth_params::all.size(); i++)
    parameters.addParameter(
      svn::synth_params::all[i].long_name,
      svn::synth_params::all[i].info.unit, 0,
      svn::synth_params::all[i].info.default_value,
      ParameterInfo::kCanAutomate, 
      svn::synth_params::all[i].id, 0,
      svn::synth_params::all[i].info.short_name);
	return kResultTrue;
}

IPlugView* PLUGIN_API
SevenController::createView(char const* name)
{
  if (name == nullptr || std::strcmp(name, "editor") != 0) return nullptr;
  return new VST3Editor(this, "view", "seven.uidesc");
}

} // namespace Svn::Vst
