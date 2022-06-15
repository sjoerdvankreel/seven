#ifndef SVN_BASE_DSP_BLOCK_INPUT_HPP
#define SVN_BASE_DSP_BLOCK_INPUT_HPP

#include <svn.base/support/support.hpp>
#include <cstdint>

namespace svn::base {

struct block_input
{
  float bpm;
  std::int32_t sample_count;
  std::int64_t stream_position;
  param_value* const* automation;
  
  // Midi note events.
  std::int32_t note_count;
  struct note_event* notes;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_BLOCK_INPUT_HPP