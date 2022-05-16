#include <svn/support/topo.hpp>
#include <vector>

namespace svn {

param::
param(char const* name, char const* detail, bool default_):
type(param_type::toggle), name(name), detail(detail), bounds()
{ bounds.toggle = { default_ }; }

param::
param(char const* name, char const* detail, float default_):
type(param_type::toggle), name(name), detail(detail), bounds()
{ bounds.real = { default_ }; }

param::
param(char const* name, char const* detail, item const* items, std::int32_t count):
type(param_type::list), name(name), detail(detail), bounds()
{ bounds.list = { items, count }; }

param::
param(char const* name, char const* detail, std::int32_t min, std::int32_t max, std::int32_t default_):
type(param_type::list), name(name), detail(detail), bounds()
{ bounds.discrete = { min, max, default_ }; }

namespace topo_detail {

static init_t init;
static std::vector<topo_t> topo_;
static std::vector<std::int32_t> topo_bounds_;

init_t::
init_t()
{
  std::int32_t bound = 0;
  std::int32_t previous_type = -1;
  for(std::int32_t t = 0; t < part_type::count; t++)
  {
    if(previous_type != t)
      topo_bounds_.push_back(bound);
    previous_type = t;
    for(std::int32_t c = 0; c < parts[t].count; c++)
      for(std::int32_t p = 0; p < parts[t].param_count; p++)
        topo_.push_back({ &parts[t], &parts[t].params[p] });
    ++bound;
  }
  topo = topo_.data();
  topo_bounds = topo_bounds_.data();
}

} // namespace topo_detail

topo_t const* topo;
std::int32_t const* topo_bounds;

static item const
filter_types[filter_type::count] =
{
  { "Lddr", "Ladder" },
  { "StVar", "State variable" }
};

static item const
unit_types[unit_type::count] =
{
  { "Blep", "Bandlimited step" },
  { "Blamp", "Bandlimited ramp" },
  { "Dsf", "Discrete summation formulae" }
};

param const
unit_params[unit_param::count] =
{
  { "On", "Enabled", false },
  { "Type", "Type", unit_types, unit_type::count },
  { "Lvl", "Level", 1.0f },
  { "Dtn", "Detune", - 50, 50, 0 },
  { "Pan", "Panning", 0.5f },
  { "Pw", "Pulse width", 1.0f }
};

param const
filter_params[filter_param::count] =
{
  { "On", "Enabled", false },
  { "Type", "Type", filter_types, filter_type::count },
  { "Frq", "Frequency", 0.5f },
  { "Res", "Resonance", 0.0f }
};

part const
parts[] =
{
  { "Unit", "Unit generator", unit_count, unit_params, unit_param::count },
  { "Filter", "Filter", filter_count, filter_params, filter_param::count }
};

} // namespace svn