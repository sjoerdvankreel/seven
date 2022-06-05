#ifndef SVN_SYNTH_DSP_VOICE_AMP_HPP
#define SVN_SYNTH_DSP_VOICE_AMP_HPP

#include <svn.base/dsp/audio_sample.hpp>
#include <cstdint>

namespace svn::synth {

class voice_amp
{
  float _velocity = 0.0f;
  float _sample_rate = 0.0f;
  std::int32_t _released = -1;

public:
  voice_amp() = default;
  voice_amp(float sample_rate, float velocity);

  std::int32_t process_block(
    struct voice_input const& input,
    base::audio_sample* audio,
    std::int32_t release_sample);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_AMP_HPP