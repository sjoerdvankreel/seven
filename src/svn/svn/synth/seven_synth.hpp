#ifndef SVN_SYNTH_SEVEN_SYNTH_HPP
#define SVN_SYNTH_SEVEN_SYNTH_HPP

#include <svn/support/audio_sample.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/synth/unit_generator.hpp>

#include <vector>
#include <cstddef>

namespace svn {

template <typename sample_type>
class seven_synth 
{
public:
  static inline constexpr std::size_t unit_count = 2;
private:
  unit_generator<sample_type> _units[unit_count];
private:
  std::size_t _max_buffer_size;
  sample_type const _sample_rate;
  std::vector<audio_sample<sample_type>> _intermediate_buffer;
public:
  seven_synth(
    sample_type sample_rate,
    std::size_t max_buffer_size);
public:
  void
  process_buffer(
    audio_buffer<sample_type>& buffer,
    automation_data<sample_type> const& automation);
};

} // namespace svn
#endif // SVN_SYNTH_SEVEN_SYNTH_HPP