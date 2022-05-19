#ifndef SVN_SUPPORT_EVENT_BUFFER_HPP
#define SVN_SUPPORT_EVENT_BUFFER_HPP

#include <svn/support/audio_sample.hpp>
#include <cstdint>

namespace svn {

inline std::int32_t constexpr note_off = -1;

struct output_buffer
{
  void* param_values;
  audio_sample* audio;
};

struct note_event
{
  std::int32_t midi;
  std::int32_t sample_index;
};

struct input_buffer
{
  float bpm;
  float sample_rate;
  note_event const* notes;
  std::int32_t note_count;
  std::int32_t sample_count;
  void const* const* automation;
};

} // namespace svn
#endif // SVN_SUPPORT_EVENT_BUFFER_HPP