#ifndef SVN_SUPPORT_TOPOLOGY_HPP
#define SVN_SUPPORT_TOPOLOGY_HPP

#include <cstdint>

namespace svn {

struct param_item
{
  char const* name;
  char const* detail;
};

struct part
{
  char const* name;
  char const* detail;
  std::int32_t count;
  struct param const* params;
  std::int32_t param_count;
};

struct param
{
  std::int32_t type;
  char const* name;
  char const* detail;
  union
  {
    struct { float default_; } real;
    struct { bool default_; } toggle;
    struct { param_item const* items; std::int32_t count; } list;
    struct { std::int32_t min; std::int32_t max; std::int32_t default_; } discrete;
  } bounds;

  param(char const* name, char const* detail, bool default_);
  param(char const* name, char const* detail, float default_);
  param(char const* name, char const* detail, param_item const* items, std::int32_t count);
  param(char const* name, char const* detail, std::int32_t min, std::int32_t max, std::int32_t default_);
};

inline std::int32_t constexpr unit_count = 2;
inline std::int32_t constexpr filter_count = 1;

extern part const parts[];
extern param const unit_params[];
extern param const filter_params[]; 

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
#endif // SVN_SUPPORT_TOPOLOGY_HPP