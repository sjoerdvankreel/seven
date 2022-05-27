#include <svn.base/dsp/dsp.hpp>
#include <svn.synth/dsp/seven_synth.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

seven_synth::
seven_synth(
  struct base::runtime_topology const* topology, float sample_rate,
  std::int32_t max_sample_count, base::param_value* state):
audio_processor(
  processor_type::synth, topology, sample_rate, max_sample_count, state),
_oscillators() {}

void
seven_synth::process_block(
  runtime_topology const& topology, input_buffer const& input,
  audio_sample* audio, audio_sample* part_audio, param_value* state)
{
  input_buffer part_input = input;
  for (std::int32_t i = 0; i < oscillator_count; i++)
  {
    std::int32_t offset = topology.bounds[part_type::oscillator][i];
    part_input.automation = input.automation + offset;
    _oscillators[i].process_block(part_input, part_audio, state + offset);
    add_audio(audio, part_audio, input.sample_count);
  }
}

} // namespace svn::synth