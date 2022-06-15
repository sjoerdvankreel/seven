#ifndef SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP
#define SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP

#include <svn.base/topology/topology_info.hpp>

namespace svn::synth {

inline std::int32_t constexpr voice_osc_count = 2;
inline std::int32_t constexpr synth_polyphony = 32;  
inline std::int32_t constexpr synth_max_ui_height = 160; 

extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const voice_osc_params[];
extern base::param_descriptor const voice_amp_params[];
extern base::param_descriptor const glob_output_params[];

struct part_type_t { enum value { voice_osc, voice_amp, glob_output, count }; };
struct glob_output_param_t { enum value { clip, drain, voices, count }; };
struct voice_amp_param_t { enum value { amp, decay, count }; };
struct voice_osc_type_t { enum value { sine, blep, blmp, count }; };
struct voice_osc_blep_type_t { enum value { saw, pulse, triangle, count }; };
struct voice_osc_blmp_type_t { enum value { saw, pulse, count }; };
struct voice_osc_param_t { enum value { on, type, blep_type, blmp_type, pan, oct, note, cent, count }; };

typedef part_type_t::value part_type;
typedef voice_osc_type_t::value voice_osc_type;
typedef voice_amp_param_t::value voice_amp_param;
typedef voice_osc_param_t::value voice_osc_param;
typedef voice_osc_blep_type_t::value voice_osc_blep_type;
typedef voice_osc_blmp_type_t::value voice_osc_blmp_type;
typedef glob_output_param_t::value glob_output_param;

} // namespace svn::synth
#endif // SVN_SYNTH_TOPOLOGY_TOPOLOGY_HPP