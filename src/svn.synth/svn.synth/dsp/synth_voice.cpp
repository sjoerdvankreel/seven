#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/synth_voice.hpp>

#include <cassert>
#include <algorithm>

namespace svn::synth {

synth_voice::
synth_voice(
  base::topology_info const* topology,
  float sample_rate, float velocity, std::int32_t midi_note):
_envelopes(), _filters(), _oscillators(), _lfos(),
_topology(topology), _velocity(velocity), _amp(sample_rate)
{
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  for(std::int32_t i = 0; i < voice_filter_count; i++)
    new(&_filters[i]) voice_filter(sample_rate, midi_note);
  _lfos.fill(voice_lfo(sample_rate));
  _envelopes.fill(envelope(sample_rate));
  _oscillators.fill(oscillator(sample_rate, midi_note));
}

bool
synth_voice::process_block(
  voice_input const& input, cv_state& cv, audio_state& audio, std::int32_t release_sample)
{
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  // Run lfo's.
  for(std::int32_t i = 0; i < voice_lfo_count; i++)
    _lfos[i].process_block(input, i, cv.voice_lfo[i].data());

  // Run envelopes.
  bool ended = false;
  for (std::int32_t i = 0; i < envelope_count; i++)
  {
    std::memset(cv.envelope[i].data(), 0, input.sample_count * sizeof(base::cv_sample));
    bool env_ended = _envelopes[i].process_block(input, i, cv.envelope[i].data(), release_sample);
    if(i == 0) ended = env_ended;
  }

  // Run generators.
  for (std::int32_t i = 0; i < oscillator_count; i++)
    _oscillators[i].process_block(input, i, cv, audio.oscillator[i].data());

  // Clear filter output in case user selected weird routing (i.e. filter 3 to filter 2).
  for (std::int32_t i = 0; i < voice_filter_count; i++)
    std::memset(audio.voice_filter[i].data(), 0, input.sample_count * sizeof(base::audio_sample32));

  // Run filters.
  for (std::int32_t i = 0; i < voice_filter_count; i++)
  {
    base::audio_sample32 const* audio_in = audio.mix(input, audio_route_output::filter, i);
    _filters[i].process_block(input, i, audio_in, audio.voice_filter[i].data());
  }

  // Run amp section.
  base::audio_sample32 const* audio_in = audio.mix(input, audio_route_output::amp, 0);
  _amp.process_block(input, cv.envelope[0].data(), audio_in, audio.voice_amp.data());
  return ended;
} 
 
} // namespace svn::synth
