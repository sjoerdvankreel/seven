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
audio_processor(topology, sample_rate, max_sample_count, state),
_part_audio(static_cast<std::size_t>(max_sample_count)),
_oscillators()
{ _oscillators.fill(oscillator(sample_rate)); }

void
seven_synth::process_block(block_input const& input,
  param_value* state, audio_sample* audio)
{
  block_input part_input = input;
  for (std::int32_t i = 0; i < oscillator_count; i++)
  {
    clear_audio(_part_audio.data(), input.global.sample_count);
    std::int32_t offset = topology().bounds[part_type::oscillator][i];
    part_input.automation = input.automation + offset;
    _oscillators[i].process_block(part_input, _part_audio.data(), state + offset);
    add_audio(audio, _part_audio.data(), input.global.sample_count);
  }
}

} // namespace svn::synth