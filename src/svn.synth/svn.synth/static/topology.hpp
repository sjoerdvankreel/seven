#ifndef SVN_SYNTH_STATIC_TOPOLOGY_HPP
#define SVN_SYNTH_STATIC_TOPOLOGY_HPP

#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>

namespace svn::synth {

inline std::int32_t constexpr voice_osc_count = 2;
inline std::int32_t constexpr synth_polyphony = 32;  
inline std::int32_t constexpr synth_max_ui_height = 160; 
inline std::int32_t constexpr synth_ui_color_cycle_step = 2;
inline std::int32_t constexpr synth_ui_color_start_index = 5;

extern std::int32_t const synth_ui_order[];
extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const voice_osc_params[];
extern base::param_descriptor const voice_amp_params[];
extern base::param_descriptor const glob_output_params[];

struct part_type_t { enum value { voice_amp, voice_osc, glob_output, count }; };
struct glob_output_param_t { enum value { clip, drain, voices, count }; };
struct voice_amp_param_t { enum value { amp, decay, count }; };
struct voice_osc_type_t { enum value { sine, blep, count }; };
struct voice_osc_param_t { enum value { on, type, pan, oct, note, cent, count }; };

typedef part_type_t::value part_type;
typedef voice_osc_type_t::value voice_osc_type;
typedef voice_amp_param_t::value voice_amp_param;
typedef voice_osc_param_t::value voice_osc_param;
typedef glob_output_param_t::value glob_output_param;

} // namespace svn::synth
#endif // SVN_SYNTH_STATIC_TOPOLOGY_HPP