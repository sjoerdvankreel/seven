#ifndef SVN_SUPPORT_TOPO_STATIC_HPP
#define SVN_SUPPORT_TOPO_STATIC_HPP

#include <svn/support/param_value.hpp>
#include <cmath>
#include <limits>
#include <cstdint>

namespace svn {

extern part_info const part_infos[];
extern param_info const unit_params[];
extern param_info const filter_params[];

inline std::int32_t constexpr unit_count = 2;
inline std::int32_t constexpr filter_count = 1;

struct part_type_t { enum value { unit, filter, count }; };
typedef part_type_t::value part_type;

struct unit_type_t { enum value { blep, blamp, dsf, count }; };
struct filter_type_t { enum value { ladder, state_variable, count }; };
typedef unit_type_t::value unit_type;
typedef filter_type_t::value filter_type;

struct filter_param_t { enum value { on, type, frequency, resonance, count }; };
struct unit_param_t { enum value { on, type, level, detune, detune2, panning, pw, count }; };
typedef unit_param_t::value unit_param;
typedef filter_param_t::value filter_param;

} // namespace svn
#endif // SVN_SUPPORT_TOPO_STATIC_HPP