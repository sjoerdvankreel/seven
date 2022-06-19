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
svn_create_graph_processor(
  svn::base::topology_info const* topology,
  std::int32_t part_type, std::int32_t part_index);

namespace svn::base {

struct graph_point { float x, y; };

class graph_processor
{
  std::int32_t const _part_index;
  topology_info const* const _topology;
  std::vector<graph_point> _graph_data;
  std::vector<audio_sample> _audio_data;
  std::vector<param_value*> _automation;
  std::vector<param_value> _automation_buffer;
  std::vector<param_value> _output_param_buffer;

public:
  virtual ~graph_processor() {}
  graph_processor(topology_info const* topology, std::int32_t part_index): 
  _automation(), _automation_buffer(), 
  _output_param_buffer(static_cast<std::size_t>(topology->output_param_count)),
  _part_index(part_index), _audio_data(), _graph_data(), _topology(topology) {}

public:
  virtual bool needs_repaint(std::int32_t runtime_param) const = 0;
  std::vector<graph_point> const&
    process(param_value const* state, std::int32_t width, std::int32_t height, float sample_rate);

protected:
  std::int32_t part_index() const { return _part_index; }
  topology_info const* topology() const { return _topology; }
  // Must know how many we render up front.
  virtual std::int32_t sample_count(param_value const* state, float sample_rate) const = 0;
  // Renders audio in (-1, 1).
  // Translation to ui coordinates is done by process().
  virtual void process_audio(block_input const& input, block_output& output, float sample_rate) = 0;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_GRAPH_PROCESSOR_HPP