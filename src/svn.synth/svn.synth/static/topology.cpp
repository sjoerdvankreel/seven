#include <svn.synth/static/topology.hpp>
#include <svn.base/support/item_name.hpp>
#include <svn.base/support/note_name.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

using namespace svn::base;

namespace svn::synth {

std::int32_t const 
synth_ui_order[part_type::count] = 
{
  part_type::voice_osc,
  part_type::voice_amp,
  part_type::glob_output
}; 

static item_name const
voice_osc_types[voice_osc_type::count] =
{
  { L"Sine", L"Sine wave" },
  { L"Blep", L"Bandlimited step" }
};

param_descriptor const 
glob_output_params[glob_output_param::count] =
{
  { { L"Clip", L"Clip" }, false },
  { { L"Drain", L"Voices exhausted" }, false },
  { { L"Voices", L"Voice count" }, L"", 0, 0, synth_polyphony }
};

param_descriptor const
voice_osc_params[voice_osc_param::count] =
{
  { { L"On", L"Enabled" }, false },
  { { L"Type", L"Type" }, param_type::list, voice_osc_types, voice_osc_type::count },
  { { L"Pan", L"Panning" }, L"%", 0.5, param_bounds::linear_unit(), param_bounds::linear(-100.0, 100.0) },
  { { L"Oct", L"Octave" }, L"", 4, 0, 9 },
  { { L"Note", L"Note" }, param_type::discrete, note_names, note_name::count },
  { { L"Cent", L"Cent" }, L"", 0.5, param_bounds::linear(-0.5, 0.5), param_bounds::linear(-50.0, 50.0) }
};

param_descriptor const
voice_amp_params[voice_amp_param::count] =
{
  { { L"Amp", L"Level" }, L"dB", 1.0, param_bounds::linear_unit(), param_bounds::decibel() },
  { { L"Decay", L"Decay" }, L"Sec", 0.0, param_bounds::linear_unit(), param_bounds::linear_unit() }
};

part_descriptor const
part_descriptors[part_type::count] =
{
  { part_type::voice_amp, false, { L"Amp", L"Voice level" }, 1, voice_amp_params, voice_amp_param::count, 2 },
  { part_type::voice_osc, false, { L"Osc", L"Voice oscillator" }, voice_osc_count, voice_osc_params, voice_osc_param::count, 3 },
  { part_type::glob_output, true, { L"Out", L"Output" }, 1, glob_output_params, glob_output_param::count, 2 }
};

} // namespace svn::synth