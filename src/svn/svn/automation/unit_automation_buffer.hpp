#ifndef SVN_AUTOMATION_UNIT_AUTOMATION_BUFFER_HPP
#define SVN_AUTOMATION_UNIT_AUTOMATION_BUFFER_HPP

#include <svn/automation/automation_buffer.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
struct unit_automation_buffer
{
  param_id* params;
  std::int32_t param_count;
  sample_type const** samples;

  void setup(
    param_id first, 
    param_id last, 
    std::int32_t unit_index, 
    automation_buffer<sample_type> const& automation);
};

} // namespace svn
#endif // SVN_AUTOMATION_UNIT_AUTOMATION_BUFFER_HPP