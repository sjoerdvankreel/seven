#ifndef SVN_VST_BASE_SDK_PARAMETER_HPP
#define SVN_VST_BASE_SDK_PARAMETER_HPP

#include <svn.base/topology/topology_info.hpp>
#include <public.sdk/source/vst/vstparameters.h>

#include <cstdint>
#include <algorithm>

namespace svn::vst::base {

// Vst parameter based on param_descriptor.
class parameter:
public Steinberg::Vst::Parameter 
{
private:
  using TChar = Steinberg::Vst::TChar;
  using String128 = Steinberg::Vst::String128;
  using ParamValue = Steinberg::Vst::ParamValue;
  svn::base::param_descriptor const* const _descriptor;

public:
  parameter(std::int32_t index, 
    svn::base::part_info const* part,
    svn::base::param_info const* param);
  
  // Forwarding to param_descriptor.
  ParamValue toPlain(ParamValue normalized) const override;
  ParamValue toNormalized(ParamValue plain) const override;
  void toString(ParamValue normalized, String128 string) const override;
  bool fromString(TChar const* string, ParamValue& normalized) const override;

  static std::int32_t vst_normalized_to_discrete(svn::base::param_descriptor const& param, double val);
  static double discrete_to_vst_normalized(svn::base::param_descriptor const& param, std::int32_t val);
};

inline double 
parameter::discrete_to_vst_normalized(
  svn::base::param_descriptor const& param, std::int32_t val)
{ 
  assert(param.type != svn::base::param_type::real);
  std::int32_t min = param.discrete.min;
  std::int32_t max = param.discrete.max;
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
parameter::vst_normalized_to_discrete(
  svn::base::param_descriptor const& param, double val)
{ 
  assert(param.type != svn::base::param_type::real);
  std::int32_t min = param.discrete.min;
  std::int32_t max = param.discrete.max;
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), 0, max - min);
}

} // namespace Svn::Vst
#endif // SVN_VST_BASE_SDK_PARAMETER_HPP