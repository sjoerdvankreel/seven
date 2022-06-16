#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>

namespace svn::synth {

inline std::int32_t constexpr oscillator_count = 2;
inline std::int32_t constexpr synth_polyphony = 32;  
inline std::int32_t constexpr synth_max_ui_height = 400; 

extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const output_params[];
extern base::param_descriptor const amplitude_params[];
extern base::param_descriptor const oscillator_params[];

struct part_type_t { enum value { oscillator, amplitude, output, count }; };
struct output_param_t { enum value { clip, drain, voices, count }; };
struct amplitude_param_t { enum value { amp, decay, count }; };
struct oscillator_type_t { enum value { sine, blep, blmp, count }; };
struct oscillator_blep_type_t { enum value { saw, pulse, triangle, count }; };
struct oscillator_blmp_type_t { enum value { saw, pulse, count }; };
struct oscillator_param_t { enum value { on, type, blep_type, blmp_type, pan, oct, note, cent, fake1, fake2, fake3, fake4, count }; };

typedef part_type_t::value part_type;
typedef output_param_t::value output_param;
typedef amplitude_param_t::value amplitude_param;
typedef oscillator_param_t::value oscillator_param;
typedef oscillator_type_t::value oscillator_type;
typedef oscillator_blep_type_t::value oscillator_blep_type;
typedef oscillator_blmp_type_t::value oscillator_blmp_type;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP