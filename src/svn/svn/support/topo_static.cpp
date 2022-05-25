#include <svn/support/topo_static.hpp>
#include <cassert>

namespace svn {

param_info::
param_info(item_info item, bool default_):
type(param_type::toggle), item(item), unit(L""), 
min(), max(), default_(), items(nullptr), real()
{
  min.discrete = 0;
  max.discrete = 1;
  this->default_.discrete = default_;
}

param_info::
param_info(item_info item, item_info const* items, std::int32_t count):
type(param_type::list), item(item), unit(L""),
min(), max(), default_(), items(items), real()
{
  assert(count > 0);
  assert(items != nullptr);

  min.discrete = 0;
  max.discrete = count - 1;
  this->default_.discrete = 0;
}

param_info::
param_info(item_info item, wchar_t const* unit, double default_, real_info const& real):
type(param_type::real), item(item), unit(unit),
min(), max(), default_(), items(nullptr), real(real)
{
  assert(unit != nullptr);
  assert(real.min < real.max);
  assert(real.display_min < real.display_max);
  assert(0.0 <= default_ && default_ <= 1.0);
  assert(0 <= real.slope && real.slope <= param_slope::count);

  min.real = 0.0;
  max.real = 1.0;
  this->default_.real = default_;
}

param_info::
param_info(item_info item, wchar_t const* unit, std::int32_t default_, std::int32_t min, std::int32_t max):
type(param_type::discrete), item(item), unit(unit),
min(), max(), default_(), items(nullptr), real()
{
  assert(unit != nullptr);
  assert(min <= default_ && default_ <= max);

  this->min.discrete = min;
  this->max.discrete = max;
  this->default_.discrete = default_;
}

static item_info const
filter_types[filter_type::count] =
{
  { L"Lddr", L"Ladder" },
  { L"StVar", L"State variable" }
};

static item_info const
unit_types[unit_type::count] =
{
  { L"Blep", L"Bandlimited step" },
  { L"Blamp", L"Bandlimited ramp" },
  { L"Dsf", L"Discrete summation formulae" }
};

part_info const
part_infos[] =
{
  { { L"Unit", L"Unit generator" }, unit_count, unit_params, unit_param::count },
  { { L"Filter", L"Filter" }, filter_count, filter_params, filter_param::count }
};

param_info const
unit_params[unit_param::count] =
{
  { { L"On", L"Enabled" }, false },
  { { L"Type", L"Type" }, unit_types, unit_type::count },
  { { L"Lvl", L"Level" }, L"dB", 1.0, { param_slope::linear, 0.0, 1.0, 0.0, 1.0 } },
  { { L"Dtn", L"Detune" }, L"Cent", 0, -50, 50 },
  { { L"Dt2", L"Detune2" }, L"Cent", 0.5, { param_slope::linear, -0.5, 0.5, -50.0, 50.0 } },
  { { L"Pan", L"Panning" }, L"%", 0.5, { param_slope::linear, 0.0, 1.0, -100.0, 100.0 } },
  { { L"Pw", L"Pulse width" }, L"%", 1.0, { param_slope::linear, 0.0, 0.5, 0.0, 100.0 } }
};

param_info const
filter_params[filter_param::count] =
{
  { { L"On", L"Enabled" }, false },
  { { L"Type", L"Type" }, filter_types, filter_type::count },
  { { L"Frq", L"Frequency" }, L"Hz", 0.5, { param_slope::quadratic, 20.0, 20000.0, 20.0, 20000.0 } },
  { { L"Res", L"Resonance" }, L"", 0.0, { param_slope::linear, 0.0, 1.0, 0.0, 1.0 } }
};

} // namespace svn