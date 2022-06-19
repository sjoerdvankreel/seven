#ifndef SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP
#define SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/graph_processor.hpp>

namespace svn::synth {

class oscillator_graph:
public svn::base::graph_processor
{
public:
  virtual bool needs_repaint(std::int32_t runtime_param) const override;
public:
  oscillator_graph(svn::base::topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor(topology, part_index) {}
protected:
  virtual void process_audio(std::vector<float>& data, float sample_rate) override;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP