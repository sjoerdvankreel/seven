#include <svn.synth/seven_synth.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

seven_synth::
seven_synth(
  runtime_topology const* topology,
  float sample_rate, 
  std::int32_t max_sample_count,
  base::param_value* state):
_input(),
_notes(static_cast<std::size_t>(max_sample_count)),
_audio(static_cast<std::size_t>(max_sample_count)),
_part_audio(static_cast<std::size_t>(max_sample_count)),
_automation(static_cast<std::size_t>(synth_param_count)),
_automation_real(static_cast<std::size_t>(real_param_count* max_sample_count)),
_automation_discrete(static_cast<std::size_t>(discrete_param_count* max_sample_count)),
_sample_rate(sample_rate),
_state(state),
_oscillators(),
_topology(topology)
{
  assert(state != nullptr);
  assert(sample_rate > 0.0);

  std::int32_t real_p = 0;
  std::int32_t discrete_p = 0;
  for (std::int32_t p = 0; p < synth_param_count; p++)
    switch (synth_params[p].info->type)
    {
    case param_type::real: _automation[p] = _automation_real.data() + real_p++ * max_sample_count; break;
    default: _automation[p] = _automation_discrete.data() + discrete_p++ * max_sample_count; break;
    }

  _input.notes = _notes.data();
  _input.automation = _automation.data();
}

void
seven_synth::state_check()
{
  for(std::int32_t i = 0; i < synth_param_count; i++)
    if(synth_params[i].info->type == param_type::real)
      assert(0.0 <= _state[i].real && _state[i].real <= 1.0);
    else
      assert(synth_params[i].info->min.discrete <= _state[i].discrete && 
        _state[i].discrete <= synth_params[i].info->max.discrete);
}

audio_sample*
seven_synth::process_block()
{
  input_buffer part_input;
  part_input.bpm = _input.bpm;
  part_input.notes = _input.notes;
  part_input.sample_rate = _sample_rate;
  part_input.note_count = _input.note_count;
  part_input.sample_count = _input.sample_count;

  state_check();
  automation_check(_input.sample_count);
  clear_audio(_audio.data(), _input.sample_count);

  for (std::int32_t i = 0; i < unit_count; i++)
  {
    std::int32_t offset = synth_bounds[part_type::unit][i];
    part_input.automation = _input.automation + offset;
    _unit_generators[i].process_block(part_input, _part_audio.data(), _state + offset);
    add_audio(_audio.data(), _part_audio.data(), _input.sample_count);
  }

  state_check();
  return _audio.data();
}

input_buffer&
seven_synth::prepare_block(std::int32_t sample_count)
{
  for (std::int32_t p = 0; p < svn::synth_param_count; p++)
    if (svn::synth_params[p].info->type != svn::param_type::real)
      for (std::int32_t s = 0; s < sample_count; s++)
        static_cast<std::int32_t*>(_input.automation[p])[s] = _state[p].discrete;
    else
      for (std::int32_t s = 0; s < sample_count; s++)
        static_cast<float*>(_input.automation[p])[s] = static_cast<float>(_state[p].real);
  return _input;
}

void
seven_synth::automation_check(std::int32_t sample_count)
{
  for (std::int32_t p = 0; p < synth_param_count; p++)
    for (std::int32_t s = 0; s < sample_count; s++)
      if (synth_params[p].info->type == param_type::real)
        assert(0.0f <= static_cast<float*>(_input.automation[p])[s] &&
          static_cast<float*>(_input.automation[p])[s] <= 1.0f);
      else
        assert(synth_params[p].info->min.discrete <= static_cast<std::int32_t*>(_input.automation[p])[s] &&
          static_cast<std::int32_t*>(_input.automation[p])[s] <= synth_params[p].info->max.discrete);
}

} // namespace svn::synth