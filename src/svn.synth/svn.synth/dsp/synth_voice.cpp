#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/synth_voice.hpp>

#include <cassert>
#include <algorithm>

namespace svn::synth {

synth_voice::
synth_voice(
  base::topology_info const* topology,
  float sample_rate, float velocity, std::int32_t midi_note):
_envelopes(), _filters(), _oscillators(),
_topology(topology), _velocity(velocity), _amp(sample_rate)
{
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  for(std::int32_t i = 0; i < voice_filter_count; i++)
    new(&_filters[i]) voice_filter(sample_rate, midi_note);
  _envelopes.fill(envelope(sample_rate));
  _oscillators.fill(oscillator(sample_rate, midi_note));
}

std::int32_t
synth_voice::process_block(
  voice_input const& input, cv_state& cv, audio_state& audio, std::int32_t release_sample)
{
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  // Run envelopes.
  std::int32_t amp_env_end = input.sample_count;
  for (std::int32_t i = 0; i < envelope_count; i++)
  {
    std::int32_t processed = _envelopes[i].process_block(input, i, cv.envelope[i].data());
    if(i == 0) amp_env_end = processed;
  }

  // Run generators.
  for (std::int32_t i = 0; i < oscillator_count; i++)
    _oscillators[i].process_block(input, i, audio.oscillator[i].data());

  // Clear filter output in case user selected weird routing (i.e. filter 3 to filter 2).
  for (std::int32_t i = 0; i < voice_filter_count; i++)
    base::clear_audio(audio.voice_filter[i].data(), input.sample_count);

  // Run filters.
  for (std::int32_t i = 0; i < voice_filter_count; i++)
  {
    base::audio_sample32 const* audio_in = audio.mix(input, audio_route_output::filter, i);
    _filters[i].process_block(input, i, audio_in, audio.voice_filter[i].data());
  }

  // Run amp section.
  base::audio_sample32 const* audio_in = audio.mix(input, audio_route_output::amp, 0);
  _amp.process_block(input, release_sample, audio_in, audio.voice_amp.data());
  return amp_env_end;
} 
 
} // namespace svn::synth
