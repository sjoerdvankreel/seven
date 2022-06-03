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
_notes(static_cast<std::size_t>(max_sample_count* topology->polyphony)),
_audio(static_cast<std::size_t>(max_sample_count)),
_automation_buffer(topology, max_sample_count),
_sample_notes(static_cast<std::size_t>(max_sample_count)),
_sample_note_counts(static_cast<std::size_t>(max_sample_count)),
_sample_rate(sample_rate),
_state(state),
_topology(topology)
{
  assert(state != nullptr);
  assert(sample_rate > 0.0f);
  assert(topology != nullptr);
  assert(max_sample_count > 0);

  for (std::int32_t s = 0; s < max_sample_count; s++)
    _sample_notes[s] = _notes.data() + s * topology->polyphony;
  _input.automation = _automation_buffer.data();
  _input.notes = topology->polyphony == 0? nullptr: _sample_notes.data();
  _input.note_count = topology->polyphony == 0 ? nullptr : _sample_note_counts.data();
}

audio_sample const*
audio_processor::process_block()
{
  state_check();
  automation_check(_input.sample_count);
  clear_audio(_audio.data(), _input.sample_count);
  transform_automation();
  process_block(_input, _audio.data());
  state_check();
  return _audio.data();
}

void 
audio_processor::transform_automation()
{
  for(std::size_t p = 0; p < _topology->params.size(); p++)
  {
    auto automation = static_cast<float*>(_input.automation[p]);
    for(std::int32_t s = 0; s < _input.sample_count; s++)
      automation[s] = _topology->params[p].descriptor->dsp.to_range(automation[s]);
  }
}

void
audio_processor::state_check()
{
  for (std::size_t i = 0; i < _topology->params.size(); i++)
    if (_topology->params[i].descriptor->type == param_type::real)
      assert(0.0 <= _state[i].real && _state[i].real <= 1.0);
    else
      assert(_topology->params[i].descriptor->min.discrete <= _state[i].discrete &&
        _state[i].discrete <= _topology->params[i].descriptor->max.discrete);
}

block_input&
audio_processor::prepare_block(std::int32_t sample_count)
{
  for(std::int32_t s = 0; s < sample_count; s++)
    _input.note_count[s] = 0;
  for (std::size_t p = 0; p < _topology->params.size(); p++)
    if (_topology->params[p].descriptor->type != param_type::real)
      for (std::int32_t s = 0; s < sample_count; s++)
        static_cast<std::int32_t*>(_input.automation[p])[s] = _state[p].discrete;
    else
      for (std::int32_t s = 0; s < sample_count; s++)
        static_cast<float*>(_input.automation[p])[s] = static_cast<float>(_state[p].real);
  return _input;
}

void
audio_processor::automation_check(std::int32_t sample_count)
{
  for (std::size_t p = 0; p < _topology->params.size(); p++)
    for (std::int32_t s = 0; s < sample_count; s++)
      if (_topology->params[p].descriptor->type == param_type::real)
        assert(0.0f <= static_cast<float*>(_input.automation[p])[s] &&
          static_cast<float*>(_input.automation[p])[s] <= 1.0f);
      else
        assert(_topology->params[p].descriptor->min.discrete <= static_cast<std::int32_t*>(_input.automation[p])[s] &&
          static_cast<std::int32_t*>(_input.automation[p])[s] <= _topology->params[p].descriptor->max.discrete);
}

} // namespace svn::base