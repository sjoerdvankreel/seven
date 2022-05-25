#include <svn/support/param_transform.hpp>
#include <cwchar>
#include <sstream>

namespace svn {

param_value 
param_to_display(param_info const& info, param_value val)
{
  param_value result;
  switch (info.type)
  {
  case param_type::real: result.real = param_real_to_range(
    info.real.slope, val.real, info.real.min, info.real.max); break;
  default: result.discrete = val.discrete; break;
  }
  return result;
}

param_value 
param_from_display(param_info const& info, param_value val)
{
  param_value result;
  switch (info.type)
  {
  case param_type::real: result.real = param_real_from_range(
    info.real.slope, val.real, info.real.min, info.real.max); break;
  default: result.discrete = val.discrete; break;
  }
  return result;
}

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
      if(!std::wcscmp(info.items[i].name, buffer))
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
  case param_type::real: str << val.real; break;
  case param_type::discrete: str << val.discrete; break;
  case param_type::list: str << info.items[val.discrete].name; break;
  case param_type::toggle: str << (val.discrete == 0 ? L"Off" : L"On"); break;
  default: assert(false); break;
  }
  std::wcsncpy(buffer, str.str().c_str(), size - 1);
}

} // namespace svn