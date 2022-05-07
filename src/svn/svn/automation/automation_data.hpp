#ifndef SVN_AUTOMATION_AUTOMATION_DATA_HPP
#define SVN_AUTOMATION_AUTOMATION_DATA_HPP

#include <svn/automation/automation_param_data.hpp>
#include <cstddef>

namespace svn {

template <typename sample_type>
struct automation_data
{
  std::size_t size;
  automation_param_data<sample_type> const* params;
};

} // namespace svn
#endif // SVN_AUTOMATION_AUTOMATION_DATA_HPP