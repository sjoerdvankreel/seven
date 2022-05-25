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
param_real_to_range(param_bounds const& bounds, float val)
{
  switch (bounds.slope)
  {
  case param_slope::db: return 20.0f * std::log10(val);
  case param_slope::linear: return bounds.min + (bounds.max - bounds.min) * val;
  case param_slope::quadratic: return bounds.min + (bounds.max - bounds.min) * val * val;
  default: assert(false); return 0.0f;
  }
}

inline float
param_real_from_range(param_bounds const& bounds, float val)
{
  switch (bounds.slope)
  {
  case param_slope::db: return std::expf(std::log(10.0f) * val / 20.0f);
  case param_slope::linear: return (val - bounds.min) / (bounds.max - bounds.min);
  case param_slope::quadratic: return std::sqrt((val - bounds.min) / (bounds.max - bounds.min));
  default: assert(false); return 0.0f;
  }
}

} // namespace svn
#endif // SVN_SUPPORT_PARAM_TRANSFORM_HPP