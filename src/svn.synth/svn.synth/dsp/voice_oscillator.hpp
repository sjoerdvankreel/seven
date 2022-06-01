#ifndef SVN_SYNTH_DSP_VOICE_OSCILLATOR_HPP
#define SVN_SYNTH_DSP_VOICE_OSCILLATOR_HPP

#include <svn.base/dsp/note_event.hpp>
#include <svn.base/dsp/block_input.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/support/param_value.hpp>

namespace svn::synth {

class voice_oscillator
{
  float _phase = 0.0f;
  float _frequency = 0.0f;
  float _sample_rate = 0.0f;
  std::int32_t _released = -1;

public:
  voice_oscillator() = default;
  voice_oscillator(float sample_rate, float frequency);

  std::int32_t process_block(
    base::block_input const& input, 
    base::audio_sample* audio,
    std::int32_t release_sample);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_OSCILLATOR_HPP