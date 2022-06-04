#ifndef SVN_SYNTH_DSP_VOICE_STATE_HPP
#define SVN_SYNTH_DSP_VOICE_STATE_HPP

#include <cstdint>

namespace svn::synth {

struct voice_state
{
  // True if processing any audio during current buffer.
  bool in_use = false;
  // True if already released.
  bool released_previous_buffer = false;
  // True if to be released in current buffer.
  bool release_this_buffer = false;
  // Find voice from midi note.
  std::int32_t midi = -1;
  // Start position in stream, used for recycling oldest voice when running out.
  std::int64_t start_position_stream = -1;
  // Start position in current buffer, 0 if started in previous buffer.
  std::int32_t start_position_buffer = -1;
  // Used to stop tracking automation after release.
  std::int32_t release_position_buffer = -1;
};

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_VOICE_STATE_HPP