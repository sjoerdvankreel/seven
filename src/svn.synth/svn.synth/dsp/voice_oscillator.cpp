#include <svn.base/dsp/dsp.hpp>
#include <svn.base/dsp/block_input.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.synth/dsp/voice_input.hpp>
#include <svn.synth/dsp/voice_oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

voice_oscillator::
voice_oscillator(float sample_rate, 
  float velocity, std::int32_t midi_note):
_velocity(velocity),
_sample_rate(sample_rate),
_frequency(base::note_to_frequency(static_cast<float>(midi_note)))
{
  assert(sample_rate > _frequency);
  assert(0 <= midi_note && midi_note < 128);
  assert(0.0f <= velocity && velocity <= 1.0f);
}

std::int32_t
voice_oscillator::process_block(
  voice_input const& input, 
  audio_sample* audio,
  std::int32_t release_sample)
{
  assert(audio != nullptr);
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  std::int32_t s;
  float decay_level = 1.0f;
  for (s = 0; s < input.sample_count; s++)
  {
    if(s == release_sample) _released = 0;
    if (_released >= 0)
    {
      float decay_seconds = input.automation.get(oscillator_param::decay, s).real;
      float decay_samples = decay_seconds * _sample_rate;
      if(_released >= decay_samples) return s;
      decay_level = 1.0f - (_released / decay_samples);
      _released++;
    }

    float level = input.automation.get(oscillator_param::level, s).real;
    float panning = input.automation.get(oscillator_param::panning, s).real;
    float sample = std::sin(2.0f * std::numbers::pi * _phase);
    float scaled = base::sanity(_velocity * decay_level * level * sample);
    audio[s].left = (1.0f - panning) * scaled;
    audio[s].right = panning * scaled;
    _phase += _frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }

  return input.sample_count;
}

} // namespace svn::synth