#ifndef SVN_SYNTH_DSP_ENVELOPE_HPP
#define SVN_SYNTH_DSP_ENVELOPE_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.synth/topology/topology.hpp>

namespace svn::synth {

struct envelope_stage_t { enum value { delay, attack, hold, decay, sustain, release, end }; };
typedef envelope_stage_t::value envelope_stage;

// Per-voice envelope generator.
class envelope
{
  bool _ended = false;
  bool _released = false;
  float _sample_rate = 0.0f;
  std::int32_t _position = 0;
  float _release_level = 0.0f;
  base::cv_sample _end_sample = {};

public:
  envelope() = default;
  explicit envelope(float sample_rate) : _sample_rate(sample_rate) {}

private:
  float generate_slope(std::int32_t slope, float stage_pos, float exp);

public:
  void setup_stages(base::automation_view const& automation, float const* const* transformed_cv,
    float bpm, float& delay, float& attack, float& hold, float& decay, float& release);
  double process_block(voice_input const& input, std::int32_t index,
    cv_state& cv, base::cv_sample* cv_out, std::int32_t release_sample, bool& ended, double& cv_time);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_ENVELOPE_HPP