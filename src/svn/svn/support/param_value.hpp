#ifndef SVN_SUPPORT_PARAM_VALUE_HPP
#define SVN_SUPPORT_PARAM_VALUE_HPP

#include <cstdint>

namespace svn {

struct param_value
{
  union
  {
    float real;
    std::int32_t discrete;
  };
};

} // namespace svn
#endif // SVN_SUPPORT_PARAM_VALUE_HPP