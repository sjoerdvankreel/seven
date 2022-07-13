#ifndef SVN_SYNTH_DSP_VOICE_AMP_HPP
#define SVN_SYNTH_DSP_VOICE_AMP_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <cstdint>

namespace svn::synth {

// Per-voice amp section.
class voice_amp
{
  float _sample_rate = 0.0f;
public:
  voice_amp() = default;
  explicit voice_amp(float sample_rate): _sample_rate(sample_rate) {}
  void process_block(voice_input const& input, float const* env1_cv, 
    base::audio_sample32 const* audio_in, base::audio_sample32* audio_out);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_AMP_HPP