#ifndef SVN_TOPO_PARAM_HPP
#define SVN_TOPO_PARAM_HPP

#include <cstdint>

namespace svn {

struct param_item
{
  char const* name;
  char const* detail;
};

struct param_type_t { enum value { real, list, toggle, discrete, count }; };
struct filter_param_t { enum value { on, type, frequency, resonance, count }; };
struct unit_param_t { enum value { on, type, level, detune, panning, pw, count }; };

typedef param_type_t::value param_type;
typedef unit_param_t::value unit_param;
typedef filter_param_t::value filter_param;

struct unit_type_t { enum value { blep, blamp, dsf, count }; };
struct filter_type_t { enum value { ladder, state_variable, count }; };

typedef unit_type_t::value unit_type;
typedef filter_type_t::value filter_type;

struct param
{
  param_type type;
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

extern param const unit_params[unit_param::count];
extern param const filter_params[filter_param::count];

} // namespace svn
#endif // SVN_TOPO_PARAM_HPP