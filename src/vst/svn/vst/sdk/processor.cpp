#include <svn/support/topo_rt.hpp>
#include <svn/vst/sdk/processor.hpp>
#include <svn/vst/support/ids.hpp>
#include <svn/vst/support/io_stream.hpp>

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

Processor::
Processor():
_synth(),
_state(static_cast<std::size_t>(svn::synth_param_count)),
_accurateParameters(static_cast<std::size_t>(svn::synth_param_count))
{
	setControllerClass(ControllerId);
  svn::init_defaults(_state.data());
  for(std::int32_t i = 0; i < svn::synth_param_count; i++)
    _accurateParameters[i].setParamID(i);
}

tresult PLUGIN_API
Processor::initialize(FUnknown* context)
{
  tresult result = AudioEffect::initialize(context);
  if (result != kResultTrue) return kResultFalse;
  addEventInput(STR16("Event In"), 1);
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
Processor::getState(IBStream* state)
{
  if(state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  IOStream stream(&streamer);
  if(!svn::io_stream::save(stream, _state.data())) return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API 
Processor::setState(IBStream* state)
{
  float value;
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  IOStream stream(&streamer);
  if(!svn::io_stream::load(stream, _state.data())) return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API
Processor::setupProcessing(ProcessSetup& setup)
{
  _synth.reset(new svn::synth(_state.data(), setup.maxSamplesPerBlock));
  return AudioEffect::setupProcessing(setup);
}

tresult PLUGIN_API 
Processor::setBusArrangements(
  SpeakerArrangement* inputs, int32 inputCount,
  SpeakerArrangement* outputs, int32 outputCount)
{
	if(inputCount != 0 || outputCount != 1 || outputs[0] != 2) return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, inputCount, outputs, outputCount);
}

tresult PLUGIN_API 
Processor::process(ProcessData& data)
{
  if (data.numSamples == 0 || data.numOutputs == 0)
  {

  }
  return kResultOk;

  /*
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  if (data.inputParameterChanges != nullptr)
    for(int32 i = 0; i < data.inputParameterChanges->getParameterCount(); i++)
      if ((queue = data.inputParameterChanges->getParameterData(i)) != nullptr)
        if(queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
          _state[queue->getParameterId()] = value;
	return kResultOk;
  */
}

} // namespace Svn::Vst
