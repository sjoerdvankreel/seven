#ifndef SVN_TOPOLOGY_PART_TOPOLOGY_HPP
#define SVN_TOPOLOGY_PART_TOPOLOGY_HPP

#include <svn/topology/param_id.hpp>
#include <cstdint>

namespace svn {

inline std::int32_t constexpr oscillator_count = 2;
inline std::int32_t constexpr filter_count = 1;

struct part_type_t { enum value { oscillator, filter, count }; };
typedef part_type_t::value part_type;

struct part_topology
{
  wchar_t const* name;
  wchar_t const* description;
  std::int32_t count;
  param_id first_param;
  param_id last_param;
};

extern part_topology const 
part_topologies[part_type::count];

} // namespace svn
#endif // SVN_TOPOLOGY_PART_TOPOLOGY_HPP