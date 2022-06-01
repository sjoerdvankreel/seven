#include <svn.base/dsp/block_input.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.synth/dsp/voice_oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

voice_oscillator::
voice_oscillator(float sample_rate, float frequency):
_frequency(frequency), 
_sample_rate(sample_rate)
{ 
  assert(frequency > 0.0f);
  assert(sample_rate > frequency);
}

std::int32_t
voice_oscillator::process_block(
  block_input const& input, 
  audio_sample* audio,
  std::int32_t release_sample)
{
  assert(audio != nullptr);
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  std::int32_t s;
  float decay_level = 1.0f;
  float* level = static_cast<float*>(input.automation[oscillator_param::level]);
  float* decay = static_cast<float*>(input.automation[oscillator_param::decay]);
  float* panning = static_cast<float*>(input.automation[oscillator_param::panning]);

  for (s = 0; s < input.sample_count; s++)
  {
    if(s == release_sample) _released = 0;
    if (_released >= 0)
    {
      float decay_samples = decay[s] * _sample_rate;
      if(_released >= decay_samples) return s;
      decay_level = 1.0f - (_released / decay_samples);
      _released++;
    }

    float sample = level[s] * std::sin(2.0f * std::numbers::pi * _phase);
    audio[s].left = (1.0f - panning[s]) * sample;
    audio[s].right = panning[s] * sample;
    _phase += _frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }

  return input.sample_count;
}

} // namespace svn::synth