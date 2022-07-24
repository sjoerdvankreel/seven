#ifndef SVN_SYNTH_DSP_AMPLITUDE_HPP
#define SVN_SYNTH_DSP_AMPLITUDE_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <cstdint>

namespace svn::synth {

// Per-voice amp section.
class amplitude
{
  float _sample_rate = 0.0f;
public:
  amplitude() = default;
  explicit amplitude(float sample_rate): _sample_rate(sample_rate) {}
  double process_block(voice_input const& input, cv_state const& cv, base::cv_sample const* env1_cv,
    base::audio_sample32 const* audio_in, base::audio_sample32* audio_out, double& mod_time);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_AMPLITUDE_HPP