#ifndef SVN_SYNTH_DSP_VOICE_OSCILLATOR_HPP
#define SVN_SYNTH_DSP_VOICE_OSCILLATOR_HPP

#include <svn.base/dsp/audio_sample.hpp>

namespace svn::synth {

class voice_oscillator
{
  float _phase = 0.0f;
  float _sample_rate = 0.0f;
  std::int32_t _midi_note = -1;

public:
  void process_block(
    struct voice_input const& input,
    base::audio_sample* audio);

  voice_oscillator() = default;
  voice_oscillator(
    float sample_rate, std::int32_t midi_note);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_OSCILLATOR_HPP