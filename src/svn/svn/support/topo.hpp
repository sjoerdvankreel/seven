#ifndef SVN_SUPPORT_TOPO_HPP
#define SVN_SUPPORT_TOPO_HPP

#include <cstdint>

namespace svn {

namespace topo_detail {

struct init_t 
{
  init_t();
  static init_t init;
};

} // namespace topo_detail;

struct item
{
  char const* name;
  char const* detail;
};

struct topo_t
{
  struct part const* part;
  struct param const* param;
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
    struct { item const* items; std::int32_t count; } list;
    struct { std::int32_t min; std::int32_t max; std::int32_t default_; } discrete;
  } bounds;

  param(char const* name, char const* detail, bool default_);
  param(char const* name, char const* detail, float default_);
  param(char const* name, char const* detail, item const* items, std::int32_t count);
  param(char const* name, char const* detail, std::int32_t min, std::int32_t max, std::int32_t default_);
};

inline std::int32_t constexpr unit_count = 2;
inline std::int32_t constexpr filter_count = 1;

extern part const parts[];
extern param const unit_params[];
extern param const filter_params[]; 

extern topo_t const* topo;
extern std::int32_t const* topo_bounds;

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
#endif // SVN_SUPPORT_TOPO_HPP