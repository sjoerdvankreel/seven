#include <svn/synth/unit_generator.hpp>
#include <cmath>

namespace svn {

template <typename sample_type>
void unit_generator<sample_type>::
process_automation(
  std::int32_t unit_index,
  std::int32_t sample_index,
  unit_automation_buffer<sample_type>& unit_automation)
{
  for (std::int32_t p = 0; p < unit_automation.param_count; p++)
  {
    param_id id = unit_automation.params[p];
    sample_type sample = unit_automation.samples[p][sample_index];
    switch (id)
    {
    case param_id::unit1_gain: _gain = sample; break;
    case param_id::unit1_panning: _panning = sample; break;
    default: assert(false); break;
    }
  }
}

template <typename sample_type>
void unit_generator<sample_type>::
process_buffer(
  process_info<sample_type> const& info,
  audio_buffer<sample_type>& audio,
  event_buffer<sample_type> const& events,
  unit_automation_buffer<sample_type>& unit_automation)
{
  unit_automation.setup(
    param_id::unit1_gain, 
    param_id::unit1_panning, info.unit_index, 
    events.automation_count, events.automation);

  auto one = static_cast<sample_type>(1.0);
  sample_type frequency = info.unit_index == 0 ? 440.0f : 880.0f;
  for (std::int32_t s = 0; s < info.sample_count; s++)
  {
    process_automation(info.unit_index, s, unit_automation);
    sample_type sample = std::sin(_phase);
    audio.samples[s] = { (one - _panning) * sample, _panning * sample };
    _phase += frequency / info.sample_rate;
    _phase -= static_cast<std::int32_t>(_phase);
  }
}

template class unit_generator<float>;
template class unit_generator<double>;

} // namespace svn