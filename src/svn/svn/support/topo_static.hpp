#ifndef SVN_SUPPORT_TOPO_STATIC_HPP
#define SVN_SUPPORT_TOPO_STATIC_HPP

#include <cstdint>

namespace svn {

struct item_info
{
  char const* name;
  char const* detail;
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
  union
  {
    struct { float default_; } real;
    struct { std::int32_t default_; } toggle;
    struct { item_info const* items; std::int32_t count; } list;
    struct { std::int32_t min; std::int32_t max; std::int32_t default_; } discrete;
  } bounds;

  param_info(item_info item, float default_);
  param_info(item_info item, std::int32_t default_);
  param_info(item_info item, item_info const* items, std::int32_t count);
  param_info(item_info item, std::int32_t min, std::int32_t max, std::int32_t default_);
};

extern part_info const part_infos[];
extern param_info const unit_params[];
extern param_info const filter_params[];

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
struct unit_param_t { enum value { on, type, level, detune, panning, pw, count }; };
typedef unit_param_t::value unit_param;
typedef filter_param_t::value filter_param;

} // namespace svn
#endif // SVN_SUPPORT_TOPO_STATIC_HPP