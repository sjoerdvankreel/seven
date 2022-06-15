#include <svn.base/static/param_descriptor.hpp>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace svn::base {

param_value 
param_descriptor::to_display(param_value val) const
{
  if(type != param_type::real) return param_value(val.discrete);
  return param_value(real.display.to_range(val.real));
}

param_value 
param_descriptor::from_display(param_value val) const
{
  if (type != param_type::real) return param_value(val.discrete);
  return param_value(real.display.from_range(val.real));
}

bool 
param_descriptor::parse(wchar_t const* buffer, param_value& val) const
{
  std::wstringstream str(buffer);
  float inf = std::numeric_limits<float>::infinity();
  switch (type)
  {
  case param_type::knob:
  case param_type::text:
    str >> val.discrete;
    if (val.discrete < discrete.min) return false;
    if (val.discrete > discrete.max) return false;
    return true;
  case param_type::list:
  case param_type::knob_list:
    for (std::int32_t i = 0; i <= discrete.max; i++)
      if (!std::wcscmp(discrete.items[i].short_, buffer))
        return val.discrete = i, true;
    return false;
  case param_type::real:
    str >> val.real;
    if (!std::wcscmp(L"-inf", buffer)) val.real = -inf;
    if (val.real < real.display.min) return false;
    if (val.real > real.display.max) return false;
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
  case param_type::knob: case param_type::text: stream << val.discrete; break;
  case param_type::real: stream << std::setprecision(real.precision) << std::fixed << val.real; break;
  case param_type::list: case param_type::knob_list: stream << discrete.items[val.discrete].short_; break;
  default: assert(false); break;
  }
  std::wstring str = stream.str();
  if(buffer == nullptr || size == 0) return str.length() + 1;
  std::memset(buffer, 0, size * sizeof(wchar_t));
  std::wcsncpy(buffer, str.c_str(), size - 1);
  return str.length() + 1;
}
 
} // namespace svn::base