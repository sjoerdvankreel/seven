#include <svn.base/support/io_stream.hpp>
#include <svn.base/support/param_value.hpp>
#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

#include <cassert>
#include <algorithm>

namespace svn::base {

static std::int32_t const version = 1;
static std::int32_t const magic = 192235685;

bool
io_stream::save(io_stream& stream, runtime_topology const& topology, param_value const* state)
{
  assert(state != nullptr);
  if(!stream.write_int32(magic)) return false;
  if(!stream.write_int32(version)) return false;
  if(!stream.write_int32(static_cast<std::int32_t>(topology.params.size()))) return false;

  for (std::size_t p = 0; p < topology.params.size(); p++)
  {
    auto const& part = topology.params[p].part;
    auto const& param = topology.params[p].descriptor;
    bool real = param->type == param_type::real;
    if(!stream.write_wstring(part->descriptor->static_name.short_)) return false;
    if(!stream.write_int32(part->type_index)) return false;
    if(!stream.write_wstring(param->static_name.short_)) return false;
    if(!stream.write_int32(real? 1: 0)) return false;
    if(real && !stream.write_float(state[p].real)) return false;
    if(!real && !stream.write_int32(state[p].discrete)) return false;
  }
  return true;
}

bool
io_stream::load(io_stream& stream, runtime_topology const& topology, param_value* state)
{
  param_value value;
  std::wstring part_name;
  std::wstring param_name;
  
  std::int32_t temp;
  std::int32_t type_index;
  std::int32_t param_real;
  std::int32_t param_count;

  assert(state != nullptr);
  if(!stream.read_int32(temp) || temp != magic) return false;
  if(!stream.read_int32(temp) || temp > version || temp <= 0) return false;
  if(!stream.read_int32(param_count) || param_count <= 0) return false;

  for (std::int32_t sp = 0; sp < param_count; sp++)
  {
    if(!stream.read_wstring(part_name)) return false;
    if(!stream.read_int32(type_index)) return false;
    if(!stream.read_wstring(param_name)) return false;
    if(!stream.read_int32(param_real) || param_real != 0 && param_real != 1) return false;
    if(param_real == 1 && !stream.read_float(value.real)) return false;
    if(param_real == 0 && !stream.read_int32(value.discrete)) return false;

    for (std::size_t rp = 0; rp < topology.params.size(); rp++)
    {
      auto const& part = topology.params[rp].part;
      auto const& param = topology.params[rp].descriptor;

      if(part_name != part->descriptor->static_name.short_) continue;
      if(type_index != part->type_index) continue;
      if(param_name != param->static_name.short_) continue;
      if(param_real == 1 && param->type != param_type::real) continue;
      if(param_real == 1) state[rp].real = std::clamp(value.real, 0.0f, 1.0f);
      if(param_real == 0) state[rp].discrete = std::clamp(value.discrete, param->min.discrete, param->max.discrete);
      break;
    }
  }

  return true;
}
  
} // namespace svn::base