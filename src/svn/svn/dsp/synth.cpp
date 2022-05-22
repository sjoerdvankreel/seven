#include <svn/dsp/synth.hpp>
#include <svn/support/topo_rt.hpp>
#include <cstdint>

namespace svn {

synth::
synth(std::int32_t max_sample_count):
_part_audio(static_cast<size_t>(max_sample_count)) {}

void
synth::process(input_buffer const& input, output_buffer& output)
{
  output_buffer part_output;
  part_output.audio = _part_audio.data();
  output.clear_audio(input.sample_count);

  input_buffer part_input;
  part_input.bpm = input.bpm;
  part_input.notes = input.notes;
  part_input.note_count = input.note_count;
  part_input.sample_rate = input.sample_rate;
  part_input.sample_count = input.sample_count;

  for (std::int32_t i = 0; i < unit_count; i++)
  {
    std::int32_t offset = synth_bounds[part_type::unit][i];
    part_input.automation = input.automation + offset;
    part_output.param_values = output.param_values + offset;
    _units[i].process(part_input, part_output);
    output.add_audio(part_output.audio, input.sample_count);
  }
}

} // namespace svn