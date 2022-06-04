#include <svn.base/dsp/dsp.hpp>
#include <svn.synth/dsp/seven_synth.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

seven_synth::
seven_synth(
  base::runtime_topology const* topology, float sample_rate,
  std::int32_t max_sample_count, base::param_value* state):
audio_processor(topology, sample_rate, max_sample_count, state),
_automation_scratch(topology, max_sample_count),
_audio_scratch(static_cast<std::size_t>(max_sample_count)),
_voices(),
_voice_states()
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);
}

void
seven_synth::process_block(
  block_input const& input, audio_sample* audio)
{
  
}

} // namespace svn::synth