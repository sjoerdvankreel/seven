#include <svn/support/param_transform.hpp>
#include <cmath>
#include <cwchar>
#include <sstream>
#include <iomanip>

namespace svn {

bool 
param_parse(param_info const& info, wchar_t const* buffer, param_value& val)
{
  double real;
  std::int32_t discrete;
  std::wstringstream str(buffer);
  switch (info.type)
  {
  case param_type::toggle:
    if(!std::wcscmp(L"On", buffer)) 
      return val.discrete = 1, true;
    if(!std::wcscmp(L"Off", buffer)) 
      return val.discrete = 0, true;
    return false;
  case param_type::discrete:
    str >> discrete;
    if(info.min.discrete <= discrete && discrete <= info.max.discrete) 
      return val.discrete = discrete, true;
    return false;
  case param_type::list:
    for(std::int32_t i = 0; i <= info.max.discrete; i++)
      if(!std::wcscmp(info.items[i].detail, buffer)) 
        return val.discrete = i, true;
    return false;
  case param_type::real:
    str >> real;
    if(info.real.display_min <= real && real <= info.real.display_max) 
      return val.real = real, true;
    return false;
  default:
    assert(false);
    return false;
  }
}

void
param_format(param_info const& info, param_value val, wchar_t* buffer, std::size_t size)
{
  std::wstringstream str;
  switch (info.type)
  {
  case param_type::discrete: str << val.discrete; break;
  case param_type::list: str << info.items[val.discrete].detail; break;
  case param_type::real: str << std::setprecision(4) << val.real; break;
  case param_type::toggle: str << (val.discrete == 0 ? L"Off" : L"On"); break;
  default: assert(false); break;
  }
  std::wcsncpy(buffer, str.str().c_str(), size - 1);
}

param_value 
param_to_display(param_info const& info, param_value val)
{
  param_value result;
  float min = info.real.display_min;
  float range = info.real.display_max - min;
  switch (info.type)
  {
  case param_type::real:
    switch (info.real.slope)
    {
    case param_slope::linear: result.real = min + range * val.real; break;
    case param_slope::quadratic: result.real = min + range * val.real * val.real; break;
    default: assert(false); break;
    }
  default: result.discrete = val.discrete; break;
  }
  return result;
}

param_value 
param_from_display(param_info const& info, param_value val)
{
  param_value result;
  float min = info.real.display_min;
  float range = info.real.display_max - min;
  switch (info.type)
  {
  case param_type::real:
    switch (info.real.slope)
    {
    case param_slope::linear: result.real = (val.real - min) / range; break;
    case param_slope::quadratic: result.real = std::sqrt((val.real - min) / range); break;
    default: assert(false); break;
    }
  default: result.discrete = val.discrete; break;
  }
  return result;
}

} // namespace svn