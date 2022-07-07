#include <svn.synth/topology/topology.hpp>
#include <sstream>

using namespace svn::base;

namespace svn::synth {
  
// ---- output ----

static param_descriptor const
output_params[output_param::count] =
{
  { "{A6024C5F-CF37-48C8-BE5B-713191155AE5}", { L"Clip", L"Clip" }, false, {0, 1, nullptr, 0}},
  { "{8363D40B-6D49-4CF0-8D86-5449997034DE}", { L"Drain", L"Voices exhausted" }, false, {1, 1, nullptr, 0}},
  { "{6190497F-E08C-49DC-8C74-BAEF6D889BCB}", { L"Voices", L"Voice count" }, L"", false, 0, synth_polyphony, 0, {2, 2, nullptr, 0}}
};

// ---- amp ----

static param_descriptor const
voice_amp_params[voice_amp_param::count] =
{ 
  { "{5A2DF5BA-7D6F-4053-983E-AA6DC5084373}", { L"Amp", L"Level" }, L"dB", {1.0f, 1, real_bounds::unit(), real_bounds::decibel()}, {0, 0, nullptr, 0}},
  { "{461FFE68-7EF6-49B6-A3E9-590E2D0D99FB}", { L"Env1", L"Env1 to level" }, L"", { 1.0f, 2, real_bounds::unit(), real_bounds::unit()}, {1, 1, nullptr, 0}},
  { "{86782F43-7079-47BE-9C7F-8BF6D12A0950}", { L"Pan", L"Panning" }, L"%", { 0.5f, 0, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { 2, 2, nullptr, 0 } }
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

static std::vector<std::pair<std::int32_t, std::int32_t>> const env_synced_timesig = beat_synced_timesig(16);
static std::vector<std::wstring> const env_synced_timesig_names = beat_synced_timesig_names(env_synced_timesig);
static std::vector<float> const env_synced_timesig_values_init = beat_synced_timesig_values(env_synced_timesig);
std::int32_t const env_synced_timesig_count = static_cast<std::int32_t>(env_synced_timesig.size());
float const* const env_synced_timesig_values = env_synced_timesig_values_init.data();

static std::wstring
format_env_timesig(std::int32_t val)
{ return env_synced_timesig_names[val]; }
static bool
parse_env_timesig(std::wstring const& val, std::int32_t& result)
{  return list_parser(val, env_synced_timesig_names, result); }

static param_descriptor const
envelope_params[envelope_param::count] = 
{  
  { "{FC3CE4BC-D8F0-487E-9BB6-826988B4B812}", { L"On", L"Enabled" }, false, {-1, 0, {}, {}}},
  { "{D622D344-A08E-4109-84D8-C310B81C2769}", { L"Type", L"Type" }, L"", false, envelope_types, envelope_type::count, {0, 0, nullptr, 0}},
  { "{F468AAD5-B183-4DCB-B6F3-88842FC647F6}", { L"Sync", L"Tempo sync" }, false, { 1, 0, nullptr, 0 } },
  { "{B1D30C73-FF09-452C-A9D5-00BB8B2CE58D}", { L"Dly", L"Delay time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 2, 2, envelope_time_relevance, 1 } },
  { "{70C49337-7141-42BC-B336-78B28F4770E3}", { L"Dly", L"Delay sync" }, L"", true, 0, env_synced_timesig_count - 1, 0, format_env_timesig, parse_env_timesig, {2, 2, envelope_sync_relevance, 1}},
  { "{AADD118E-B9CE-464E-83C0-1FAE5A62F530}", { L"Hld", L"Hold time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 3, 2, envelope_time_relevance, 1 } },
  { "{AC5D88B7-8CDB-4401-9FB0-20EF0195ABD8}", { L"Hld", L"Hold sync" }, L"", true, 0, env_synced_timesig_count - 1, 0, format_env_timesig, parse_env_timesig, { 3, 2, envelope_sync_relevance, 1 } },
  { "{721AFEB5-A17E-4C37-BF3A-94F645234B73}", { L"Sus", L"Sustain level" }, L"dB", { 0.5f, 1, real_bounds::unit(), real_bounds::decibel() }, { 4, 2, nullptr, 0 } },
  { "{E0A45A06-3C0C-4409-BBFA-A2834F0C7BAD}", { L"Att", L"Attack time" }, L"Sec", { 0.05f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 5, 2, envelope_time_relevance, 1 } },
  { "{E0CFCBB7-B050-4C47-B78C-2511AD2EE6B6}", { L"Att", L"Attack sync" }, L"", true, 0, env_synced_timesig_count - 1, 1, format_env_timesig, parse_env_timesig, { 5, 2, envelope_sync_relevance, 1 } },
  { "{1400CEB5-25B1-42B1-ADA1-B5FDF8C6D1E5}", { L"Slp", L"Attack slope" }, L"", false, envelope_slopes, envelope_slope::count, { 6, 1, nullptr, 0 } },
  { "{5A57D560-64D2-44D5-BD78-82B7C4BEA79C}", { L"Mid", L"Attack log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 7, 1, envelope_attack_log_relevance, 1 } },
  { "{A62B5DB0-4D41-4DA4-AE14-E25A2C983B21}", { L"Dcy", L"Decay time" }, L"Sec", { 0.1f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 8, 2, envelope_time_relevance, 1 } },
  { "{09B32681-21EF-4C65-B5FD-9FA64ED4AF5E}", { L"Dcy", L"Decay sync" }, L"", true, 0, env_synced_timesig_count - 1, 9, format_env_timesig, parse_env_timesig, { 8, 2, envelope_sync_relevance, 1 } },
  { "{62381FB9-9060-4425-97A6-B57ECB2BECCA}", { L"Slp", L"Decay slope" }, L"", false, envelope_slopes, envelope_slope::count, { 9, 1, nullptr, 0 } },
  { "{8B5D1F67-DA80-4170-982D-909CC8A025E2}", { L"Mid", L"Decay log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 10, 1, envelope_decay_log_relevance, 1 } },
  { "{75ACC8A6-F228-445C-9788-A367AE32EAAA}", { L"Rel", L"Release time" }, L"Sec", { 0.2f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 11, 2, envelope_time_relevance, 1 } },
  { "{DDA55E4D-A100-40BA-A7C2-F4C284CACE08}", { L"Rel", L"Release sync" }, L"", true, 0, env_synced_timesig_count - 1, 15, format_env_timesig, parse_env_timesig, { 11, 2, envelope_sync_relevance, 1 } },
  { "{6B978A93-54D1-4990-BD2E-BC143EA816AF}", { L"Slp", L"Release slope" }, L"", false, envelope_slopes, envelope_slope::count, { 12, 1, nullptr, 0 } },
  { "{4E2A6D8B-275C-4E1E-84BE-B21FE16E1E0F}", { L"Mid", L"Release log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 13, 1, envelope_release_log_relevance, 1 } }
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
  { L"BSF", L"Band stop" },
  { L"APF", L"All pass" }
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
  { "{5813593C-E25A-4C8B-B7E5-7C95DAC257C2}", { L"On", L"Enabled" }, false, {-1, 0, {}, {}}},
  { "{956F08A1-B3C6-412B-AEEA-55E722744AE8}", { L"Type", L"Type" }, L"", false, voice_filter_types, voice_filter_type::count, { 0, 2, nullptr, 0 } },
  { "{7498AB5E-4BE6-45B0-8CBF-8D166FF82E32}", { L"Stvr", L"State var type" }, L"", false, voice_filter_stvar_types, voice_filter_stvar_type::count, { 1, 2, vfilter_stvar_relevance, 1 } },
  { "{99F7D808-53EB-4A4F-9FBF-4F1F57E4B742}", { L"Freq", L"State var frequency" }, L"Hz", { 0.25f, 0, real_bounds::quadratic(filter_min_freq, filter_max_freq), real_bounds::quadratic(filter_min_freq, filter_max_freq) }, { 2, 2, vfilter_stvar_relevance, 1 } },
  { "{AA394A33-B616-4EAF-830D-313F2FE316AF}", { L"Res", L"State var resonance" }, L"", { 0.0f, 2, real_bounds::unit(), real_bounds::unit() }, { 3, 2, vfilter_stvar_relevance, 1 } },
  { "{BF8775C6-FD22-4653-BC21-64CDC8D24AFF}", { L"Kbd", L"State var keyboard tracking" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { 4, 2, vfilter_stvar_relevance, 1 } },
  { "{36AA381A-82F4-4259-9D87-431B8C49EAB4}", { L"Dly+", L"Comb plus delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { 1, 2, vfilter_comb_relevance, 1 } },
  { "{9B330083-CF9C-4161-9A66-74AF468EF521}", { L"Gn+", L"Comb plus gain" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { 2, 2, vfilter_comb_relevance, 1 } },
  { "{25A32954-6975-4591-87B0-F0718206E965}", { L"Dly-", L"Comb min delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { 3, 2, vfilter_comb_relevance, 1 } },
  { "{E67FACFD-E13D-4618-85B6-5FED1E3761F3}", { L"Gn-", L"Comb min gain" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { 4, 2, vfilter_comb_relevance, 1 } }
};
 
// ---- oscillator ----

static graph_descriptor const
oscillator_graphs[oscillator_graph::count] =
{
  { oscillator_graph::wave, 0, 2, 2, 1, L"Waveform"},
  { oscillator_graph::spectrum, 2, 2, 1, 1, L"Frequency spectrum"}
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
  { "{CA9274AE-9F57-4373-8C59-3786ACC1C1CD}", { L"On", L"Enabled" }, false, {-1, 0, {}, {}}},
  { "{C5C4B00A-3D26-4342-ACE6-0AE936FFBCA3}", { L"Amp", L"Level" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 0, 2, nullptr, 0 } },
  { "{63C96678-9823-4B96-B2C0-78014A268DB5}", { L"Pan", L"Panning" }, L"%", { 0.5f, 0, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { 1, 2, nullptr, 0 } },
  { "{84A7AEC8-25E9-4242-B32E-2E9E780F0E31}", { L"Type", L"Type" }, L"", false, oscillator_types, oscillator_type::count, { 2, 2, nullptr, 0 } },
  { "{EAFF1105-BA8D-4C55-8BEC-6B73AECF95E2}", { L"Anlg", L"Analog type" }, L"", false, oscillator_anlg_types, oscillator_anlg_type::count, { 3, 2, osc_anlg_relevance, 1 } },
  { "{35DAF80A-6EE0-4A3C-9E81-B225A466F4B2}", { L"Pw", L"Analog pulse width" }, L"%", { 1.0f, 0, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }, { 5, 2, osc_anlg_pw_relevance, 2 } },
  { "{8B45133B-38AA-4E23-9CB9-B05A4954A947}", { L"Prts", L"Dsf partials" }, L"", true, 0, 9999, 0, { 3, 2, osc_dsf_relevance, 1 } },
  { "{083A6619-0309-48CA-8E9E-9A309EA61044}", { L"Dist", L"Dsf distance" }, L"", { (1.0f - 0.05f) / (20.0f - 0.05f), 2, real_bounds::linear(0.05f, 20.0f), real_bounds::linear(0.05f, 20.0f) }, { 4, 2, osc_dsf_relevance, 1 } },
  { "{FFD6C9F3-B7D4-4819-A63A-40BC907F91AF}", { L"Roll", L"Dsf rolloff" }, L"", { 0.5f, 4, real_bounds::unit(), real_bounds::unit() }, { 5, 2, osc_dsf_relevance, 1 } },
  { "{5E3DB4DC-B459-43C4-9BBD-0FF8F2232AFA}", { L"Oct", L"Octave" }, L"", true, 0, 9, 4, { 6, 0, nullptr, 0 } },
  { "{501C5217-5A5B-48D8-AEFE-CFE67417E8AD}", { L"Note", L"Note" }, L"", true, note_names, 12, { 7, 0, nullptr, 0 } },
  { "{FD0A2D20-FCCD-4939-B1CC-BD2EBD998A25}", { L"Cent", L"Cent" }, L"", { 0.5f, 0, real_bounds::linear(-0.5f, 0.5f), real_bounds::linear(-50.0f, 50.0f) }, { 8, 0, nullptr, 0 } },
  { "{F5B30B84-5C3E-471F-9B27-D1FB6E06D0AF}", { L"Uni", L"Unison voices" }, L"", true, 1, oscillator_max_voices, 1, { 9, 1, nullptr, 0 } },
  { "{70C38B3D-E81C-42D9-A59A-9619DB318DFD}", { L"Dtn", L"Unison detune" }, L"", { 0.0f, 1, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { 10, 1, nullptr, 0 } },
  { "{412B4D8D-4272-40ED-949F-479FB8407BF7}", { L"Sprd", L"Unison stereo spread" }, L"", { 0.0f, 1, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { 11, 1, nullptr, 0 } }
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
  { "{2E9F0478-B911-43DF-BB51-0C5836E4853F}", { L"In1", L"Input 1" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, {0, 0, nullptr, 0}},
  { "{295DC5F0-FB32-4D43-8799-D79F23FD3AA9}", { L"Out1", L"Output 1" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 1, 0, nullptr, 0 } },
  { "{A3B15FE9-56DB-493B-A4E1-31A004F3C937}", { L"Amt1", L"Amount 1" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 2, 0, nullptr, 0 } },
  { "{A3A59082-CF73-4C28-A3FC-037729C9CB42}", { L"In2", L"Input 2" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 3, 0, nullptr, 0 } },
  { "{843EB41C-199F-4FAC-ACC4-841B3663248D}", { L"Out2", L"Output 2" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 4, 0, nullptr, 0 } },
  { "{810B55DF-6230-45C7-B478-7A3569DC9127}", { L"Amt2", L"Amount 2" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 5, 0, nullptr, 0 } },
  { "{A8E6882A-8945-4F59-92B9-78004EAF5818}", { L"In3", L"Input 3" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 6, 0, nullptr, 0 } },
  { "{D2E34E63-DE9E-4620-924D-1897614BF983}", { L"Out3", L"Output 3" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 7, 0, nullptr, 0 } },
  { "{6721B1EC-9688-48A3-B5B8-0ADD0A9CF16B}", { L"Amt3", L"Amount 3" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 8, 0, nullptr, 0 } },
  { "{89316C97-EB2A-4066-A675-7FCA8A81E908}", { L"In4", L"Input 4" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 9, 1, nullptr, 0 } },
  { "{D76EABE0-2383-4E35-A996-9FAFFF309AD2}", { L"Out4", L"Output 4" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 10, 1, nullptr, 0 } },
  { "{72593AE1-72BB-408F-819D-3D0C8E632AA8}", { L"Amt4", L"Amount 4" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 11, 1, nullptr, 0 } },
  { "{A87E3D26-1F7F-4D2B-ACA6-CA681126210C}", { L"In5", L"Input 5" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 12, 1, nullptr, 0 } },
  { "{CD659D0E-4818-4D36-9D01-1256C24F8A1F}", { L"Out5", L"Output 5" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 13, 1, nullptr, 0 } },
  { "{B7BABC4D-C3D0-4CD6-A43C-6E63C502C88D}", { L"Amt5", L"Amount 5" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 14, 1, nullptr, 0 } },
  { "{EEDC9213-70B9-4BB6-8D5C-7979D6ABF41F}", { L"In6", L"Input 6" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 15, 1, nullptr, 0 } },
  { "{2437E287-4170-4F98-B179-060FF03D8ACF}", { L"Out6", L"Output 6" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 16, 1, nullptr, 0 } },
  { "{E8016B91-9823-4B4C-AED8-C1F954FC0C70}", { L"Amt6", L"Amount 6" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 17, 1, nullptr, 0 } },
  { "{FABC471B-29FA-45A4-9ACA-6DD21582550F}", { L"In7", L"Input 7" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 18, 2, nullptr, 0 } },
  { "{BD316F66-A86A-46DD-B901-DA3EB0E4CB29}", { L"Out7", L"Output 7" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 19, 2, nullptr, 0 } },
  { "{D765242F-C63B-4B5F-824B-0E8C5E3732EA}", { L"Amt7", L"Amount 7" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 20, 2, nullptr, 0 } },
  { "{61A21B33-F174-4BB8-B70E-D0ABED3AFC32}", { L"In8", L"Input 8" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 21, 2, nullptr, 0 } },
  { "{F52DEACA-9B5C-4D4C-9381-79967288389B}", { L"Out8", L"Output 8" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 22, 2, nullptr, 0 } },
  { "{AAF3C200-9937-4CE2-BB76-DB0FA6FE43AB}", { L"Amt8", L"Amount 8" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 23, 2, nullptr, 0 } },
  { "{EE82872C-0641-48CA-A030-5FCBA09FC0F7}", { L"In9", L"Input 9" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { 24, 2, nullptr, 0 } },
  { "{DDD8D342-8ACA-4203-85EC-AA8EEE666A16}", { L"Out9", L"Output 9" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { 25, 2, nullptr, 0 } },
  { "{D0CBABC4-9772-471B-AF8B-FE6BF6197CA8}", { L"Amt9", L"Amount 9" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 26, 2, nullptr, 0 } }
};
             
// ---- global topo ----
  
// ffadad-ffd6a5-fdffb6-caffbf-9bf6ff-
// a0c4ff-
// bdb2ff-ffc6ff-fffffc 
part_descriptor const 
part_descriptors[part_type::count] =  
{
  { "{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}", { L"Osc", L"Oscillator" }, part_type::oscillator, false, oscillator_count, oscillator_params, oscillator_param::count, oscillator_graphs, oscillator_graph::count, { 0, 3, 0, L"Voice", {0xFF, 0xAD, 0xAD}}},
  { "{FC4885FE-431C-477A-B5B7-84863DB8C07D}", { L"Env", L"Envelope" }, part_type::envelope, false, envelope_count, envelope_params, envelope_param::count, &envelope_graph, 1, { 1, 3, 0, L"Voice", { 0xA0, 0xC4, 0xFF }}},
  { "{E6344937-C1F7-4F2A-83E7-EA27D48DEC4E}", { L"Amp", L"Voice amp" }, part_type::voice_amp, false, 1, voice_amp_params, voice_amp_param::count, nullptr, 0, { 2, 3, -1, L"Voice", { 0xFD, 0xFF, 0xB6 } } },
  { "{2C377544-C124-48F5-A4F4-1E301B108C58}", { L"VFilter", L"Voice filter" }, part_type::voice_filter, false, voice_filter_count, voice_filter_params, voice_filter_param::count, &voice_filter_graph, 1, { 4, 3, 0, L"Voice", { 0xFF, 0xD6, 0xA5 } } },
  { "{7A77C027-FC8F-4425-9BF0-393267D92F0C}", { L"Audio", L"Audio route" }, part_type::audio_route, false, 1, audio_route_params, audio_route_param::count, nullptr, 0, { 5, 3, -1, L"Route", { 0xCA, 0xFF, 0xBF } } },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { L"Out", L"Output" }, part_type::output, true, 1, output_params, output_param::count, nullptr, 0, { 3, 3, -1, L"Global", { 0x9B, 0xF6, 0xFF } } }
};    
      
} // namespace svn::synth          