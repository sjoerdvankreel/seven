#ifndef SVN_SUPPORT_AUDIO_BUFFER_HPP
#define SVN_SUPPORT_AUDIO_BUFFER_HPP

#include <svn/support/audio_sample.hpp>
#include <cstddef>

namespace svn {

template <typename sample_type>
struct audio_buffer
{
  std::size_t size;
  audio_sample<sample_type>* samples;
};

} // namespace svn
#endif // SVN_SUPPORT_AUDIO_BUFFER_HPP