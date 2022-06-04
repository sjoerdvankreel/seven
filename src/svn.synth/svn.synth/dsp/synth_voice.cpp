#include <svn.base/dsp/dsp.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.synth/dsp/synth_voice.hpp>
#include <svn.synth/dsp/voice_input.hpp>

#include <cassert>
#include <algorithm>

namespace svn::synth {

synth_voice::
synth_voice(
  base::runtime_topology const* topology,
  float sample_rate, float velocity, std::int32_t midi_note):
_topology(topology),
_oscillators()
{ 
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  _oscillators.fill(voice_oscillator(sample_rate, velocity, midi_note));
}

std::int32_t
synth_voice::process_block(
  voice_input const& input,
  base::audio_sample* audio_scratch,
  base::audio_sample* audio,
  std::int32_t release_sample)
{
  assert(audio != nullptr);
  assert(release_sample >= -1);
  assert(audio_scratch != nullptr);
  assert(release_sample < input.sample_count);

  std::int32_t result = 0;
  std::int32_t part_samples = 0;
  voice_input part_input = input;

  for (std::int32_t i = 0; i < oscillator_count; i++)
  {
    clear_audio(audio_scratch, input.sample_count);
    std::int32_t offset = _topology->bounds[part_type::oscillator][i];
    part_input.automation = input.automation.rearrange_params(oscillator_param::count, offset);
    part_samples = _oscillators[i].process_block(part_input, audio_scratch, release_sample);
    add_audio(audio, audio_scratch, input.sample_count);
    result = std::max(result, part_samples);
  }
  return result;
}

} // namespace svn::synth
