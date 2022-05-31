#ifndef SVN_SYNTH_DSP_OSCILLATOR_HPP
#define SVN_SYNTH_DSP_OSCILLATOR_HPP

#include <svn.base/dsp/note_event.hpp>
#include <svn.base/dsp/input_buffer.hpp>
#include <svn.base/dsp/audio_sample.hpp>
#include <svn.base/support/param_value.hpp>

namespace svn::synth {

class oscillator
{
  float _phase = 0.0f;
  float _frequency = 0.0f;
  float _sample_rate = 0.0f;
  std::int32_t _midi_note = base::note_none;

public:
  oscillator() = default;
  oscillator(float sample_rate);

  void process_block(
    base::input_buffer const& input, 
    base::audio_sample* audio, base::param_value* state);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_OSCILLATOR_HPP