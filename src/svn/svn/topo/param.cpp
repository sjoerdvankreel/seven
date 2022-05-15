#include <svn/topo/param.hpp>

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
param(char const* name, char const* detail, param_item const* items, std::int32_t count):
type(param_type::list), name(name), detail(detail), bounds()
{ bounds.list = { items, count }; }

param::
param(char const* name, char const* detail, std::int32_t min, std::int32_t max, std::int32_t default_):
type(param_type::list), name(name), detail(detail), bounds()
{ bounds.discrete = { min, max, default_ }; }

static param_item const
filter_types[filter_type::count] =
{
  { "Lddr", "Ladder" },
  { "StVar", "State variable" }
};

static param_item const
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

} // namespace svn