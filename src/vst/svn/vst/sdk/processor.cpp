#include <svn/support/topo_rt.hpp>
#include <svn/vst/sdk/processor.hpp>
#include <svn/vst/support/param.hpp>
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
    int32 index;
    ParamValue value;
    IParamValueQueue* queue;
    auto changes = data.inputParameterChanges;
    if(changes == nullptr) return kResultOk;
    for (int32 i = 0; i < changes->getParameterCount(); i++)
      if ((queue = changes->getParameterData(i)) != nullptr)
        if (queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
          if(svn::synth_params[queue->getParameterId()].info->type == svn::param_type::real)
            _state[queue->getParameterId()].real = value;
          else
            _state[queue->getParameterId()].discrete = paramDenormalizeDiscrete(queue->getParameterId(), value);
    return kResultOk;
  }

  auto& input = _synth->input();
  input.note_count = 0;
  input.sample_count = data.numSamples;
  input.bpm = static_cast<float>(data.processContext->tempo);
  input.sample_rate = static_cast<float>(data.processContext->sampleRate);

  Event event;
  std::int32_t note = 0;
  if (data.inputEvents != nullptr)
    for(std::int32_t i = 0; i < data.inputEvents->getEventCount(); i++)
      if (data.inputEvents->getEvent(i, event) == kResultOk)
      {
        switch (event.type)
        {
        case Event::kNoteOffEvent:
          input.notes[input.note_count].midi = svn::note_off;
          break;
        case Event::kNoteOnEvent:
          input.notes[input.note_count].midi = event.noteOn.pitch;
          input.notes[input.note_count].velocity = event.noteOn.velocity;
          break;
        default:
          break;
        }    
        input.notes[input.note_count].sample_index = event.sampleOffset;
        ++input.note_count;
      }
}

} // namespace Svn::Vst
