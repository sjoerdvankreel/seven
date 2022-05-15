#ifndef SVN_SUPPORT_AUDIO_BUFFER_HPP
#define SVN_SUPPORT_AUDIO_BUFFER_HPP

#include <svn/support/audio_sample.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
struct audio_buffer
{
  audio_sample<sample_type>* samples;
public:  
  void clear(std::int32_t sample_count);
  void add(std::int32_t sample_count, audio_buffer y);
};

template <typename sample_type>
void audio_buffer<sample_type>::
clear(std::int32_t sample_count)
{
  for(std::int32_t i = 0; i < sample_count; i++)
    samples[i].clear();
}

template <typename sample_type>
void audio_buffer<sample_type>::
add(std::int32_t sample_count, audio_buffer<sample_type> y)
{
  for(std::int32_t i = 0; i < sample_count; i++)
    samples[i] += y.samples[i];
}

} // namespace svn
#endif // SVN_SUPPORT_AUDIO_BUFFER_HPP