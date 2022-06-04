#ifndef SVN_BASE_DSP_NOTE_EVENT_HPP
#define SVN_BASE_DSP_NOTE_EVENT_HPP

#include <cstdint>

namespace svn::base {

inline std::int32_t constexpr note_off = -1;

struct note_event
{
  float velocity = 0.0f;
  // Midi note or note off.
  std::int32_t midi = -2;
  // Index in current buffer.
  std::int32_t sample_index;
  // Either channel or pitch.
  std::int32_t correlation = -1;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_NOTE_EVENT_HPP