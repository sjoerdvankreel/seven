#ifndef SVN_BASE_DSP_GRAPH_PROCESSOR_HPP
#define SVN_BASE_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/topology/topology_info.hpp>
#include <vector>
#include <cstdint>

namespace svn::base {

extern class graph_processor* 
create_graph_processor(topology_info const* topology, 
  std::int32_t part_type, std::int32_t part_index);

struct graph_point { float x, y; };

class graph_processor
{
  std::vector<float> _audio_data;
  std::vector<graph_point> _graph_data;
  topology_info const* const _topology;

public:
  virtual ~graph_processor() {}
  graph_processor(topology_info const* topology): 
  _audio_data(), _graph_data(), _topology(topology) {}

protected:
  topology_info const* topology() const { return _topology; }
  virtual void process_audio(std::vector<float>& data, float sample_rate) = 0;

public:
  virtual bool needs_repaint(std::int32_t param) const = 0;
  std::vector<graph_point> const& process(std::int32_t width, std::int32_t height, float sample_rate);
};

} // namespace svn::base
#endif // SVN_BASE_DSP_GRAPH_PROCESSOR_HPP