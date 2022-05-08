#ifndef SVN_SUPPORT_AUDIO_BUFFER_HPP
#define SVN_SUPPORT_AUDIO_BUFFER_HPP

#include <svn/support/audio_sample.hpp>
#include <cassert>
#include <cstddef>

namespace svn {

template <typename sample_type>
struct audio_buffer
{
  audio_sample<sample_type>* samples;
public:  
  void clear(std::size_t size);
  void add(std::size_t size, audio_buffer y);
};

template <typename sample_type>
void audio_buffer<sample_type>::
clear(std::size_t size)
{
  for(std::size_t i = 0; i < size; i++)
    samples[i].clear();
}

template <typename sample_type>
void audio_buffer<sample_type>::
add(std::size_t size, audio_buffer<sample_type> y)
{
  for(std::size_t i = 0; i < size; i++)
    samples[i] += y.samples[i];
}

} // namespace svn
#endif // SVN_SUPPORT_AUDIO_BUFFER_HPP