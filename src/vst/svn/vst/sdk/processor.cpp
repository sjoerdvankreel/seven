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
  auto& input = _synth->input();
  input.sample_count = data.numSamples;
  input.bpm = static_cast<float>(data.processContext->tempo);
  input.sample_rate = static_cast<float>(data.processContext->sampleRate);

  if (data.numSamples == 0 || data.numOutputs == 0) return processNoAudio(data);
  processNoteEvents(data);
  processAutomation(data);
  svn::audio_sample* audio = _synth->process();

  for (std::int32_t s = 0; s < data.numSamples; s++)
  {
    data.outputs[0].channelBuffers32[0][s] = audio[s].left;
    data.outputs[0].channelBuffers32[1][s] = audio[s].right;
  }
}

void
Processor::processNoteEvent(std::int32_t index, Event const& event)
{
  auto& input = _synth->input();
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

void 
Processor::processNoteEvents(ProcessData const& data)
{
  Event event;
  std::int32_t index = 0;
  auto& input = _synth->input();

  input.note_count = 0;
  if (data.inputEvents == nullptr) return;
  for (std::int32_t i = 0; i < data.inputEvents->getEventCount(); i++)
    if (data.inputEvents->getEvent(i, event) == kResultOk)
      if(event.type == Event::kNoteOnEvent || event.type == Event::kNoteOffEvent) 
        processNoteEvent(input.note_count++, event);
}

void
Processor::processAutomation(ProcessData const& data)
{
  IParamValueQueue* queue;
  auto& input = _synth->input();
  auto changes = data.inputParameterChanges;

  for (std::int32_t p = 0; p < svn::synth_param_count; p++)
    if (svn::synth_params[p].info->type == svn::param_type::real)
      for (std::int32_t s = 0; s < data.numSamples; s++)
        static_cast<float*>(input.automation[p])[s] = static_cast<float>(_state[p].real);
    else
      for (std::int32_t s = 0; s < data.numSamples; s++)
        static_cast<std::int32_t*>(input.automation[p])[s] = _state[p].discrete;

  if (changes == nullptr) return;
  for (std::int32_t i = 0; i < changes->getParameterCount(); i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
    {

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

} // namespace Svn::Vst
