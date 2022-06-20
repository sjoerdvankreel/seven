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
};

} // namespace Svn::Vst
#endif // SVN_VST_BASE_SDK_PARAMETER_HPP