#include <svn/dsp/synth.hpp>
#include <svn/support/topo_rt.hpp>
#include <cstdint>
#include <cassert>

namespace svn {

synth::
synth(std::int32_t max_sample_count):
_part_audio(static_cast<std::size_t>(max_sample_count)),
_input_notes(static_cast<std::size_t>(max_sample_count)),
_output_audio(static_cast<std::size_t>(max_sample_count)),
_output_params(static_cast<std::size_t>(synth_param_count)),
_automation(static_cast<std::size_t>(synth_param_count)),
_automation_real(static_cast<std::size_t>(synth_param_count), 
  std::vector<float>(static_cast<std::size_t>(max_sample_count))),
_automation_discrete(static_cast<std::size_t>(synth_param_count), 
  std::vector<std::int32_t>(static_cast<std::size_t>(max_sample_count)))
{
  for (std::int32_t p = 0; p < synth_param_count; p++)
    switch (synth_params[p].info->type)
    {
    case param_type::list:
      _automation[p] = _automation_discrete[p].data();
      _output_params[p].discrete = 0;
      break;
    case param_type::real:
      _automation[p] = _automation_real[p].data();
      _output_params[p].real = synth_params[p].info->bounds.real.default_;
      break;
    case param_type::toggle:
      _automation[p] = _automation_discrete[p].data();
      _output_params[p].discrete = synth_params[p].info->bounds.toggle.default_;
      break;
    case param_type::discrete:
      _automation[p] = _automation_discrete[p].data();
      _output_params[p].discrete = synth_params[p].info->bounds.discrete.default_;
      break;
    default:
      assert(false);
      break;
    }

  _input.notes = _input_notes.data();
  _input.automation = _automation.data();
  _output.audio = _output_audio.data();
  _output.param_values = _output_params.data();
}

audio_sample*
synth::process()
{
  output_buffer part_output;
  part_output.audio = _part_audio.data();
  _output.clear_audio(_input.sample_count);

  input_buffer part_input;
  part_input.bpm = _input.bpm;
  part_input.notes = _input.notes;
  part_input.note_count = _input.note_count;
  part_input.sample_rate = _input.sample_rate;
  part_input.sample_count = _input.sample_count;

  for (std::int32_t i = 0; i < unit_count; i++)
  {
    std::int32_t offset = synth_bounds[part_type::unit][i];
    part_input.automation = _input.automation + offset;
    part_output.param_values = _output.param_values + offset;
    _units[i].process(part_input, part_output);
    _output.add_audio(part_output.audio, _input.sample_count);
  }

  return _output.audio;
}

} // namespace svn