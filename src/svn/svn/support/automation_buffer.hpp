#ifndef SVN_SUPPORT_AUTOMATION_BUFFER_HPP
#define SVN_SUPPORT_AUTOMATION_BUFFER_HPP

#include <svn/param/param_id.hpp>

namespace svn {

template <typename sample_type>
struct automation_buffer
{
  param_id id;
  sample_type const* values;
};

} // namespace svn
#endif // SVN_SUPPORT_AUTOMATION_BUFFER_HPP