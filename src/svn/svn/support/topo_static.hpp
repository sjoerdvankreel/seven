#ifndef SVN_SUPPORT_TOPO_STATIC_HPP
#define SVN_SUPPORT_TOPO_STATIC_HPP

#include <svn/support/param_value.hpp>
#include <cmath>
#include <limits>
#include <cstdint>

namespace svn {

struct param_bounds
{
  std::int32_t slope;
  double min;
  double max;
  double exponent;
};

struct item_info
{
  wchar_t const* name;
  wchar_t const* detail;
};

struct part_info
{
  item_info item;
  std::int32_t count;
  struct param_info const* params;
  std::int32_t param_count;
};

struct param_info
{
  std::int32_t type;
  item_info item;
  wchar_t const* unit;
  param_value default_;
  param_value min;
  param_value max;
  item_info const* items;
  param_bounds dsp;
  param_bounds display;

  param_info(item_info item, bool default_);
  param_info(item_info item, item_info const* items, std::int32_t count);
  param_info(item_info item, wchar_t const* unit, std::int32_t default_, std::int32_t min, std::int32_t max);
  param_info(item_info item, wchar_t const* unit, double default_, param_bounds const& dsp, param_bounds const& display);
};

extern part_info const part_infos[];
extern param_info const unit_params[];
extern param_info const filter_params[];

struct param_slope_t { enum value { linear, logarithmic, decibel, count }; };
typedef param_slope_t::value param_slope;

inline param_bounds constexpr 
bounds_linear_unit =
{ param_slope::linear, 0.0, 1.0, };

inline param_bounds constexpr 
bounds_decibel =
{ param_slope::decibel, -std::numeric_limits<double>::infinity(), 0.0 };

inline param_bounds 
bounds_linear(double min, double max)
{ return { param_slope::linear, min, max, 0.0 }; }

inline param_bounds
bounds_logarithmic(double min, double max, double reference)
{ return { param_slope::logarithmic, min, max, std::log((reference - min) / (max - min)) / std::log(0.5) }; }

inline std::int32_t constexpr unit_count = 2;
inline std::int32_t constexpr filter_count = 1;

struct part_type_t { enum value { unit, filter, count }; };
struct param_type_t { enum value { real, list, toggle, discrete, count }; };
typedef part_type_t::value part_type;
typedef param_type_t::value param_type;

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