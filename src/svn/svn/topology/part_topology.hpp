#ifndef SVN_TOPOLOGY_PART_TOPOLOGY_HPP
#define SVN_TOPOLOGY_PART_TOPOLOGY_HPP

#include <svn/topology/param_id.hpp>

namespace svn {

struct part_topology
{
  param_id first_param;
  param_id last_param;
  wchar_t const* name;
  wchar_t const* description;
};

extern part_topology const
part_topologies[];

} // namespace svn
#endif // SVN_TOPOLOGY_PART_TOPOLOGY_HPP