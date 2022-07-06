#ifndef SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP
#define SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/graph_processor.hpp>
#include <vector>

namespace svn::synth {

class envelope_graph:
public svn::base::graph_processor<float>
{
  static inline std::int32_t constexpr env_graph_rate = 100.0f;

private:
  void setup_stages(param_value const* const* automation, float sample_rate, float bpm, 
    float& delay, float& attack, float& hold, float& decay, float& release) const;

public:
  envelope_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<float>(topology, part_index) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void dsp_to_plot(std::vector<float> const& dsp, std::vector<float>& plot, float sample_rate) override;
  void process_dsp_core(block_input const& input, float* output, float sample_rate, float bpm) override;
};

class oscillator_wave_graph:
public svn::base::graph_processor<base::audio_sample32>
{
public:
  oscillator_wave_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::audio_sample32>(topology, part_index) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void dsp_to_plot(std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate) override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
};

class oscillator_spectrum_graph:
public svn::base::graph_processor<base::audio_sample32>
{
  std::vector<float> _mono;
  oscillator_wave_graph _wave;
  svn::base::spectrum_analyzer _analyzer;

public:
  oscillator_spectrum_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::audio_sample32>(topology, part_index), 
  _mono(), _wave(topology, part_index), _analyzer() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
  void dsp_to_plot(std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate) override;
};

class filter_ir_graph:
public svn::base::graph_processor<base::audio_sample32>
{
  std::vector<base::audio_sample32> _audio_in;
public:
  filter_ir_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::audio_sample32>(topology, part_index), _audio_in() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void dsp_to_plot(std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate) override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP