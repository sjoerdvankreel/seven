#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/synth_voice.hpp>

#include <cassert>
#include <algorithm>

namespace svn::synth {

synth_voice::
synth_voice(
  base::topology_info const* topology,
  float sample_rate, float velocity, std::int32_t midi_note):
_filters(),
_oscillators(),
_topology(topology),
_amp(sample_rate, velocity)
{
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  _filters.fill(voice_filter(sample_rate, midi_note));
  _oscillators.fill(oscillator(sample_rate, midi_note));
}

std::int32_t
synth_voice::process_block(
  voice_input const& input, audio_state& audio, std::int32_t release_sample)
{
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  voice_input part_input = input;
  for (std::int32_t i = 0; i < oscillator_count; i++)
  {
    part_input.automation = input.automation.rearrange_params(part_type::oscillator, i);
    _oscillators[i].process_block(part_input, audio.oscillator_output[i].data());
  }

  for (std::int32_t i = 0; i < voice_filter_count; i++)
    base::clear_audio(audio.voice_filter_output[i].data(), input.sample_count);
  for (std::int32_t i = 0; i < voice_filter_count; i++)
  {
    part_input.automation = input.automation.rearrange_params(part_type::voice_filter, i);
    _filters[i].process_block(part_input, audio.scratch.data(), audio.voice_filter_output[i].data());
    base::add_audio(audio.output.data(), audio.voice_filter_output[i].data(), input.sample_count);
  }

  part_input.automation = input.automation.rearrange_params(part_type::voice_amp, 0);
  return _amp.process_block(part_input, audio.output.data(), release_sample);
} 
 
} // namespace svn::synth
