#ifndef SVN_SYNTH_DSP_SUPPORT_HPP
#define SVN_SYNTH_DSP_SUPPORT_HPP

#include <svn.synth/topology/topology.hpp>
#include <svn.base/dsp/automation_view.hpp>
#include <cstdint>

namespace svn::synth {

// Voice block input.
struct voice_input
{
  float bpm;
  std::int32_t sample_count;
  std::int64_t stream_position;
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
  double vamp;
  double total;
  double audio;
  double vfilter;
};

} // namespace svn::base
#endif // SVN_SYNTH_DSP_SUPPORT_HPP