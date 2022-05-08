#ifndef SVN_AUTOMATION_PARAM_AUTOMATION_BUFFER_HPP
#define SVN_AUTOMATION_PARAM_AUTOMATION_BUFFER_HPP

#include <svn/param/param_id.hpp>

namespace svn {

template <typename sample_type>
struct param_automation_buffer
{
  param_id id;
  sample_type const* samples;
};

} // namespace svn
#endif // SVN_AUTOMATION_PARAM_AUTOMATION_BUFFER_HPP