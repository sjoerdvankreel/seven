#ifndef SVN_VST_SUPPORT_PARAM_HPP
#define SVN_VST_SUPPORT_PARAM_HPP

#include <svn/support/topo_rt.hpp>
#include <cstdint>

namespace Svn::Vst {

std::int32_t
paramStepCount(std::int32_t param)
{
  auto const& info = *svn::synth_params[param].info;
  if(info.type == svn::param_type::real) return 0;
  return info.max.discrete - info.min.discrete;
}

double
paramNormalizeDefault(std::int32_t param);

double
paramNormalizeReal(std::int32_t param, float val);
float
paramDenormalizeReal(std::int32_t param, double val);

std::int32_t
paramNormalizeDiscrete(std::int32_t param, std::int32_t val);
double
paramDenormalizeDiscrete(std::int32_t param, double val);

} // namespace Svn::Vst
#endif // SVN_VST_SUPPORT_PARAM_HPP