#ifndef SVN_BASE_DSP_GRAPH_PROCESSOR_HPP
#define SVN_BASE_DSP_GRAPH_PROCESSOR_HPP

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
  std::vector<float> _audio_data;
  std::vector<graph_point> _graph_data;
  topology_info const* const _topology;

public:
  virtual ~graph_processor() {}
  graph_processor(topology_info const* topology, std::int32_t part_index): 
  _part_index(part_index), _audio_data(), _graph_data(), _topology(topology) {}

protected:
  std::int32_t part_index() const { return _part_index; }
  topology_info const* topology() const { return _topology; }
  // Renders audio in (-1, 1).
  // Translation to ui coordinates is done by process().
  virtual void process_audio(std::vector<float>& data, float sample_rate) = 0;

public:
  virtual bool needs_repaint(std::int32_t runtime_param) const = 0;
  std::vector<graph_point> const& process(std::int32_t width, std::int32_t height, float sample_rate);
};

} // namespace svn::base
#endif // SVN_BASE_DSP_GRAPH_PROCESSOR_HPP