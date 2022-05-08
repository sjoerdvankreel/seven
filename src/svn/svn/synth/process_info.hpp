#ifndef SVN_SYNTH_PROCESS_INFO_HPP
#define SVN_SYNTH_PROCESS_INFO_HPP

#include <svn/synth/param_state.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
struct process_info 
{
  sample_type sample_rate;
  std::int32_t unit_index;
  std::int32_t sample_count;
  param_state_t* param_state;
};

} // namespace svn
#endif // SVN_SYNTH_PROCESS_INFO_HPP