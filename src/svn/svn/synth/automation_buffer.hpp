#ifndef SVN_AUTOMATION_DATA_HPP
#define SVN_AUTOMATION_DATA_HPP

#include <cstddef>
#include <svn/synth_params.hpp>

namespace svn {

template <typename sample_type>
struct automation_param_data
{
  synth_param param;
  sample_type const* values;
  std::size_t const* indices;
};

template <typename sample_type>
struct automation_data
{
  std::size_t count;
  automation_param_data<sample_type> const* params;
};

} // namespace svn
#endif // SVN_AUTOMATION_DATA_HPP