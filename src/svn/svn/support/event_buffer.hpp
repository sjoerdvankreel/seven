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

struct output_buffer
{
  audio_sample* audio;
  struct param_value* param_values;

  void clear(std::int32_t sample_count);
  float param_real(std::int32_t param) const;
  std::int32_t param_discrete(std::int32_t param) const;
  void param_real(std::int32_t param, float val);
  void param_discrete(std::int32_t param, std::int32_t val);
};

struct input_buffer
{
  float bpm;
  float sample_rate;
  note_event const* notes;
  std::int32_t note_count;
  std::int32_t sample_count;
  void const* const* automation;

  bool automated(std::int32_t param) const;
  float param_real(std::int32_t param, std::int32_t sample) const;
  std::int32_t param_discrete(std::int32_t param, std::int32_t sample) const;
};

inline void
output_buffer::clear(std::int32_t sample_count)
{
  for(std::int32_t s = 0; s < sample_count; s++)
    audio[s].left = audio[s].right = 0.0f;
}

inline float
output_buffer::param_real(std::int32_t param) const
{ return param_values[param].real; }
inline std::int32_t
output_buffer::param_discrete(std::int32_t param) const
{ return param_values[param].discrete; }

inline void
output_buffer::param_real(std::int32_t param, float val)
{ param_values[param].real = val; }
inline void
output_buffer::param_discrete(std::int32_t param, std::int32_t val)
{ param_values[param].discrete = val; }

inline bool
input_buffer::automated(std::int32_t param) const
{ return automation[param] != nullptr; }
inline float 
input_buffer::param_real(std::int32_t param, std::int32_t sample) const
{ return static_cast<float const*>(automation[param])[sample]; }
inline std::int32_t
input_buffer::param_discrete(std::int32_t param, std::int32_t sample) const
{ return static_cast<std::int32_t const*>(automation[param])[sample]; }

} // namespace svn
#endif // SVN_SUPPORT_EVENT_BUFFER_HPP