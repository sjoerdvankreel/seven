#ifndef SVN_BASE_DSP_GLOBAL_INPUT_HPP
#define SVN_BASE_DSP_GLOBAL_INPUT_HPP

#include <cstdint>

namespace svn::base {

struct global_input
{
  float bpm;
  std::int32_t sample_count;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_GLOBAL_INPUT_HPP