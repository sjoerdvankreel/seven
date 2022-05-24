#ifndef SVN_SUPPORT_TOPO_RT_HPP
#define SVN_SUPPORT_TOPO_RT_HPP

#include <string>
#include <cstdint>

namespace svn {

struct synth_part
{
  std::wstring name;
  std::int32_t index;
  struct part_info const* info;
}; 

struct synth_param
{
  std::wstring detail;
  synth_part const* part;
  std::int32_t part_index;
  struct param_info const* info;
};

extern std::int32_t synth_part_count;
extern std::int32_t synth_param_count;
extern std::int32_t real_param_count;
extern std::int32_t discrete_param_count;

extern synth_part const* synth_parts;
extern synth_param const* synth_params;
extern std::int32_t const* const* synth_bounds;

void init_topology();
void destroy_topology();
void init_defaults(struct param_value* state);

} // namespace svn
#endif // SVN_SUPPORT_TOPO_RT_HPP