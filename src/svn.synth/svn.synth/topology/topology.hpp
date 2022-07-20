#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>
#include <vector>

namespace svn::synth {

// ---- shared ----

struct cv_kind_t { enum value { time_unipolar, time_bipolar, sync_unipolar, sync_bipolar, count }; };
typedef cv_kind_t::value cv_kind;

// ---- global topo ----

inline std::int32_t constexpr envelope_count = 3;
inline std::int32_t constexpr oscillator_count = 4;
inline std::int32_t constexpr voice_lfo_count = 3;
inline std::int32_t constexpr voice_filter_count = 3;

extern base::part_descriptor const part_descriptors[];
inline std::int32_t constexpr synth_polyphony = 32;
inline std::int32_t constexpr synth_max_ui_height = 760;
inline base::ui_color constexpr synth_ui_end_color = { 0x6E, 0xFA, 0xD5 };
inline base::ui_color constexpr synth_ui_start_color = { 0xB7, 0xC3, 0xFD }; 

struct part_type_t { enum value { oscillator, envelope, voice_lfo, voice_amp, voice_filter, audio_route, cv_route, selector, output, count }; };
typedef part_type_t::value part_type;
  
// ---- amp ----

struct voice_amp_param_t { enum value { level, env1, pan, count }; };
typedef voice_amp_param_t::value voice_amp_param;

// ---- selector ----

struct selector_param_t { enum value { osc, vlfo, vflt, env, count }; };
typedef selector_param_t::value selector_param;

// ---- output ----

struct output_param_t { enum value { clip, drain, voices, total_cpu, osc_cpu, vlfo_cpu, vflt_cpu, env_cpu, vamp_cpu, audio_cpu, cv_cpu, aux_cpu, count }; };
typedef output_param_t::value output_param;

// ---- envelope ----

extern std::vector<float> const env_timesig_values;
struct envelope_type_t { enum value { dahdsr, dahdr, count }; };
struct envelope_slope_t { enum value { lin, log, quad, sqrt, count }; };
struct envelope_param_t { enum value { on, type, kind, delay_time, delay_sync, hold_time, hold_sync, sustain_level, attack_time, attack_sync, attack_slope, attack_mid, decay_time, decay_sync, decay_slope, decay_mid, release_time, release_sync, release_slope, release_mid, count }; };

typedef envelope_type_t::value envelope_type;
typedef envelope_slope_t::value envelope_slope;
typedef envelope_param_t::value envelope_param;

// ---- voice lfo ----

inline float constexpr voice_lfo_min_freq = 0.1f;
inline float constexpr voice_lfo_max_freq = 20.0f;
extern std::vector<float> const voice_lfo_timesig_values;

struct voice_lfo_type_t { enum value { sine, custom, random, count }; };
struct voice_lfo_param_t { enum value { on, type, kind, freq_time, freq_sync, count }; };

typedef voice_lfo_type_t::value voice_lfo_type;
typedef voice_lfo_param_t::value voice_lfo_param;

// ---- voice filter ----

inline float constexpr filter_min_freq = 20.0f;
inline float constexpr filter_max_freq = 20000.0f;
inline float constexpr comb_filter_max_ms = 5.0f;
inline float constexpr max_sample_rate = 384000.0f;

struct voice_filter_type_t { enum value { state_var, comb, count }; };
struct voice_filter_stvar_type_t { enum value { lpf, hpf, bpf, bsf, apf, count }; };
struct voice_filter_param_t { enum value { on, type, stvar_type, stvar_freq, stvar_res, stvar_kbd, comb_dly_plus, comb_gain_plus, comb_dly_min, comb_gain_min, count }; };

typedef voice_filter_type_t::value voice_filter_type;
typedef voice_filter_param_t::value voice_filter_param;
typedef voice_filter_stvar_type_t::value voice_filter_stvar_type;

// ---- oscillator ----

inline std::int32_t constexpr oscillator_max_voices = 8;

struct oscillator_type_t { enum value { analog, dsf, count }; };
struct oscillator_graph_t { enum value { wave, spectrum, count }; };
struct oscillator_analog_type_t { enum value { sine, saw, pulse, triangle, count }; };
struct oscillator_param_t { enum value { 
  on, amp, pan, type, analog_type, analog_pw, 
  dsf_partials, dsf_distance, dsf_rolloff, 
  octave, note, cent, unison, unison_detune, unison_spread, count }; };

typedef oscillator_type_t::value oscillator_type;
typedef oscillator_param_t::value oscillator_param;
typedef oscillator_graph_t::value oscillator_graph;
typedef oscillator_analog_type_t::value oscillator_analog_type;

// ---- audio route ----

struct audio_route_input_t { enum value { off, osc, filter, count }; };
struct audio_route_output_t { enum value { off, filter, amp, count }; };
struct audio_route_param_t { enum value { in1, out1, amt1, in2, out2, amt2, in3, out3, amt3, in4, out4, amt4, in5, out5, amt5, in6, out6, amt6, in7, out7, amt7, in8, out8, amt8, in9, out9, amt9, count }; };

typedef audio_route_param_t::value audio_route_param;
typedef audio_route_input_t::value audio_route_input;
typedef audio_route_output_t::value audio_route_output;

inline std::int32_t constexpr audio_route_count = 9;
inline std::int32_t constexpr audio_outputs_count = 1 + voice_filter_count + 1;
inline std::int32_t constexpr audio_inputs_count = 1 + oscillator_count + voice_filter_count;
inline std::int32_t constexpr audio_output_counts[audio_route_output::count] = { 1, voice_filter_count, 1 };
inline std::int32_t constexpr audio_input_counts[audio_route_input::count] = { 1, oscillator_count, voice_filter_count };

// ---- cv route ----

struct cv_route_input_t { enum value { off, envelope, vlfo, count }; };
struct cv_route_input_op_t { enum value { add, mul, count }; };

struct cv_route_output_t { enum value { off, osc, vflt, vamp, count }; };
struct cv_route_vamp_output_t { enum value { lvl, pan, count }; };
struct cv_route_osc_output_t { enum value { amp, pan, anlg_pw, dsf_distance, dsf_rolloff, cent, uni_detune, uni_spread, count }; };
struct cv_route_vflt_output_t { enum value { stvar_freq, stvar_res, stvar_kbd, comb_dly_plus, comb_gain_plus, comb_dly_min, comb_gain_min, count }; };
struct cv_route_param_t { enum value { plot_tgt, plot_time, in1, out1, amt1, in2, out2, amt2, in3, out3, amt3, in4, out4, amt4, in5, out5, amt5, in6, out6, amt6, in7, out7, amt7, in8, out8, amt8, in9, out9, amt9, in10, out10, amt10, in11, out11, amt11, in12, out12, amt12, in13, out13, amt13, in14, out14, amt14, in15, out15, amt15, count }; };

typedef cv_route_param_t::value cv_route_param;
typedef cv_route_input_t::value cv_route_input;
typedef cv_route_input_op_t::value cv_route_input_op;
typedef cv_route_output_t::value cv_route_output;
typedef cv_route_osc_output_t::value cv_route_osc_output;
typedef cv_route_vamp_output_t::value cv_route_vamp_output;
typedef cv_route_vflt_output_t::value cv_route_vflt_output;

inline std::int32_t constexpr cv_route_part_mapping[cv_route_output_t::count] = { -1, part_type::oscillator, part_type::voice_filter, part_type::voice_amp };
inline std::int32_t constexpr cv_route_vamp_mapping[cv_route_vamp_output::count] = { voice_amp_param::level, voice_amp_param::pan };
inline std::int32_t constexpr cv_route_osc_mapping[cv_route_osc_output::count] = { oscillator_param::amp, oscillator_param::pan, oscillator_param::analog_pw, oscillator_param::dsf_distance, oscillator_param::dsf_rolloff, oscillator_param::cent, oscillator_param::unison_detune, oscillator_param::unison_spread };
inline std::int32_t constexpr cv_route_vflt_mapping[cv_route_vflt_output::count] = { voice_filter_param::stvar_freq, voice_filter_param::stvar_res, voice_filter_param::stvar_kbd, voice_filter_param::comb_dly_plus, voice_filter_param::comb_gain_plus, voice_filter_param::comb_dly_min, voice_filter_param::comb_gain_min };
inline std::int32_t const* const cv_route_param_mapping[cv_route_output_t::count] = { nullptr, cv_route_osc_mapping , cv_route_vflt_mapping, cv_route_vamp_mapping };

inline std::int32_t constexpr cv_route_count = 15;
inline std::int32_t constexpr cv_route_param_offset = 2; // for plot params
inline std::int32_t constexpr cv_input_counts[cv_route_input::count] = { 1, envelope_count, voice_lfo_count };
inline std::int32_t constexpr cv_input_op_counts[cv_route_input::count] = { 0, cv_route_input_op::count, cv_route_input_op::count };
inline std::int32_t constexpr cv_inputs_count = 1 + envelope_count * cv_route_input_op::count + voice_lfo_count * cv_route_input_op::count;
inline std::int32_t constexpr cv_output_counts[cv_route_output::count] = { 1, oscillator_count, voice_filter_count, 1 };
inline std::int32_t constexpr cv_output_modulated_counts[cv_route_output::count] = { 0, oscillator_param::count, voice_filter_param::count, voice_amp_param::count };
inline std::int32_t constexpr cv_output_target_counts[cv_route_output::count] = { 0, cv_route_osc_output::count, cv_route_vflt_output::count, cv_route_vamp_output::count };
inline std::int32_t constexpr cv_outputs_count = 1 + cv_route_vamp_output_t::count + cv_route_osc_output::count * oscillator_count + cv_route_vflt_output::count * voice_filter_count;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP