#include <svn.base/dsp/support.hpp>
#include <svn.base/dsp/audio_processor.hpp>
#include <cassert>

namespace svn::base {

audio_processor::
audio_processor(
  topology_info const* topology, float sample_rate,
  std::int32_t max_sample_count, base::param_value* state):
_input(), _output(), 
_notes(static_cast<std::size_t>(topology->max_note_events)),
_audio(static_cast<std::size_t>(max_sample_count)),
_sample_rate(sample_rate),
_state(state),
_topology(topology),
_automation(static_cast<std::size_t>(topology->input_param_count)),
_automation_buffer(static_cast<std::size_t>(topology->input_param_count)* max_sample_count),
_output_param_buffer(static_cast<std::size_t>(topology->output_param_count))
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  _output.audio = _audio.data();
  _output.output_params = _output_param_buffer.data();
  _input.note_count = 0;
  _input.automation = _automation.data();
  _input.notes = topology->max_note_events == 0? nullptr: _notes.data();

  for (std::int32_t p = 0; p < topology->input_param_count; p++)
    _automation[p] = _automation_buffer.data() + p * max_sample_count;
}

block_output const&
audio_processor::process_block()
{
  _topology->state_check(_state);
  automation_check(_input.sample_count);
  std::uint64_t state = disable_denormals();
  process_block(_input, _output);
  restore_denormals(state);
  for(std::int32_t s = 0; s < _input.sample_count; s++)
    sanity_audio(_output.audio[s]);
  return _output;
}

block_input&
audio_processor::prepare_block(std::int32_t sample_count)
{
  _input.note_count = 0;
  for (std::int32_t p = 0; p < _topology->output_param_count; p++)
  {
    auto const& descriptor = *_topology->params[_topology->input_param_count + p].descriptor;
    if (descriptor.type == param_type::real)
      _output.output_params[p].real = 0.0f;
    else
      _output.output_params[p].discrete = descriptor.discrete.default_;
  }
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    auto const& descriptor = *_topology->params[p].descriptor;
    if (descriptor.type == param_type::real)
      for (std::int32_t s = 0; s < sample_count; s++)
        _input.automation[p][s].real = _state[p].real;
    else
      for (std::int32_t s = 0; s < sample_count; s++)
        _input.automation[p][s].discrete = _state[p].discrete;
  }
  return _input;
}

void
audio_processor::automation_check(std::int32_t sample_count)
{
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
  {
    auto const& descriptor = *_topology->params[p].descriptor;
    for (std::int32_t s = 0; s < sample_count; s++)
      if(descriptor.type == param_type::real)
        assert(0.0f <= _input.automation[p][s].real && _input.automation[p][s].real <= 1.0f);
      else
      {
        std::int32_t val = _input.automation[p][s].discrete;
        (void)val;
        assert(descriptor.discrete.min <= val && val <= descriptor.discrete.max);
      }
  }
}

} // namespace svn::base