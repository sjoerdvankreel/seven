#ifndef SVN_SUPPORT_TOPO_RT_HPP
#define SVN_SUPPORT_TOPO_RT_HPP

#include <cstdint>

namespace svn {

struct synth_part
{
  std::int32_t index;
  struct part_info const* info;
}; 

struct synth_param
{
  synth_part const* part;
  struct param_info const* info;
};

extern std::int32_t synth_part_count;
extern synth_part const* synth_parts;
extern std::int32_t const* synth_part_bounds;

extern std::int32_t synth_param_count;
extern synth_param const* synth_params;
extern std::int32_t const* synth_param_bounds;

} // namespace svn
#endif // SVN_SUPPORT_TOPO_RT_HPP