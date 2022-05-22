#ifndef SVN_VST_SUPPORT_PARAM_HPP
#define SVN_VST_SUPPORT_PARAM_HPP

#include <svn/support/topo_rt.hpp>
#include <svn/support/topo_static.hpp>

#include <cmath>
#include <cstdint>

namespace Svn::Vst {

inline std::int32_t
paramStepCount(std::int32_t param)
{
  auto const& info = *svn::synth_params[param].info;
  if(info.type == svn::param_type::real) return 0;
  return info.max.discrete - info.min.discrete;
}

inline std::int32_t
paramDenormalizeDiscrete(std::int32_t param, double val)
{
  auto const& info = *svn::synth_params[param].info;
  std::int32_t stepCount = paramStepCount(param);
  return std::min(stepCount, static_cast<std::int32_t>(val * (stepCount + 1)));
}

inline std::int32_t
paramNormalizeDiscrete(std::int32_t param, std::int32_t val)
{
  auto const& info = *svn::synth_params[param].info;
  return (val - info.min.discrete) / static_cast<double>(paramStepCount(param));
}

inline double
paramNormalizeDefault(std::int32_t param)
{
  auto const& info = *svn::synth_params[param].info;
  if (info.type == svn::param_type::real) return static_cast<double>(info.default_.real);
  return paramNormalizeDiscrete(param, info.default_.discrete);
}

} // namespace Svn::Vst
#endif // SVN_VST_SUPPORT_PARAM_HPP