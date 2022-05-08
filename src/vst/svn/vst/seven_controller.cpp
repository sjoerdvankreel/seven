#include <svn/vst/seven_vst.hpp>
#include <svn/vst/seven_controller.hpp>
#include <svn/param/synth_params.hpp>
#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>

using namespace VSTGUI;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

FUnknown* 
SevenController::createInstance(void* context)
{ return static_cast<IEditController*>(new SevenController); }

tresult PLUGIN_API 
SevenController::initialize(FUnknown* context)
{
	tresult result = EditController::initialize(context);
	if(result != kResultTrue) return result;
  parameters.addParameter(STR16(svn::synth_params::unit1_gain.description), STR16 ("dB"), 0, .5,
		                         Vst::ParameterInfo::kCanAutomate, HelloWorldParams::kParamVolId, 0,
		                         STR16 ("Param1"));
	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API PlugController::createView (const char* name)
{
	// someone wants my editor
	if (name && strcmp (name, "editor") == 0)
	{
		auto* view = new VST3Editor (this, "view", "plug.uidesc");
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setComponentState (IBStream* state)
{
	// we receive the current state of the component (processor part)
	// we read our parameters and bypass value...
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
		return kResultFalse;
	setParamNormalized (HelloWorldParams::kParamVolId, savedParam1);

	return kResultOk;
}

} // namespace Svn::Vst
