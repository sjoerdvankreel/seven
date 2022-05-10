#include <svn/synth/unit_generator.hpp>
#include <cmath>

namespace svn {

template <typename sample_type>
void unit_generator<sample_type>::
process_buffer(
  process_info<sample_type> const& info,
  audio_buffer<sample_type>& audio,
  event_buffer<sample_type> const& events,
  unit_automation_buffer<sample_type>& unit_automation)
{
  unit_automation.setup(
    param_id::unit1_level, 
    param_id::unit1_pulse_width, info.unit_index,
    events.automation_count, events.automation);

  auto one = static_cast<sample_type>(1.0);
  sample_type frequency = info.unit_index == 0 ? 440.0f : 880.0f;
  for (std::int32_t s = 0; s < info.sample_count; s++)
  {
    sample_type sample = std::sin(_phase);
    unit_automation.process(s, *info.param_state);
    sample_type level = unit_automation.value(param_id::unit1_level, *info.param_state);
    sample_type panning = unit_automation.value(param_id::unit1_panning, *info.param_state);
    audio.samples[s] = { (one - panning) * sample, panning * sample };
    _phase += frequency / info.sample_rate;
    _phase -= static_cast<std::int32_t>(_phase);
  }
}

template class unit_generator<float>;
template class unit_generator<double>;

} // namespace svn