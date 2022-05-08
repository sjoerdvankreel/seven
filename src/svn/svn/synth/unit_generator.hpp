#ifndef SVN_SYNTH_UNIT_GENERATOR_HPP
#define SVN_SYNTH_UNIT_GENERATOR_HPP

#include <svn/param/synth_params.hpp>
#include <svn/support/process_info.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/automation/automation_buffer.hpp>
#include <svn/automation/unit_automation_buffer.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
class unit_generator 
{
  sample_type _phase = 0.0f;
  sample_type _gain = synth_params::unit_gain_info.default_value;
  sample_type _panning = synth_params::unit_panning_info.default_value;
private:
  void process_automation(
    std::int32_t unit_index,
    std::int32_t sample_index,
    unit_automation_buffer<sample_type>& unit_automation);
public:
  void process_buffer(
    process_info<sample_type> const& info,
    audio_buffer<sample_type>& audio,
    automation_buffer<sample_type> const& automation,
    unit_automation_buffer<sample_type>& unit_automation);
};

} // namespace svn
#endif // SVN_SYNTH_UNIT_GENERATOR_HPP