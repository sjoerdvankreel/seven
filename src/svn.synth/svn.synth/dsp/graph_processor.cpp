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
oscillator_graph::sample_count(float sample_rate) const
{
  return 0;
}

void 
oscillator_graph::process_audio(block_input const& input, block_output& output, float sample_rate)
{
  
}

} // namespace svn::synth