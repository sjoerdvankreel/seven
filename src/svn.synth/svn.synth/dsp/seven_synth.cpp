#include <svn.base/dsp/dsp.hpp>
#include <svn.base/dsp/block_input.hpp>
#include <svn.synth/dsp/seven_synth.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

#include <cassert>
#include <algorithm>

using namespace svn::base;

namespace svn::synth {

seven_synth::
seven_synth(
  base::runtime_topology const* topology, float sample_rate,
  std::int32_t max_sample_count, base::param_value* state):
audio_processor(topology, sample_rate, max_sample_count, state),
_automation_scratch(topology, max_sample_count),
_audio_scratch(static_cast<std::size_t>(max_sample_count)),
_voices(),
_voice_states()
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);
}

void
seven_synth::setup_voice_release(
  note_event const& note)
{
  for (std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use &&
      _voice_states[v].midi == note.midi &&
      _voice_states[v].release_position_buffer == -1)
      _voice_states[v].release_position_buffer = note.sample_index;
}

void
seven_synth::setup_voice(
  note_event const& note, 
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
  _voice_states[slot].finished = false;
  _voice_states[slot].midi = note.midi;
  _voice_states[slot].release_position_buffer = -1;
  _voice_states[slot].start_position_buffer = note.sample_index;
  _voice_states[slot].start_position_stream = stream_position + note.sample_index;
  _voices[slot] = synth_voice(topology(), sample_rate(), -1.0f, note.velocity);
}

void
seven_synth::process_block(
  block_input const& input, audio_sample* audio)
{
  base::block_input voice_input;
  voice_input.note_count = 0;
  voice_input.notes = nullptr;
  voice_input.bpm = input.bpm;

  // Set up activation and release state.
  for(std::int32_t n = 0; n < input.note_count; n++)
    if(input.notes[n].note_on)
      setup_voice(input.notes[n], input.stream_position);
    else
      setup_voice_release(input.notes[n]);

  // Process voices that are active anywhere in this buffer.
  for(std::int32_t v = 0; v < synth_polyphony; v++)
    if (_voice_states[v].in_use)
    {
      std::int32_t voice_start = _voice_states[v].start_position_buffer;
      std::int32_t voice_release = _voice_states[v].release_position_buffer;
      assert(voice_release == -1 || voice_release >= voice_start);
      voice_input.sample_count = input.sample_count - voice_start;
      voice_input.stream_position = input.stream_position + voice_start;
      //voice_input.automation
      //if (_voice_states[v].release_position_buffer != -1)
      //{

      //}
    }
}

} // namespace svn::synth