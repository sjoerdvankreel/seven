#ifndef SVN_BASE_DSP_NOTE_EVENT_HPP
#define SVN_BASE_DSP_NOTE_EVENT_HPP

#include <cstdint>

namespace svn::base {

inline std::int32_t constexpr note_off = -1;
inline std::int32_t constexpr note_none = -2;

struct note_event
{
  float velocity = 0.0f;
  std::int32_t midi = note_none;
  std::int32_t correlation = -1;
};

} // namespace svn::base
#endif // SVN_BASE_DSP_NOTE_EVENT_HPP