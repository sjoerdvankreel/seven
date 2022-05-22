#include <svn/support/topo_rt.hpp>
#include <svn/support/io_stream.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/event_buffer.hpp>

#include <cassert>
#include <algorithm>

namespace svn {

static std::int32_t const version = 1;
static std::int32_t const magic = 192235685;

bool
io_stream::save(io_stream& stream, param_value* param_values)
{
  if(!stream.write_int32(magic)) return false;
  if(!stream.write_int32(version)) return false;
  if(!stream.write_int32(synth_param_count)) return false;

  for (std::int32_t p = 0; p < synth_param_count; p++)
  {
    auto const& part = synth_params[p].part;
    auto const& param = synth_params[p].info;
    bool real = param->type == param_type::real;
    if(!stream.write_wstring(part->info->item.name)) return false;
    if(!stream.write_int32(part->index)) return false;
    if(!stream.write_wstring(param->item.name)) return false;
    if(!stream.write_int32(real? 1: 0)) return false;
    if(real && !stream.write_float(param_values[p].real)) return false;
    if(!real && !stream.write_int32(param_values[p].discrete)) return false;
  }
  return true;
}

bool
io_stream::load(io_stream& stream, param_value* param_values)
{
  float real;
  std::wstring part_name;
  std::wstring param_name;
  std::int32_t val;
  std::int32_t discrete;
  std::int32_t part_index;
  std::int32_t param_real;
  std::int32_t param_count;

  if(!stream.read_int32(val) || val != magic) return false;
  if(!stream.read_int32(val) || val > version || val <= 0) return false;
  if(!stream.read_int32(param_count) || param_count <= 0) return false;

  for (std::int32_t sp = 0; sp < param_count; sp++)
  {
    if(!stream.read_wstring(part_name)) return false;
    if(!stream.read_int32(part_index)) return false;
    if(!stream.read_wstring(param_name)) return false;
    if(!stream.read_int32(param_real) || param_real != 0 || param_real != 1) return false;
    if(param_real == 1 && !stream.read_float(real)) return false;
    if(param_real == 0 && !stream.read_int32(discrete)) return false;

    for (std::int32_t rp = 0; rp < synth_param_count; rp++)
    {
      auto const& part = synth_params[rp].part;
      auto const& param = synth_params[rp].info;

      if(part_name != part->info->item.name) continue;
      if(part_index != part->index) continue;
      if(param_name != param->item.name) continue;
      if(param_real == 1 && param->type != param_type::real) continue;
      if(param_real == 1) param_values[rp].real = std::clamp(real, 0.0f, 1.0f);
      if(param_real == 1) continue;
      switch (synth_params[rp].info->type)
      {
      case param_type::toggle: param_values[rp].discrete = std::clamp(discrete, 0, 1); break;
      case param_type::list: param_values[rp].discrete = std::clamp(discrete, 0, param->bounds.list.count); break;
      case param_type::discrete: param_values[rp].discrete = std::clamp(discrete, param->bounds.discrete.min, param->bounds.discrete.max); break;
      default: assert(false); break;
      }
    }
  }

  return true;
}
  
} // namespace svn
