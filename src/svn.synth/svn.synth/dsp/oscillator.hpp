#ifndef SVN_SYNTH_DSP_OSCILLATOR_HPP
#define SVN_SYNTH_DSP_OSCILLATOR_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/audio_sample.hpp>

#include <array>

namespace svn::synth {

// Per-voice unit generator.
class oscillator
{
  float _phase = 0.0f;
  float _sample_rate = 0.0f;
  std::int32_t _midi_note = -1;
  std::array<float, oscillator_max_spread> _phases;

  static inline float const min_pw = 0.05;

private:
  float generate_blep_saw(float phase, float increment) const;
  float generate_poly_blep(float phase, float increment) const;
  float generate_poly_blamp(float phase, float increment) const;
  float generate_blamp_triangle(float phase, float increment) const;
  float generate_dsf(svn::base::automation_view const& automation, std::int32_t sample, float frequency) const;
  float generate_wave(svn::base::automation_view const& automation, std::int32_t sample, float midi, float frequency);
  float generate_analog_spread(svn::base::automation_view const& automation, std::int32_t sample, float midi, float frequency);
  float generate_analog(svn::base::automation_view const& automation, std::int32_t sample, float phase, float increment) const;
  float generate_blep_pulse(svn::base::automation_view const& automation, std::int32_t sample, float phase, float increment) const;

public:
  oscillator() = default;
  oscillator(float sample_rate, std::int32_t midi_note);
  void process_block(voice_input const& input, std::int32_t index, base::audio_sample32* audio_out);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_OSCILLATOR_HPP