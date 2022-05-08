#ifndef SVN_SUPPORT_PROCESS_INFO_HPP
#define SVN_SUPPORT_PROCESS_INFO_HPP

#include <cstdint>

namespace svn {

template <typename sample_type>
struct process_info 
{
  sample_type sample_rate;
  std::int32_t unit_index;
  std::int32_t sample_count;
};

} // namespace svn
#endif // SVN_SUPPORT_PROCESS_INFO_HPP