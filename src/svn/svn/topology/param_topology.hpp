#ifndef SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP
#define SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP

#include <svn/topology/param_id.hpp>
#include <cstdint>

namespace svn {

struct param_item
{
  wchar_t const* name;
  wchar_t const* description;
};

struct param_type_t { enum value { real, list, toggle, discrete }; };
typedef param_type_t::value param_type;

struct param_topology
{
  param_type type;
  wchar_t const* name;
  wchar_t const* description;
  union
  {
    struct { float default_; } real;
    struct { bool default_; } toggle;
    struct { param_item const* items; std::int32_t count; } list;
    struct { std::int32_t min; std::int32_t max; std::int32_t default_; } discrete;
  } bounds;

  param_topology(wchar_t const* name, wchar_t const* description, bool default_);
  param_topology(wchar_t const* name, wchar_t const* description, float default_);
  param_topology(wchar_t const* name, wchar_t const* description, param_item const* items, std::int32_t count);
  param_topology(wchar_t const* name, wchar_t const* description, std::int32_t min, std::int32_t max, std::int32_t default_);
};

extern param_topology const 
param_topologies[param_id::count];

} // namespace svn
#endif // SVN_TOPOLOGY_PARAM_TOPOLOGY_HPP