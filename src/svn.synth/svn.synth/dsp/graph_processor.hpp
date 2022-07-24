#ifndef SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP
#define SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/graph_processor.hpp>
#include <vector>

namespace svn::synth {

class envelope_graph:
public svn::base::graph_processor<base::cv_sample>
{
  static inline float constexpr env_graph_rate = 1000.0f;

private:
  void setup_stages(param_value const* const* automation, float bpm, 
    float& delay, float& attack, float& hold, float& decay, float& release) const;

public:
  envelope_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::cv_sample>(topology, part_index) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::cv_sample* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::cv_sample> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

class lfo_graph:
public svn::base::graph_processor<base::cv_sample>
{
  static inline float constexpr lfo_graph_rate = 1000.0f;

public:
  lfo_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::cv_sample>(topology, part_index) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::cv_sample* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::cv_sample> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

class amp_graph:
public svn::base::graph_processor<base::cv_sample>
{
  envelope_graph _env;

public:
  amp_graph(topology_info const* topology) :
  svn::base::graph_processor<base::cv_sample>(topology, 0), _env(topology, 0) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  base::param_value transform_param(std::int32_t rt_index, param_value value) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::cv_sample* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::cv_sample> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

class cv_route_graph:
public svn::base::graph_processor<float>
{
  static inline float constexpr cv_route_graph_rate = 1000.0f;
  static inline float constexpr cv_route_graph_velocity = 0.5f;

public:
  cv_route_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<float>(topology, part_index) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, float* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<float> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

class oscillator_wave_graph:
public svn::base::graph_processor<base::audio_sample32>
{
public:
  oscillator_wave_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::audio_sample32>(topology, part_index) {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  base::param_value transform_param(std::int32_t rt_index, param_value value) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
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
  base::param_value transform_param(std::int32_t rt_index, param_value value) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

class filter_ir_graph:
public svn::base::graph_processor<base::audio_sample32>
{
  std::vector<base::audio_sample32> _audio_in;
public:
  filter_ir_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::audio_sample32>(topology, part_index), _audio_in() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  base::param_value transform_param(std::int32_t rt_index, param_value value) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

class filter_fr_graph:
public svn::base::graph_processor<base::audio_sample32>
{
  base::fft _fft;
  filter_ir_graph _ir;
  std::vector<float> _mono;
public:
  filter_fr_graph(topology_info const* topology, std::int32_t part_index) :
  svn::base::graph_processor<base::audio_sample32>(topology, part_index), _fft(), _ir(topology, part_index), _mono() {}

  bool needs_repaint(std::int32_t runtime_param) const override;
  base::param_value transform_param(std::int32_t rt_index, param_value value) const override;
  std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const override;
  void process_dsp_core(block_input const& input, base::audio_sample32* output, float sample_rate, float bpm) override;
  void dsp_to_plot(base::param_value const* state, std::vector<base::audio_sample32> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) override;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_GRAPH_PROCESSOR_HPP