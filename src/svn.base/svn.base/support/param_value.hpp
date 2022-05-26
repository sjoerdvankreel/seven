#ifndef SVN_BASE_SUPPORT_PARAM_VALUE_HPP
#define SVN_BASE_SUPPORT_PARAM_VALUE_HPP

#include <cstdint>

namespace svn::base {

union param_value
{
  float real;
  std::int32_t discrete;

  param_value();
  explicit param_value(float val);
  explicit param_value(std::int32_t val);
};

} // namespace svn::base
#endif // SVN_BASE_SUPPORT_PARAM_VALUE_HPP