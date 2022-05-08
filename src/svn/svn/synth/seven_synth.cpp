#include <svn/synth/seven_synth.hpp>

namespace svn {

template <typename sample_type>
seven_synth<sample_type>::
seven_synth(param_state_t* state):
_param_state(state),
_audio_scratch(),
_unit_automation_id_scratch(param_id::count),
_unit_automation_sample_scratch(param_id::count) {}

template <typename sample_type>
void seven_synth<sample_type>::
init(
  sample_type sample_rate,
  std::int32_t max_sample_count)
{
  _sample_rate = sample_rate;
  _max_sample_count = max_sample_count;
  _audio_scratch.resize(max_sample_count);
}

template <typename sample_type>
void seven_synth<sample_type>::
process_buffer(
  std::int32_t sample_count,
  audio_buffer<sample_type>& audio,
  event_buffer<sample_type> const& events)
{
  process_info<sample_type> info;
  info.sample_rate = _sample_rate;
  info.sample_count = sample_count;

  audio.clear(sample_count);
  audio_buffer<sample_type> audio_scratch;
  audio_scratch.samples = _audio_scratch.data();

  unit_automation_buffer<sample_type> unit_automation;
  unit_automation.param_count = 0;
  unit_automation.params = _unit_automation_id_scratch.data();
  unit_automation.samples = _unit_automation_sample_scratch.data();

  for (std::int32_t i = 0; i < unit_count; i++)
  {
    info.unit_index = i;
    _units[i].process_buffer(info, audio_scratch, events, unit_automation);
    audio.add(sample_count, audio_scratch);
  }
}

template class seven_synth<float>;
template class seven_synth<double>;

} // namespace svn