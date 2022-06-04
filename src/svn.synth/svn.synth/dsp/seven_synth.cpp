#include <svn.base/dsp/dsp.hpp>
#include <svn.base/dsp/block_input.hpp>
#include <svn.base/dsp/automation_view.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.synth/dsp/seven_synth.hpp>
#include <svn.synth/dsp/voice_input.hpp>

#include <cassert>
#include <algorithm>

using namespace svn::base;

namespace svn::synth {

seven_synth::
seven_synth(
  base::runtime_topology const* topology, float sample_rate,
  std::int32_t max_sample_count, base::param_value* state):
audio_processor(topology, sample_rate, max_sample_count, state),
_voice_audio(static_cast<std::size_t>(max_sample_count)),
_voice_audio_scratch(static_cast<std::size_t>(max_sample_count)),
_automation_fixed(),
_voices(),
_voice_states()
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);
}

void
seven_synth::return_voice(std::int32_t index)
{
  assert(_voice_states[index].in_use);
  _voice_states[index] = voice_state();
}

void
seven_synth::setup_voice_release(
  note_event const& note, 
  std::int32_t sample_count)
{
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use &&
      _voice_states[v].midi == note.midi &&
      _voice_states[v].release_position_buffer == sample_count)
      _voice_states[v].release_position_buffer = note.sample_index;
}

void
seven_synth::setup_voice(
  note_event const& note, 
  std::int32_t sample_count,
  std::int64_t stream_position)
{
  std::int32_t slot = -1;
  std::int64_t slot_position = -1;
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (!_voice_states[v].in_use)
    {
      slot = v;
      break;
    } else
    {
      std::int64_t this_position = _voice_states[v].start_position_stream;
      if (slot == -1 || this_position < slot_position)
      {
        slot = v;
        slot_position = this_position;
      }
    }

  _voice_states[slot].in_use = true;
  _voice_states[slot].midi = note.midi;
  // Not released this buffer by default.
  _voice_states[slot].release_position_buffer = sample_count;
  _voice_states[slot].start_position_buffer = note.sample_index;
  _voice_states[slot].start_position_stream = stream_position + note.sample_index;
  _voices[slot] = synth_voice(topology(), sample_rate(), -1.0f, note.velocity);
}

void
seven_synth::process_block(
  block_input const& input, audio_sample* audio)
{
  voice_input vinput;
  vinput.bpm = input.bpm;
  vinput.automation = base::automation_view(
    _automation_fixed.data(), input.automation, 
    static_cast<std::int32_t>(topology()->params.size()),
    static_cast<std::int32_t>(topology()->params.size()),
    0, input.sample_count, 0, input.sample_count);

  // Correct voice info for leftovers from previous round.
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use)
    {
      // Run along from the beginning.
      _voice_states[v].start_position_buffer = 0;

      _voice_states[v].release_position_buffer = -1;
    }

  // Set up new activation and release state.
  for(std::int32_t n = 0; n < input.note_count; n++)
    if(input.notes[n].note_on)
      setup_voice(input.notes[n], input.sample_count, input.stream_position);
    else
      setup_voice_release(input.notes[n], input.sample_count);

  // Process voices that are active anywhere in this buffer.
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use)
    {
      std::int32_t voice_start = _voice_states[v].start_position_buffer;
      std::int32_t voice_release = _voice_states[v].release_position_buffer;
      assert(voice_start <= voice_release && voice_release <= input.sample_count);
      vinput.sample_count = input.sample_count - voice_start;
      vinput.stream_position = input.stream_position + voice_start;
      vinput.automation.rearrange_samples(voice_start, voice_release - voice_start);

      clear_audio(_voice_audio.data(), vinput.sample_count);
      if(voice_release < input.sample_count)
        for(std::size_t p = 0; p < topology()->params.size(); p++)
          _automation_fixed[p] = input.automation[p][voice_release];
      std::int32_t processed = _voices[v].process_block(vinput, 
        _voice_audio_scratch.data(), _voice_audio.data(), voice_release - voice_start);
      base::add_audio(audio + voice_start, _voice_audio.data(), processed);
      if(processed < vinput.sample_count) return_voice(v);
    }
}

} // namespace svn::synth