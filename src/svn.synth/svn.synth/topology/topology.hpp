#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>

namespace svn::synth {

inline float constexpr filter_min_freq = 20.0f;
inline float constexpr filter_max_freq = 20000.0f;
inline float constexpr comb_filter_max_ms = 5.0f;
inline float constexpr max_sample_rate = 384000.0f;

inline std::int32_t constexpr envelope_count = 3;
inline std::int32_t constexpr oscillator_count = 4;
inline std::int32_t constexpr voice_filter_count = 3;
inline std::int32_t constexpr oscillator_max_spread = 8;
inline std::int32_t constexpr synth_polyphony = 32;
inline std::int32_t constexpr synth_max_ui_height = 480; 
inline std::int32_t constexpr audio_route_count = 9;

inline std::int32_t constexpr audio_inputs_count = oscillator_count + voice_filter_count + 1 /* off */;
inline std::int32_t constexpr audio_outputs_count = voice_filter_count + 1 /* off */ + 1 /* amp */;
inline std::int32_t constexpr audio_input_counts[audio_inputs_count] = { 1, oscillator_count, voice_filter_count };
inline std::int32_t constexpr audio_output_counts[audio_outputs_count] = { 1, voice_filter_count, 1 };

extern base::part_descriptor const part_descriptors[];

struct envelope_type_t { enum value { dahdsr, dahdr, count }; };
struct envelope_slope_t { enum value { lin, log, quad, sqrt, count }; };
struct envelope_param_t { enum value { on, type, sync, delay_time, delay_sync, hold_time, hold_sync, sustain_level, attack_time, attack_sync, attack_slope, attack_mid, decay_time, decay_sync, decay_slope, decay_mid, release_time, release_sync, release_slope, release_mid, count }; };

struct voice_filter_type_t { enum value { state_var, comb, count }; };
struct voice_filter_stvar_type_t { enum value { lpf, hpf, bpf, bsf, count }; };
struct voice_filter_param_t { enum value { on, type, stvar_type, stvar_freq, stvar_res, stvar_kbd, comb_dly_plus, comb_gain_plus, comb_dly_min, comb_gain_min, count }; };

struct oscillator_type_t { enum value { analog, dsf, count }; };
struct oscillator_graph_t { enum value { wave, spectrum, count }; };
struct oscillator_anlg_type_t { enum value { sin, saw, pulse, tri, count }; };
struct oscillator_param_t { enum value { on, amp, pan, type, anlg_type, anlg_spread, anlg_detune, anlg_pw, dsf_partials, dsf_distance, dsf_rolloff, oct, note, cent, count }; };

struct voice_amp_param_t { enum value { level, decay, count }; };
struct output_param_t { enum value { clip, drain, voices, count }; };
struct part_type_t { enum value { oscillator, envelope, voice_amp, voice_filter, audio_route, output, count }; };

struct audio_route_input_t { enum value { off, osc, filter, count }; };
struct audio_route_output_t { enum value { off, filter, amp, count }; };
struct audio_route_param_t { enum value { in1, out1, amt1, in2, out2, amt2, in3, out3, amt3, in4, out4, amt4, in5, out5, amt5, in6, out6, amt6, in7, out7, amt7, in8, out8, amt8, in9, out9, amt9, count }; };

typedef part_type_t::value part_type;
typedef output_param_t::value output_param;
typedef voice_amp_param_t::value voice_amp_param;

typedef envelope_type_t::value envelope_type;
typedef envelope_slope_t::value envelope_slope;
typedef envelope_param_t::value envelope_param;

typedef audio_route_param_t::value audio_route_param;
typedef audio_route_input_t::value audio_route_input;
typedef audio_route_output_t::value audio_route_output;

typedef oscillator_type_t::value oscillator_type;
typedef oscillator_param_t::value oscillator_param;
typedef oscillator_graph_t::value oscillator_graph;
typedef oscillator_anlg_type_t::value oscillator_anlg_type;

typedef voice_filter_type_t::value voice_filter_type;
typedef voice_filter_param_t::value voice_filter_param;
typedef voice_filter_stvar_type_t::value voice_filter_stvar_type;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP