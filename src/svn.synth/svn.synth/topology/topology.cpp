#include <svn.synth/topology/topology.hpp>
#include <sstream>

using namespace svn::base;

namespace svn::synth {
         
// ---- cpu ----  

static param_descriptor const
cpu_params[cpu_param::count] =
{
  { "{485325E3-3E15-46C4-84FA-5D743BB75C3A}", { L"Total", L"Total" }, L"%", false, 0, 100, 0, { 2, 1, nullptr, 0 } },
  { "{1A5EA805-8678-4B9F-A03F-92E1A9D3C519}", { L"Osc", L"Oscillator" }, L"%", false, 0, 100, 0, { 0, 0, nullptr, 0 } },
  { "{C39636DA-EBF5-4AA5-AFBB-BBCD5762D979}", { L"LFO", L"LFO" }, L"%", false, 0, 100, 0, { 7, 2, nullptr, 0 } },
  { "{C3FBB5D7-E5EA-4EEB-9795-22298F7380B9}", { L"Filter", L"Filter" }, L"%", false, 0, 100, 0, { 1, 0, nullptr, 0 } },
  { "{0DF43E23-9273-46BC-8BBE-52B2C0E612F7}", { L"Env", L"Envelope" }, L"%", false, 0, 100, 0, { 6, 2, nullptr, 0 } },
  { "{DC142F66-E4B2-4BEA-8B2E-A9501B32B1FB}", { L"Amp", L"Amplitude" }, L"%", false, 0, 100, 0, { 3, 0, nullptr, 0 } },
  { "{295F92F6-37CD-453E-B8C2-E03B6648A1E4}", { L"Audio", L"Audio route" }, L"%", false, 0, 100, 0, { 4, 0, nullptr, 0 } },
  { "{BCF00561-4D8E-4AB8-94CE-AF7EF65881FB}", { L"CV", L"CV route" }, L"%", false, 0, 100, 0, { 8, 2, nullptr, 0 } },
  { "{BCF00561-4D8E-4AB8-94CE-AF7EF65881FB}", { L"Aux", L"Auxiliary" }, L"%", false, 0, 100, 0, { 5, 1, nullptr, 0 } }
};

// ---- output ----  

static param_descriptor const
output_params[output_param::count] =
{
  { "{A6024C5F-CF37-48C8-BE5B-713191155AE5}", { L"Clip", L"Clip" }, false, { 0, 0, nullptr, 0 } },
  { "{8363D40B-6D49-4CF0-8D86-5449997034DE}", { L"Drain", L"Voices exhausted" }, false, { 1, 1, nullptr, 0 } },
  { "{6190497F-E08C-49DC-8C74-BAEF6D889BCB}", { L"Voices", L"Voice count" }, L"", false, 0, synth_polyphony, 0, { 2, 2, nullptr, 0 } }
};

// ---- amp ----  

static graph_descriptor const amp_graph = { -1, 0, 2, 1, 1, L"Amp" };

static param_descriptor const
amplitude_params[amplitude_param::count] =
{
  { "{5A2DF5BA-7D6F-4053-983E-AA6DC5084373}", { L"Amp", L"Level" }, L"dB", {1.0f, 2, real_bounds::unit(), real_bounds::decibel()}, { 0, 0, nullptr, 0} },
  { "{86782F43-7079-47BE-9C7F-8BF6D12A0950}", { L"Pan", L"Panning" }, L"%", { 0.5f, 2, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { 1, 1, nullptr, 0 } }
};

// ---- active ----  

wchar_t const* envelope_item_info(std::int32_t index)
{ return index > 0? nullptr: L"Amp"; };
 
static std::vector<std::wstring> const active_lfo_names = list_names(L"LFO", lfo_count, nullptr);
static std::vector<std::wstring> const active_filter_names = list_names(L"Filter", filter_count, nullptr);
static std::vector<std::wstring> const active_cv_route_names = list_names(L"CV", cv_route_count, nullptr);
static std::vector<std::wstring> const active_oscillator_names = list_names(L"Osc", oscillator_count, nullptr);
static std::vector<std::wstring> const active_audio_route_names = list_names(L"Audio", audio_route_count, nullptr);
static std::vector<std::wstring> const active_envelope_names = list_names(L"Env", envelope_count, envelope_item_info);
 
static param_descriptor const
active_params[active_param::count] =
{
  { "{0EEADBD0-D37B-4B53-A2D5-F22E2154F2D8}", { L"Oscillator", L"Oscillator" }, L"", false, &active_oscillator_names, 0, param_no_ui },
  { "{EEA97414-8C1C-4378-A68B-409692FFA920}", { L"LFO", L"LFO" }, L"", false, &active_lfo_names, 0, param_no_ui },
  { "{D5DD0DFC-AC9D-42E6-9D2B-924786382825}", { L"Filter", L"Filter" }, L"", false, &active_filter_names, 0, param_no_ui },
  { "{556BF692-55B0-48B4-BD6A-E2CEFA17B012}", { L"Envelope", L"Envelope" }, L"", false, &active_envelope_names, 0, param_no_ui },
  { "{556BF692-55B0-48B4-BD6A-E2CEFA17B012}", { L"Audio", L"Audio route" }, L"", false, &active_audio_route_names, 0, param_no_ui },
  { "{BFCAD318-CB76-4AF7-9B43-3F1776144813}", { L"CV", L"CV route" }, L"", false, &active_cv_route_names, 0, param_no_ui },
};

// ---- lfo ----

static std::vector<std::pair<std::int32_t, std::int32_t>> const lfo_timesig = beat_synced_timesig(16, false);
static std::vector<std::wstring> const lfo_timesig_names = beat_synced_timesig_names(lfo_timesig);
static std::vector<float> const lfo_timesig_values = beat_synced_timesig_values(lfo_timesig);

static graph_descriptor const lfo_graph = { -1, 0, 2, 2, 1, L"LFO" };
static std::vector<std::wstring> const lfo_types = { L"Sine", L"Custom", L"Random" };
static param_relevance const lfo_time_relevance[1] = { { lfo_param::synced, { 0 } } };
static param_relevance const lfo_sync_relevance[1] = { { lfo_param::synced, { 1 } } };

static param_descriptor const
lfo_params[lfo_param::count] =
{
  { "{42FB0553-788E-470F-906A-D95FED2ED980}", { L"On", L"Enabled" }, false, { -1, 0, nullptr, 0 } },
  { "{F744C553-8CFA-4262-98A7-37E187BF27FF}", { L"Type", L"Type" }, L"", false, &lfo_types, 0, { 0, 0, nullptr, 0 } },
  { "{0E9A5C22-34A2-4658-9DE6-718DF9028ECD}", { L"Bipolar", L"Bipolar" }, false, { 2, 1, nullptr, 0 } },
  { "{83C1ED1B-095E-4F58-B091-39DA4F0125BF}", { L"Beat sync", L"Sync to beat" }, false, { 3, 1, nullptr, 0 } },
  { "{E320A1F0-2FCA-46F2-BBCB-0504D65503BC}", { L"Period", L"Period" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(lfo_min_period, lfo_max_period), real_bounds::quadratic(lfo_min_period, lfo_max_period) }, { 1, 2, lfo_time_relevance, 1 } },
  { "{09618D35-EFAD-4E2E-8FD0-04B6F5AC14D5}", { L"Step", L"Tempo" }, L"", true, &lfo_timesig_names, 0, { 1, 2, lfo_sync_relevance, 1 } }
};  

// ---- envelope ----

static std::vector<std::pair<std::int32_t, std::int32_t>> const env_timesig = beat_synced_timesig(16, true);
static std::vector<std::wstring> const env_timesig_names = beat_synced_timesig_names(env_timesig); 
static std::vector<float> const env_timesig_values = beat_synced_timesig_values(env_timesig);

static graph_descriptor const envelope_graph = { -1, 0, 2, 3, 1, L"Envelope" };
static std::vector<std::wstring> const envelope_types = { L"DAHDSR", L"DAHDR" };
static std::vector<std::wstring> const envelope_slopes = { L"Linear", L"Quadratic", L"Inverted", L"Logarithmic" };
static param_relevance const envelope_time_relevance[1] = { { envelope_param::synced, { 0 } } };
static param_relevance const envelope_sync_relevance[1] = { { envelope_param::synced, { 1 } } };
static param_relevance const envelope_decay_log_relevance[1] = { { envelope_param::decay_slope, { envelope_slope::logarithmic } } };
static param_relevance const envelope_attack_log_relevance[1] = { { envelope_param::attack_slope, { envelope_slope::logarithmic } } };
static param_relevance const envelope_release_log_relevance[1] = { { envelope_param::release_slope, { envelope_slope::logarithmic } } };
 
static param_descriptor const 
envelope_params[envelope_param::count] =   
{   
  { "{FC3CE4BC-D8F0-487E-9BB6-826988B4B812}", { L"On", L"Enabled" }, false, { -1, 0, nullptr, 0 } },
  { "{D622D344-A08E-4109-84D8-C310B81C2769}", { L"Type", L"Type" }, L"", false, &envelope_types, 0, { 0, 0, nullptr, 0}},
  { "{2D8C38A2-99AD-4080-8262-26D24F7644F0}", { L"Bipolar", L"Bipolar" }, false, { 4, 0, nullptr, 0 } },
  { "{82B2C892-1A12-4FDC-A26D-7733C3C3B782}", { L"Beat sync", L"Sync to beat" }, false, { 5, 0, nullptr, 0 } },
  { "{B1D30C73-FF09-452C-A9D5-00BB8B2CE58D}", { L"Delay", L"Delay time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 1, 2, envelope_time_relevance, 1 } },
  { "{70C49337-7141-42BC-B336-78B28F4770E3}", { L"Delay", L"Delay sync" }, L"", true, &env_timesig_names, 0, { 1, 2, envelope_sync_relevance, 1}},
  { "{AADD118E-B9CE-464E-83C0-1FAE5A62F530}", { L"Hold", L"Hold time" }, L"Sec", { 0.0f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 2, 2, envelope_time_relevance, 1 } },
  { "{AC5D88B7-8CDB-4401-9FB0-20EF0195ABD8}", { L"Hold", L"Hold sync" }, L"", true, &env_timesig_names, 0, { 2, 2, envelope_sync_relevance, 1 } },
  { "{721AFEB5-A17E-4C37-BF3A-94F645234B73}", { L"Sustain", L"Sustain level" }, L"dB", { 0.5f, 2, real_bounds::unit(), real_bounds::decibel() }, { 3, 2, nullptr, 0 } },
  { "{E0A45A06-3C0C-4409-BBFA-A2834F0C7BAD}", { L"Attack", L"Attack time" }, L"Sec", { 0.05f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 6, 2, envelope_time_relevance, 1 } },
  { "{E0CFCBB7-B050-4C47-B78C-2511AD2EE6B6}", { L"Attack", L"Attack sync" }, L"", true, &env_timesig_names, 1, { 6, 2, envelope_sync_relevance, 1 } },
  { "{1400CEB5-25B1-42B1-ADA1-B5FDF8C6D1E5}", { L"Slope", L"Attack slope" }, L"", false, &envelope_slopes, 0, { 7, 1, nullptr, 0 } },
  { "{5A57D560-64D2-44D5-BD78-82B7C4BEA79C}", { L"Mid", L"Attack log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 8, 1, envelope_attack_log_relevance, 1 } },
  { "{A62B5DB0-4D41-4DA4-AE14-E25A2C983B21}", { L"Decay", L"Decay time" }, L"Sec", { 0.1f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 9, 2, envelope_time_relevance, 1 } },
  { "{09B32681-21EF-4C65-B5FD-9FA64ED4AF5E}", { L"Decay", L"Decay sync" }, L"", true, &env_timesig_names, 1, { 9, 2, envelope_sync_relevance, 1 } },
  { "{62381FB9-9060-4425-97A6-B57ECB2BECCA}", { L"Slope", L"Decay slope" }, L"", false, &envelope_slopes, 0, { 10, 1, nullptr, 0 } },
  { "{8B5D1F67-DA80-4170-982D-909CC8A025E2}", { L"Mid", L"Decay log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 11, 1, envelope_decay_log_relevance, 1 } },
  { "{75ACC8A6-F228-445C-9788-A367AE32EAAA}", { L"Release", L"Release time" }, L"Sec", { 0.2f, 2, real_bounds::quadratic(0.0f, 10.0f), real_bounds::quadratic(0.0f, 10.0f) }, { 12, 2, envelope_time_relevance, 1 } },
  { "{DDA55E4D-A100-40BA-A7C2-F4C284CACE08}", { L"Release", L"Release sync" }, L"", true, &env_timesig_names, 1, { 12, 2, envelope_sync_relevance, 1 } },
  { "{6B978A93-54D1-4990-BD2E-BC143EA816AF}", { L"Slope", L"Release slope" }, L"", false, &envelope_slopes, 0, { 13, 1, nullptr, 0 } },
  { "{4E2A6D8B-275C-4E1E-84BE-B21FE16E1E0F}", { L"Mid", L"Release log midpoint" }, L"", { 0.5f, 2, real_bounds::unit(), real_bounds::unit() }, { 14, 1, envelope_release_log_relevance, 1 } }
}; 
  
// ---- filter ---- 
  
static graph_descriptor const
filter_graphs[filter_graph::count] =
{
  { filter_graph::impulse, 0, 1, 2, 1, L"Impulse response"},
  { filter_graph::frequency, 0, 2, 2, 1, L"Frequency response" }
};

static std::vector<std::wstring> const filter_types = { L"StateVariable", L"Comb" };
static std::vector<std::wstring> const filter_state_var_types = { L"LPF", L"HPF", L"BPF", L"BSF", L"APF" };
static param_relevance const filter_comb_relevance[1] = { { filter_param::type, { filter_type::comb } } };
static param_relevance const filter_state_var_relevance[1] = { { filter_param::type, { filter_type::state_var } } };
   
static param_descriptor const  
filter_params[filter_param::count] = 
{  
  { "{5813593C-E25A-4C8B-B7E5-7C95DAC257C2}", { L"On", L"Enabled" }, false, { -1, 0, nullptr, 0 } },
  { "{956F08A1-B3C6-412B-AEEA-55E722744AE8}", { L"Type", L"Type" }, L"", false, &filter_types, 0, { 0, 2, nullptr, 0 } },
  { "{7498AB5E-4BE6-45B0-8CBF-8D166FF82E32}", { L"StVar", L"State variable type" }, L"", false, &filter_state_var_types, 0, { 1, 2, filter_state_var_relevance, 1 } },
  { "{99F7D808-53EB-4A4F-9FBF-4F1F57E4B742}", { L"Freq", L"State variable frequency" }, L"Hz", { 0.25f, 0, real_bounds::quadratic(filter_min_freq, filter_max_freq), real_bounds::quadratic(filter_min_freq, filter_max_freq) }, { 2, 2, filter_state_var_relevance, 1 } },
  { "{AA394A33-B616-4EAF-830D-313F2FE316AF}", { L"Res", L"State variable resonance" }, L"%", { 0.0f, 2, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f) }, { 3, 2, filter_state_var_relevance, 1 } },
  { "{BF8775C6-FD22-4653-BC21-64CDC8D24AFF}", { L"Kbd", L"State variable keyboard tracking" }, L"%", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 4, 2, filter_state_var_relevance, 1 } },
  { "{36AA381A-82F4-4259-9D87-431B8C49EAB4}", { L"Delay+", L"Comb+ delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { 1, 2, filter_comb_relevance, 1 } },
  { "{9B330083-CF9C-4161-9A66-74AF468EF521}", { L"Gain+", L"Comb+ gain" }, L"%", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 2, 2, filter_comb_relevance, 1 } },
  { "{25A32954-6975-4591-87B0-F0718206E965}", { L"Delay-", L"Comb- delay" }, L"Ms", { 0.5f, 2, real_bounds::linear(0.0f, 0.005f), real_bounds::linear(0.0f, 5.0f) }, { 3, 2, filter_comb_relevance, 1 } },
  { "{E67FACFD-E13D-4618-85B6-5FED1E3761F3}", { L"Gain-", L"Comb- gain" }, L"%", { 0.5f, 2, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 4, 2, filter_comb_relevance, 1 } }
};
 
// ---- oscillator ----

static graph_descriptor const
oscillator_graphs[oscillator_graph::count] =
{
  { oscillator_graph::wave, 0, 1, 3, 1, L"Waveform"},
  { oscillator_graph::spectrum, 0, 2, 3, 1, L"Frequency spectrum" }
};  

static param_relevance const 
oscillator_analog_pw_relevance[2] =
{
  { oscillator_param::type, { oscillator_type::analog } },
  { oscillator_param::analog_type, { oscillator_analog_type::pulse } }
};

static std::vector<std::wstring> const oscillator_types = { L"Analog", L"DSF" };
static std::vector<std::wstring> const oscillator_analog_types = { L"Sine", L"Saw", L"Pulse", L"Triangle" };
static param_relevance const oscillator_dsf_relevance[1] = { { oscillator_param::type, { oscillator_type::dsf } } };
static param_relevance const oscillator_analog_relevance[1] = { { oscillator_param::type, { oscillator_type::analog } } };
 
static param_descriptor const
oscillator_params[oscillator_param::count] =
{ 
  { "{CA9274AE-9F57-4373-8C59-3786ACC1C1CD}", { L"On", L"Enabled" }, false, { -1, 0, nullptr, 0 } },
  { "{C5C4B00A-3D26-4342-ACE6-0AE936FFBCA3}", { L"Amp", L"Level" }, L"dB", { 1.0f, 2, real_bounds::unit(), real_bounds::decibel() }, { 0, 2, nullptr, 0 } },
  { "{63C96678-9823-4B96-B2C0-78014A268DB5}", { L"Pan", L"Panning" }, L"%", { 0.5f, 2, real_bounds::unit(), real_bounds::linear(-100.0f, 100.0f) }, { 1, 2, nullptr, 0 } },
  { "{84A7AEC8-25E9-4242-B32E-2E9E780F0E31}", { L"Type", L"Type" }, L"", false, &oscillator_types, 0, { 2, 2, nullptr, 0 } },
  { "{EAFF1105-BA8D-4C55-8BEC-6B73AECF95E2}", { L"Analog", L"Analog type" }, L"", false, &oscillator_analog_types, 0, { 3, 2, oscillator_analog_relevance, 1 } },
  { "{35DAF80A-6EE0-4A3C-9E81-B225A466F4B2}", { L"PW", L"Analog pulse width" }, L"%", { 1.0f, 0, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }, { 5, 2, oscillator_analog_pw_relevance, 2 } },
  { "{8B45133B-38AA-4E23-9CB9-B05A4954A947}", { L"Partials", L"DSF partials" }, L"", true, 0, 9999, 0, { 3, 2, oscillator_dsf_relevance, 1 } },
  { "{083A6619-0309-48CA-8E9E-9A309EA61044}", { L"Distance", L"DSF distance" }, L"", { (1.0f - 0.05f) / (20.0f - 0.05f), 2, real_bounds::linear(0.05f, 20.0f), real_bounds::linear(0.05f, 20.0f) }, { 4, 2, oscillator_dsf_relevance, 1 } },
  { "{FFD6C9F3-B7D4-4819-A63A-40BC907F91AF}", { L"Rolloff", L"DSF rolloff" }, L"%", { 0.5f, 2, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f) }, { 5, 2, oscillator_dsf_relevance, 1 } },
  { "{5E3DB4DC-B459-43C4-9BBD-0FF8F2232AFA}", { L"Octave", L"Octave" }, L"", true, 0, 9, 4, { 6, 0, nullptr, 0 } },
  { "{501C5217-5A5B-48D8-AEFE-CFE67417E8AD}", { L"Note", L"Note" }, L"", true, &note_names, 0, { 7, 0, nullptr, 0 } },
  { "{FD0A2D20-FCCD-4939-B1CC-BD2EBD998A25}", { L"Cent", L"Cent" }, L"", { 0.5f, 2, real_bounds::linear(-0.5f, 0.5f), real_bounds::linear(-50.0f, 50.0f) }, { 8, 0, nullptr, 0 } },
  { "{F5B30B84-5C3E-471F-9B27-D1FB6E06D0AF}", { L"Unison", L"Unison voices" }, L"", true, 1, oscillator_max_voices, 1, { 9, 1, nullptr, 0 } },
  { "{70C38B3D-E81C-42D9-A59A-9619DB318DFD}", { L"Detune", L"Unison detune" }, L"%", { 0.0f, 2, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f)}, { 10, 1, nullptr, 0 } },
  { "{412B4D8D-4272-40ED-949F-479FB8407BF7}", { L"Spread", L"Unison stereo spread" }, L"%", { 0.0f, 2, real_bounds::unit(), real_bounds::linear(0.0f, 100.0f) }, { 11, 1, nullptr, 0 } }
};    
  
// ---- audio route ---- 
 
static wchar_t const* const audio_route_input_names[audio_route_input::count] = { L"Off", L"Osc", L"Filter" };
static wchar_t const* const audio_route_output_names[audio_route_output::count] = { L"Off", L"Filter", L"Amp" };
static std::vector<std::wstring> const audio_route_input_names_list = multi_list_names(audio_route_input_names, audio_route_input_counts, audio_route_input::count);
static std::vector<std::wstring> const audio_route_output_names_list = multi_list_names(audio_route_output_names, audio_route_output_counts, audio_route_output::count);

static param_descriptor const
audio_route_params[audio_route_param::count] =
{
  { "{14096099-485D-4EB9-B055-E393DE2E993C}", { L"On", L"Enabled" }, false, { -1, 0, nullptr, 0 } },
  { "{2E9F0478-B911-43DF-BB51-0C5836E4853F}", { L"In1", L"Input 1" }, L"", false, &audio_route_input_names_list, 0, { 0, 0, nullptr, 0}},
  { "{295DC5F0-FB32-4D43-8799-D79F23FD3AA9}", { L"Out1", L"Output 1" }, L"", false, &audio_route_output_names_list, 0, { 1, 0, nullptr, 0 } },
  { "{A3B15FE9-56DB-493B-A4E1-31A004F3C937}", { L"Amt1", L"Amount 1" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 2, 0, nullptr, 0 } },
  { "{A3A59082-CF73-4C28-A3FC-037729C9CB42}", { L"In2", L"Input 2" }, L"", false, &audio_route_input_names_list, 0, { 3, 1, nullptr, 0 } },
  { "{843EB41C-199F-4FAC-ACC4-841B3663248D}", { L"Out2", L"Output 2" }, L"", false, &audio_route_output_names_list, 0, { 4, 1, nullptr, 0 } },
  { "{810B55DF-6230-45C7-B478-7A3569DC9127}", { L"Amt2", L"Amount 2" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 5, 1, nullptr, 0 } },
  { "{A8E6882A-8945-4F59-92B9-78004EAF5818}", { L"In3", L"Input 3" }, L"", false, &audio_route_input_names_list, 0, { 6, 2, nullptr, 0 } },
  { "{D2E34E63-DE9E-4620-924D-1897614BF983}", { L"Out3", L"Output 3" }, L"", false, &audio_route_output_names_list, 0, { 7, 2, nullptr, 0 } },
  { "{6721B1EC-9688-48A3-B5B8-0ADD0A9CF16B}", { L"Amt3", L"Amount 3" }, L"dB", { 1.0f, 1, real_bounds::unit(), real_bounds::decibel() }, { 8, 2, nullptr, 0 } }
}; 

// ---- cv route ----                
    
static graph_descriptor const cv_route_graph = { -1, 0, 1, 2, 2, L"CV" };
static wchar_t const* const cv_route_input_names[cv_route_input::count] = { L"Off", L"Env", L"LFO" };
static list_item_info const cv_route_input_infos[cv_route_input::count] = { nullptr, envelope_item_info, nullptr };
static wchar_t const* const cv_route_input_op_name[cv_route_input_op::count] = { L"Add", L"Mul" };
static wchar_t const* const* const cv_route_input_op_names[cv_route_input::count] = { nullptr, cv_route_input_op_name, cv_route_input_op_name };
static wchar_t const* const cv_route_output_names[cv_route_output::count] = { L"Off", L"Osc", L"Filter", L"Amp" };
static wchar_t const* const cv_route_amp_output_names[cv_route_amp_output::count] = { L"Lvl", L"Pan" };
static wchar_t const* const cv_route_filter_output_names[cv_route_filter_output::count] = { L"Frq", L"Res", L"Kbd", L"Dly+", L"Gn+", L"Dly-", L"Gn-" };
static wchar_t const* const cv_route_osc_output_names[cv_route_osc_output::count] = { L"Amp", L"Pan", L"PW", L"Dist", L"Roll", L"Cent", L"Dtn", L"Sprd" };
static wchar_t const* const* const cv_route_output_target_names[cv_route_output::count] = { nullptr, cv_route_osc_output_names, cv_route_filter_output_names, cv_route_amp_output_names };
static std::vector<std::wstring> const cv_route_input_names_list = zip_list_names(cv_route_input_names, cv_route_input_infos, cv_route_input_counts, cv_route_input_op_names, cv_route_input_op_counts, cv_route_input::count);
static std::vector<std::wstring> const cv_route_output_names_list = zip_list_names(cv_route_output_names, nullptr, cv_route_output_counts, cv_route_output_target_names, cv_route_output_target_counts, cv_route_output::count);
     
static param_descriptor const  
cv_route_params[cv_route_param::count] =  
{  
  { "{1F6DEE15-DEE7-443B-B9F8-E65BFF9C9C4A}", { L"On", L"Enabled" }, false, { -1, 0, nullptr, 0 } },
  { "{CE1DC1C7-72C5-4811-8C35-8485FFAFFABC}", { L"Plot", L"Plot target" }, L"", false, &cv_route_output_names_list, 0, { 0, 1, nullptr, 0 } },
  { "{41FB9033-220C-4DA7-836A-22808E17167F}", { L"Time", L"Plot length" }, L"Sec", { 0.5f, 2, real_bounds::quadratic(0.01f, 10.0f), real_bounds::quadratic(0.01f, 10.0f) }, { 1, 1, nullptr, 0 } },
  { "{3B025C6A-0230-491A-A51F-7CF1C81B69C9}", { L"In1", L"Input 1" }, L"", false, &cv_route_input_names_list, 0, { 2, 0, nullptr, 0} },
  { "{5FDD8C86-8F2D-4613-BB98-BB673F502412}", { L"Out1", L"Output 1" }, L"", false, &cv_route_output_names_list, 0, { 3, 0, nullptr, 0 } },
  { "{469D130F-2E4A-4960-871D-032B6F588313}", { L"Amt1", L"Amount 1" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 4, 0, nullptr, 0 } },
  { "{2833E378-210B-404F-A4CB-0D6204A72CF0}", { L"In2", L"Input 2" }, L"", false, &cv_route_input_names_list, 0, { 5, 0, nullptr, 0 } },
  { "{D0B28D9E-8888-42EB-8D3C-177FB4585E42}", { L"Out2", L"Output 2" }, L"", false, &cv_route_output_names_list, 0, { 6, 0, nullptr, 0 } },
  { "{58AFE21F-7945-4919-BB67-60CE8892A8AF}", { L"Amt2", L"Amount 2" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 7, 0, nullptr, 0 } },
  { "{25041AB5-2A06-4305-8009-C26D56311D26}", { L"In3", L"Input 3" }, L"", false, &cv_route_input_names_list, 0, { 8, 1, nullptr, 0 } },
  { "{37420523-6A9D-4125-BAAB-24A28B9E0992}", { L"Out3", L"Output 3" }, L"", false, &cv_route_output_names_list, 0, { 9, 1, nullptr, 0 } },
  { "{9C1F6472-6D48-42E6-B79E-3A00F33F70F5}", { L"Amt3", L"Amount 3" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 10, 1, nullptr, 0 } },
  { "{958C1FF6-54BA-4933-9454-125357EE854E}", { L"In4", L"Input 4" }, L"", false, &cv_route_input_names_list, 0, { 11, 1, nullptr, 0 } },
  { "{13056333-4562-4D49-9A1E-7EF8FD074956}", { L"Out4", L"Output 4" }, L"", false, &cv_route_output_names_list, 0, { 12, 1, nullptr, 0 } },
  { "{7A8C3621-D7B9-4D63-9856-DFE2C9396FCD}", { L"Amt4", L"Amount 4" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 13, 1, nullptr, 0 } },
  { "{DE0CBCF0-A808-4379-A044-2F4C5CFAC9CF}", { L"In5", L"Input 5" }, L"", false, &cv_route_input_names_list, 0, { 14, 2, nullptr, 0 } },
  { "{B043E6E9-2207-4AF7-8AB0-F30B8B722F2C}", { L"Out5", L"Output 5" }, L"", false, &cv_route_output_names_list, 0, { 15, 2, nullptr, 0 } },
  { "{FA974BFF-3DD9-4B42-9FAD-49B9F8196D46}", { L"Amt5", L"Amount 5" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 16, 2, nullptr, 0 } },
  { "{2E34D57F-7277-48D6-9C7E-63897D735005}", { L"In6", L"Input 6" }, L"", false, &cv_route_input_names_list, 0, { 17, 2, nullptr, 0 } },
  { "{4E840E8E-ADDD-4C43-A48F-901EEFA616C8}", { L"Out6", L"Output 6" }, L"", false, &cv_route_output_names_list, 0, { 18, 2, nullptr, 0 } },
  { "{645B184E-6AEF-4119-8933-4623749D6280}", { L"Amt6", L"Amount 6" }, L"%", { 0.5f, 0, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }, { 19, 2, nullptr, 0 } }
};   
                
// ---- global topo ----    

struct part_group_t { enum value { audio, cv, route, global, count }; };
typedef part_group_t::value part_group;
static ui_color const  
group_colors[part_group::count] = { 0x00ffff, 0x00dcff, 0x55b4fc, 0x8a8bd4 };
              
part_descriptor const          
part_descriptors[part_type::count] =            
{         
  { "{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}", { L"Osc", L"Oscillator" }, part_type::oscillator, false, false, oscillator_count, oscillator_params, oscillator_param::count, oscillator_graphs, oscillator_graph::count, { 0, 3, 0, 0, active_param::oscillator, L"Voice audio", group_colors[part_group::audio] }},
  { "{FC4885FE-431C-477A-B5B7-84863DB8C07D}", { L"Env", L"Envelope" }, part_type::envelope, false, false, envelope_count, envelope_params, envelope_param::count, &envelope_graph, 1, { 2, 3, 0, 1, active_param::envelope, L"Voice CV", group_colors[part_group::cv] } },
  { "{56DE75BB-BE73-4B27-B37F-77F6E408F986}", { L"LFO", L"LFO" }, part_type::lfo, false, false, lfo_count, lfo_params, lfo_param::count, &lfo_graph, 1, { 3, 3, 0, 0, active_param::lfo, L"Voice CV", group_colors[part_group::cv] } },
  { "{E6344937-C1F7-4F2A-83E7-EA27D48DEC4E}", { L"Amp", L"Amplitude" }, part_type::amplitude, false, false, 1, amplitude_params, amplitude_param::count, &amp_graph, 1, { 4, 3, -1, -1, -1, L"Voice audio", group_colors[part_group::audio] } },
  { "{2C377544-C124-48F5-A4F4-1E301B108C58}", { L"Filter", L"Filter" }, part_type::filter, false, false, filter_count, filter_params, filter_param::count, filter_graphs, filter_graph::count, { 1, 3, 0, 0, active_param::filter, L"Voice audio", group_colors[part_group::audio] } },
  { "{7A77C027-FC8F-4425-9BF0-393267D92F0C}", { L"Audio", L"Audio route" }, part_type::audio_route, false, false, audio_route_count, audio_route_params, audio_route_param::count, nullptr, 0, { 5, 3, 0, 0, active_param::audio_route, L"Voice route", group_colors[part_group::route] } },
  { "{E6814824-7F56-4A9C-92B6-F5EB001B9513}", { L"CV", L"CV route" }, part_type::cv_route, false, false, cv_route_count, cv_route_params, cv_route_param::count, &cv_route_graph, 1, { 6, 3, 0, 0, active_param::cv_route, L"Voice route", group_colors[part_group::route] } },
  { "{C972E264-1739-4DB6-B1DB-5D31057BD218}", { L"Active", L"Active" }, part_type::active, false, true, 1, active_params, active_param::count, nullptr, -1, part_no_ui },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { L"Output", L"Output" }, part_type::output, true, false, 1, output_params, output_param::count, nullptr, 0, { 7, 3, -1, -1, -1, L"Global", group_colors[part_group::global] } },
  { "{93F3BCD1-30CC-4CC4-BFA8-B363786DBEAB}", { L"CPU", L"CPU usage" }, part_type::cpu, true, false, 1, cpu_params, cpu_param::count, nullptr, 0, { 8, 3, -1, -1, -1, L"Global", group_colors[part_group::global] } }
};    
          
} // namespace svn::synth             