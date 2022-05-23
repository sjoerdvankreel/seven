#ifndef SVN_VST_SDK_PARAMETER_HPP
#define SVN_VST_SDK_PARAMETER_HPP

#include <public.sdk/source/vst/vstparameters.h>
#include <svn/support/topo_rt.hpp>

namespace Svn::Vst {

class SvnParameter:
public Steinberg::Vst::Parameter
{
private:
  svn::synth_param const* const _param;
private:
  using TChar = Steinberg::Vst::TChar;
  using String128 = Steinberg::Vst::String128;
  using ParamValue = Steinberg::Vst::ParamValue;
public:
  SvnParameter(svn::synth_param const* param);
public:
  ParamValue toPlain(ParamValue valueNormalized) const override;
  ParamValue toNormalized(ParamValue plainValue) const override;
  void toString(ParamValue valueNormalized, String128 string) const override;
  bool fromString(const TChar* string, ParamValue& valueNormalized) const override;
};

inline SvnParameter::
SvnParameter(svn::synth_param const* param):
_param(param) {}

} // namespace Svn::Vst
#endif // SVN_VST_SDK_PARAMETER_HPP