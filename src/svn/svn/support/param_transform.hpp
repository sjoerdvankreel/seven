#ifndef SVN_SUPPORT_PARAM_TRANSFORM_HPP
#define SVN_SUPPORT_PARAM_TRANSFORM_HPP

#include <svn/support/topo_static.hpp>

#include <cmath>
#include <cstdint>
#include <cstddef>
#include <cassert>

namespace svn {

param_value param_to_display(param_info const& info, param_value val);
param_value param_from_display(param_info const& info, param_value val);
bool param_parse(param_info const& info, wchar_t const* buffer, param_value& val);
void param_format(param_info const& info, param_value val, wchar_t* buffer, std::size_t size);

inline float 
param_real_to_range(std::int32_t slope, float val, float min, float max)
{
  switch (slope)
  {
  case param_slope::linear: return min + (max - min) * val;
  case param_slope::quadratic: return min + (max - min) * val * val;
  default: assert(false); return 0.0f;
  }
}

inline float
param_real_from_range(std::int32_t slope, float val, float min, float max)
{
  switch (slope)
  {
  case param_slope::linear: return (val - min) / (max - min);
  case param_slope::quadratic: return std::sqrt((val - min) / (max - min));
  default: assert(false); return 0.0f;
  }
}

} // namespace svn
#endif // SVN_SUPPORT_PARAM_TRANSFORM_HPP