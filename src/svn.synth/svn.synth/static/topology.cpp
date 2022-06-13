#include <svn.synth/static/topology.hpp>
#include <svn.base/support/item_name.hpp>
#include <svn.base/support/note_name.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

using namespace svn::base;

namespace svn::synth {
  
static item_name const
voice_osc_types[voice_osc_type::count] =
{
  { L"Sine", L"Sine wave" },
  { L"Blep", L"Bandlimited step" }
}; 

static item_name const
voice_osc_blep_types[voice_osc_blep_type::count] =
{
  { L"Saw", L"Sawtooth" },
  { L"Pulse", L"Pulse" },
  { L"Triangle", L"Triangle" }
};

param_descriptor const 
glob_output_params[glob_output_param::count] =
{
  { { L"Clip", L"Clip" }, false, 0, 1, false, -1, 0 },
  { { L"Drain", L"Voices exhausted" }, false, 1, 1, false, -1, 0 },
  { { L"Voices", L"Voice count" }, param_type::discrete_text, L"", 0, 0, synth_polyphony, 2, 2, false, -1, 0 }
};    

param_descriptor const
voice_osc_params[voice_osc_param::count] =
{
  { { L"On", L"Enabled" }, false, 0, 0, false, -1, 0 },
  { { L"Type", L"Type" }, param_type::list, voice_osc_types, voice_osc_type::count, 1, 0, false, -1, 0 },
  { { L"Blep", L"Blep type" }, param_type::list, voice_osc_blep_types, voice_osc_blep_type::count, 2, 0, false, voice_osc_param::type, voice_osc_type::blep },
  { { L"Pan", L"Panning" }, L"%", 0.5, 0, param_bounds::linear_unit(), param_bounds::linear(-100.0, 100.0), 3, 1, false, -1, 0 },
  { { L"Oct", L"Octave" }, param_type::discrete, L"", 4, 0, 9, 4, 2, false, -1, 0 },
  { { L"Note", L"Note" }, param_type::discrete_list, note_names, note_name::count, 5, 2, false, -1, 0 },
  { { L"Cent", L"Cent" }, L"", 0.5, 0, param_bounds::linear(-0.5, 0.5), param_bounds::linear(-50.0, 50.0), 6, 2, false, -1, 0 }
}; 
   
param_descriptor const
voice_amp_params[voice_amp_param::count] =
{
  { { L"Amp", L"Level" }, L"dB", 1.0, 1, param_bounds::linear_unit(), param_bounds::decibel(), 0, 1, true, -1, 0 },
  { { L"Dcy", L"Decay" }, L"Sec", 0.0, 2, param_bounds::linear_unit(), param_bounds::linear_unit(), 1, 1, false, -1, 0 }
};      
 
part_descriptor const 
part_descriptors[part_type::count] =
{
  { part_type::voice_osc, false, { L"Osc", L"Voice oscillator" }, voice_osc_count, voice_osc_params, voice_osc_param::count, 3, 0, 1 },
  { part_type::voice_amp, false, { L"Amp", L"Voice level" }, 1, voice_amp_params, voice_amp_param::count, 2, -1, 5 },
  { part_type::glob_output, true, { L"Out", L"Output" }, 1, glob_output_params, glob_output_param::count, 1, -1, 6 }
};
 
} // namespace svn::synth