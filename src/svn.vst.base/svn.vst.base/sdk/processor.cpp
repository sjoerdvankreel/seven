#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/utility/processdataslicer.h>

#include <svn.vst.base/sdk/processor.hpp>
#include <svn.vst.base/sdk/parameter.hpp>
#include <svn.vst.base/support/vst_init.hpp>
#include <svn.vst.base/support/vst_io_stream.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

#include <cassert>
#include <cstdint>

using namespace svn::base;
using namespace Steinberg;
using namespace Steinberg::Vst;

namespace svn::vst::base {

processor::
processor(
  Steinberg::FUID controller_id,
  runtime_topology const* topology):
_state(static_cast<std::size_t>(topology->params.size())),
_accurateParameters(static_cast<std::size_t>(topology->params.size())),
_topology(topology),
_processor()
{
	setControllerClass(controller_id);
  topology->init_defaults(_state.data());
  for(std::size_t i = 0; i < topology->params.size(); i++)
    _accurateParameters[i].setParamID(static_cast<std::int32_t>(i));
}

tresult PLUGIN_API
processor::canProcessSampleSize(int32 symbolic_size)
{ 
  if(symbolic_size == kSample32) return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
processor::initialize(FUnknown* context)
{
  tresult result = AudioEffect::initialize(context);
  if (result != kResultTrue) return kResultFalse;
  addEventInput(STR16("Event In"), 1);
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
processor::setupProcessing(ProcessSetup& setup)
{
  _processor.reset(create_audio_processor(_topology, setup).release());
  return AudioEffect::setupProcessing(setup);
}

tresult PLUGIN_API
processor::getState(IBStream* state)
{
  if(state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.save(_processor->topology(), _state.data())) return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API 
processor::setState(IBStream* state)
{
  float value;
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.load(_processor->topology(), _state.data())) return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API 
processor::setBusArrangements(
  SpeakerArrangement* inputs, int32 input_count,
  SpeakerArrangement* outputs, int32 output_count)
{
	if(input_count != 0 || output_count != 1 || outputs[0] != 2) return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, input_count, outputs, output_count);
}

tresult PLUGIN_API
processor::process(ProcessData& data)
{
  input_buffer& input = _processor->prepare_block(data.numSamples);
  input.bpm = 0.0f;
  input.note_count = 0;
  input.sample_rate = 0.0f;
  input.sample_count = data.numSamples;  

  if(data.processContext != nullptr && data.processContext->kTempoValid) 
    input.bpm = static_cast<float>(data.processContext->tempo);
  if (data.numSamples == 0 || data.numOutputs == 0) 
    return process_parameters(data);

  process_notes(input, data);
  process_automation(input, data);
  audio_sample const* audio = _processor->process_block();
  for (std::int32_t s = 0; s < data.numSamples; s++)
  {
    data.outputs[0].channelBuffers32[0][s] = audio[s].left;
    data.outputs[0].channelBuffers32[1][s] = audio[s].right;
  }
  return process_parameters(data);
}

void
processor::process_note(
  input_buffer& input, std::int32_t index, Event const& event)
{
  input.notes[index].sample_index = event.sampleOffset;
  switch (event.type)
  {
  case Event::kNoteOffEvent:
    input.notes[index].midi = note_off;
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
processor::process_notes(input_buffer& input, ProcessData const& data)
{
  Event event;
  std::int32_t index = 0;
  if (data.inputEvents == nullptr) return;
  for (std::int32_t i = 0; i < data.inputEvents->getEventCount(); i++)
    if (data.inputEvents->getEvent(i, event) == kResultOk)
      if (event.type == Event::kNoteOnEvent || event.type == Event::kNoteOffEvent)
        process_note(input, input.note_count++, event);
}

tresult
processor::process_parameters(ProcessData const& data)
{
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;
  if (changes == nullptr) return kResultOk;
  for (int32 i = 0; i < changes->getParameterCount(); i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
    {
      ParamID id = queue->getParameterId();
      auto const& param = *_topology->params[id].descriptor;
      if (queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
        if (param.type == param_type::real)
          _state[id].real = value;
        else
          _state[id].discrete = parameter::vst_normalized_to_discrete(param, value);
    }
  return kResultOk;
}

void
processor::process_automation(input_buffer& input, ProcessData const& data)
{
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;  
  if (changes == nullptr) return;
  for (std::int32_t i = 0; i < changes->getParameterCount(); i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
    {
      auto id = queue->getParameterId();
      auto const& param = *_topology->params[id].descriptor;
      if (param.type == param_type::real)
        _accurateParameters[id].setValue(_state[id].real);
      else
        _accurateParameters[id].setValue(
          parameter::discrete_to_vst_normalized(param, _state[id].discrete));

      _accurateParameters[id].beginChanges(queue);
      for (std::int32_t s = 0; s < data.numSamples; s++)
        if (param.type == param_type::real)
          static_cast<float*>(input.automation[id])[s] = _accurateParameters[id].advance(1);
        else
          static_cast<std::int32_t*>(input.automation[id])[s] 
            = parameter::vst_normalized_to_discrete(param, _accurateParameters[id].advance(1));
      _accurateParameters[id].endChanges();
    }
}

} // namespace svn::vst::base