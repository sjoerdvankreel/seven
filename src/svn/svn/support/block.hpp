#ifndef SVN_SUPPORT_BLOCK_HPP
#define SVN_SUPPORT_BLOCK_HPP

#include <cstdint>

namespace svn {

struct block_t
{
  float rate;
  std::int32_t count;
  float const* const* automation;
};

} // namespace svn
#endif // SVN_SUPPORT_BLOCK_HPP