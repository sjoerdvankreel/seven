#include <svn.synth/topology/topology.hpp>
#include <sstream>

using namespace svn::base;

namespace svn::synth {

// ---- shared ----

static item_name const
cv_kinds[cv_kind::count] =
{
  { L"Unipolar", L"Unipolar" },
  { L"Bipolar", L"Bipolar" },
  { L"UniSync", L"Unipolar synced" },
  { L"BipSync", L"Bipolar synced" }
}; 
       
// ---- output ----  
 
static param_descriptor const 
output_params[output_param::count] =
{
  { "{A6024C5F-CF37-48C8-BE5B-713191155AE5}", { L"Clip", L"Clip" }, false, { false, false, 0, 0, nullptr, 0 } },
  { "{8363D40B-6D49-4CF0-8D86-5449997034DE}", { L"Drain", L"Voices exhausted" }, false, { false, false, 1, 0, nullptr, 0 } },
  { "{6190497F-E08C-49DC-8C74-BAEF6D889BCB}", { L"Voices", L"Voice count" }, L"", false, 0, synth_polyphony, 0, { false, false, 2, 0, nullptr, 0 } },
  { "{485325E3-3E15-46C4-84FA-5D743BB75C3A}", { L"Total cpu", L"Total cpu" }, L"%", false, 0, 100, 0, { false, true, 3, 1, nullptr, 0 } },
  { "{1A5EA805-8678-4B9F-A03F-92E1A9D3C519}", { L"Osc cpu", L"Osc cpu" }, L"%", false, 0, 100, 0, { false, true, 4, 1, nullptr, 0 } },
  { "{C39636DA-EBF5-4AA5-AFBB-BBCD5762D979}", { L"VLFO cpu", L"Voice LFO cpu" }, L"%", false, 0, 100, 0, { false, true, 5, 1, nullptr, 0 } },
  { "{C3FBB5D7-E5EA-4EEB-9795-22298F7380B9}", { L"VFlt cpu", L"Voice filter cpu" }, L"%", false, 0, 100, 0, { false, true, 6, 1, nullptr, 0 } },
  { "{0DF43E23-9273-46BC-8BBE-52B2C0E612F7}", { L"Env cpu", L"Envelope cpu" }, L"%", false, 0, 100, 0, { false, true, 7, 1, nullptr, 0 } },
  { "{DC142F66-E4B2-4BEA-8B2E-A9501B32B1FB}", { L"Amp cpu", L"Voice amp cpu" }, L"%", false, 0, 100, 0, { false, true, 8, 1, nullptr, 0 } },
  { "{295F92F6-37CD-453E-B8C2-E03B6648A1E4}", { L"Audio cpu", L"Audio route cpu" }, L"%", false, 0, 100, 0, { false, true, 9, 1, nullptr, 0 } },
  { "{BCF00561-4D8E-4AB8-94CE-AF7EF65881FB}", { L"CV cpu", L"Cv route cpu" }, L"%", false, 0, 100, 0, { false, true, 10, 1, nullptr, 0 } },
  { "{BCF00561-4D8E-4AB8-94CE-AF7EF65881FB}", { L"Aux cpu", L"Auxiliary cpu" }, L"%", false, 0, 100, 0, { false, true, 11, 1, nullptr, 0 } }
};

// ---- amp ---- 
  
static param_descriptor const  
voice_amp_params[voice_amp_param::count] =
{ 
  { "{5A2DF5BA-7D6F-4053-983E-AA6DC5084373}", { L"Amp", L"Level" }, L"dB", {1.0f, 1, real_bounds::unit(), real_bounds::decibel()}, { false, false, 0, 0, nullptr, 0}},
  { "{461FFE68-7EF6-49B6-A3E9-590E2D0D99FB}", { L"Env1", L"Env1 to level" }, L"", { 1.0f, 2, real_bounds::unit(), real_bounds::unit()}, { false, false, 1, 1, nullptr, 0}},
  { "{86782F43-7079-47BE-9C7F-8BF6D12A0950}", { L"Pan", L"Panning" }, L"%", { 0.5f, 0, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { false, false,  2, 2, nullptr, 0 } }
};

// ---- voice lfo ----

static std::vector<std::pair<std::int32_t, std::int32_t>> const voice_lfo_timesig = beat_synced_timesig(16, false);
static std::vector<std::wstring> const voice_lfo_timesig_names = beat_synced_timesig_names(voice_lfo_timesig);
static std::vector<float> const voice_lfo_timesig_values_init = beat_synced_timesig_values(voice_lfo_timesig);
std::int32_t const voice_lfo_timesig_count = static_cast<std::int32_t>(voice_lfo_timesig.size());
float const* const voice_lfo_timesig_values = voice_lfo_timesig_values_init.data();

static graph_descriptor const voice_lfo_graph = { -1, 0, 2, 1, 1, L"LFO" };
static param_relevance const voice_lfo_time_relevance[1] = { { voice_lfo_param::kind, { cv_kind::time_bipolar, cv_kind::time_unipolar } } };
static param_relevance const voice_lfo_sync_relevance[1] = { { voice_lfo_param::kind, { cv_kind::sync_bipolar, cv_kind::sync_unipolar } } };

static std::wstring format_voice_lfo_timesig(std::int32_t val) { return voice_lfo_timesig_names[val]; }
static bool parse_voice_lfo_timesig(std::wstring const& val, std::int32_t& result) { return list_parser(val, voice_lfo_timesig_names, result); }

static item_name const 
voice_lfo_types[voice_lfo_type::count] =
{
  { L"Sin", L"Sine" },
  { L"Cust", L"Custom" },
  { L"Rnd", L"Random" }
};

static param_descriptor const
voice_lfo_params[voice_lfo_param::count] =
{
  { "{42FB0553-788E-470F-906A-D95FED2ED980}", { L"On", L"Enabled" }, false, { false, false, -1, 0, {}, {}}},
  { "{F744C553-8CFA-4262-98A7-37E187BF27FF}", { L"Type", L"Type" }, L"", false, voice_lfo_types, voice_lfo_type::count, { false, false, 0, 0, nullptr, 0}},
  { "{83C1ED1B-095E-4F58-B091-39DA4F0125BF}", { L"Kind", L"Sync/polarity kind" }, L"", false, cv_kinds, cv_kind::count, { false, false, 1, 0, nullptr, 0} },
  { "{E320A1F0-2FCA-46F2-BBCB-0504D65503BC}", { L"Freq", L"Frequency" }, L"Hz", { 0.0f, 2, real_bounds::quadratic(voice_lfo_min_freq, voice_lfo_max_freq), real_bounds::quadratic(voice_lfo_min_freq, voice_lfo_max_freq) }, { false, false, 2, 0, voice_lfo_time_relevance, 1 } },
  { "{09618D35-EFAD-4E2E-8FD0-04B6F5AC14D5}", { L"Step", L"Tempo" }, L"", true, 0, voice_lfo_timesig_count - 1, 0, format_voice_lfo_timesig, parse_voice_lfo_timesig, { false, false, 2, 0, voice_lfo_sync_relevance, 1 } }
};  

// ---- envelope ----

static std::vector<std::pair<std::int32_t, std::int32_t>> const env_timesig = beat_synced_timesig(16, true);
static std::vector<std::wstring> const env_timesig_names = beat_synced_timesig_names(env_timesig);
static std::vector<float> const env_timesig_values_init = beat_synced_timesig_values(env_timesig);
std::int32_t const env_timesig_count = static_cast<std::int32_t>(env_timesig.size());
float const* const env_timesig_values = env_timesig_values_init.data();

static graph_descriptor const envelope_graph = { -1, 0, 2, 1, 1, L"Envelope" };
static param_relevance const envelope_decay_log_relevance[1] = { { envelope_param::decay_slope, { envelope_slope::log } } };
static param_relevance const envelope_attack_log_relevance[1] = { { envelope_param::attack_slope, { envelope_slope::log } } };
static param_relevance const envelope_release_log_relevance[1] = { { envelope_param::release_slope, { envelope_slope::log } } };
static param_relevance const envelope_time_relevance[1] = { { envelope_param::kind, { cv_kind::time_bipolar, cv_kind::time_unipolar } } };
static param_relevance const envelope_sync_relevance[1] = { { envelope_param::kind, { cv_kind::sync_bipolar, cv_kind::sync_unipolar } } };

static std::wstring format_env_timesig(std::int32_t val) { return env_timesig_names[val]; }
static bool parse_env_timesig(std::wstring const& val, std::int32_t& result) { return list_parser(val, env_timesig_names, result); }

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

static param_descriptor const
envelope_params[envelope_param::count] =  
{  
  { "{FC3CE4BC-D8F0-487E-9BB6-826988B4B812}", { L"On", L"Enabled" }, false, { false, false, -1, 0, {}, {}}},
  { "{D622D344-A08E-4109-84D8-C310B81C2769}", { L"Type", L"Type" }, L"", false, envelope_types, envelope_type::count, { false, false, 0, 0, nullptr, 0}},
  { "{F468AAD5-B183-4DCB-B6F3-88842FC647F6}", { L"Kind", L"Sync/polarity kind" }, L"", false, cv_kinds, cv_kind::count, { false, false, 1, 0, nullptr, 0} },
  { "{B1D30C73-FF09-452C-A9D5-00BB8B2CE58D}", { L"Dly", L"Delay time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { false, false, 2, 2, envelope_time_relevance, 1 } },
  { "{70C49337-7141-42BC-B336-78B28F4770E3}", { L"Dly", L"Delay sync" }, L"", true, 0, env_timesig_count - 1, 0, format_env_timesig, parse_env_timesig, { false, false, 2, 2, envelope_sync_relevance, 1}},
  { "{AADD118E-B9CE-464E-83C0-1FAE5A62F530}", { L"Hld", L"Hold time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { false, false, 3, 2, envelope_time_relevance, 1 } },
  { "{AC5D88B7-8CDB-4401-9FB0-20EF0195ABD8}", { L"Hld", L"Hold sync" }, L"", true, 0, env_timesig_count - 1, 0, format_env_timesig, parse_env_timesig, { false, false, 3, 2, envelope_sync_relevance, 1 } },
  { "{721AFEB5-A17E-4C37-BF3A-94F645234B73}", { L"Sus", L"Sustain level" }, L"dB", { 0.5f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 4, 2, nullptr, 0 } },
  { "{E0A45A06-3C0C-4409-BBFA-A2834F0C7BAD}", { L"Att", L"Attack time" }, L"Sec", { 0.05f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { false, false, 5, 2, envelope_time_relevance, 1 } },
  { "{E0CFCBB7-B050-4C47-B78C-2511AD2EE6B6}", { L"Att", L"Attack sync" }, L"", true, 0, env_timesig_count - 1, 1, format_env_timesig, parse_env_timesig, { false, false, 5, 2, envelope_sync_relevance, 1 } },
  { "{1400CEB5-25B1-42B1-ADA1-B5FDF8C6D1E5}", { L"Slp", L"Attack slope" }, L"", false, envelope_slopes, envelope_slope::count, { false, false, 6, 1, nullptr, 0 } },
  { "{5A57D560-64D2-44D5-BD78-82B7C4BEA79C}", { L"Mid", L"Attack log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { false, false, 7, 1, envelope_attack_log_relevance, 1 } },
  { "{A62B5DB0-4D41-4DA4-AE14-E25A2C983B21}", { L"Dcy", L"Decay time" }, L"Sec", { 0.1f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { false, false, 8, 2, envelope_time_relevance, 1 } },
  { "{09B32681-21EF-4C65-B5FD-9FA64ED4AF5E}", { L"Dcy", L"Decay sync" }, L"", true, 0, env_timesig_count - 1, 9, format_env_timesig, parse_env_timesig, { false, false, 8, 2, envelope_sync_relevance, 1 } },
  { "{62381FB9-9060-4425-97A6-B57ECB2BECCA}", { L"Slp", L"Decay slope" }, L"", false, envelope_slopes, envelope_slope::count, { false, false, 9, 1, nullptr, 0 } },
  { "{8B5D1F67-DA80-4170-982D-909CC8A025E2}", { L"Mid", L"Decay log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { false, false, 10, 1, envelope_decay_log_relevance, 1 } },
  { "{75ACC8A6-F228-445C-9788-A367AE32EAAA}", { L"Rel", L"Release time" }, L"Sec", { 0.2f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { false, false, 11, 2, envelope_time_relevance, 1 } },
  { "{DDA55E4D-A100-40BA-A7C2-F4C284CACE08}", { L"Rel", L"Release sync" }, L"", true, 0, env_timesig_count - 1, 15, format_env_timesig, parse_env_timesig, { false, false, 11, 2, envelope_sync_relevance, 1 } },
  { "{6B978A93-54D1-4990-BD2E-BC143EA816AF}", { L"Slp", L"Release slope" }, L"", false, envelope_slopes, envelope_slope::count, { false, false, 12, 1, nullptr, 0 } },
  { "{4E2A6D8B-275C-4E1E-84BE-B21FE16E1E0F}", { L"Mid", L"Release log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { false, false, 13, 1, envelope_release_log_relevance, 1 } }
}; 
  
// ---- voice filter ---- 

static graph_descriptor const voice_filter_graph = { -1, 0, 2, 1, 1, L"Impulse response" };
static param_relevance const vfilter_comb_relevance[1] = { { voice_filter_param::type, { voice_filter_type::comb } } };
static param_relevance const vfilter_stvar_relevance[1] = { { voice_filter_param::type, { voice_filter_type::state_var } } };

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
  
static param_descriptor const
voice_filter_params[voice_filter_param::count] =
{  
  { "{5813593C-E25A-4C8B-B7E5-7C95DAC257C2}", { L"On", L"Enabled" }, false, { false, false, -1, 0, {}, {}}},
  { "{956F08A1-B3C6-412B-AEEA-55E722744AE8}", { L"Type", L"Type" }, L"", false, voice_filter_types, voice_filter_type::count, { false, false, 0, 2, nullptr, 0 } },
  { "{7498AB5E-4BE6-45B0-8CBF-8D166FF82E32}", { L"Stvr", L"State var type" }, L"", false, voice_filter_stvar_types, voice_filter_stvar_type::count, { false, false, 1, 2, vfilter_stvar_relevance, 1 } },
  { "{99F7D808-53EB-4A4F-9FBF-4F1F57E4B742}", { L"Freq", L"State var frequency" }, L"Hz", { 0.25f, 0, real_bounds::quadratic(filter_min_freq, filter_max_freq), real_bounds::quadratic(filter_min_freq, filter_max_freq) }, { false, false, 2, 2, vfilter_stvar_relevance, 1 } },
  { "{AA394A33-B616-4EAF-830D-313F2FE316AF}", { L"Res", L"State var resonance" }, L"", { 0.0f, 2, real_bounds::unit(), real_bounds::unit() }, { false, false, 3, 2, vfilter_stvar_relevance, 1 } },
  { "{BF8775C6-FD22-4653-BC21-64CDC8D24AFF}", { L"Kbd", L"State var keyboard tracking" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { false, false, 4, 2, vfilter_stvar_relevance, 1 } },
  { "{36AA381A-82F4-4259-9D87-431B8C49EAB4}", { L"Dly+", L"Comb plus delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { false, false, 1, 2, vfilter_comb_relevance, 1 } },
  { "{9B330083-CF9C-4161-9A66-74AF468EF521}", { L"Gn+", L"Comb plus gain" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { false, false, 2, 2, vfilter_comb_relevance, 1 } },
  { "{25A32954-6975-4591-87B0-F0718206E965}", { L"Dly-", L"Comb min delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { false, false, 3, 2, vfilter_comb_relevance, 1 } },
  { "{E67FACFD-E13D-4618-85B6-5FED1E3761F3}", { L"Gn-", L"Comb min gain" }, L"", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-1.0f, 1.0f) }, { false, false, 4, 2, vfilter_comb_relevance, 1 } }
};
 
// ---- oscillator ----

static param_relevance const osc_dsf_relevance[1] = { { oscillator_param::type, { oscillator_type::dsf } } };
static param_relevance const osc_anlg_relevance[1] = { { oscillator_param::type, { oscillator_type::analog } } };

static param_relevance const osc_anlg_pw_relevance[2] =
{
  { oscillator_param::type, { oscillator_type::analog } },
  { oscillator_param::anlg_type, { oscillator_anlg_type::pulse } }
};

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

static param_descriptor const
oscillator_params[oscillator_param::count] =
{ 
  { "{CA9274AE-9F57-4373-8C59-3786ACC1C1CD}", { L"On", L"Enabled" }, false, { false, false, -1, 0, {}, {}}},
  { "{C5C4B00A-3D26-4342-ACE6-0AE936FFBCA3}", { L"Amp", L"Level" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 0, 2, nullptr, 0 } },
  { "{63C96678-9823-4B96-B2C0-78014A268DB5}", { L"Pan", L"Panning" }, L"%", { 0.5f, 0, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { false, false, 1, 2, nullptr, 0 } },
  { "{84A7AEC8-25E9-4242-B32E-2E9E780F0E31}", { L"Type", L"Type" }, L"", false, oscillator_types, oscillator_type::count, { false, false, 2, 2, nullptr, 0 } },
  { "{EAFF1105-BA8D-4C55-8BEC-6B73AECF95E2}", { L"Anlg", L"Analog type" }, L"", false, oscillator_anlg_types, oscillator_anlg_type::count, { false, false, 3, 2, osc_anlg_relevance, 1 } },
  { "{35DAF80A-6EE0-4A3C-9E81-B225A466F4B2}", { L"Pw", L"Analog pulse width" }, L"%", { 1.0f, 0, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }, { false, false, 5, 2, osc_anlg_pw_relevance, 2 } },
  { "{8B45133B-38AA-4E23-9CB9-B05A4954A947}", { L"Prts", L"Dsf partials" }, L"", true, 0, 9999, 0, { false, false, 3, 2, osc_dsf_relevance, 1 } },
  { "{083A6619-0309-48CA-8E9E-9A309EA61044}", { L"Dist", L"Dsf distance" }, L"", { (1.0f - 0.05f) / (20.0f - 0.05f), 2, real_bounds::linear(0.05f, 20.0f), real_bounds::linear(0.05f, 20.0f) }, { false, false, 4, 2, osc_dsf_relevance, 1 } },
  { "{FFD6C9F3-B7D4-4819-A63A-40BC907F91AF}", { L"Roll", L"Dsf rolloff" }, L"", { 0.5f, 4, real_bounds::unit(), real_bounds::unit() }, { false, false, 5, 2, osc_dsf_relevance, 1 } },
  { "{5E3DB4DC-B459-43C4-9BBD-0FF8F2232AFA}", { L"Oct", L"Octave" }, L"", true, 0, 9, 4, { false, false, 6, 0, nullptr, 0 } },
  { "{501C5217-5A5B-48D8-AEFE-CFE67417E8AD}", { L"Note", L"Note" }, L"", true, note_names, 12, { false, false, 7, 0, nullptr, 0 } },
  { "{FD0A2D20-FCCD-4939-B1CC-BD2EBD998A25}", { L"Cent", L"Cent" }, L"", { 0.5f, 0, real_bounds::linear(-0.5f, 0.5f), real_bounds::linear(-50.0f, 50.0f) }, { false, false, 8, 0, nullptr, 0 } },
  { "{F5B30B84-5C3E-471F-9B27-D1FB6E06D0AF}", { L"Uni", L"Unison voices" }, L"", true, 1, oscillator_max_voices, 1, { false, false, 9, 1, nullptr, 0 } },
  { "{70C38B3D-E81C-42D9-A59A-9619DB318DFD}", { L"Dtn", L"Unison detune" }, L"", { 0.0f, 1, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { false, false, 10, 1, nullptr, 0 } },
  { "{412B4D8D-4272-40ED-949F-479FB8407BF7}", { L"Sprd", L"Unison stereo spread" }, L"", { 0.0f, 0, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f) }, { false, false, 11, 1, nullptr, 0 } }
};   
 
// ---- audio route ---- 
 
static wchar_t const* const audio_input_names[audio_route_input::count] = { L"Off", L"Osc", L"VFlt" }; 
static wchar_t const* const audio_output_names[audio_route_output::count] = { L"Off", L"VFlt", L"Amp" };
static std::vector<std::wstring> const audio_input_names_list = multi_list_names(audio_input_names, audio_input_counts, audio_route_input::count);
static std::vector<std::wstring> const audio_output_names_list = multi_list_names(audio_output_names, audio_output_counts, audio_route_output::count);

static std::wstring format_audio_input(std::int32_t val) { return audio_input_names_list[val]; }
static std::wstring format_audio_output(std::int32_t val) { return audio_output_names_list[val]; }; 
static bool parse_audio_input(std::wstring const& val, std::int32_t& result) { return list_parser(val, audio_input_names_list, result); }
static bool parse_audio_output(std::wstring const& val, std::int32_t& result) { return list_parser(val, audio_output_names_list, result); }

static param_descriptor const
audio_route_params[audio_route_param::count] =
{
  { "{2E9F0478-B911-43DF-BB51-0C5836E4853F}", { L"In1", L"Input 1" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, {false, false, 0, 0, nullptr, 0}},
  { "{295DC5F0-FB32-4D43-8799-D79F23FD3AA9}", { L"Out1", L"Output 1" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 1, 0, nullptr, 0 } },
  { "{A3B15FE9-56DB-493B-A4E1-31A004F3C937}", { L"Amt1", L"Amount 1" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 2, 0, nullptr, 0 } },
  { "{A3A59082-CF73-4C28-A3FC-037729C9CB42}", { L"In2", L"Input 2" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 3, 0, nullptr, 0 } },
  { "{843EB41C-199F-4FAC-ACC4-841B3663248D}", { L"Out2", L"Output 2" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 4, 0, nullptr, 0 } },
  { "{810B55DF-6230-45C7-B478-7A3569DC9127}", { L"Amt2", L"Amount 2" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 5, 0, nullptr, 0 } },
  { "{A8E6882A-8945-4F59-92B9-78004EAF5818}", { L"In3", L"Input 3" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 6, 0, nullptr, 0 } },
  { "{D2E34E63-DE9E-4620-924D-1897614BF983}", { L"Out3", L"Output 3" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 7, 0, nullptr, 0 } },
  { "{6721B1EC-9688-48A3-B5B8-0ADD0A9CF16B}", { L"Amt3", L"Amount 3" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 8, 0, nullptr, 0 } },
  { "{89316C97-EB2A-4066-A675-7FCA8A81E908}", { L"In4", L"Input 4" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 9, 1, nullptr, 0 } },
  { "{D76EABE0-2383-4E35-A996-9FAFFF309AD2}", { L"Out4", L"Output 4" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 10, 1, nullptr, 0 } },
  { "{72593AE1-72BB-408F-819D-3D0C8E632AA8}", { L"Amt4", L"Amount 4" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 11, 1, nullptr, 0 } },
  { "{A87E3D26-1F7F-4D2B-ACA6-CA681126210C}", { L"In5", L"Input 5" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 12, 1, nullptr, 0 } },
  { "{CD659D0E-4818-4D36-9D01-1256C24F8A1F}", { L"Out5", L"Output 5" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 13, 1, nullptr, 0 } },
  { "{B7BABC4D-C3D0-4CD6-A43C-6E63C502C88D}", { L"Amt5", L"Amount 5" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 14, 1, nullptr, 0 } },
  { "{EEDC9213-70B9-4BB6-8D5C-7979D6ABF41F}", { L"In6", L"Input 6" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 15, 1, nullptr, 0 } },
  { "{2437E287-4170-4F98-B179-060FF03D8ACF}", { L"Out6", L"Output 6" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 16, 1, nullptr, 0 } },
  { "{E8016B91-9823-4B4C-AED8-C1F954FC0C70}", { L"Amt6", L"Amount 6" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 17, 1, nullptr, 0 } },
  { "{FABC471B-29FA-45A4-9ACA-6DD21582550F}", { L"In7", L"Input 7" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 18, 2, nullptr, 0 } },
  { "{BD316F66-A86A-46DD-B901-DA3EB0E4CB29}", { L"Out7", L"Output 7" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 19, 2, nullptr, 0 } },
  { "{D765242F-C63B-4B5F-824B-0E8C5E3732EA}", { L"Amt7", L"Amount 7" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 20, 2, nullptr, 0 } },
  { "{61A21B33-F174-4BB8-B70E-D0ABED3AFC32}", { L"In8", L"Input 8" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 21, 2, nullptr, 0 } },
  { "{F52DEACA-9B5C-4D4C-9381-79967288389B}", { L"Out8", L"Output 8" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 22, 2, nullptr, 0 } },
  { "{AAF3C200-9937-4CE2-BB76-DB0FA6FE43AB}", { L"Amt8", L"Amount 8" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 23, 2, nullptr, 0 } },
  { "{EE82872C-0641-48CA-A030-5FCBA09FC0F7}", { L"In9", L"Input 9" }, L"", false, 0, audio_inputs_count - 1, 0, &format_audio_input, &parse_audio_input, { false, false, 24, 2, nullptr, 0 } },
  { "{DDD8D342-8ACA-4203-85EC-AA8EEE666A16}", { L"Out9", L"Output 9" }, L"", false, 0, audio_outputs_count - 1, 0, &format_audio_output, &parse_audio_output, { false, false, 25, 2, nullptr, 0 } },
  { "{D0CBABC4-9772-471B-AF8B-FE6BF6197CA8}", { L"Amt9", L"Amount 9" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { false, false, 26, 2, nullptr, 0 } }
};

// ---- cv route ---- 
   
static graph_descriptor const cv_route_graph = { -1, 0, 1, 2, 2, L"CV" };
static wchar_t const* const cv_input_names[cv_route_input::count] = { L"Off", L"Env", L"VLFO" };
static wchar_t const* const cv_input_op_name[cv_route_input_op::count] = { L"Add", L"Mul" };
static wchar_t const* const* const cv_input_op_names[cv_route_input::count] = { nullptr, cv_input_op_name, cv_input_op_name };
static wchar_t const* const cv_output_names[cv_route_output::count] = { L"Off", L"Osc", L"VFlt", L"Amp" };
static wchar_t const* const cv_vamp_output_names[cv_route_vamp_output::count] = { L"Lvl", L"Pan" };
static wchar_t const* const cv_vflt_output_names[cv_route_vflt_output::count] = { L"Frq", L"Res", L"Kbd", L"Dly+", L"Gn+", L"Dly-", L"Gn-" };
static wchar_t const* const cv_osc_output_names[cv_route_osc_output::count] = { L"Amp", L"Pan", L"Pw", L"Dist", L"Roll", L"Cent", L"Dtn", L"Sprd" };
static wchar_t const* const* const cv_output_target_names[cv_route_output::count] = { nullptr, cv_osc_output_names, cv_vflt_output_names, cv_vamp_output_names };
static std::vector<std::wstring> const cv_input_names_list = zip_list_names(cv_input_names, cv_input_counts, cv_input_op_names, cv_input_op_counts, cv_route_input::count);
static std::vector<std::wstring> const cv_output_names_list = zip_list_names(cv_output_names, cv_output_counts, cv_output_target_names, cv_output_target_counts, cv_route_output::count);
   
static std::wstring format_cv_input(std::int32_t val) { return cv_input_names_list[val]; }
static std::wstring format_cv_output(std::int32_t val) { return cv_output_names_list[val]; }
static bool parse_cv_input(std::wstring const& val, std::int32_t& result) { return list_parser(val, cv_input_names_list, result); }
static bool parse_cv_output(std::wstring const& val, std::int32_t& result) { return list_parser(val, cv_output_names_list, result); }
    
static param_descriptor const 
cv_route_params[cv_route_param::count] =  
{  
  { "{CE1DC1C7-72C5-4811-8C35-8485FFAFFABC}", { L"Plot", L"Plot target" }, L"", false, 0, cv_outputs_count - 1, 0,&format_cv_output,&parse_cv_output, { true, false, 0, 1, nullptr, 0 } },
  { "{41FB9033-220C-4DA7-836A-22808E17167F}", { L"Time", L"Plot length" }, L"Sec", { 0.5f, 2, real_bounds::quadratic(0.01f, 10.0f), real_bounds::quadratic(0.01f, 10.0f) }, { false, false, 1, 1, nullptr, 0 } },
  { "{3B025C6A-0230-491A-A51F-7CF1C81B69C9}", { L"In1", L"Input 1" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, {true, false, 2, 0, nullptr, 0} },
  { "{5FDD8C86-8F2D-4613-BB98-BB673F502412}", { L"Out1", L"Output 1" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 3, 0, nullptr, 0 } },
  { "{469D130F-2E4A-4960-871D-032B6F588313}", { L"Amt1", L"Amount 1" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 4, 0, nullptr, 0 } },
  { "{2833E378-210B-404F-A4CB-0D6204A72CF0}", { L"In2", L"Input 2" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 5, 0, nullptr, 0 } },
  { "{D0B28D9E-8888-42EB-8D3C-177FB4585E42}", { L"Out2", L"Output 2" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 6, 0, nullptr, 0 } },
  { "{58AFE21F-7945-4919-BB67-60CE8892A8AF}", { L"Amt2", L"Amount 2" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 7, 0, nullptr, 0 } },
  { "{25041AB5-2A06-4305-8009-C26D56311D26}", { L"In3", L"Input 3" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 8, 0, nullptr, 0 } },
  { "{37420523-6A9D-4125-BAAB-24A28B9E0992}", { L"Out3", L"Output 3" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 9, 0, nullptr, 0 } },
  { "{9C1F6472-6D48-42E6-B79E-3A00F33F70F5}", { L"Amt3", L"Amount 3" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 10, 0, nullptr, 0 } },
  { "{958C1FF6-54BA-4933-9454-125357EE854E}", { L"In4", L"Input 4" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 11, 0, nullptr, 0 } },
  { "{13056333-4562-4D49-9A1E-7EF8FD074956}", { L"Out4", L"Output 4" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 12, 0, nullptr, 0 } },
  { "{7A8C3621-D7B9-4D63-9856-DFE2C9396FCD}", { L"Amt4", L"Amount 4" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 13, 0, nullptr, 0 } },
  { "{DE0CBCF0-A808-4379-A044-2F4C5CFAC9CF}", { L"In5", L"Input 5" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 14, 0, nullptr, 0 } },
  { "{B043E6E9-2207-4AF7-8AB0-F30B8B722F2C}", { L"Out5", L"Output 5" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 15, 0, nullptr, 0 } },
  { "{FA974BFF-3DD9-4B42-9FAD-49B9F8196D46}", { L"Amt5", L"Amount 5" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 16, 0, nullptr, 0 } },
  { "{2E34D57F-7277-48D6-9C7E-63897D735005}", { L"In6", L"Input 6" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 17, 1, nullptr, 0 } },
  { "{4E840E8E-ADDD-4C43-A48F-901EEFA616C8}", { L"Out6", L"Output 6" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 18, 1, nullptr, 0 } },
  { "{645B184E-6AEF-4119-8933-4623749D6280}", { L"Amt6", L"Amount 6" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 19, 1, nullptr, 0 } },
  { "{11B875C8-815C-48DD-B6FC-4B2F6509D1B6}", { L"In7", L"Input 7" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 20, 1, nullptr, 0 } },
  { "{B0C99125-73D5-493C-B1A4-A145BAF71455}", { L"Out7", L"Output 7" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 21, 1, nullptr, 0 } },
  { "{5E433D4A-6E06-46DC-82DA-6F20A6C5652C}", { L"Amt7", L"Amount 7" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 22, 1, nullptr, 0 } },
  { "{A72F92CE-A125-42A1-A1BE-2C45D96F7420}", { L"In8", L"Input 8" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 23, 1, nullptr, 0 } },
  { "{65C29F9C-BC32-4087-94BC-21E524ED2840}", { L"Out8", L"Output 8" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 24, 1, nullptr, 0 } },
  { "{E5E5897C-0818-49F5-B099-7778760B91A9}", { L"Amt8", L"Amount 8" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 25, 1, nullptr, 0 } },
  { "{68B976D6-3C1A-4D6F-B1FD-AEEF8FD95F4B}", { L"In9", L"Input 9" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 26, 1, nullptr, 0 } },
  { "{B7E00C89-2899-4F98-9C1B-C89801D6CFB3}", { L"Out9", L"Output 9" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 27, 1, nullptr, 0 } },
  { "{2A24302A-C793-41AC-BAEE-9BF154C63504}", { L"Amt9", L"Amount 9" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 28, 1, nullptr, 0 } },
  { "{0991AAF8-9BB5-4BDF-885A-93231158D717}", { L"In10", L"Input 10" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 29, 1, nullptr, 0 } },
  { "{5D776DFF-11F0-48F0-8F05-2E43589C5556}", { L"Out10", L"Output 10" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 30, 1, nullptr, 0 } },
  { "{02DF43DA-3D49-4571-87FC-F4DF12A0ADBE}", { L"Amt10", L"Amount 10" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 31, 1, nullptr, 0 } },
  { "{60B208B7-771B-4DFD-B9C0-97E097C340BF}", { L"In11", L"Input 11" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 32, 2, nullptr, 0 } },
  { "{9CC8A1E9-A060-41DC-AA0E-685D9BC65CAF}", { L"Out11", L"Output 11" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 33, 2, nullptr, 0 } },
  { "{C1F0E288-FB7F-4762-BA3F-03A2508813B5}", { L"Amt11", L"Amount 11" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 34, 2, nullptr, 0 } },
  { "{C5F2A2F7-33D0-4B82-903D-82214B938FD2}", { L"In12", L"Input 12" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 35, 2, nullptr, 0 } },
  { "{6C10DABE-1531-4138-AE21-92794637E535}", { L"Out12", L"Output 12" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 36, 2, nullptr, 0 } },
  { "{213502CD-8FEF-45AB-8FDF-69350CC67F72}", { L"Amt12", L"Amount 12" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 37, 2, nullptr, 0 } },
  { "{570C4799-B355-4EDB-AB38-32AECDE1C07C}", { L"In13", L"Input 13" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 38, 2, nullptr, 0 } },
  { "{DE40B39E-2744-44AD-B9EB-A56ECBEB27FF}", { L"Out13", L"Output 13" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 39, 2, nullptr, 0 } },
  { "{11DC6780-F5F5-4455-90D7-7E387C8F18E0}", { L"Amt13", L"Amount 13" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 40, 2, nullptr, 0 } },
  { "{37EFEDDD-336C-4ACB-A3CB-499D3A2F6427}", { L"In14", L"Input 14" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 41, 2, nullptr, 0 } },
  { "{D0069229-060E-4652-9ECE-CB1474FA791D}", { L"Out14", L"Output 14" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 42, 2, nullptr, 0 } },
  { "{C96385B6-30B7-44BB-9987-30003367804E}", { L"Amt14", L"Amount 14" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 43, 2, nullptr, 0 } },
  { "{4DA00E54-7133-4AC4-9B22-286A7F2CD3E7}", { L"In15", L"Input 15" }, L"", false, 0, cv_inputs_count - 1, 0, &format_cv_input, &parse_cv_input, { true, false, 44, 2, nullptr, 0 } },
  { "{DF96C8A8-43D8-4515-922D-E7F7F4511AD0}", { L"Out15", L"Output 15" }, L"", false, 0, cv_outputs_count - 1, 0, &format_cv_output, &parse_cv_output, { true, true, 45, 2, nullptr, 0 } },
  { "{F6DA01D0-6BFD-4F14-9776-95250FC57CA6}", { L"Amt15", L"Amount 15" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { false, true, 46, 2, nullptr, 0 } }
};  
               
// ---- global topo ---- 
  
part_descriptor const      
part_descriptors[part_type::count] =    
{   
  { "{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}", { L"Osc", L"Oscillator" }, part_type::oscillator, false, oscillator_count, oscillator_params, oscillator_param::count, oscillator_graphs, oscillator_graph::count, { 0, 3, 0, L"Voice" } },
  { "{FC4885FE-431C-477A-B5B7-84863DB8C07D}", { L"Env", L"Envelope" }, part_type::envelope, false, envelope_count, envelope_params, envelope_param::count, &envelope_graph, 1, { 3, 3, 0, L"Voice" } },
  { "{56DE75BB-BE73-4B27-B37F-77F6E408F986}", { L"VLFO", L"Voice LFO" }, part_type::voice_lfo, false, voice_lfo_count, voice_lfo_params, voice_lfo_param::count, &voice_lfo_graph, 1, { 1, 3, 0, L"Voice" } },
  { "{E6344937-C1F7-4F2A-83E7-EA27D48DEC4E}", { L"Amp", L"Voice amp" }, part_type::voice_amp, false, 1, voice_amp_params, voice_amp_param::count, nullptr, 0, { 4, 3, -1, L"Voice" } },
  { "{2C377544-C124-48F5-A4F4-1E301B108C58}", { L"VFilter", L"Voice filter" }, part_type::voice_filter, false, voice_filter_count, voice_filter_params, voice_filter_param::count, &voice_filter_graph, 1, { 2, 3, 0, L"Voice" } },
  { "{7A77C027-FC8F-4425-9BF0-393267D92F0C}", { L"Audio", L"Audio route" }, part_type::audio_route, false, 1, audio_route_params, audio_route_param::count, nullptr, 0, { 5, 3, -1, L"Route" } },
  { "{E6814824-7F56-4A9C-92B6-F5EB001B9513}", { L"CV", L"CV route" }, part_type::cv_route, false, 1, cv_route_params, cv_route_param::count, &cv_route_graph, 1, { 6, 3, -1, L"Route" } },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { L"Out", L"Output" }, part_type::output, true, 1, output_params, output_param::count, nullptr, 0, { 7, 3, -1, L"Global" } }
};    
       
} // namespace svn::synth           