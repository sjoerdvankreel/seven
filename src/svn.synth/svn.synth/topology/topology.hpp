#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>

namespace svn::synth {

inline std::int32_t constexpr oscillator_count = 2;
inline std::int32_t constexpr oscillator_max_spread = 8;
inline std::int32_t constexpr synth_polyphony = 32;
inline std::int32_t constexpr synth_max_ui_height = 260; 

extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const output_params[];
extern base::param_descriptor const amplitude_params[];
extern base::param_descriptor const oscillator_params[];

struct part_type_t { enum value { oscillator, amplitude, output, count }; };
struct output_param_t { enum value { clip, drain, voices, count }; };
struct amplitude_param_t { enum value { amp, decay, count }; };
struct oscillator_type_t { enum value { analog, dsf, count }; };
struct oscillator_anlg_type_t { enum value { sin, saw, pulse, tri, count }; };
struct oscillator_param_t { enum value { on, amp, pan, type, anlg_type, anlg_detune, anlg_spread, anlg_pw, dsf_partials, dsf_distance, dsf_rolloff, oct, note, cent, count }; };

typedef part_type_t::value part_type;
typedef output_param_t::value output_param;
typedef amplitude_param_t::value amplitude_param;
typedef oscillator_param_t::value oscillator_param;
typedef oscillator_type_t::value oscillator_type;
typedef oscillator_anlg_type_t::value oscillator_anlg_type;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP