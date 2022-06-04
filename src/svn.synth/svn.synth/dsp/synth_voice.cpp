#include <svn.base/dsp/dsp.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.synth/dsp/synth_voice.hpp>

#include <cassert>
#include <algorithm>

namespace svn::synth {

synth_voice::
synth_voice(
  base::runtime_topology const* topology,
  base::audio_sample* audio_scratch,
  float sample_rate, float frequency, float velocity):
_topology(topology),
_audio_scratch(audio_scratch),
_oscillators()
{ 
  assert(topology != nullptr);
  assert(audio_scratch != nullptr);
  assert(frequency > 0.0f);
  assert(sample_rate > frequency);
  assert(0.0f <= velocity && velocity <= 1.0f);
  _oscillators.fill(voice_oscillator(sample_rate, frequency, velocity));
}

std::int32_t
synth_voice::process_block(
  base::block_input const& input,
  struct base::audio_sample* audio,
  std::int32_t release_sample)
{
  assert(audio != nullptr);
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  std::int32_t result = 0;
  std::int32_t part_samples = 0;
  block_input part_input = input;

  for (std::int32_t i = 0; i < oscillator_count; i++)
  {
    clear_audio(_audio_scratch, input.sample_count);
    std::int32_t offset = _topology->bounds[part_type::oscillator][i];
    part_input.automation = input.automation + offset;
    part_samples = _oscillators[i].process_block(part_input, _audio_scratch, release_sample);
    add_audio(audio, _audio_scratch, input.sample_count);
    result = std::max(result, part_samples);
  }
  return result;
}

} // namespace svn::synth
