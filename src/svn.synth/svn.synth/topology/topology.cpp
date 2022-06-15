#include <svn.synth/topology/topology.hpp>

using namespace svn::base;

namespace svn::synth {
  
static item_name const
voice_osc_types[voice_osc_type::count] =
{
  { L"Sine", L"Sine wave" },
  { L"Blep", L"Bandlimited step" },
  { L"Blmp", L"Bandlimited ramp" }
}; 

static item_name const
voice_osc_blep_types[voice_osc_blep_type::count] =
{
  { L"Saw", L"Sawtooth" },
  { L"Pulse", L"Pulse" },
  { L"Triangle", L"Triangle" }
};

static item_name const
voice_osc_blmp_types[voice_osc_blmp_type::count] =
{
  { L"Saw", L"Sawtooth" },
  { L"Pulse", L"Pulse" }
};

param_descriptor const 
glob_output_params[glob_output_param::count] =
{
  { { L"Clip", L"Clip" }, false, { 0, 1, false, -1, 0 } },
  { { L"Drain", L"Voices exhausted" }, false, { 1, 1, false, -1, 0 } },
  { { L"Voices", L"Voice count" }, L"", false, 0, synth_polyphony, 0, { 2, 2, false, -1, 0 } }
};

param_descriptor const
voice_amp_params[voice_amp_param::count] =
{
  { { L"Amp", L"Level" }, L"dB", { 1.0, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 1, true, -1, 0 } },
  { { L"Dcy", L"Decay" }, L"Sec", { 0.0, 2, real_bounds::unit(), real_bounds::unit() }, { 1, 1, false, -1, 0 } }
};

param_descriptor const
voice_osc_params[voice_osc_param::count] =
{
  { { L"On", L"Enabled" }, false, { - 1, 0, false, -1, 0 } },
  { { L"Type", L"Type" }, L"", false, voice_osc_types, voice_osc_type::count, { 0, 0, false, -1, 0 } },
  { { L"Blep", L"Blep type" }, L"", false, voice_osc_blep_types, voice_osc_blep_type::count, {1, 0, false, voice_osc_param::type, voice_osc_type::blep}},
  { { L"Blmp", L"Blmp type" }, L"", false, voice_osc_blmp_types, voice_osc_blmp_type::count, { 1, 0, false, voice_osc_param::type, voice_osc_type::blmp } },
  { { L"Pan", L"Panning" }, L"%", { 0.5, 0, real_bounds::unit(), real_bounds::linear(-100.0, 100.0) }, { 2, 1, false, -1, 0 } },
  { { L"Oct", L"Octave" }, L"", true, 0, 9, 4, { 3, 2, false, -1, 0 } },
  { { L"Note", L"Note" }, L"", true, note_names, 12, { 4, 2, false, -1, 0 } },
  { { L"Cent", L"Cent" }, L"", { 0.5, 0, real_bounds::linear(-0.5, 0.5), real_bounds::linear(-50.0, 50.0) }, { 5, 2, false, -1, 0 } }
};  
 
part_descriptor const 
part_descriptors[part_type::count] =
{
  { { L"Osc", L"Voice oscillator" }, part_type::voice_osc, false, voice_osc_count, voice_osc_params, voice_osc_param::count, { 1, 3, 0 } },
  { { L"Amp", L"Voice level" }, part_type::voice_amp, false, 1, voice_amp_params, voice_amp_param::count, { 5, 2, -1 } },
  { { L"Out", L"Output" }, part_type::glob_output, true, 1, glob_output_params, glob_output_param::count, { 6, 1, -1 } }
};
 
} // namespace svn::synth