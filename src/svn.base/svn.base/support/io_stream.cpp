#include <svn.base/support/io_stream.hpp>

#include <vector>
#include <cassert>
#include <algorithm>

namespace svn::base {

static std::int32_t const version = 2;
static std::int32_t const magic = 192235685;

bool
io_stream::save(topology_info const& topology, param_value const* state)
{
  std::size_t chars;
  std::vector<char> str;
  
  assert(state != nullptr);
  topology.state_check(state);

  if(!write_int32(magic)) return false;
  if(!write_int32(version)) return false;
  if(!write_int32(topology.input_param_count)) return false;

  for (std::int32_t p = 0; p < topology.input_param_count; p++)
  {
    auto const& param = *topology.params[p].descriptor;
    auto const& part = topology.parts[topology.params[p].part_index];

    if(!write_string(part.descriptor->guid)) return false;
    if(!write_int32(part.type_index)) return false;
    if(!write_string(param.guid)) return false;
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
      if(!write_string(std::string(str.data()))) return false;
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
  std::string str_value;
  std::string part_guid;
  std::string param_guid;
  std::wstring wstr_value;

  std::int32_t file_magic;
  std::int32_t type_index;
  std::int32_t param_count;
  std::int32_t file_version;

  assert(state != nullptr);
  if(!read_int32(file_magic) || file_magic != magic) return false;
  if(!read_int32(file_version) || file_version > version || file_version <= 0) return false;
  if(!read_int32(param_count) || param_count <= 0) return false;

  // Set defaults in case some params are missing or invalid.
  topology.init_defaults(state);

  for (std::int32_t sp = 0; sp < param_count; sp++)
  {
    if(!read_string(part_guid)) return false;
    if(!read_int32(type_index)) return false;
    if(!read_string(param_guid)) return false;
    if(!read_int32(type)) return false;
    if(type < 0 || type >= param_type::count) return false;

    switch (type)
    {
    case param_type::real:
      if(!read_float(value.real)) return false;
      break;
    case param_type::list:
    case param_type::knob_list:
      if(file_version == 1 && !read_wstring(wstr_value)) return false;
      if(file_version == 2 && !read_string(str_value)) return false;
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

    if (file_version == 1)
    {
      str_value.clear();
      for (std::size_t i = 0; i < wstr_value.length(); i++)
      {
        assert(wstr_value[i] < 128);
        str_value.push_back(static_cast<char>(wstr_value[i]));
      }
    }

    for (std::int32_t rp = 0; rp < topology.input_param_count; rp++)
    {
      auto const& param = topology.params[rp].descriptor;
      auto const& part = topology.parts[topology.params[rp].part_index];
      if(part_guid != part.descriptor->guid) continue;
      if(type_index != part.type_index) continue;
      if(param_guid != param->guid) continue;
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

  topology.state_check(state);
  return true;
}
  
} // namespace svn::base