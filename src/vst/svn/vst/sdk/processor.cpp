#include <svn/support/topo_rt.hpp>
#include <svn/vst/sdk/processor.hpp>
#include <svn/vst/support/param.hpp>
#include <svn/vst/support/ids.hpp>
#include <svn/vst/support/io_stream.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/utility/processdataslicer.h>

#include <cassert>
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
  float sampleRate = static_cast<float>(setup.sampleRate);
  _synth.reset(new svn::seven_synth(_state.data(), setup.sampleRate, setup.maxSamplesPerBlock));
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
  svn::input_buffer& input = _synth->prepare_block(data.numSamples);
  input.bpm = 0.0f;
  input.note_count = 0;
  input.sample_rate = 0.0f;
  input.sample_count = data.numSamples;  
  if(data.processContext != nullptr && data.processContext->kTempoValid) 
    input.bpm = static_cast<float>(data.processContext->tempo);

  if (data.numSamples == 0 || data.numOutputs == 0) 
    return processNoAudio(data);
  processNoteEvents(input, data);
  processAutomation(input, data);
  svn::audio_sample* audio = _synth->process_block();
  for (std::int32_t s = 0; s < data.numSamples; s++)
  {
    data.outputs[0].channelBuffers32[0][s] = audio[s].left;
    data.outputs[0].channelBuffers32[1][s] = audio[s].right;
  }

  return kResultOk;
}

void
Processor::processNoteEvents(svn::input_buffer& input, ProcessData const& data)
{
  Event event;
  std::int32_t index = 0;
  if (data.inputEvents == nullptr) return;
  for (std::int32_t i = 0; i < data.inputEvents->getEventCount(); i++)
    if (data.inputEvents->getEvent(i, event) == kResultOk)
      if (event.type == Event::kNoteOnEvent || event.type == Event::kNoteOffEvent)
        processNoteEvent(input, input.note_count++, event);
}

void
Processor::processNoteEvent(svn::input_buffer& input, std::int32_t index, Event const& event)
{
  input.notes[index].sample_index = event.sampleOffset;
  switch (event.type)
  {
  case Event::kNoteOffEvent:
    input.notes[index].midi = svn::note_off;
    break;
  case Event::kNoteOnEvent:
    input.notes[index].midi = event.noteOn.pitch;
    input.notes[index].velocity = event.noteOn.velocity;
    break;
  default:
    assert(false);
    break;
  }
}

tresult
Processor::processNoAudio(ProcessData const& data)
{
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;
  if (changes == nullptr) return kResultOk;
  for (int32 i = 0; i < changes->getParameterCount(); i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
      if (queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
        if (svn::synth_params[queue->getParameterId()].info->type == svn::param_type::real)
          _state[queue->getParameterId()].real = value;
        else
          _state[queue->getParameterId()].discrete = paramDenormalizeDiscrete(queue->getParameterId(), value);
  return kResultOk;
}

void
Processor::processAutomation(svn::input_buffer& input, ProcessData const& data)
{
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;  
  if (changes == nullptr) return;
  for (std::int32_t i = 0; i < changes->getParameterCount(); i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
    {
      auto id = queue->getParameterId();
      if (svn::synth_params[id].info->type == svn::param_type::real)
        _accurateParameters[id].setValue(_state[id].real);
      else
        _accurateParameters[id].setValue(paramNormalizeDiscrete(id, _state[id].discrete));
      _accurateParameters[id].beginChanges(queue);
      for (std::int32_t s = 0; s < data.numSamples; s++)
        if (svn::synth_params[id].info->type == svn::param_type::real)
          static_cast<float*>(input.automation[id])[s] = _accurateParameters[id].advance(1);
        else
          static_cast<std::int32_t*>(input.automation[id])[s] = paramDenormalizeDiscrete(id, _accurateParameters[id].advance(1));
      _accurateParameters[id].endChanges();
    }
}

} // namespace Svn::Vst
