#include <svn.synth/topology/topology.hpp>

using namespace svn::base;

namespace svn::synth {

// ---- amp ----

static param_descriptor const
voice_amp_params[voice_amp_param::count] =
{ 
  { { L"Amp", L"Level" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 1, nullptr, 0 } },
  { { L"Dcy", L"Decay" }, L"Sec", { 0.0f, 2, real_bounds::unit(), real_bounds::unit() }, { 1, 1, nullptr, 0 } }
};
  
// ---- output ----

static param_descriptor const
output_params[output_param::count] =
{
  { { L"Clip", L"Clip" }, false, { 0, 1, nullptr, 0 } },
  { { L"Drain", L"Voices exhausted" }, false, { 1, 1, nullptr, 0 } },
  { { L"Voices", L"Voice count" }, L"", false, 0, synth_polyphony, 0, { 2, 2, nullptr, 0 } }
};

// ---- voice filter ----

static graph_descriptor const
voice_filter_graph =
{ -1, 0, 2, 1, 1 };

static item_name const
voice_filter_types[voice_filter_type::count] =
{
  { L"StVar", L"State variable" },
  { L"Comb", L"Comb" }
};
 
static item_name const
voice_filter_stvar_types[voice_filter_stvar_type::count] =
{
  { L"LPF", L"Low pass" },
  { L"HPF", L"High pass" },
  { L"BPF", L"Band pass" },
  { L"BSF", L"Band stop" }
};

static param_relevance const
vfilter_comb_relevance[1] =
{ { voice_filter_param::type, voice_filter_type::comb } };

static param_relevance const 
vfilter_stvar_relevance[1] =
{ { voice_filter_param::type, voice_filter_type::state_var } };
  
static param_descriptor const
voice_filter_params[voice_filter_param::count] =
{ 
  { { L"On", L"Enabled" }, false, { -1, 0, { }, { } } }, 
  { { L"Type", L"Type" }, L"", false, voice_filter_types, voice_filter_type::count, { 0, 2, nullptr, 0 } },
  { { L"Stvr", L"State var type" }, L"", false, voice_filter_stvar_types, voice_filter_stvar_type::count, { 1, 2, vfilter_stvar_relevance, 1 } },
  { { L"Freq", L"State var frequency" }, L"Hz", { 0.5f, 0, real_bounds::log(20.0f, 20000.0f, 5000.0f), real_bounds::log(20.0f, 20000.0f, 5000.0f) }, { 2, 2, vfilter_stvar_relevance, 1 } },
  { { L"Res", L"State var resonance" }, L"", { 0.0f, 2, real_bounds::unit(), real_bounds::unit() }, { 3, 2, vfilter_stvar_relevance, 1 } },
  { { L"Kbd", L"State var keyboard tracking" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { 4, 2, vfilter_stvar_relevance, 1 } },
  { { L"Dly+", L"Comb plus delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { 1, 2, vfilter_comb_relevance, 1 } },
  { { L"Gn+", L"Comb plus gain" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { 2, 2, vfilter_comb_relevance, 1 } },
  { { L"Dly-", L"Comb min delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { 3, 2, vfilter_comb_relevance, 1 } },
  { { L"Gn-", L"Comb min gain" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { 4, 2, vfilter_comb_relevance, 1 } }
};
 
// ---- oscillator ----

static graph_descriptor const
oscillator_graphs[oscillator_graph::count] =
{
  { oscillator_graph::wave, 0, 2, 1, 1 },
  { oscillator_graph::spectrum, 1, 2, 1, 1 }
};

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

static param_relevance const
osc_dsf_relevance[1] =
{ { oscillator_param::type, oscillator_type::dsf } };

static param_relevance const
osc_anlg_relevance[1] =
{ { oscillator_param::type, oscillator_type::analog } };

static param_relevance const
osc_anlg_pw_relevance[2] =
{
  { oscillator_param::type, oscillator_type::analog },
  { oscillator_param::anlg_type, oscillator_anlg_type::pulse }
};
          
static param_descriptor const
oscillator_params[oscillator_param::count] =
{ 
  { { L"On", L"Enabled" }, false, { -1, 0, { }, { } } },
  { { L"Amp", L"Level" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 0, nullptr, 0 } },
  { { L"Pan", L"Panning" }, L"%", { 0.5f, 0, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { 1, 0, nullptr, 0 } },
  { { L"Type", L"Type" }, L"", false, oscillator_types, oscillator_type::count, { 2, 2, nullptr, 0 } },
  { { L"Anlg", L"Analog type" }, L"", false, oscillator_anlg_types, oscillator_anlg_type::count, { 3, 2, osc_anlg_relevance, 1 } },
  { { L"Dtn", L"Analog detune" }, L"", { 0.0f, 1, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { 4, 2, osc_anlg_relevance, 1 } },
  { { L"Sprd", L"Analog spread" }, L"", true, 1, oscillator_max_spread, 1, { 5, 2, osc_anlg_relevance, 1 } },
  { { L"Pw", L"Analog pulse width" }, L"%", { 1.0f, 0, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }, { 6, 2, osc_anlg_pw_relevance, 2 } },
  { { L"Prts", L"Dsf partials" }, L"", true, 0, 9999, 0, { 4, 2, osc_dsf_relevance, 1 } },
  { { L"Dist", L"Dsf distance" }, L"", { (1.0f - 0.05f) / (20.0f - 0.05f), 2, real_bounds::linear(0.05f, 20.0f), real_bounds::linear(0.05f, 20.0f) }, { 5, 2, osc_dsf_relevance, 1 } },
  { { L"Roll", L"Dsf rolloff" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 6, 2, osc_dsf_relevance, 1 } },
  { { L"Oct", L"Octave" }, L"", true, 0, 9, 4, { 7, 1, nullptr, 0 } },
  { { L"Note", L"Note" }, L"", true, note_names, 12, { 8, 1, nullptr, 0 } },
  { { L"Cent", L"Cent" }, L"", { 0.5f, 0, real_bounds::linear(-0.5f, 0.5f), real_bounds::linear(-50.0f, 50.0f) }, { 9, 1, nullptr, 0 } },
};  
            
// ---- global topo ----

// ffadad-ffd6a5-fdffb6-caffbf-9bf6ff-a0c4ff-bdb2ff-ffc6ff-fffffc
part_descriptor const 
part_descriptors[part_type::count] =  
{
  { { L"Osc", L"Oscillator" }, part_type::oscillator, false, oscillator_count, oscillator_params, oscillator_param::count, oscillator_graphs, oscillator_graph::count, { 3, 0, L"Voice", { 0xFF, 0xAD, 0xAD }}},
  { { L"VFilter", L"Voice filter" }, part_type::voice_filter, false, voice_filter_count, voice_filter_params, voice_filter_param::count, &voice_filter_graph, 1, { 3, 0, L"Voice", { 0xFF, 0xD6, 0xA5 } } },
  { { L"Amp", L"Voice amp" }, part_type::voice_amp, false, 1, voice_amp_params, voice_amp_param::count, nullptr, 0, { 2, -1, L"Voice", { 0xFD, 0xFF, 0xB6 } } },
  { { L"Out", L"Output" }, part_type::output, true, 1, output_params, output_param::count, nullptr, 0, { 3, -1, L"Global", { 0xCA, 0xFF, 0xB6 } } }
}; 
    
} // namespace svn::synth       