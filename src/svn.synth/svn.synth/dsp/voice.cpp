#include <svn.synth/dsp/voice.hpp>
#include <svn.base/dsp/support.hpp>

#include <cassert>
#include <algorithm>

namespace svn::synth {

voice::
voice(
  base::topology_info const* topology,
  float sample_rate, float velocity, std::int32_t midi_note):
_oscillators(),
_topology(topology),
_amplitude(sample_rate, velocity)
{
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  _oscillators.fill(oscillator(sample_rate, midi_note));
}

std::int32_t
voice::process_block(
  voice_input const& input, base::audio_sample* audio_scratch,
  base::audio_sample* audio, std::int32_t release_sample)
{
  assert(audio != nullptr);
  assert(release_sample >= -1);
  assert(audio_scratch != nullptr);
  assert(release_sample < input.sample_count);

  voice_input part_input = input;
  for (std::int32_t i = 0; i < oscillator_count; i++)
  {
    base::clear_audio(audio_scratch, input.sample_count);
    part_input.automation = input.automation.rearrange_params(part_type::oscillator, i);
    _oscillators[i].process_block(part_input, audio_scratch);
    base::add_audio(audio, audio_scratch, input.sample_count);
  }
  part_input.automation = input.automation.rearrange_params(part_type::amplitude, 0);
  return _amplitude.process_block(part_input, audio, release_sample);
}

} // namespace svn::synth
