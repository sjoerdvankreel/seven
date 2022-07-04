#include <svn.synth/topology/topology.hpp>
#include <sstream>

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

// ---- envelope ----

static graph_descriptor const
envelope_graph =
{ -1, 0, 2, 1, 1, L"Envelope" };

static param_relevance const
envelope_time_relevance[1] =
{ { envelope_param::sync, 0 } };
static param_relevance const
envelope_sync_relevance[1] =
{ { envelope_param::sync, 1 } };

static param_relevance const
envelope_attack_log_relevance[1] =
{ { envelope_param::attack_slope, envelope_slope::log } };
static param_relevance const
envelope_decay_log_relevance[1] =
{ { envelope_param::decay_slope, envelope_slope::log } };
static param_relevance const
envelope_release_log_relevance[1] =
{ { envelope_param::release_slope, envelope_slope::log } };

static item_name const
envelope_types[envelope_type::count] =
{
  { L"Dahdsr", L"DAHDSR" },
  { L"Dahdr", L"DAHDR" }
};
 
static item_name const
envelope_slopes[envelope_slope::count] =
{
  { L"Lin", L"Linear" },
  { L"Log", L"Logarithmic" },
  { L"Quad", L"Quadratic" },
  { L"Sqrt", L"Squared" }
};

static std::wstring
format_env_sync(std::int32_t val)
{ return L""; }

static bool
parse_env_sync(std::wstring const& val, std::int32_t& result)
{ return false; }

static param_descriptor const
envelope_params[envelope_param::count] = 
{  
  { { L"On", L"Enabled" }, false, { -1, 0, { }, { } } },
  { { L"Type", L"Type" }, L"", false, envelope_types, envelope_type::count, { 0, 0, nullptr, 0 } },
  { { L"Sync", L"Tempo sync" }, false, { 1, 0, nullptr, 0 } },
  { { L"Dly", L"Delay time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 2, 0, envelope_time_relevance, 1 } },
  { { L"Dly", L"Delay sync" }, L"", true, 0, 17 * 16, 0, &format_env_sync, &parse_env_sync, { 2, 0, envelope_sync_relevance, 1 } },
  { { L"Hld", L"Hold time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 3, 0, envelope_time_relevance, 1 } },
  { { L"Hld", L"Hold sync" }, L"", true, 0, 17 * 16, 0, &format_env_sync, &parse_env_sync, { 3, 0, envelope_sync_relevance, 1 } },
  { { L"Sus", L"Sustain level" }, L"dB", { 0.5f, 1, real_bounds::unit(), real_bounds::decibel() }, { 4, 0, nullptr, 0 } },
  { { L"Att", L"Attack time" }, L"Sec", { 0.05f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 5, 0, envelope_time_relevance, 1 } },
  { { L"Att", L"Attack sync" }, L"", true, 0, 17 * 16, 0, &format_env_sync, &parse_env_sync, { 5, 0, envelope_sync_relevance, 1 } },
  { { L"Slp", L"Attack slope" }, L"", false, envelope_slopes, envelope_slope::count, { 6, 0, nullptr, 0 } },
  { { L"Mid", L"Attack midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 7, 0, envelope_attack_log_relevance, 1 } },
  { { L"Dcy", L"Decay time" }, L"Sec", { 0.1f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 8, 0, envelope_time_relevance, 1 } },
  { { L"Dcy", L"Decay sync" }, L"", true, 0, 17 * 16, 0, &format_env_sync, &parse_env_sync, { 8, 0, envelope_sync_relevance, 1 } },
  { { L"Slp", L"Decay slope" }, L"", false, envelope_slopes, envelope_slope::count, { 9, 0, nullptr, 0 } },
  { { L"Mid", L"Decay midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 10, 0, envelope_decay_log_relevance, 1 } },
  { { L"Rel", L"Release time" }, L"Sec", { 0.2f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 11, 0, envelope_time_relevance, 1 } },
  { { L"Rel", L"Release sync" }, L"", true, 0, 17 * 16, 0, &format_env_sync, &parse_env_sync, { 11, 0, envelope_sync_relevance, 1 } },
  { { L"Slp", L"Release slope" }, L"", false, envelope_slopes, envelope_slope::count, { 12, 0, nullptr, 0 } },
  { { L"Mid", L"Release midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 13, 0, envelope_release_log_relevance, 1 } }
}; 
 
// ---- voice filter ----

static graph_descriptor const
voice_filter_graph =
{ -1, 0, 2, 1, 1, L"Impulse response" };

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
  { { L"Freq", L"State var frequency" }, L"Hz", { 0.25f, 0, real_bounds::quadratic(filter_min_freq, filter_max_freq), real_bounds::quadratic(filter_min_freq, filter_max_freq) }, { 2, 2, vfilter_stvar_relevance, 1 } },
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
  { oscillator_graph::wave, 0, 2, 1, 1, L"Waveform"},
  { oscillator_graph::spectrum, 1, 2, 1, 1, L"Frequency spectrum"}
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
  { { L"Sprd", L"Analog spread" }, L"", true, 1, oscillator_max_spread, 1, { 4, 2, osc_anlg_relevance, 1 } },
  { { L"Dtn", L"Analog detune" }, L"", { 0.0f, 1, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { 5, 2, osc_anlg_relevance, 1 } },
  { { L"Pw", L"Analog pulse width" }, L"%", { 1.0f, 0, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }, { 6, 2, osc_anlg_pw_relevance, 2 } },
  { { L"Prts", L"Dsf partials" }, L"", true, 0, 9999, 0, { 4, 2, osc_dsf_relevance, 1 } },
  { { L"Dist", L"Dsf distance" }, L"", { (1.0f - 0.05f) / (20.0f - 0.05f), 2, real_bounds::linear(0.05f, 20.0f), real_bounds::linear(0.05f, 20.0f) }, { 5, 2, osc_dsf_relevance, 1 } },
  { { L"Roll", L"Dsf rolloff" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 6, 2, osc_dsf_relevance, 1 } },
  { { L"Oct", L"Octave" }, L"", true, 0, 9, 4, { 7, 1, nullptr, 0 } },
  { { L"Note", L"Note" }, L"", true, note_names, 12, { 8, 1, nullptr, 0 } },
  { { L"Cent", L"Cent" }, L"", { 0.5f, 0, real_bounds::linear(-0.5f, 0.5f), real_bounds::linear(-50.0f, 50.0f) }, { 9, 1, nullptr, 0 } },
};  

// ---- audio route ----

static wchar_t const* audio_input_names[audio_route_input::count] =
{ L"Off", L"Osc", L"VFlt" };
static wchar_t const* audio_output_names[audio_route_output::count] =
{ L"Off", L"VFlt", L"Amp" };

static std::wstring 
format_audio_input(std::int32_t val)
{ return multi_list_formatter(val, audio_input_names, audio_input_counts, 3); }
static std::wstring 
format_audio_output(std::int32_t val)
{ return multi_list_formatter(val, audio_output_names, audio_output_counts, 3); }
static bool
parse_audio_input(std::wstring const& val, std::int32_t& result)
{ return multi_list_parser(val, audio_input_names, audio_input_counts, 3, result); }
static bool
parse_audio_output(std::wstring const& val, std::int32_t& result)
{ return multi_list_parser(val, audio_output_names, audio_output_counts, 3, result); }

static param_descriptor const
audio_route_params[audio_route_param::count] =
{
  { { L"In1", L"Input 1" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 0, 0, nullptr, 0 } },
  { { L"Out1", L"Output 1" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 1, 0, nullptr, 0 } },
  { { L"Amt1", L"Amount 1" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 2, 0, nullptr, 0 } },
  { { L"In2", L"Input 2" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 3, 0, nullptr, 0 } },
  { { L"Out2", L"Output 2" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 4, 0, nullptr, 0 } },
  { { L"Amt2", L"Amount 2" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 5, 0, nullptr, 0 } },
  { { L"In3", L"Input 3" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 6, 0, nullptr, 0 } },
  { { L"Out3", L"Output 3" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 7, 0, nullptr, 0 } },
  { { L"Amt3", L"Amount 3" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 8, 0, nullptr, 0 } }, 
  { { L"In4", L"Input 4" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 9, 1, nullptr, 0 } },
  { { L"Out4", L"Output 4" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 10, 1, nullptr, 0 } },
  { { L"Amt4", L"Amount 4" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 11, 1, nullptr, 0 } },
  { { L"In5", L"Input 5" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 12, 1, nullptr, 0 } },
  { { L"Out5", L"Output 5" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 13, 1, nullptr, 0 } },
  { { L"Amt5", L"Amount 5" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 14, 1, nullptr, 0 } },
  { { L"In6", L"Input 6" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 15, 1, nullptr, 0 } },
  { { L"Out6", L"Output 6" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 16, 1, nullptr, 0 } },
  { { L"Amt6", L"Amount 6" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 17, 1, nullptr, 0 } },
  { { L"In7", L"Input 7" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 18, 2, nullptr, 0 } },
  { { L"Out7", L"Output 7" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 19, 2, nullptr, 0 } },
  { { L"Amt7", L"Amount 7" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 20, 2, nullptr, 0 } },
  { { L"In8", L"Input 8" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 21, 2, nullptr, 0 } },
  { { L"Out8", L"Output 8" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 22, 2, nullptr, 0 } },
  { { L"Amt8", L"Amount 8" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 23, 2, nullptr, 0 } },
  { { L"In9", L"Input 9" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 24, 2, nullptr, 0 } },
  { { L"Out9", L"Output 9" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 25, 2, nullptr, 0 } },
  { { L"Amt9", L"Amount 9" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 26, 2, nullptr, 0 } }
};
             
// ---- global topo ----
  
// ffadad-ffd6a5-fdffb6-caffbf-9bf6ff-
// a0c4ff-
// bdb2ff-ffc6ff-fffffc 
part_descriptor const 
part_descriptors[part_type::count] =  
{
  { { L"Osc", L"Oscillator" }, part_type::oscillator, false, oscillator_count, oscillator_params, oscillator_param::count, oscillator_graphs, oscillator_graph::count, { 3, 0, L"Voice", { 0xFF, 0xAD, 0xAD }}},
  { { L"Env", L"Envelope" }, part_type::envelope, false, envelope_count, envelope_params, envelope_param::count, &envelope_graph, 1, { 3, 0, L"Voice", { 0xA0, 0xC4, 0xFF }}},
  { { L"Amp", L"Voice amp" }, part_type::voice_amp, false, 1, voice_amp_params, voice_amp_param::count, nullptr, 0, { 2, -1, L"Voice", { 0xFD, 0xFF, 0xB6 } } },
  { { L"VFilter", L"Voice filter" }, part_type::voice_filter, false, voice_filter_count, voice_filter_params, voice_filter_param::count, &voice_filter_graph, 1, { 3, 0, L"Voice", { 0xFF, 0xD6, 0xA5 } } },
  { { L"Audio", L"Audio route" }, part_type::audio_route, false, 1, audio_route_params, audio_route_param::count, nullptr, 0, { 3, -1, L"Route", { 0xCA, 0xFF, 0xBF } } },
  { { L"Out", L"Output" }, part_type::output, true, 1, output_params, output_param::count, nullptr, 0, { 3, -1, L"Global", { 0x9B, 0xF6, 0xFF } } }
};    
      
} // namespace svn::synth        