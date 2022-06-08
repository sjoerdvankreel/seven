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
  { { L"Clip", L"Clip" }, false, 1, false },
  { { L"Drain", L"Voices exhausted" }, false, 1, false },
  { { L"Voices", L"Voice count" }, false, 2, L"", 0, 0, synth_polyphony }
};    

param_descriptor const
voice_osc_params[voice_osc_param::count] =
{
  { { L"On", L"Enabled" }, false, 0, false },
  { { L"On2", L"Enabled2" }, false, 0, false },
  { { L"Type", L"Type" }, false, 1, param_type::list, voice_osc_types, voice_osc_type::count },
  { { L"Pan", L"Panning" }, false, 1, L"%", 0.5, 0, param_bounds::linear_unit(), param_bounds::linear(-100.0, 100.0) },
  { { L"Oct", L"Octave" }, false, 2, L"", 4, 0, 9 },
  { { L"Note", L"Note" }, false, 2, param_type::discrete_list, note_names, note_name::count },
  { { L"Cent", L"Cent" }, false, 2, L"", 0.5, 0, param_bounds::linear(-0.5, 0.5), param_bounds::linear(-50.0, 50.0) }
}; 
   
param_descriptor const
voice_amp_params[voice_amp_param::count] =
{
  { { L"Amp", L"Level" }, true, 1, L"dB", 1.0, 1, param_bounds::linear_unit(), param_bounds::decibel() },
  { { L"Dcy", L"Decay" }, false, 1, L"Sec", 0.0, 2, param_bounds::linear_unit(), param_bounds::linear_unit() }
};      
 
part_descriptor const
part_descriptors[part_type::count] =
{
  { part_type::voice_amp, false, { L"Amp", L"Voice level" }, 1, voice_amp_params, voice_amp_param::count, 2, -1 },
  { part_type::voice_osc, false, { L"Osc", L"Voice oscillator" }, voice_osc_count, voice_osc_params, voice_osc_param::count, 3, 0 },
  { part_type::glob_output, true, { L"Out", L"Output" }, 1, glob_output_params, glob_output_param::count, 2, -1 }
};

} // namespace svn::synth