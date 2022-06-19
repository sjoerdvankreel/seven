#ifndef SVN_BASE_DSP_GRAPH_PROCESSOR_HPP
#define SVN_BASE_DSP_GRAPH_PROCESSOR_HPP

#include <svn.base/topology/topology_info.hpp>
#include <vector>
#include <cstdint>

namespace svn::base {

extern class graph_processor* 
create_graph_processor(topology_info const* topology, 
  std::int32_t type_index, std::int32_t part_index);

class graph_processor
{
  std::vector<float> _data;
  std::int32_t const _type_index;
  std::int32_t const _part_index;
  topology_info const* const _topology;

protected:
  topology_info const* topology() const { return _topology; }
  virtual void process(std::vector<float>& data, float sample_rate) = 0;

public:
  float const* process(float sample_rate);
  virtual bool needs_repaint(std::int32_t param) const = 0;

public:
  virtual ~graph_processor() {}
  graph_processor(topology_info const* topology, std::int32_t type_index, std::int32_t part_index): 
  _data(), _type_index(type_index), _part_index(part_index), _topology(topology) {}
};

inline float const* 
graph_processor::process(float sample_rate)
{
  _data.clear();
  process(_data, sample_rate);
  return _data.data();
}

} // namespace svn::base
#endif // SVN_BASE_DSP_GRAPH_PROCESSOR_HPP