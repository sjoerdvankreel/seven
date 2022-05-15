#ifndef SVN_STREAM_AUDIO_BUFFER_HPP
#define SVN_STREAM_AUDIO_BUFFER_HPP

#include <svn/stream/audio_sample.hpp>
#include <cstdint>

namespace svn {

struct audio_buffer
{
  audio_sample* samples;
  void clear(std::int32_t count);
  void add(audio_buffer y, std::int32_t count);
};

inline void
audio_buffer::clear(std::int32_t count)
{
  for(std::int32_t i = 0; i < count; i++)
    samples[i].clear();
}

inline void
audio_buffer::add(audio_buffer y, std::int32_t count)
{
  for(std::int32_t i = 0; i < count; i++)
    samples[i] += y.samples[i];
}

} // namespace svn
#endif // SVN_STREAM_AUDIO_BUFFER_HPP