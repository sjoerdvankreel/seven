#ifndef SVN_SUPPORT_EVENT_BUFFER_HPP
#define SVN_SUPPORT_EVENT_BUFFER_HPP

#include <svn/support/audio_sample.hpp>
#include <cstdint>

namespace svn {

inline std::int32_t constexpr note_off = -1;

struct param_value
{
  union
  {
    float real;
    std::int32_t discrete;
  };
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

struct output_buffer
{
  audio_sample* audio;
  struct param_value* param_values;

  void clear_audio(std::int32_t sample_count);
  void add_audio(audio_sample* other, std::int32_t sample_count);
};

inline void
output_buffer::clear_audio(std::int32_t sample_count)
{
  for(std::int32_t s = 0; s < sample_count; s++)
    audio[s].left = audio[s].right = 0.0f;
}

inline void
output_buffer::add_audio(audio_sample* other, std::int32_t sample_count)
{
  for (std::int32_t s = 0; s < sample_count; s++)
    audio[s] += other[s];
}

inline float
param_real(input_buffer const& input, output_buffer& output, std::int32_t param, std::int32_t sample)
{
  auto automation = static_cast<float const*>(input.automation[param]);
  return output.param_values[param].real = automation[sample];
}

inline float
param_discrete(input_buffer const& input, output_buffer& output, std::int32_t param, std::int32_t sample)
{
  auto automation = static_cast<std::int32_t const*>(input.automation[param]);
  return output.param_values[param].discrete = automation[sample];
}

} // namespace svn
#endif // SVN_SUPPORT_EVENT_BUFFER_HPP