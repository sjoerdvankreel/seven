#ifndef SVN_VST_BASE_SUPPORT_SUPPORT_HPP
#define SVN_VST_BASE_SUPPORT_SUPPORT_HPP

#include <svn.base/topology/topology_info.hpp>
#include <pluginterfaces/base/ftypes.h>
#include <algorithm>
#include <cassert>

namespace svn::vst::base {

std::basic_string<Steinberg::Vst::TChar>
to_vst_string(char const* str);

inline double 
discrete_to_vst_normalized(
  svn::base::param_descriptor const& param, std::int32_t val)
{ 
  assert(param.type != svn::base::param_type::real);
  std::int32_t min = param.discrete.min;
  std::int32_t max = param.discrete.max;
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
vst_normalized_to_discrete(
  svn::base::param_descriptor const& param, double val)
{ 
  assert(param.type != svn::base::param_type::real);
  std::int32_t min = param.discrete.min;
  std::int32_t max = param.discrete.max;
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), 0, max - min);
}

inline double 
base_to_vst_normalized(
  svn::base::topology_info const* topology, std::int32_t param, svn::base::param_value val)
{
  auto const& descriptor = *topology->params[param].descriptor;
  if (descriptor.type == svn::base::param_type::real) return val.real;
  return discrete_to_vst_normalized(descriptor, val.discrete);
}

inline svn::base::param_value
vst_normalized_to_base(
  svn::base::topology_info const* topology, std::int32_t param, double val)
{ 
  auto const& descriptor = *topology->params[param].descriptor;
  if(descriptor.type == svn::base::param_type::real) return svn::base::param_value(static_cast<float>(val));
  return svn::base::param_value(vst_normalized_to_discrete(descriptor, val));
}

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SUPPORT_SUPPORT_HPP