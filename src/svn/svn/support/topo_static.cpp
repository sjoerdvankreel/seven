#include <svn/support/topo_static.hpp>

namespace svn {

namespace {

struct toggle_formatter:
public param_formatter
{
  void format(
    param_value value, 
    wchar_t* buffer, 
    std::size_t size) const override;
};

struct discrete_formatter:
public param_formatter
{
  void format(
    param_value value,
    wchar_t* buffer,
    std::size_t size) const override;
};

struct list_formatter:
public param_formatter
{
  item_info const* const _items;
  list_formatter(item_info const* items);

  void format(
    param_value value,
    wchar_t* buffer,
    std::size_t size) const override;
};

struct real_formatter:
public param_formatter
{
  double const _display_min;
  double const _display_max;
  real_formatter(double display_min, double display_max);

  void format(
    param_value value,
    wchar_t* buffer,
    std::size_t size) const override;
};

} // namespace anonymous

param_info::
param_info(item_info item, bool default_):
type(param_type::toggle), item(item), unit(L""), 
min(), max(), default_(), formatter(std::make_unique<toggle_formatter>())
{
  min.discrete = 0;
  max.discrete = 1;
  this->default_.discrete = default_? 1: 0;
}

param_info::
param_info(item_info item, item_info const* items, std::int32_t count):
type(param_type::list), item(item), unit(L""),
min(), max(), default_(), formatter(std::make_unique<list_formatter>(items))
{
  min.discrete = 0;
  max.discrete = count - 1;
  this->default_.discrete = 0;
}

param_info::
param_info(item_info item, wchar_t const* unit, std::int32_t min, std::int32_t max, std::int32_t default_):
type(param_type::discrete), item(item), unit(unit),
min(), max(), default_(), formatter(std::make_unique<discrete_formatter>())
{
  this->min.discrete = min;
  this->max.discrete = max;
  this->default_.discrete = default_;
}

param_info::
param_info(item_info item, wchar_t const* unit, double default_, std::unique_ptr<param_formatter>&& formatter):
type(param_type::real), item(item), unit(unit),
min(), max(), default_(), formatter(std::move(formatter))
{
  min.real = 0.0;
  max.real = 0.0;
  this->default_.real = default_;
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

param_info const
unit_params[unit_param::count] =
{
  { { L"On", L"Enabled" }, 0 },
  { { L"Type", L"Type" }, unit_types, unit_type::count },
  { { L"Lvl", L"Level" }, L"dB", 1.0, std::make_unique<real_formatter>(0.0, 1.0) },
  { { L"Dtn", L"Detune" }, L"Cent", -50, 50, 0},
  { { L"Pan", L"Panning" }, L"%", 0.5, std::make_unique<real_formatter>(-100.0, 100.0) },
  { { L"Pw", L"Pulse width" }, L"%", 1.0, std::make_unique<real_formatter>(0.0, 100.0) }
};

param_info const
filter_params[filter_param::count] =
{
  { { L"On", L"Enabled" }, 0 },
  { { L"Type", L"Type" }, filter_types, filter_type::count },
  { { L"Frq", L"Frequency" }, L"Hz", 0.5, std::make_unique<real_formatter>(20.0, 20000.0) },
  { { L"Res", L"Resonance" }, L"", 0.0, std::make_unique<real_formatter>(0.0, 1.0) }
};

part_info const
part_infos[] =
{
  { { L"Unit", L"Unit generator" }, unit_count, unit_params, unit_param::count },
  { { L"Filter", L"Filter" }, filter_count, filter_params, filter_param::count }
};

} // namespace svn