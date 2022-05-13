#include <svn/topology/part_id.hpp>
#include <svn/topology/synth_topology.hpp>

namespace svn::topology {

std::int32_t const
part_count[part_id::count] = 
{ 
  unit_count,
  filter_count
};

} // namespace svn::synth_topology