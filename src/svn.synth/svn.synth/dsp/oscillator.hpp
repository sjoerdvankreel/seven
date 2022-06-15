#ifndef SVN_SYNTH_DSP_OSCILLATOR_HPP
#define SVN_SYNTH_DSP_OSCILLATOR_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.base/dsp/audio_sample.hpp>

namespace svn::synth {

// Per-voice unit generator.
class oscillator
{
  float _phase = 0.0f;
  float _sample_rate = 0.0f;
  std::int32_t _midi_note = -1;

public:
  oscillator() = default;
  oscillator(float sample_rate, std::int32_t midi_note) :
  _sample_rate(sample_rate), _midi_note(midi_note) {}

  void process_block(voice_input const& input, base::audio_sample* audio);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_OSCILLATOR_HPP