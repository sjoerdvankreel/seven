#include <svn/support/topo_rt.hpp>
#include <svn/support/io_stream.hpp>
#include <svn/support/topo_static.hpp>
#include <svn/support/event_buffer.hpp>

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
    bool real = synth_params[p].info->type == param_type::real;
    if(!stream.write_string(synth_params[p].part->info->item.name)) return false;
    if(!stream.write_int32(synth_params[p].part->index)) return false;
    if(!stream.write_string(synth_params[p].info->item.name)) return false;
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
  std::string part_name;
  std::string param_name;
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
    if(!stream.read_string(part_name)) return false;
    if(!stream.read_int32(part_index)) return false;
    if(!stream.read_string(param_name)) return false;
    if(!stream.read_int32(param_real) || param_real != 0 || param_real != 1) return false;
    if(param_real == 1 && !stream.read_float(real)) return false;
    if(param_real == 0 && !stream.read_int32(discrete)) return false;

    for (std::int32_t rp = 0; rp < synth_param_count; rp++)
    {
      if(part_name != synth_params[rp].part->info->item.name) continue;
      if(part_index != synth_params[rp].part->index) continue;
      if(param_name != synth_params[rp].info->item.name) continue;
      if(param_real == 1 && synth_params[rp].info->type != param_type::real) continue;
      if(param_real == 1) param_values[rp].real = real;
      else param_values[rp].discrete = discrete;
    }
  }

  return true;
}
  
} // namespace svn
