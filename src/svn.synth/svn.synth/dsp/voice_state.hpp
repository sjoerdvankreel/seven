#ifndef SVN_SYNTH_DSP_VOICE_STATE_HPP
#define SVN_SYNTH_DSP_VOICE_STATE_HPP

#include <cstdint>

namespace svn::synth {

struct voice_state
{
  // True if processing any audio during current buffer.
  bool in_use = false;
  // Start position in stream, used for recycling oldest voice when running out.
  std::int64_t start_position_stream = -1;
  // Start position in current buffer, 0 if started in previous buffer.
  std::int32_t start_position_buffer = 0;
  // Active if tracking automation, i.e. channel was not reused.
  std::int32_t deactivation_position_buffer = -1;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_STATE_HPP