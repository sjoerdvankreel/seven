#include <svn.base/static/param_descriptor.hpp>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace svn::base {

param_descriptor::
param_descriptor(        
  item_name const& static_name, bool ui_edit_font_small, 
  std::int32_t ui_param_group, param_type type, 
  item_name const* list, std::int32_t count):
type(type), unit(L""), static_name(static_name), 
ui_edit_font_small(ui_edit_font_small), 
ui_param_group(ui_param_group), list(list),
dsp(param_bounds::none()), display(param_bounds::none()),
min(0), max(count - 1), default_(0), precision(0)
{ 
  assert(count > 0);
  assert(list != nullptr);
  assert(0 <= ui_param_group && ui_param_group < 3);
  assert(type == param_type::list || type == param_type::discrete_list);
}

param_descriptor::
param_descriptor(   
  item_name const& static_name, bool ui_edit_font_small, 
  std::int32_t ui_param_group, wchar_t const* unit, 
  float default_, std::int32_t precision,
  param_bounds const& dsp, param_bounds const& display):
type(param_type::real), unit(unit), static_name(static_name), 
ui_edit_font_small(ui_edit_font_small), 
ui_param_group(ui_param_group), list(nullptr),
dsp(dsp), display(display), min(0.0f), max(1.0f), 
default_(default_), precision(precision)
{           
  assert(precision >= 0);
  assert(unit != nullptr);
  assert(dsp.min < dsp.max);
  assert(display.min < display.max);
  assert(0.0 <= default_ && default_ <= 1.0);
  assert(0 <= ui_param_group && ui_param_group < 3); 
  assert(0 <= dsp.slope && dsp.slope <= param_slope::count);
  assert(0 <= display.slope && display.slope <= param_slope::count);
} 

param_descriptor::   
param_descriptor(
  item_name const& static_name, bool ui_edit_font_small, 
  std::int32_t ui_param_group, bool default_):
type(param_type::toggle), unit(L""), static_name(static_name), 
ui_edit_font_small(ui_edit_font_small), 
ui_param_group(ui_param_group), list(nullptr),
dsp(param_bounds::none()), display(param_bounds::none()), 
min(0), max(1), default_(default_? 1: 0), precision(0)
{ assert(0 <= ui_param_group && ui_param_group < 3); }

param_descriptor::
param_descriptor( 
  item_name const& static_name, bool ui_edit_font_small, 
  std::int32_t ui_param_group, param_type type, wchar_t const* unit,
  std::int32_t default_, std::int32_t min, std::int32_t max):
type(type), unit(unit), static_name(static_name), 
ui_edit_font_small(ui_edit_font_small), 
ui_param_group(ui_param_group), list(nullptr),
dsp(param_bounds::none()), display(param_bounds::none()),
min(min), max(max), default_(default_), precision(0)
{
  assert(unit != nullptr);
  assert(min <= default_ && default_ <= max);
  assert(0 <= ui_param_group && ui_param_group < 3);
  assert(type == param_type::discrete || type == param_type::discrete_text);
}

param_value 
param_descriptor::to_display(param_value val) const
{
  if(type != param_type::real) return param_value(val.discrete);
  return param_value(display.to_range(val.real));
}

param_value 
param_descriptor::from_display(param_value val) const
{
  if (type != param_type::real) return param_value(val.discrete);
  return param_value(display.from_range(val.real));
}

bool 
param_descriptor::parse(wchar_t const* buffer, param_value& val) const
{
  std::wstringstream str(buffer);
  float inf = std::numeric_limits<float>::infinity();
  switch (type)
  {
  case param_type::discrete:
  case param_type::discrete_text:
    str >> val.discrete;
    if (val.discrete < min.discrete) return false;
    if (val.discrete > max.discrete) return false;
    return true;
  case param_type::list:
  case param_type::discrete_list:
    for (std::int32_t i = 0; i <= max.discrete; i++)
      if (!std::wcscmp(list[i].short_, buffer))
        return val.discrete = i, true;
    return false;
  case param_type::real:
    str >> val.real;
    if (!std::wcscmp(L"-inf", buffer)) val.real = -inf;
    if (val.real < display.min) return false;
    if (val.real > display.max) return false;
    return true;
  case param_type::toggle:
    if (!std::wcscmp(L"On", buffer)) val.discrete = 1;
    else if (!std::wcscmp(L"Off", buffer)) val.discrete = 0;
    else return false;
    return true;
  default:
    assert(false);
    return false;
  }
}

std::size_t 
param_descriptor::format(param_value val, wchar_t* buffer, std::size_t size) const
{
  std::wstringstream stream;
  switch (type)
  { 
  case param_type::toggle: stream << (val.discrete == 0 ? L"Off" : L"On"); break;
  case param_type::discrete: case param_type::discrete_text: stream << val.discrete; break;
  case param_type::real: stream << std::setprecision(precision) << std::fixed << val.real; break;
  case param_type::list: case param_type::discrete_list: stream << list[val.discrete].short_; break;
  default: assert(false); break;
  }
  std::wstring str = stream.str();
  if(buffer == nullptr || size == 0) return str.length() + 1;
  std::memset(buffer, 0, size * sizeof(wchar_t));
  std::wcsncpy(buffer, str.c_str(), size - 1);
  return str.length() + 1;
}
 
} // namespace svn::base