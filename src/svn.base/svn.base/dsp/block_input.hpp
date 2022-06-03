#ifndef SVN_BASE_DSP_BLOCK_INPUT_HPP
#define SVN_BASE_DSP_BLOCK_INPUT_HPP

#include <cstdint>

namespace svn::base {

struct block_input
{
  float bpm;
  // Float* for real parameters, std::int32_t* for discrete. 
  // Points to the first runtime parameter for the runtime part (e.g. to filter 2 frequency for a filter component).
  void* const* automation; 
  // Midi note events, sample * polyphony.
  struct note_event** notes;
  // Midi note event count per sample.
  std::int32_t* note_count;
  std::int32_t sample_count;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_BLOCK_INPUT_HPP