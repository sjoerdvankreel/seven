#ifndef SVN_SYNTH_HPP
#define SVN_SYNTH_HPP

#include <svn/types.hpp>
#include <svn/params.hpp>
#include <svn/audio_buffer.hpp>
#include <svn/unit_generator.hpp>

#include <cstddef>

namespace svn {

template <typename sample_type>
class synth 
{
public:
  static inline constexpr std::size_t unit_count = 2;
private:
  std::size_t _max_buffer_size;
  sample_type const _sample_rate;
  unit_generator<sample_type> _units[unit_count];
public:
  void
  process_buffer(
    automation_data_t<sample_type> automation_data,
    audio_buffer<sample_type>& output_buffer,
    size_t buffer_size);
};

} // namespace svn
#endif // SVN_SYNTH_HPP