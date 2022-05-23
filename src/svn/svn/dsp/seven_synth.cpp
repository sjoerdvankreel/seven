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

audio_sample*
seven_synth::process()
{
  input_buffer part_input;
  part_input.bpm = _input.bpm;
  part_input.notes = _input.notes;
  part_input.sample_rate = _sample_rate;
  part_input.note_count = _input.note_count;
  part_input.sample_count = _input.sample_count;

  clear_audio(_audio.data(), _input.sample_count);
  for (std::int32_t i = 0; i < unit_count; i++)
  {
    std::int32_t offset = synth_bounds[part_type::unit][i];
    part_input.automation = _input.automation + offset;
    _unit_generators[i].process(part_input, _part_audio.data(), _state + offset);
    add_audio(_audio.data(), _part_audio.data(), _input.sample_count);
  }

  return _audio.data();
}

} // namespace svn