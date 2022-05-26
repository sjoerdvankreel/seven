#ifndef SVN_BASE_DSP_NOTE_EVENT_HPP
#define SVN_BASE_DSP_NOTE_EVENT_HPP

#include <cstdint>

namespace svn {

inline std::int32_t constexpr note_off = -1;

struct note_event
{
  float velocity;
  std::int32_t midi; // Midi note index.
  std::int32_t sample_index; // Index in the process block where the event occurs.
};

} // namespace svn
#endif // SVN_BASE_DSP_NOTE_EVENT_HPP