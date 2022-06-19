#include <svn.synth/topology/topology.hpp>

using namespace svn::base;

namespace svn::synth {
  
static item_name const
oscillator_types[oscillator_type::count] =
{
  { L"Sine", L"Sine wave" },
  { L"Blep", L"Bandlimited step" },
  { L"Blmp", L"Bandlimited ramp" }
}; 

static item_name const
oscillator_blep_types[oscillator_blep_type::count] =
{
  { L"Saw", L"Sawtooth" },
  { L"Pulse", L"Pulse" },
  { L"Triangle", L"Triangle" }
};

static item_name const
oscillator_blmp_types[oscillator_blmp_type::count] =
{
  { L"Saw", L"Sawtooth" },
  { L"Pulse", L"Pulse" }
};

static graph_descriptor const
oscillator_graph = { 0, 3, 2, 1 };

param_descriptor const 
output_params[output_param::count] =
{
  { { L"Clip", L"Clip" }, false, { 0, 1, false, -1, 0 } },
  { { L"Drain", L"Voices exhausted" }, false, { 1, 1, false, -1, 0 } },
  { { L"Voices", L"Voice count" }, L"", false, 0, synth_polyphony, 0, { 2, 2, false, -1, 0 } }
};

param_descriptor const
amplitude_params[amplitude_param::count] =
{
  { { L"Amp", L"Level" }, L"dB", { 1.0, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 1, true, -1, 0 } },
  { { L"Dcy", L"Decay" }, L"Sec", { 0.0, 2, real_bounds::unit(), real_bounds::unit() }, { 1, 1, false, -1, 0 } }
};    
    
param_descriptor const     
oscillator_params[oscillator_param::count] =
{
  { { L"On", L"Enabled" }, false, { -1, 0, false, -1, 0 } },
  { { L"Type", L"Type" }, L"", false, oscillator_types, oscillator_type::count, { 0, 0, false, -1, 0 } },
  { { L"Blep", L"Blep type" }, L"", false, oscillator_blep_types, oscillator_blep_type::count, {1, 0, false, oscillator_param::type, oscillator_type::blep}},
  { { L"Blmp", L"Blmp type" }, L"", false, oscillator_blmp_types, oscillator_blmp_type::count, { 1, 0, false, oscillator_param::type, oscillator_type::blmp } },
  { { L"Pan", L"Panning" }, L"%", { 0.5, 0, real_bounds::unit(), real_bounds::linear(-100.0, 100.0) }, { 2, 1, false, -1, 0 } },
  { { L"Oct", L"Octave" }, L"", true, 0, 9, 4, { 3, 2, false, -1, 0 } },
  { { L"Note", L"Note" }, L"", true, note_names, 12, { 4, 2, false, -1, 0 } },
  { { L"Cent", L"Cent" }, L"", { 0.5, 0, real_bounds::linear(-0.5, 0.5), real_bounds::linear(-50.0, 50.0) }, { 5, 2, false, -1, 0 } }
};   
      
part_descriptor const 
part_descriptors[part_type::count] =
{
  { { L"Osc", L"Oscillator" }, part_type::oscillator, false, oscillator_count, oscillator_params, oscillator_param::count, &oscillator_graph, { 1, 4, 0 } },
  { { L"Amp", L"Amplitude" }, part_type::amplitude, false, 1, amplitude_params, amplitude_param::count, nullptr, { 5, 2, -1 } },
  { { L"Out", L"Output" }, part_type::output, true, 1, output_params, output_param::count, nullptr, { 6, 1, -1 } }
}; 
 
} // namespace svn::synth