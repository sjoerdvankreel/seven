#ifndef SVN_VST_SDK_PARAMETER_HPP
#define SVN_VST_SDK_PARAMETER_HPP

#include <public.sdk/source/vst/vstparameters.h>
#include <svn/support/topo_rt.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/param_transform.hpp>

#include <cstdint>
#include <algorithm>

namespace Svn::Vst {

class Parameter:
public Steinberg::Vst::Parameter 
{
  svn::synth_param const* const _param;
private:
  using TChar = Steinberg::Vst::TChar;
  using String128 = Steinberg::Vst::String128;
  using ParamValue = Steinberg::Vst::ParamValue;
public:
  Parameter(std::int32_t index, svn::synth_param const* param);
public:
  ParamValue toPlain(ParamValue normalized) const override;
  ParamValue toNormalized(ParamValue plain) const override;
  void toString(ParamValue normalized, String128 string) const override;
  bool fromString(TChar const* string, ParamValue& normalized) const override;
public:
  static std::int32_t toDiscrete(svn::param_info const& info, double val);
  static double fromDiscrete(svn::param_info const& info, std::int32_t val);
};

inline double 
Parameter::fromDiscrete(svn::param_info const& info, std::int32_t val)
{ 
  std::int32_t min = info.min.discrete;
  std::int32_t max = info.max.discrete;
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
Parameter::toDiscrete(svn::param_info const& info, double val)
{ 
  std::int32_t min = info.min.discrete;
  std::int32_t max = info.max.discrete;
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), min, max);
}

} // namespace Svn::Vst
#endif // SVN_VST_SDK_PARAMETER_HPP