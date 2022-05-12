#ifndef SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP
#define SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP

#include <svn/topology/param_type.hpp>

namespace svn {

struct param_topology
{
  param_type type;
  wchar_t const* name;
  wchar_t const* description;
};

extern param_topology const 
param_topologies[];

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP