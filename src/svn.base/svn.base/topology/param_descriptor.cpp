#include <svn.base/topology/param_descriptor.hpp>

#include <cassert>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace svn::base {

bool 
param_descriptor::parse(char const* buffer, param_value& val) const
{
  std::stringstream str(buffer);
  float inf = std::numeric_limits<float>::infinity();
  switch (type)
  {
  case param_type::knob:
  case param_type::text:
    str >> val.discrete;
    if (val.discrete < discrete.min) return false;
    if (val.discrete > discrete.max) return false;
    return true;
  case param_type::real:
    str >> val.real;
    if (!std::strcmp("-inf", buffer)) val.real = -inf;
    if (val.real < real.display.min) return false;
    if (val.real > real.display.max) return false;
    return true;
  case param_type::toggle:
    if (!std::strcmp("On", buffer)) val.discrete = 1;
    else if (!std::strcmp("Off", buffer)) val.discrete = 0;
    else return false;
    return true;
  case param_type::list:
  case param_type::knob_list:
    for (std::int32_t i = 0; i <= discrete.max; i++)
      if (!std::strcmp((*discrete.items)[i].c_str(), buffer))
        return val.discrete = i, true;
    return false;
  default:
    assert(false);
    return false;
  }
}  

std::size_t 
param_descriptor::format(param_value val, char* buffer, std::size_t size) const
{
  std::stringstream stream;
  switch (type) 
  { 
  case param_type::knob: case param_type::text: stream << val.discrete; break;
  case param_type::toggle: stream << (val.discrete == 0 ? "Off" : "On"); break;
  case param_type::real: stream << std::setprecision(real.precision) << std::fixed << val.real; break;
  case param_type::list: case param_type::knob_list: stream << (*discrete.items)[val.discrete]; break;
  default: assert(false); break;
  }
  std::string str = stream.str();
  if(buffer == nullptr || size == 0) return str.length() + 1;
  std::memset(buffer, 0, size * sizeof(char));
  std::strncpy(buffer, str.c_str(), size - 1);
  return str.length() + 1;
} 
 
} // namespace svn::base