#ifndef SVN_BASE_DSP_GRAPH_PROCESSOR_HPP
#define SVN_BASE_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/io_types.hpp>
#include <svn.base/topology/topology_info.hpp>
#include <vector>
#include <cstdint>

namespace svn::base {
class graph_processor;
} // namespace svn::base

// Must be provided by the dsp project.
extern svn::base::graph_processor*
svn_create_graph_processor(svn::base::topology_info const* topology,
  std::int32_t part_type, std::int32_t graph_type, std::int32_t part_index);

namespace svn::base {

struct graph_point { float x, y; };

// Renders pretty images. Need not be audio.
class graph_processor
{
  std::int32_t const _part_index;
  topology_info const* const _topology;
  std::vector<float> _plot_data;
  std::vector<graph_point> _graph_data;
  std::vector<audio_sample32> _audio_data;
  std::vector<param_value*> _automation;
  std::vector<param_value> _automation_buffer;
  std::vector<param_value> _output_param_buffer;

  // Saves some typing in derived class headers.
protected:
  using param_value = svn::base::param_value;
  using block_input = svn::base::block_input;
  using block_output = svn::base::block_output;
  using topology_info = svn::base::topology_info;
  using audio_sample32 = svn::base::audio_sample32;

protected:
  graph_processor(topology_info const* topology, std::int32_t part_index): 
  _automation(), _automation_buffer(), _part_index(part_index),
  _output_param_buffer(static_cast<std::size_t>(topology->output_param_count)),
  _plot_data(), _audio_data(), _graph_data(), _topology(topology) {}

private:
  // Do the full audio dsp stuff without transforming to plot.
  std::vector<audio_sample32> const& process_audio(param_value const* state, float sample_rate);

public:
  virtual ~graph_processor() {}
  std::int32_t part_index() const { return _part_index; }
  topology_info const* topology() const { return _topology; }

  // Allow for some reuse between graphs.
  // Repaint if that parameter changes?
  virtual bool needs_repaint(std::int32_t runtime_param) const = 0;
  // Need to know audio size up front.
  virtual std::int32_t audio_sample_count(param_value const* state, float sample_rate) const = 0;
  // Renders data in sample_count audio samples.
  virtual void process_audio_core(block_input const& input, block_output& output, float sample_rate) = 0;
  // Transforms audio to plot in (0, 1).
  virtual void audio_to_plot(std::vector<audio_sample32> const& audio, std::vector<float>& plot, float sample_rate) = 0;

  // This is the main entry point for the graph plot.
  std::vector<graph_point> const& plot(param_value const* state, float sample_rate, std::int32_t width, std::int32_t height);
};

} // namespace svn::base
#endif // SVN_BASE_DSP_GRAPH_PROCESSOR_HPP