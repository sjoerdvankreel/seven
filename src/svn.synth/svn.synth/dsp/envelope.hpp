#ifndef SVN_SYNTH_DSP_ENVELOPE_HPP
#define SVN_SYNTH_DSP_ENVELOPE_HPP

#include <svn.synth/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>

namespace svn::synth {

// Per-voice envelope generator.
class envelope
{
  float _sample_rate = 0.0f;
public:
  envelope() = default;
  envelope(float sample_rate): _sample_rate(sample_rate) {}
  void process_block(voice_input const& input, std::int32_t index, float* cv_out);
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_ENVELOPE_HPP