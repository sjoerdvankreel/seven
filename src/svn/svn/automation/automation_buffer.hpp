#ifndef SVN_AUTOMATION_AUTOMATION_BUFFER_HPP
#define SVN_AUTOMATION_AUTOMATION_BUFFER_HPP

#include <svn/automation/param_automation_buffer.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
struct automation_buffer
{
  std::int32_t param_count;
  param_automation_buffer<sample_type> const* params;
};

} // namespace svn
#endif // SVN_AUTOMATION_AUTOMATION_BUFFER_HPP