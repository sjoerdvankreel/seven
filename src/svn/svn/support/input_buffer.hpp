#ifndef SVN_SUPPORT_INPUT_BUFFER_HPP
#define SVN_SUPPORT_INPUT_BUFFER_HPP

#include <cstdint>

namespace svn {

inline std::int32_t constexpr note_off = -1;

struct note_event
{
  std::int32_t midi;
  std::int32_t sample_index;
};

struct input_buffer
{
  float bpm;
  float sample_rate;
  void** automation;
  note_event* notes;
  std::int32_t note_count;
  std::int32_t sample_count;
};

} // namespace svn
#endif // SVN_SUPPORT_INPUT_BUFFER_HPP