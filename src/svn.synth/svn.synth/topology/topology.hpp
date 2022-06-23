#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>

namespace svn::synth {

inline std::int32_t constexpr oscillator_count = 3;
inline std::int32_t constexpr voice_filter_count = 2;
inline std::int32_t constexpr oscillator_max_spread = 8;
inline std::int32_t constexpr synth_polyphony = 32;
inline std::int32_t constexpr synth_max_ui_height = 400; 

extern base::part_descriptor const part_descriptors[];

struct amplitude_param_t { enum value { amp, decay, count }; };
struct output_param_t { enum value { clip, drain, voices, count }; };
struct part_type_t { enum value { oscillator, voice_filter, amplitude, output, count }; };

struct oscillator_type_t { enum value { analog, dsf, count }; };
struct oscillator_graph_t { enum value { wave, spectrum, count }; };
struct oscillator_anlg_type_t { enum value { sin, saw, pulse, tri, count }; };
struct oscillator_param_t { enum value { on, amp, pan, type, anlg_type, anlg_detune, anlg_spread, anlg_pw, dsf_partials, dsf_distance, dsf_rolloff, oct, note, cent, count }; };

struct voice_filter_type_t { enum value { state_var, comb, count }; };
struct voice_filter_stvar_type_t { enum value { lpf, hpf, bpf, bsf, count }; };
struct voice_filter_param_t { enum value { on, type, stvar_type, stvar_freq, stvar_res, stvar_kbd, comb_dly_plus, comb_gain_plus, comb_dly_min, comb_gain_min, unit1, unit2, unit3, count }; };

typedef part_type_t::value part_type;
typedef output_param_t::value output_param;
typedef amplitude_param_t::value amplitude_param;

typedef oscillator_type_t::value oscillator_type;
typedef oscillator_param_t::value oscillator_param;
typedef oscillator_graph_t::value oscillator_graph;
typedef oscillator_anlg_type_t::value oscillator_anlg_type;

typedef voice_filter_type_t::value voice_filter_type;
typedef voice_filter_param_t::value voice_filter_param;
typedef voice_filter_stvar_type_t::value voice_filter_stvar_type;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP