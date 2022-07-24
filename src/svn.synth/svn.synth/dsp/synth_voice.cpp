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
_topology(topology), _velocity(velocity), _amplitude(sample_rate)
{
  assert(topology != nullptr);  
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
  for(std::int32_t i = 0; i < filter_count; i++)
    new(&_filters[i]) filter(sample_rate, midi_note);
  _lfos.fill(lfo(sample_rate));
  _envelopes.fill(envelope(sample_rate));
  _oscillators.fill(oscillator(sample_rate, midi_note));
}

bool
synth_voice::process_block(voice_input const& input, cv_state& cv, 
  audio_state& audio, std::int32_t release_sample, cpu_usage& usage)
{
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  // Set velocity.
  std::fill(cv.velocity.begin(), cv.velocity.begin() + input.sample_count, base::cv_sample({_velocity, false}));

  // Run lfo's.
  for(std::int32_t i = 0; i < lfo_count; i++)
    usage.lfo += _lfos[i].process_block(input, i, cv.lfo[i].data());

  // Run envelopes.
  bool ended = false;
  bool env_ended = false;
  for (std::int32_t i = 0; i < envelope_count; i++)
  {
    std::memset(cv.envelope[i].data(), 0, input.sample_count * sizeof(base::cv_sample));
    usage.env += _envelopes[i].process_block(input, i, cv.envelope[i].data(), release_sample, env_ended);
    if(i == 0) ended = env_ended;
  }

  // Run generators.
  for (std::int32_t i = 0; i < oscillator_count; i++)
    usage.osc += _oscillators[i].process_block(input, i, cv, audio.oscillator[i].data(), usage.cv);

  // Clear filter output in case user selected weird routing (i.e. filter 3 to filter 2).
  for (std::int32_t i = 0; i < filter_count; i++)
    std::memset(audio.filter[i].data(), 0, input.sample_count * sizeof(base::audio_sample32));

  // Run filters.
  for (std::int32_t i = 0; i < filter_count; i++)
  {
    base::audio_sample32 const* audio_in;
    usage.audio += audio.mix(input, audio_route_output::filter, i, audio_in);
    usage.filter += _filters[i].process_block(input, i, cv, audio_in, audio.filter[i].data(), usage.cv);
  }

  // Run amp section.
  base::audio_sample32 const* audio_in;
  usage.audio += audio.mix(input, audio_route_output::amp, 0, audio_in);
  usage.amp += _amplitude.process_block(input, cv, cv.envelope[0].data(), audio_in, audio.amplitude.data(), usage.cv);
  return ended;
}  
 
} // namespace svn::synth
