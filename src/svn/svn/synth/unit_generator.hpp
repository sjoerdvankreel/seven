#ifndef SVN_UNIT_GENERATOR_HPP
#define SVN_UNIT_GENERATOR_HPP

#include <svn/types.hpp>
#include <svn/audio_buffer.hpp>

namespace svn {

template <typename sample_type>
struct unit_generator 
{
  void
  process_buffer(
    automation_data_t<sample_type> automation_data,
    audio_buffer<sample_type>& output_buffer, 
    sample_type sample_rate,
    size_t buffer_size);
};

} // namespace svn
#endif // SVN_UNIT_GENERATOR_HPP