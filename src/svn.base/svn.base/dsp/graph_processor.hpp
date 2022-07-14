#ifndef SVN_BASE_DSP_GRAPH_PROCESSOR_HPP
#define SVN_BASE_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/dsp/io_types.hpp>
#include <svn.base/topology/topology_info.hpp>
#include <vector>
#include <cstdint>

namespace svn::base {
class graph_processor_base;
} // namespace svn::base

// Must be provided by the dsp project.
extern svn::base::graph_processor_base*
svn_create_graph_processor(svn::base::topology_info const* topology,
  std::int32_t part_type, std::int32_t graph_type, std::int32_t part_index);

namespace svn::base {

struct graph_point { float x, y; };

// Interface class so we don't need to deal with 
// graph specific types yet (e.g. audio vs cv data).
class graph_processor_base
{
public:
  virtual ~graph_processor_base() = 0 {}

  // Allow for some reuse between graphs.
  // Repaint if that parameter changes?
  virtual bool needs_repaint(std::int32_t runtime_param) const = 0;

  // This is the main entry point for the graph plot.
  // Data must be in [0, 1] even if bipolar.
  virtual std::vector<graph_point> const& plot(
    param_value const* state, float sample_rate, 
    float bpm, std::int32_t width, std::int32_t height, bool& bipolar) = 0;
};

// Renders pretty images.
template <class T>
class graph_processor: 
public graph_processor_base
{
  std::int32_t const _part_index;
  topology_info const* const _topology;

  std::vector<T> _raw_data;
  std::vector<float> _plot_data;
  std::vector<graph_point> _graph_data;
  std::vector<param_value*> _automation;
  std::vector<param_value> _automation_buffer;

  // Saves some typing in derived class headers.
protected:
  using param_value = svn::base::param_value;
  using block_input = svn::base::block_input;
  using topology_info = svn::base::topology_info;

protected:
  graph_processor(topology_info const* topology, std::int32_t part_index): 
  _automation(), _automation_buffer(), _part_index(part_index),
  _raw_data(), _plot_data(), _graph_data(), _topology(topology) {}

private:
  // Do the full dsp stuff without transforming to plot.
  std::vector<T> const& process_dsp(param_value const* state, float sample_rate, float bpm);

public:
  virtual ~graph_processor() {}
  std::int32_t part_index() const { return _part_index; }
  topology_info const* topology() const { return _topology; }

  std::vector<graph_point> const& plot(
    param_value const* state, float sample_rate,
    float bpm, std::int32_t width, std::int32_t height, bool& bipolar) override;

  // Need to know size up front.
  virtual std::int32_t sample_count(param_value const* state, float sample_rate, float bpm) const = 0;
  // Renders data in sample_count samples.
  virtual void process_dsp_core(block_input const& input, T* output, float sample_rate, float bpm) = 0;
  // Transforms raw data to plot in (0, 1).
  virtual void dsp_to_plot(param_value const* state, std::vector<T> const& dsp, std::vector<float>& plot, float sample_rate, bool& bipolar) = 0;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_GRAPH_PROCESSOR_HPP