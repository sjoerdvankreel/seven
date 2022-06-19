#ifndef SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP
#define SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/graph_processor.hpp>

namespace svn::synth {

class oscillator_graph:
public svn::base::graph_processor
{
  using block_input = svn::base::block_input;
  using block_output = svn::base::block_output;
  using topology_info = svn::base::topology_info;

public:
  virtual bool needs_repaint(std::int32_t runtime_param) const override;

public:
  oscillator_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor(topology, part_index) {}

protected:
  virtual std::int32_t sample_count(float sample_rate) const override;
  virtual void process_audio(block_input const& input, block_output& output, float sample_rate) override;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP