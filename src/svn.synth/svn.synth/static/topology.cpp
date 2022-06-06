#include <svn.synth/static/topology.hpp>
#include <svn.base/support/item_name.hpp>
#include <svn.base/support/note_name.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

using namespace svn::base;

namespace svn::synth {

std::int32_t const 
synth_ui_order[part_type::count + 1] = 
{
  part_type::voice_amp,
  ui_order_output_params,
  part_type::voice_osc
};

static item_name const
voice_osc_types[voice_osc_type::count] =
{
  { L"Sin", L"Sine wave" },
  { L"Blep", L"Bandlimited step" }
};

param_descriptor const 
output_params[output_param::count] =
{
  { { L"Clip", L"Clip" }, false },
  { { L"Exhausted", L"Exhausted" }, false },
  { { L"Voices", L"Voice count" }, L"", 0, 0, synth_polyphony }
};

part_descriptor const
part_descriptors[part_type::count] =
{
  { part_type::voice_amp, { L"Amp", L"Voice level" }, 1, voice_amp_params, voice_amp_param::count, 2 },
  { part_type::voice_osc, { L"Osc", L"Voice oscillator" }, voice_osc_count, voice_osc_params, voice_osc_param::count, 3 }
};

param_descriptor const
voice_osc_params[voice_osc_param::count] =
{
  { { L"On", L"Enabled" }, false },
  { { L"Type", L"Type" }, voice_osc_types, voice_osc_type::count },
  { { L"Pan", L"Panning" }, L"%", 0.5, param_bounds::linear_unit(), param_bounds::linear(-100.0, 100.0) },
  { { L"Oct", L"Octave" }, L"", 4, 0, 9 },
  { { L"Note", L"Note" }, note_names, note_name::count },
  { { L"Dtn", L"Detune" }, L"Cent", 0.5, param_bounds::linear(-0.5, 0.5), param_bounds::linear(-50.0, 50.0) }
};

param_descriptor const
voice_amp_params[voice_amp_param::count] =
{
  { { L"Amp", L"Level" }, L"dB", 1.0, param_bounds::linear_unit(), param_bounds::decibel() },
  { { L"Dcy", L"Decay" }, L"Sec", 0.0, param_bounds::linear_unit(), param_bounds::linear_unit() }
};

} // namespace svn::synth