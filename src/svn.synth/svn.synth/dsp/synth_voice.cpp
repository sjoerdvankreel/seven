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
  for(std::int32_t i = 0; i < voice_filter_count; i++)
    new(&_filters[i]) voice_filter(sample_rate, midi_note);
  _oscillators.fill(oscillator(sample_rate, midi_note));
}

std::int32_t
synth_voice::process_block(
  voice_input const& input, audio_state& audio, std::int32_t release_sample)
{
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  for (std::int32_t i = 0; i < oscillator_count; i++)
    _oscillators[i].process_block(input, i, audio.oscillator[i].data());

  // Clear filter output in case user selected weird routing (i.e. filter 3 to filter 2).
  for (std::int32_t i = 0; i < voice_filter_count; i++)
    base::clear_audio(audio.voice_filter[i].data(), input.sample_count);
  for (std::int32_t i = 0; i < voice_filter_count; i++)
  {
    base::audio_sample32 const* audio_in = audio.mix(input, audio_route_output::filter, i);
    _filters[i].process_block(input, i, audio_in, audio.voice_filter[i].data());
  }

  base::audio_sample32 const* audio_in = audio.mix(input, audio_route_output::amp, 0);
  return _amp.process_block(input, release_sample, audio_in, audio.voice_amp.data());
} 
 
} // namespace svn::synth
