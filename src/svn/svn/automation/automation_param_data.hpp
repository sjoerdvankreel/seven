#ifndef SVN_AUTOMATION_AUTOMATION_PARAM_DATA_HPP
#define SVN_AUTOMATION_AUTOMATION_PARAM_DATA_HPP

#include <svn/param/param_id.hpp>
#include <cstddef>

namespace svn {

template <typename sample_type>
struct automation_param_data
{
  param_id id;
  sample_type const* values;
  std::size_t const* indices;
};

} // namespace svn
#endif // SVN_AUTOMATION_AUTOMATION_PARAM_DATA_HPP