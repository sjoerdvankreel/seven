#include <svn.base/support/io_stream.hpp>

#include <set>
#include <tuple>
#include <vector>
#include <cassert>
#include <algorithm>

namespace svn::base {

static std::int32_t const version = 1;
static std::int32_t const magic = 192235685;

bool
io_stream::save(topology_info const& topology, param_value const* state)
{
  std::size_t chars;
  std::vector<wchar_t> str;
  assert(state != nullptr);

  if(!write_int32(magic)) return false;
  if(!write_int32(version)) return false;
  if(!write_int32(topology.input_param_count)) return false;

  // Keep track of stuff we've seen, don't write different params under the same id.
  std::set<std::tuple<std::wstring, std::int32_t, std::wstring>> seen;
  for (std::int32_t p = 0; p < topology.input_param_count; p++)
  {
    auto const& param = *topology.params[p].descriptor;
    auto const& part = topology.parts[topology.params[p].part_index];

    // Just debugging.
    auto this_id = std::make_tuple(
      std::wstring(part.descriptor->static_name.short_), 
      part.type_index, 
      std::wstring(param.static_name.short_));
    assert(seen.find(this_id) == seen.end());
    seen.insert(this_id);

    if(!write_wstring(part.descriptor->static_name.short_)) return false;
    if(!write_int32(part.type_index)) return false;
    if(!write_wstring(param.static_name.short_)) return false;
    if(!write_int32(param.type)) return false;

    switch (param.type)
    {
    case param_type::real: 
      if(!write_float(state[p].real)) return false;
      break;
    case param_type::knob:
    case param_type::text:
    case param_type::toggle:
      if(!write_int32(state[p].discrete)) return false;
      break;
    case param_type::list: 
    case param_type::knob_list:
      chars = param.format(state[p], nullptr, 0);
      str.reserve(chars);
      param.format(state[p], str.data(), chars);
      if(!write_wstring(std::wstring(str.data()))) return false;
      break;
    default:
      assert(false);
      break;
    }
  }
  return true;
}

bool
io_stream::load(topology_info const& topology, param_value* state)
{
  std::int32_t type;
  param_value value;
  std::wstring str_value;
  std::wstring part_name;
  std::wstring param_name;
  
  std::int32_t temp;
  std::int32_t type_index;
  std::int32_t param_count;

  assert(state != nullptr);
  if(!read_int32(temp) || temp != magic) return false;
  if(!read_int32(temp) || temp > version || temp <= 0) return false;
  if(!read_int32(param_count) || param_count <= 0) return false;

  for (std::int32_t sp = 0; sp < param_count; sp++)
  {
    if(!read_wstring(part_name)) return false;
    if(!read_int32(type_index)) return false;
    if(!read_wstring(param_name)) return false;
    if(!read_int32(type)) return false;
    if(type < 0 || type >= param_type::count) return false;

    switch (type)
    {
    case param_type::real:
      if(!read_float(value.real)) return false;
      break;
    case param_type::list:
    case param_type::knob_list:
      if (!read_wstring(str_value)) return false;
      break;
    case param_type::text:
    case param_type::knob:
    case param_type::toggle:
      if(!read_int32(value.discrete)) return false;
      break;
    default:
      assert(false);
      break;
    }

    for (std::int32_t rp = 0; rp < topology.input_param_count; rp++)
    {
      auto const& param = topology.params[rp].descriptor;
      auto const& part = topology.parts[topology.params[rp].part_index];
      if(part_name != part.descriptor->static_name.short_) continue;
      if(type_index != part.type_index) continue;
      if(param_name != param->static_name.short_) continue;
      if(type != param->type) continue;

      switch (type)
      {
      case param_type::real:  
        state[rp].real = std::clamp(value.real, 0.0f, 1.0f);
        break;
      case param_type::text:
      case param_type::knob:
      case param_type::toggle:
        state[rp].discrete = std::clamp(value.discrete, param->discrete.min, param->discrete.max);
        break;
      case param_type::list:
      case param_type::knob_list:
        if(param->parse(str_value.data(), value)) state[rp] = value;
        break;
      default:
        assert(false);
        break;
      }
      break;
    }
  }

  return true;
}
  
} // namespace svn::base