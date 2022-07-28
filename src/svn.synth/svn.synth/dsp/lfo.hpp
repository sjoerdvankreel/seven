#ifndef SVN_SYNTH_DSP_LFO_HPP
#define SVN_SYNTH_DSP_LFO_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/support.hpp>
#include <svn.synth/dsp/cv_state.hpp>
#include <svn.synth/topology/topology.hpp>

namespace svn::synth {

// Per-voice lfo generator.
class lfo
{
  float _phase = 0.0f;
  float _sample_rate = 0.0f;
public:
  lfo() = default;
  explicit lfo(float sample_rate) : _sample_rate(sample_rate) {}
public:
  double process_block(voice_input const& input, std::int32_t index, cv_state& cv, base::cv_sample* cv_out, double& cv_time);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_LFO_HPP