#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>
#include <vector>

namespace svn::synth {

// ---- global topo ----

inline std::int32_t constexpr lfo_count = 6;
inline std::int32_t constexpr filter_count = 4;
inline std::int32_t constexpr cv_route_count = 3;
inline std::int32_t constexpr envelope_count = 4;
inline std::int32_t constexpr oscillator_count = 4;
inline std::int32_t constexpr audio_route_count = 2;

extern base::part_descriptor const part_descriptors[];
inline std::int32_t constexpr synth_polyphony = 32;
inline std::int32_t constexpr synth_max_ui_height = 560;

struct part_type_t { enum value { oscillator, envelope, lfo, amplitude, filter, audio_route, cv_route, active, output, cpu, count }; };
typedef part_type_t::value part_type;
  
// ---- amp ----
 
struct amplitude_param_t { enum value { level, pan, count }; };
typedef amplitude_param_t::value amplitude_param;

// ---- output ----

struct output_param_t { enum value { clip, drain, voices, count }; };
typedef output_param_t::value output_param;

// ---- cpu ----

struct cpu_param_t { enum value { total, osc, lfo, filter, env, amp, audio, cv, aux, count }; };
typedef cpu_param_t::value cpu_param;

// ---- active ----

struct active_param_t { enum value { oscillator, lfo, filter, envelope, audio_route, cv_route, count }; };
typedef active_param_t::value active_param;

// ---- lfo ----

inline float constexpr lfo_min_period = 0.05f;
inline float constexpr lfo_max_period = 10.0f;
extern std::vector<float> const lfo_timesig_values;

struct lfo_type_t { enum value { sine, custom, random, count }; };
struct lfo_param_t { enum value { on, type, bipolar, synced, period_time, period_sync, count }; };

typedef lfo_type_t::value lfo_type;
typedef lfo_param_t::value lfo_param;

// ---- filter ----

inline float constexpr filter_min_freq = 20.0f;
inline float constexpr filter_max_freq = 20000.0f;
inline float constexpr comb_filter_max_ms = 5.0f;
inline float constexpr max_sample_rate = 384000.0f;

struct filter_type_t { enum value { state_var, comb, count }; };
struct filter_graph_t { enum value { impulse, frequency, count }; };
struct filter_state_var_type_t { enum value { lpf, hpf, bpf, bsf, apf, count }; };
struct filter_param_t { enum value { 
  on, type, 
  state_var_type, state_var_freq, state_var_res, state_var_kbd, 
  comb_dly_plus, comb_gain_plus, comb_dly_min, comb_gain_min, count }; };

typedef filter_type_t::value filter_type;
typedef filter_graph_t::value filter_graph;
typedef filter_param_t::value filter_param;
typedef filter_state_var_type_t::value filter_state_var_type;

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

// ---- envelope ----

extern std::vector<float> const env_timesig_values;
struct envelope_type_t { enum value { dahdsr, dahdr1, dahdr2, count }; };
struct envelope_slope_t { enum value { linear, quadratic, inverted, logarithmic, count }; };
struct envelope_param_t { enum value { 
  on, type, bipolar, synced, 
  delay_time, delay_sync, hold_time, hold_sync, sustain_level, 
  attack_time, attack_sync, attack_slope, attack_mid, 
  decay_time, decay_sync, decay_slope, decay_mid, 
  release_time, release_sync, release_slope, release_mid, count }; };

typedef envelope_type_t::value envelope_type;
typedef envelope_slope_t::value envelope_slope;
typedef envelope_param_t::value envelope_param;

// ---- audio route ----

struct audio_route_input_t { enum value { off, osc, filter, count }; };
struct audio_route_output_t { enum value { off, filter, amp, count }; };
struct audio_route_param_t { enum value { 
  on, in1, out1, amt1, in2, out2, amt2, in3, out3, amt3, 
  in4, out4, amt4, in5, out5, amt5, in6, out6, amt6, count }; };

typedef audio_route_param_t::value audio_route_param;
typedef audio_route_input_t::value audio_route_input;
typedef audio_route_output_t::value audio_route_output;

inline std::int32_t constexpr audio_route_route_count = 6;
inline std::int32_t constexpr audio_route_param_offset = 1; // for enabled
inline std::int32_t constexpr audio_route_output_counts[audio_route_output::count] = { 1 /* off */, filter_count, 1 /* amp */ };
inline std::int32_t constexpr audio_route_input_counts[audio_route_input::count] = { 1 /* off */, oscillator_count, filter_count };

// ---- cv route ----

struct cv_route_input_op_t { enum value { add, multiply, count }; };
struct cv_route_input_t { enum value { off, velocity, envelope, lfo, count }; };

struct cv_route_output_t { enum value { 
  off, osc, filter, amp, audio, count }; };
struct cv_route_amp_output_t { enum value { 
  lvl, pan, count }; };
struct cv_route_audio_output_t { enum value { 
  amt1, amt2, amt3, amt4, amt5, amt6, count }; };
struct cv_route_osc_output_t { enum value { 
  amp, pan, anlg_pw, 
  dsf_distance, dsf_rolloff, 
  cent, uni_detune, uni_spread, count }; };
struct cv_route_filter_output_t { enum value { 
  state_var_freq, state_var_res, state_var_kbd, 
  comb_dly_plus, comb_gain_plus, comb_dly_min, comb_gain_min, count }; };
struct cv_route_param_t { enum value { 
  on, plot_tgt, plot_time, 
  in1, out1, amt1, in2, out2, amt2, in3, out3, amt3, 
  in4, out4, amt4, in5, out5, amt5, in6, out6, amt6, count }; };

typedef cv_route_param_t::value cv_route_param;
typedef cv_route_input_t::value cv_route_input;
typedef cv_route_input_op_t::value cv_route_input_op;
typedef cv_route_output_t::value cv_route_output;
typedef cv_route_osc_output_t::value cv_route_osc_output;
typedef cv_route_amp_output_t::value cv_route_amp_output;
typedef cv_route_audio_output_t::value cv_route_audio_output;
typedef cv_route_filter_output_t::value cv_route_filter_output;

inline std::int32_t constexpr cv_route_part_mapping[cv_route_output_t::count] = { 
  -1, part_type::oscillator, part_type::filter, part_type::amplitude, part_type::audio_route };
inline std::int32_t constexpr cv_route_amp_mapping[cv_route_amp_output::count] = { 
  amplitude_param::level, amplitude_param::pan };
inline std::int32_t constexpr cv_route_audio_mapping[cv_route_audio_output::count] = { 
  audio_route_param::amt1, audio_route_param::amt2, audio_route_param::amt3, 
  audio_route_param::amt4, audio_route_param::amt5, audio_route_param::amt6 };
inline std::int32_t constexpr cv_route_osc_mapping[cv_route_osc_output::count] = { 
  oscillator_param::amp, oscillator_param::pan, oscillator_param::analog_pw, 
  oscillator_param::dsf_distance, oscillator_param::dsf_rolloff, 
  oscillator_param::cent, oscillator_param::unison_detune, oscillator_param::unison_spread };
inline std::int32_t constexpr cv_route_filter_mapping[cv_route_filter_output::count] = { filter_param::state_var_freq, filter_param::state_var_res, filter_param::state_var_kbd, filter_param::comb_dly_plus, filter_param::comb_gain_plus, filter_param::comb_dly_min, filter_param::comb_gain_min };
inline std::int32_t const* const cv_route_param_mapping[cv_route_output_t::count] = { nullptr, cv_route_osc_mapping , cv_route_filter_mapping, cv_route_amp_mapping, cv_route_audio_mapping };

inline std::int32_t constexpr cv_route_route_count = 6;
inline std::int32_t constexpr cv_route_param_offset = 3; // for enabled + plot params
inline std::int32_t constexpr cv_route_input_counts[cv_route_input::count] = { 1 /* off */, 1 /* velocity */, envelope_count, lfo_count};
inline std::int32_t constexpr cv_route_input_op_counts[cv_route_input::count] = { 0 /* off */, cv_route_input_op::count, cv_route_input_op::count, cv_route_input_op::count};
inline std::int32_t constexpr cv_route_output_counts[cv_route_output::count] = { 1 /* off */, oscillator_count, filter_count, 1 /* amp */, audio_route_count };
inline std::int32_t constexpr cv_route_output_modulated_counts[cv_route_output::count] = { 0 /* off */, oscillator_param::count, filter_param::count, amplitude_param::count, audio_route_param::count };
inline std::int32_t constexpr cv_route_output_target_counts[cv_route_output::count] = { 0 /* off */, cv_route_osc_output::count, cv_route_filter_output::count, cv_route_amp_output::count, cv_route_audio_output::count };

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP