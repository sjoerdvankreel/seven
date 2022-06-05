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
_oscillators(),
_topology(topology),
_amp(sample_rate, velocity)
{
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  _oscillators.fill(voice_oscillator(sample_rate, midi_note));
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

  voice_input part_input = input;
  for (std::int32_t i = 0; i < voice_osc_count; i++)
  {
    clear_audio(audio_scratch, input.sample_count);
    std::int32_t offset = _topology->bounds[part_type::voice_osc][i];
    part_input.automation = input.automation.rearrange_params(voice_osc_param::count, offset);
    _oscillators[i].process_block(part_input, audio_scratch);
    add_audio(audio, audio_scratch, input.sample_count);
  }
  return _amp.process_block(input, audio, release_sample);
}

} // namespace svn::synth
