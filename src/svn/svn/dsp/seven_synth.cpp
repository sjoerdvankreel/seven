#include <svn/dsp/shared_dsp.hpp>
#include <svn/dsp/seven_synth.hpp>
#include <svn/support/topo_rt.hpp>

#include <cassert>
#include <cstdint>

namespace svn {

seven_synth::
seven_synth(param_value* state, float sample_rate, std::int32_t max_sample_count):
_input(),
_state(state),
_unit_generators(),
_sample_rate(sample_rate),
_part_audio(static_cast<std::size_t>(max_sample_count)),
_notes(static_cast<std::size_t>(max_sample_count)),
_audio(static_cast<std::size_t>(max_sample_count)),
_automation(static_cast<std::size_t>(synth_param_count)),
_automation_real(static_cast<std::size_t>(synth_param_count), 
  std::vector<float>(static_cast<std::size_t>(max_sample_count))),
_automation_discrete(static_cast<std::size_t>(synth_param_count), 
  std::vector<std::int32_t>(static_cast<std::size_t>(max_sample_count)))
{
  assert(state != nullptr);
  assert(sample_rate > 0.0);

  for (std::int32_t p = 0; p < synth_param_count; p++)
    switch (synth_params[p].info->type)
    {
    case param_type::real: _automation[p] = _automation_real[p].data(); break;
    default: _automation[p] = _automation_discrete[p].data(); break;
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

} // namespace svn