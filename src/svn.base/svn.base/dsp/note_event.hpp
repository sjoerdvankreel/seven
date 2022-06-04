#ifndef SVN_BASE_DSP_NOTE_EVENT_HPP
#define SVN_BASE_DSP_NOTE_EVENT_HPP

#include <cstdint>

namespace svn::base {

struct note_event
{
  // Note on/off.
  bool note_on = false;
  // Velocity in case of note on.
  float velocity = 0.0f;
  // Midi note index for both on and off events.
  std::int32_t midi = -1;
  // Index in current buffer.
  std::int32_t sample_index = -1;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_NOTE_EVENT_HPP