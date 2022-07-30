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

private:
  template <class sample_generator_type>
  void generate_unison(voice_input const& input, std::int32_t unison_voices, std::int32_t midi, 
    float const* const* transformed_cv, sample_generator_type sample_generator, base::audio_sample32* audio_out);

public:
  oscillator() = default;
  oscillator(float sample_rate, std::int32_t midi_note);
  double process_block(voice_input const& input, std::int32_t index, cv_state& cv, base::audio_sample32* audio_out, double& cv_time);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_OSCILLATOR_HPP