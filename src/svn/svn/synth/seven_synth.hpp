#ifndef SVN_SYNTH_SEVEN_SYNTH_HPP
#define SVN_SYNTH_SEVEN_SYNTH_HPP

#include <svn/param/param_id.hpp>
#include <svn/event/event_buffer.hpp>
#include <svn/synth/unit_generator.hpp>
#include <svn/support/audio_buffer.hpp>

#include <vector>
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
  std::int32_t _max_sample_count;
  sample_type const _sample_rate;
  std::vector<audio_sample<sample_type>> _audio_scratch;
  std::vector<param_id> _unit_automation_id_scratch;
  std::vector<sample_type const*> _unit_automation_sample_scratch;
public:
  seven_synth(
    sample_type sample_rate, 
    std::int32_t max_buffer_size);
public:
  void process_buffer(
    std::int32_t sample_count,
    audio_buffer<sample_type>& audio,
    event_buffer<sample_type> const& events);
};

} // namespace svn
#endif // SVN_SYNTH_SEVEN_SYNTH_HPP