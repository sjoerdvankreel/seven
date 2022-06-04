#ifndef SVN_VST_BASE_SDK_PARAMETER_HPP
#define SVN_VST_BASE_SDK_PARAMETER_HPP

#include <public.sdk/source/vst/vstparameters.h>
#include <svn.base/runtime/runtime_param.hpp>
#include <svn.base/static/param_descriptor.hpp>

#include <cstdint>
#include <algorithm>

namespace svn::vst::base {

class parameter:
public Steinberg::Vst::Parameter 
{
private:
  using TChar = Steinberg::Vst::TChar;
  using String128 = Steinberg::Vst::String128;
  using ParamValue = Steinberg::Vst::ParamValue;
  svn::base::param_descriptor const* const _descriptor;

public:

  // Creates an input parameter.
  parameter(
    std::int32_t index, 
    svn::base::runtime_param const* param);

  // Creates an output parameter.
  // Index starts counting from input parameter count.
  parameter(
    std::int32_t index,
    svn::base::param_descriptor const* descriptor);

  static std::int32_t vst_normalized_to_discrete(
    svn::base::param_descriptor const& param, double val);
  static double discrete_to_vst_normalized(
    svn::base::param_descriptor const& param, std::int32_t val);

  ParamValue toPlain(ParamValue normalized) const override;
  ParamValue toNormalized(ParamValue plain) const override;
  void toString(ParamValue normalized, String128 string) const override;
  bool fromString(TChar const* string, ParamValue& normalized) const override;
};

inline double 
parameter::discrete_to_vst_normalized(
  svn::base::param_descriptor const& param, std::int32_t val)
{ 
  std::int32_t min = param.min.discrete;
  std::int32_t max = param.max.discrete;
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
parameter::vst_normalized_to_discrete(
  svn::base::param_descriptor const& param, double val)
{ 
  std::int32_t min = param.min.discrete;
  std::int32_t max = param.max.discrete;
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), 0, max - min);
}

} // namespace Svn::Vst
#endif // SVN_VST_BASE_SDK_PARAMETER_HPP