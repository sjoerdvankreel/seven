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

std::int32_t const output_param_update_msec = 200;

processor::
processor(
  Steinberg::FUID controller_id,
  runtime_topology const* topology):
_state(static_cast<std::size_t>(topology->input_param_count)),
_accurate_parameters(static_cast<std::size_t>(topology->input_param_count)),
_topology(topology),
_processor()
{
	setControllerClass(controller_id);
  topology->init_defaults(_state.data());
  for(std::int32_t i = 0; i < topology->input_param_count; i++)
    _accurate_parameters[i].setParamID(static_cast<std::int32_t>(i));
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
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  addEventInput(STR16("Event In"), _topology->max_note_events);
  return kResultTrue;
}

tresult PLUGIN_API
processor::getState(IBStream* state)
{
  if(state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.save(*_topology, _state.data())) return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API 
processor::setState(IBStream* state)
{
  float value;
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.load(*_topology, _state.data())) return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API
processor::setupProcessing(ProcessSetup& setup)
{
  float sample_rate = static_cast<float>(setup.sampleRate);
  std::int32_t max_sample_count = setup.maxSamplesPerBlock;
  _processor.reset(init_create_audio_processor(
    _topology, sample_rate, max_sample_count, _state.data()).release());
  return AudioEffect::setupProcessing(setup);
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
  block_input& input = _processor->prepare_block(data.numSamples);
  
  input.bpm = 0.0f;
  input.stream_position = -1L;
  input.sample_count = data.numSamples;  

  if(data.processContext != nullptr)
    input.stream_position = data.processContext->projectTimeSamples;
  if(data.processContext != nullptr && data.processContext->kTempoValid) 
    input.bpm = static_cast<float>(data.processContext->tempo);
  if (data.numSamples == 0 || data.numOutputs == 0) 
  {
    process_input_parameters(data);
    return kResultOk;
  }

  process_notes(input, data);
  process_automation(input, data);
  block_output const& output = _processor->process_block();
  for (std::int32_t s = 0; s < data.numSamples; s++)
  {
    data.outputs[0].channelBuffers32[0][s] = output.audio[s].left;
    data.outputs[0].channelBuffers32[1][s] = output.audio[s].right;
  }
  process_input_parameters(data);
  process_output_parameters(output, data);
  return kResultOk;
}

void
processor::process_notes(block_input& input, ProcessData const& data)
{
  Event event;
  if (data.inputEvents == nullptr) return;
  int32 count = data.inputEvents->getEventCount();
  std::int32_t capacity = _processor->topology()->max_note_events;
  for (std::int32_t i = 0; i < count; i++)
    if (data.inputEvents->getEvent(i, event) == kResultOk)
      if (event.type == Event::kNoteOnEvent || event.type == Event::kNoteOffEvent)
        if (input.note_count == capacity) break;
        else
        {
          auto& note = input.notes[input.note_count++];
          switch (event.type)
          {
          case Event::kNoteOnEvent:
            note.note_on = true;
            note.midi = event.noteOn.pitch;
            note.velocity = event.noteOn.velocity;
            note.sample_index = event.sampleOffset;
            break;
          case Event::kNoteOffEvent:
            note.note_on = false;
            note.velocity = 0.0f;
            note.midi = event.noteOff.pitch;
            note.sample_index = event.sampleOffset;
            break;
          default:
            assert(false);
            break;
          }
        }
}

void
processor::process_input_parameters(ProcessData const& data)
{
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;
  if (changes == nullptr) return;
  int32 count = changes->getParameterCount();
  for (int32 i = 0; i < count; i++)
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
}

void
processor::process_automation(block_input& input, ProcessData const& data)
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
        _accurate_parameters[id].setValue(_state[id].real);
      else
        _accurate_parameters[id].setValue(
          parameter::discrete_to_vst_normalized(param, _state[id].discrete));

      _accurate_parameters[id].beginChanges(queue);
      for (std::int32_t s = 0; s < data.numSamples; s++)
        if (param.type == param_type::real)
          input.automation[id][s].real = _accurate_parameters[id].advance(1);
        else
          input.automation[id][s].discrete 
            = parameter::vst_normalized_to_discrete(param, _accurate_parameters[id].advance(1));
      _accurate_parameters[id].endChanges();
    }
}

void
processor::process_output_parameters(
  svn::base::block_output const& output, ProcessData& data)
{
  int32 index;
  double normalized;
  if (!data.outputParameterChanges) return;
  if (data.processContext == nullptr) return;
  int64_t block_end_samples = data.processContext->projectTimeSamples + data.numSamples;
  std::int32_t update_block_size = output_param_update_msec * data.processContext->sampleRate / 1000;
  if(block_end_samples < output_param_update_samples + update_block_size) return;
  
  output_param_update_samples = block_end_samples;
  std::int32_t input_count = _processor->topology()->input_param_count;
  for (std::int32_t p = 0; p < _processor->topology()->output_param_count; p++)
  { 
    IParamValueQueue* queue = data.outputParameterChanges->addParameterData(input_count + p, index);
    if (queue == nullptr) continue;
    auto const& descriptor = *_processor->topology()->params[input_count + p].descriptor;
    param_value value = output.output_params[p];
    if(descriptor.type == param_type::real)
      normalized = descriptor.real.dsp.from_range(value.real);
    else
      normalized = parameter::discrete_to_vst_normalized(descriptor, value.discrete);
    queue->addPoint(0, normalized, index);
  }
}

} // namespace svn::vst::base