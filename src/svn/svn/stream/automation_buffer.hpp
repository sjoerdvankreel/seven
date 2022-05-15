#ifndef SVN_EVENT_AUTOMATION_BUFFER_HPP
#define SVN_EVENT_AUTOMATION_BUFFER_HPP

#include <svn/param/param_id.hpp>

namespace svn {

template <typename sample_type>
struct automation_buffer
{
  param_id param;
  sample_type const* samples;
};

} // namespace svn
#endif // SVN_EVENT_AUTOMATION_BUFFER_HPP