#ifndef SVN_VST_SUPPORT_PARAM_HPP
#define SVN_VST_SUPPORT_PARAM_HPP

#include <cstdint>

namespace Svn::Vst {

std::int32_t
paramStepCount(std::int32_t param);
double
paramNormalizeDefault(std::int32_t param);

double
paramNormalizeReal(std::int32_t param, float val);
double
paramNormalizeDiscrete(std::int32_t param, std::int32_t val);

} // namespace Svn::Vst
#endif // SVN_VST_SUPPORT_PARAM_HPP