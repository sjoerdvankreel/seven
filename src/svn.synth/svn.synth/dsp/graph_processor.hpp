#ifndef SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP
#define SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/graph_processor.hpp>
#include <vector>

namespace svn::synth {

class oscillator_wave_graph:
public svn::base::graph_processor
{
  std::vector<base::audio_sample32> _audio_out;
public:
  oscillator_wave_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor(topology, part_index), _audio_out() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t audio_sample_count(param_value const* state, float sample_rate) const override;
  void process_audio_core(block_input const& input, block_output& output, float sample_rate) override;
  void audio_to_plot(std::vector<audio_sample32> const& audio, std::vector<float>& plot, float sample_rate) override;
};

class oscillator_spectrum_graph:
public svn::base::graph_processor
{
  std::vector<float> _mono;
  oscillator_wave_graph _wave;
  svn::base::spectrum_analyzer _analyzer;

public:
  oscillator_spectrum_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor(topology, part_index), _mono(), _wave(topology, part_index), _analyzer() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t audio_sample_count(param_value const* state, float sample_rate) const override;
  void process_audio_core(block_input const& input, block_output& output, float sample_rate) override;
  void audio_to_plot(std::vector<audio_sample32> const& audio, std::vector<float>& plot, float sample_rate) override;
};

class filter_ir_graph:
public svn::base::graph_processor
{
  std::vector<base::audio_sample32> _audio_in;
  std::vector<base::audio_sample32> _audio_out;
public:
  filter_ir_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor(topology, part_index), _audio_in(), _audio_out() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t audio_sample_count(param_value const* state, float sample_rate) const override;
  void process_audio_core(block_input const& input, block_output& output, float sample_rate) override;
  void audio_to_plot(std::vector<audio_sample32> const& audio, std::vector<float>& plot, float sample_rate) override;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP