#ifndef SVN_SYNTH_SEVEN_SYNTH_HPP
#define SVN_SYNTH_SEVEN_SYNTH_HPP

#include <svn/synth/unit_generator.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/support/automation_data.hpp>

#include <vector>
#include <cstddef>
#include <cstdint>

namespace svn {

template <typename sample_type>
class seven_synth 
{
public:
  static inline constexpr std::int32_t unit_count = 2;
private:
  unit_generator<sample_type> _units[unit_count];
private:
  std::size_t _max_sample_count;
  sample_type const _sample_rate;
  std::vector<audio_sample<sample_type>> _audio_scratch;
public:
  seven_synth(sample_type sample_rate, std::size_t max_buffer_size);
public:
  void process_buffer(
    std::size_t sample_count,
    audio_buffer<sample_type>& audio,
    automation_data<sample_type> const& automation);
};

} // namespace svn
#endif // SVN_SYNTH_SEVEN_SYNTH_HPP