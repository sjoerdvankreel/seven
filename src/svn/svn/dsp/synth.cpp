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
  input_buffer part_input;
  output_buffer part_output;

  part_input.bpm = input.bpm;
  part_input.notes = input.notes;
  part_input.note_count = input.note_count;
  part_input.sample_rate = input.sample_rate;
  part_input.sample_count = input.sample_count;

  for (std::int32_t i = 0; i < unit_count; i++)
  {
    //part_input.automation = topo
  }
}

} // namespace svn