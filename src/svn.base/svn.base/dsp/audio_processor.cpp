#include <svn.base/dsp/dsp.hpp>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

namespace svn::base {

audio_processor::
audio_processor(
  runtime_topology const* topology, float sample_rate, 
  std::int32_t max_sample_count, base::param_value* state):
_input(),
_notes(static_cast<std::size_t>(topology->max_note_events)),
_audio(static_cast<std::size_t>(max_sample_count)),
_sample_rate(sample_rate),
_state(state),
_topology(topology),
_automation(topology->params.size()),
_automation_buffer(topology->params.size() * max_sample_count)
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  _input.note_count = 0;
  _input.automation = _automation.data();
  _input.notes = topology->max_note_events == 0? nullptr: _notes.data();

  std::int32_t real_p = 0;
  std::int32_t discrete_p = 0;
  for (std::size_t p = 0; p < topology->params.size(); p++)
    _automation[p] = _automation_buffer.data() + p * max_sample_count;
}

audio_sample const*
audio_processor::process_block()
{
  state_check();
  automation_check(_input.sample_count);
  clear_audio(_audio.data(), _input.sample_count);
  transform_automation();
  process_block(_input, _audio.data());
  return _audio.data();
}

void
audio_processor::state_check()
{
  for (std::size_t p = 0; p < _topology->params.size(); p++)
  {
    auto const& descriptor = *_topology->params[p].descriptor;
    if (descriptor.type == param_type::real)
      assert(0.0 <= _state[p].real && _state[p].real <= 1.0);
    else
      assert(descriptor.min.discrete <= _state[p].discrete &&
        _state[p].discrete <= descriptor.max.discrete);
  }
}

block_input&
audio_processor::prepare_block(std::int32_t sample_count)
{
  _input.note_count = 0;
  for (std::size_t p = 0; p < _topology->params.size(); p++)
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
  for (std::size_t p = 0; p < _topology->params.size(); p++)
  {
    auto const& descriptor = *_topology->params[p].descriptor;
    for (std::int32_t s = 0; s < sample_count; s++)
      if(descriptor.type == param_type::real)
        assert(0.0f <= _input.automation[p][s].real && _input.automation[p][s].real <= 1.0f);
      else
        assert(descriptor.min.discrete <= _input.automation[p][s].discrete &&
          _input.automation[p][s].discrete <= descriptor.max.discrete);
  }
}

void
audio_processor::transform_automation()
{
  for (std::size_t p = 0; p < _topology->params.size(); p++)
  {
    auto const& descriptor = *_topology->params[p].descriptor;
    if (descriptor.type == param_type::real)
      for (std::int32_t s = 0; s < _input.sample_count; s++)
        _input.automation[p][s].real = descriptor.dsp.to_range(_input.automation[p][s].real);
  }
}

} // namespace svn::base