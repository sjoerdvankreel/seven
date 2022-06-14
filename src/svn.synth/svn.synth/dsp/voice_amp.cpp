#include <svn.synth/dsp/voice_amp.hpp>
#include <svn.synth/dsp/voice_input.hpp>
#include <svn.synth/static/topology.hpp>

#include <cassert>

using namespace svn::base;

namespace svn::synth {

voice_amp::
voice_amp(float sample_rate, float velocity) :
_velocity(velocity),
_sample_rate(sample_rate)
{ 
  assert(sample_rate > 0.0f);
  assert(0.0f <= velocity && velocity <= 1.0f); 
} 

std::int32_t
voice_amp::process_block(voice_input const& input, 
  audio_sample* audio, std::int32_t release_sample)
{
  assert(audio != nullptr);
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);

  std::int32_t s;
  for (s = 0; s < input.sample_count; s++)
  {
    float decay_level = 1.0f;
    if(s == release_sample) _released = 0;
    if (_released >= 0)
    {
      float decay_seconds = input.automation.get(voice_amp_param::decay, s).real;
      float decay_samples = decay_seconds * _sample_rate;
      if(_released >= decay_samples) return s;
      decay_level = 1.0f - (_released / decay_samples);
      _released++;
    }
    float amp = input.automation.get(voice_amp_param::amp, s).real;
    audio[s] *= _velocity * decay_level * amp;
  }

  return input.sample_count;
}

} // namespace svn::synth