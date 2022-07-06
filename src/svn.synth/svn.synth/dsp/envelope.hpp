#ifndef SVN_SYNTH_DSP_ENVELOPE_HPP
#define SVN_SYNTH_DSP_ENVELOPE_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>

namespace svn::synth {

// Per-voice envelope generator.
class envelope
{
  float _sample_rate = 0.0f;
  std::int32_t _position = 0;
public:
  envelope() = default;
  explicit envelope(float sample_rate) : _sample_rate(sample_rate) {}
public:
  void setup_stages(base::automation_view const& automation, std::int32_t s,
    float bpm, float& delay, float& attack, float& hold, float& decay, float& release);
  // Returns less than sample count if done.
  std::int32_t process_block(voice_input const& input, std::int32_t index, float* cv_out);
private:
  float generate_slope(base::automation_view const& automation, 
    std::int32_t slope_param, std::int32_t mid_param, std::int32_t s, float stage_pos);
  float generate_stage(base::automation_view const& automation, std::int32_t s,
    float delay, float attack, float hold, float decay, float sustain, float release, bool& done);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_ENVELOPE_HPP