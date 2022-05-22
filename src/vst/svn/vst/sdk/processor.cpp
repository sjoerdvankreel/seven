#include <svn/vst/seven_ids.hpp>
#include <svn/vst/seven_vst.hpp>
#include <svn/vst/seven_processor.hpp>
#include <svn/synth/seven_synth.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/utility/processdataslicer.h>

#include <cstdint>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

FUnknown* 
SevenProcessor::createInstance(void* context) 
{ return static_cast<IAudioProcessor*>(new SevenProcessor); }

SevenProcessor::
SevenProcessor():
_state(),
_synth32(),
_synth64(),
_accurateParameters()
{
	setControllerClass(ControllerId);
  for(std::uint32_t i = 0; i < svn::param_id::count; i++)
    _accurateParameters[i].setParamID(i);
}

tresult PLUGIN_API
SevenProcessor::getState(IBStream* state)
{
  IBStreamer streamer(state, kLittleEndian);
  for (std::int32_t i = 0; i < svn::param_id::count; i++)
    streamer.writeFloat(_state.values[i]);
  return kResultOk;
}

tresult PLUGIN_API 
SevenProcessor::initialize(FUnknown* context)
{
	tresult result = AudioEffect::initialize(context);
	if (result != kResultTrue) return kResultFalse;
  addEventInput(STR16("Event In"), 1);
	addAudioOutput (STR16 ("Stereo Out"), SpeakerArr::kStereo);
	return kResultTrue;
}

tresult PLUGIN_API 
SevenProcessor::setState(IBStream* state)
{
  float value;
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  for(std::int32_t i = 0; i < svn::param_id::count; i++)
    if(!streamer.readFloat(value)) 
      return kResultFalse;
    else
      _state.values[i] = value;
  return kResultOk;
}

tresult PLUGIN_API 
SevenProcessor::setBusArrangements(
  SpeakerArrangement* inputs, int32 inputCount,
  SpeakerArrangement* outputs, int32 outputCount)
{
	if(inputCount != 0 || outputCount != 1 || outputs[0] != 2) return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, inputCount, outputs, outputCount);
}

tresult PLUGIN_API
SevenProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
  if (symbolicSampleSize == kSample32 || symbolicSampleSize == kSample64) return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API 
SevenProcessor::setupProcessing(ProcessSetup& setup)
{
  _synth32.reset();
  _synth64.reset();
  if (setup.symbolicSampleSize == kSample32)
    _synth32.reset(new svn::seven_synth<float>(&_state, setup.sampleRate, setup.maxSamplesPerBlock));
  if (setup.symbolicSampleSize == kSample64)
    _synth64.reset(new svn::seven_synth<double>(&_state, setup.sampleRate, setup.maxSamplesPerBlock));
	return AudioEffect::setupProcessing(setup);
}

tresult PLUGIN_API 
SevenProcessor::process(ProcessData& data)
{
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  if (data.inputParameterChanges != nullptr)
    for(int32 i = 0; i < data.inputParameterChanges->getParameterCount(); i++)
      if ((queue = data.inputParameterChanges->getParameterData(i)) != nullptr)
        if(queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
          _state.values[queue->getParameterId()] = value;
	return kResultOk;
}

} // namespace Svn::Vst
