#ifndef SVN_SYNTH_STATIC_TOPOLOGY_HPP
#define SVN_SYNTH_STATIC_TOPOLOGY_HPP

#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>

namespace svn::synth {

inline std::int32_t constexpr filter_count = 1;
inline std::int32_t constexpr oscillator_count = 2;
inline std::int32_t constexpr synth_polyphony = 32;

extern base::param_descriptor const output_params[];
extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const filter_params[];
extern base::param_descriptor const oscillator_params[];

struct part_type_t { enum value { oscillator, filter, count }; };
struct oscillator_type_t { enum value { blep, blamp, dsf, count }; };
struct filter_type_t { enum value { ladder, state_variable, count }; };
struct filter_param_t { enum value { on, type, frequency, resonance, count }; };
struct oscillator_param_t { enum value { on, type, level, detune, detune2, panning, pw, decay, count }; };
struct output_param_t { enum value { clip, exhausted, voices, count }; };

typedef part_type_t::value part_type;
typedef filter_type_t::value filter_type;
typedef filter_param_t::value filter_param;
typedef output_param_t::value output_param;
typedef oscillator_type_t::value oscillator_type;
typedef oscillator_param_t::value oscillator_param;

} // namespace svn::synth
#endif // SVN_SYNTH_STATIC_TOPOLOGY_HPP