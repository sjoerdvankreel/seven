#include <svn.synth/topology/topology.hpp>

using namespace svn::base;

namespace svn::synth {
  
static item_name const
oscillator_types[oscillator_type::count] =
{
  { L"Anlg", L"Analog" },
  { L"Dsf", L"Discrete summation formulae" },
}; 
            
static item_name const 
oscillator_anlg_types[oscillator_anlg_type::count] =
{
  { L"Sin", L"Sine" },
  { L"Saw", L"Sawtooth" },
  { L"Pulse", L"Pulse" },
  { L"Tri", L"Triangle" }
};

static graph_descriptor const
oscillator_graph = { 0, 2, 2, 1 };

param_descriptor const 
output_params[output_param::count] =
{
  { { L"Clip", L"Clip" }, false, { 0, 1, false, { }, { } } },
  { { L"Drain", L"Voices exhausted" }, false, { 1, 1, false, { }, { } } },
  { { L"Voices", L"Voice count" }, L"", false, false, 0, synth_polyphony, 0, { 2, 2, false, { }, { } } }
};

param_descriptor const
amplitude_params[amplitude_param::count] =
{
  { { L"Amp", L"Level" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 1, true, { }, { } } },
  { { L"Dcy", L"Decay" }, L"Sec", { 0.0f, 2, real_bounds::unit(), real_bounds::unit() }, { 1, 1, false, { }, { } } }
};    
        
param_descriptor const       
oscillator_params[oscillator_param::count] =
{
  { { L"On", L"Enabled" }, false, { -1, 0, false, { }, { } } },
  { { L"Amp", L"Level" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 0, true, { }, { } } },
  { { L"Pan", L"Panning" }, L"%", { 0.5f, 0, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { 1, 0, false, { }, { } } },
  { { L"Type", L"Type" }, L"", false, oscillator_types, oscillator_type::count, { 2, 2, false, { }, { } } },
  { { L"Anlg", L"Analog type" }, L"", false, oscillator_anlg_types, oscillator_anlg_type::count, { 3, 2, false, { oscillator_param::type }, { oscillator_type::analog } } },
  { { L"Dtn", L"Analog detune" }, L"", { 0.0f, 1, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { 4, 2, true, { oscillator_param::type }, { oscillator_type::analog }}},
  { { L"Sprd", L"Analog spread" }, L"", true, false, 1, oscillator_max_spread, 1, { 5, 2, false, { oscillator_param::type }, { oscillator_type::analog } } },
  { { L"Pw", L"Analog pulse width" }, L"%", { 1.0f, 0, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }, { 6, 2, false, { oscillator_param::type, oscillator_param::anlg_type }, { oscillator_type::analog, oscillator_anlg_type::pulse } } },
  { { L"Prts", L"Dsf partials" }, L"", true, true, 0, 99, 0, { 4, 2, true, { oscillator_param::type }, { oscillator_type::dsf } } },
  { { L"Dist", L"Dsf distance" }, L"", { 1.0f, 1, real_bounds::linear(0.0f, 20.0f), real_bounds::linear(0.0f, 20.0f) }, { 5, 2, false, { oscillator_param::type }, { oscillator_type::dsf } } },
  { { L"Roll", L"Dsf rolloff" }, L"", { 1.0f, 2, real_bounds::unit(), real_bounds::unit() }, { 6, 2, false, { oscillator_param::type}, {oscillator_type::dsf } } },
  { { L"Oct", L"Octave" }, L"", true, false, 0, 9, 4, { 7, 1, false, { }, { } } },
  { { L"Note", L"Note" }, L"", true, note_names, 12, { 8, 1, false, { }, { } } },
  { { L"Cent", L"Cent" }, L"", { 0.5f, 0, real_bounds::linear(-0.5f, 0.5f), real_bounds::linear(-50.0f, 50.0f) }, { 9, 1, false, { }, { } } },
}; 
      
part_descriptor const 
part_descriptors[part_type::count] =
{
  { { L"Osc", L"Oscillator" }, part_type::oscillator, false, oscillator_count, oscillator_params, oscillator_param::count, &oscillator_graph, { 1, 3, 0 } },
  { { L"Amp", L"Amplitude" }, part_type::amplitude, false, 1, amplitude_params, amplitude_param::count, nullptr, { 5, 2, -1 } },
  { { L"Out", L"Output" }, part_type::output, true, 1, output_params, output_param::count, nullptr, { 6, 1, -1 } }
}; 
 
} // namespace svn::synth