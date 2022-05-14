#ifndef SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP
#define SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP

#include <svn/topology/param_type.hpp>
#include <svn/topology/param_bounds.hpp>
#include <cstdint>

namespace svn {

struct param_topology
{
  param_type type;
  wchar_t const* name;
  wchar_t const* description;
  union
  {
    param_bounds<float> real;
    param_bounds<std::int32_t> discrete;
  };

  param_topology(
    wchar_t const* name, 
    wchar_t const* description, 
    float min, 
    float max, 
    float default_);

  param_topology(
    wchar_t const* name,
    wchar_t const* description,
    std::int32_t min,
    std::int32_t max,
    std::int32_t default_);
};

extern param_topology const 
param_topologies[];

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP