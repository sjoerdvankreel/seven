#ifndef SVN_SYNTH_DSP_SUPPORT_HPP
#define SVN_SYNTH_DSP_SUPPORT_HPP

#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/automation_view.hpp>
#include <cstdint>

namespace svn::synth {

// Voice block input.
struct voice_input
{
  float bpm = 0.0f;
  std::int32_t sample_count = 0;
  std::int64_t stream_position = 0;
  base::automation_view automation;
};

// Cpu usage time in seconds.
struct cpu_usage
{
  double cv;
  double aux;
  double lfo;
  double env;
  double osc;
  double amp;
  double total;
  double audio;
  double filter;
};

} // namespace svn::base
#endif // SVN_SYNTH_DSP_SUPPORT_HPP