#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/voice_amp.hpp>
#include <svn.synth/topology/topology.hpp>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

std::int32_t
voice_amp::process_block(voice_input const& input, std::int32_t release_sample,
  base::audio_sample32 const* audio_in, base::audio_sample32* audio_out)
{
  std::int32_t s;
  assert(release_sample >= -1);
  assert(release_sample < input.sample_count);
  automation_view automation(input.automation.rearrange_params(part_type::voice_amp, 0));

  for (s = 0; s < input.sample_count; s++)
  {
    float decay_level = 1.0f;
    if(s == release_sample) _released = 0;
    if (_released >= 0)
    {
      float decay_seconds = automation.get(voice_amp_param::decay, s).real;
      float decay_samples = decay_seconds * _sample_rate;
      if(_released >= decay_samples) return s;
      decay_level = 1.0f - (_released / decay_samples);
      _released++;
    } 
    float level = automation.get(voice_amp_param::level, s).real;
    audio_out[s] = sanity_audio(audio_in[s] * _velocity * decay_level * level);
  }
  return input.sample_count;
}

} // namespace svn::synth