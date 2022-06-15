#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>

namespace svn::synth {

inline std::int32_t constexpr oscillator_count = 2;
inline std::int32_t constexpr synth_polyphony = 32;  
inline std::int32_t constexpr synth_max_ui_height = 160; 

extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const oscillator_params[];
extern base::param_descriptor const voice_amp_params[];
extern base::param_descriptor const glob_output_params[];

struct part_type_t { enum value { oscillator, voice_amp, glob_output, count }; };
struct glob_output_param_t { enum value { clip, drain, voices, count }; };
struct voice_amp_param_t { enum value { amp, decay, count }; };
struct oscillator_type_t { enum value { sine, blep, blmp, count }; };
struct oscillator_blep_type_t { enum value { saw, pulse, triangle, count }; };
struct oscillator_blmp_type_t { enum value { saw, pulse, count }; };
struct oscillator_param_t { enum value { on, type, blep_type, blmp_type, pan, oct, note, cent, count }; };

typedef part_type_t::value part_type;
typedef oscillator_type_t::value oscillator_type;
typedef voice_amp_param_t::value voice_amp_param;
typedef oscillator_param_t::value oscillator_param;
typedef oscillator_blep_type_t::value oscillator_blep_type;
typedef oscillator_blmp_type_t::value oscillator_blmp_type;
typedef glob_output_param_t::value glob_output_param;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP