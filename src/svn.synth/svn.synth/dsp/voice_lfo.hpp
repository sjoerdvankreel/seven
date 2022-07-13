#ifndef SVN_SYNTH_DSP_VOICE_LFO_HPP
#define SVN_SYNTH_DSP_VOICE_LFO_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>

namespace svn::synth {

// Per-voice lfo generator.
class voice_lfo
{
  float _phase = 0.0f;
  float _sample_rate = 0.0f;
public:
  voice_lfo() = default;
  explicit voice_lfo(float sample_rate) : _sample_rate(sample_rate) {}
public:
  void process_block(voice_input const& input, std::int32_t index, base::cv_sample* cv_out);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_LFO_HPP