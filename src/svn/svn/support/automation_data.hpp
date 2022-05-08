#ifndef SVN_SUPPORT_AUTOMATION_DATA_HPP
#define SVN_SUPPORT_AUTOMATION_DATA_HPP

#include <svn/support/automation_buffer.hpp>
#include <cstddef>

namespace svn {

template <typename sample_type>
struct automation_data
{
  std::size_t buffer_count;
  automation_buffer const* buffers;
};

} // namespace svn
#endif // SVN_SUPPORT_AUTOMATION_DATA_HPP