#include <svn/support/topo_static.hpp>

namespace svn {

param_info::
param_info(item_info item, float default_):
type(param_type::real), item(item), bounds()
{ bounds.real = { default_ }; }

param_info::
param_info(item_info item, std::int32_t default_):
type(param_type::toggle), item(item), bounds()
{ bounds.toggle = { default_ }; }

param_info::
param_info(item_info item, item_info const* items, std::int32_t count):
type(param_type::list), item(item), bounds()
{ bounds.list = { items, count }; }

param_info::
param_info(item_info item, std::int32_t min, std::int32_t max, std::int32_t default_):
type(param_type::discrete), item(item), bounds()
{ bounds.discrete = { min, max, default_ }; }

static item_info const
filter_types[filter_type::count] =
{
  { "Lddr", "Ladder" },
  { "StVar", "State variable" }
};

static item_info const
unit_types[unit_type::count] =
{
  { "Blep", "Bandlimited step" },
  { "Blamp", "Bandlimited ramp" },
  { "Dsf", "Discrete summation formulae" }
};

param_info const
unit_params[unit_param::count] =
{
  { { "On", "Enabled" }, 0 },
  { { "Type", "Type" }, unit_types, unit_type::count },
  { { "Lvl", "Level" }, 1.0f },
  { { "Dtn", "Detune" }, - 50, 50, 0 },
  { { "Pan", "Panning" }, 0.5f },
  { { "Pw", "Pulse width" }, 1.0f }
};

param_info const
filter_params[filter_param::count] =
{
  { { "On", "Enabled" }, 0 },
  { { "Type", "Type" }, filter_types, filter_type::count },
  { { "Frq", "Frequency" }, 0.5f },
  { { "Res", "Resonance" }, 0.0f }
};

part_info const
part_infos[] =
{
  { { "Unit", "Unit generator" }, unit_count, unit_params, unit_param::count },
  { { "Filter", "Filter" }, filter_count, filter_params, filter_param::count }
};

} // namespace svn