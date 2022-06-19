#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.synth/dsp/graph_processor.hpp>
#include <svn.synth/topology/topology.hpp>
#include <cassert>

using namespace svn::base;

svn::base::graph_processor*
svn_create_graph_processor(
  svn::base::topology_info const* topology,
  std::int32_t part_type, std::int32_t part_index)
{
  switch (part_type)
  {
  case svn::synth::part_type::oscillator:
    assert(0 <= part_index && part_index < svn::synth::oscillator_count);
    return new svn::synth::oscillator_graph(topology, part_index);
  default:
    assert(false);
    return nullptr;
  }
}

namespace svn::synth {

bool 
oscillator_graph::needs_repaint(std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_bounds[part_type::oscillator][part_index()];
  return begin <= runtime_param && runtime_param < begin + oscillator_param::count;
}

std::int32_t 
oscillator_graph::sample_count(param_value const* state, float sample_rate) const
{
  std::int32_t begin = topology()->param_bounds[part_type::oscillator][part_index()];
  float cent = state[begin + oscillator_param::cent].real;
  std::int32_t note = state[begin + oscillator_param::note].discrete;
  std::int32_t octave = state[begin + oscillator_param::oct].discrete;
  float frequency = note_to_frequency(12 * (octave + 1) + note + cent);
  return static_cast<std::int32_t>(std::ceil(sample_rate / frequency)) + 4.0f;
}

void 
oscillator_graph::process_audio(block_input const& input, block_output& output, float sample_rate)
{
  voice_input vinput;
  vinput.bpm = input.bpm;
  vinput.sample_count = input.sample_count;
  vinput.stream_position = input.stream_position;
  vinput.automation = automation_view(
    topology(), nullptr, input.automation, topology()->input_param_count, 
    topology()->input_param_count, 0, input.sample_count, 0, input.sample_count);
  vinput.automation = vinput.automation.rearrange_params(part_type::oscillator, part_index());
  oscillator osc(sample_rate, midi_note_c4);
  osc.process_block(vinput, output.audio);
}

} // namespace svn::synth