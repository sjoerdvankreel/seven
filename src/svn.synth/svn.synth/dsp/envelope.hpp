#ifndef SVN_SYNTH_DSP_ENVELOPE_HPP
#define SVN_SYNTH_DSP_ENVELOPE_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/support.hpp>
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
public:
  // True if done.
  double process_block(voice_input const& input, std::int32_t index,
    base::cv_sample* cv_out, std::int32_t release_sample, bool& result);
  void setup_stages(base::automation_view const& automation, std::int32_t s,
    float bpm, float& delay, float& attack, float& hold, float& decay, float& release);
private:
  float generate_slope(base::automation_view const& automation,
    std::int32_t slope_param, std::int32_t mid_param, std::int32_t s, float stage_pos);
  std::pair<envelope_stage, float> generate_stage(base::automation_view const& automation,
    std::int32_t s, bool dahdsr, float delay, float attack, float hold, float decay, float sustain, float release);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_ENVELOPE_HPP