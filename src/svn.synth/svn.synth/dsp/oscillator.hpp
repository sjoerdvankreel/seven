#ifndef SVN_SYNTH_DSP_OSCILLATOR_HPP
#define SVN_SYNTH_DSP_OSCILLATOR_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/audio_sample.hpp>

#include <array>

namespace svn::synth {

// Per-voice unit generator.
class oscillator
{
  float _sample_rate = 0.0f;
  std::int32_t _midi_note = -1;
  std::array<float, oscillator_max_voices> _phases;

  static inline float const min_pw = 0.05;

public:
  oscillator() = default;
  oscillator(float sample_rate, std::int32_t midi_note);
  double process_block(voice_input const& input, std::int32_t index, cv_state& cv, base::audio_sample32* audio_out, double& mod_time);

private:
  float generate_blep_saw(float phase, float increment) const;
  float generate_poly_blep(float phase, float increment) const;
  float generate_poly_blamp(float phase, float increment) const;
  float generate_blamp_triangle(float phase, float increment) const;
  float generate_dsf(svn::base::automation_view const& automation, float const* const* modulated, std::int32_t sample, float phase, float frequency) const;
  float generate_analog(svn::base::automation_view const& automation, float const* const* modulated, std::int32_t sample, float phase, float increment) const;
  float generate_blep_pulse(svn::base::automation_view const& automation, float const* const* modulated, std::int32_t sample, float phase, float increment) const;
  float generate_wave(svn::base::automation_view const& automation, float const* const* modulated, std::int32_t sample, float phase, float frequency, float increment) const;
  base::audio_sample32 generate_unison(svn::base::automation_view const& automation, float const* const* modulated, std::int32_t s, float midi, float frequency, float panning);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_OSCILLATOR_HPP