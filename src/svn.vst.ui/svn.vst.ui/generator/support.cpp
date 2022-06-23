#include <svn.vst.ui/support/support.hpp>
#include <svn.vst.ui/generator/support.hpp>
#include <svn.base/topology/param_descriptor.hpp>

#include <sstream>

using namespace rapidjson;
using namespace svn::base;

namespace svn::vst::ui {

std::string
get_color_value(rgb color, std::int32_t alpha_index)
{ return "#" + print_rgb_hex(color, true, alpha_index); }
std::string
get_color_name(rgb color, std::int32_t alpha_index)
{ return "color_" + print_rgb_hex(color, true, alpha_index); }

std::string
size_to_string(std::int32_t w, std::int32_t h)
{
  std::string result;
  result += std::to_string(w);
  result += ", ";
  result += std::to_string(h);
  return result;
}

Value&
add_member(Value& container, std::string const& key,
  Value const& value, Document::AllocatorType& allocator)
{
  Value key_copy(key.c_str(), allocator);
  Value value_copy(value, allocator);
  return container.AddMember(key_copy, value_copy, allocator);
}

Value&
add_member(Value& container, std::string const& key,
  std::string const& value, Document::AllocatorType& allocator)
{
  Value key_copy(key.c_str(), allocator);
  Value value_copy(value.c_str(), allocator);
  return container.AddMember(key_copy, value_copy, allocator);
}

void
add_child(
  Value& container, std::string const& key,
  Value const& value, Document::AllocatorType& allocator)
{
  if (!container.HasMember("children"))
    container.AddMember("children", Value(kObjectType), allocator);
  add_member(container["children"], key, value, allocator);
}

void
add_attribute(
  Value& container, std::string const& key,
  std::string const& value, Document::AllocatorType& allocator)
{
  if (!container.HasMember("attributes"))
    container.AddMember("attributes", Value(kObjectType), allocator);
  add_member(container["attributes"], key, value, allocator);
}

std::string
get_param_control_class(
  topology_info const& topology,
  param_ui_description const& param)
{ 
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::text: return "CTextEdit";
  case param_type::list: return "COptionMenu";
  case param_type::toggle: return "CCheckBox";
  case param_type::real:
  case param_type::knob:
  case param_type::knob_list: return "seven_rotary_knob";
  default: assert(false); return "";
  }
}
 
std::string
get_control_tag(topology_info const& topology, std::size_t p)
{
  auto const& param = topology.params[p];
  std::wstring tag_name = topology.parts[param.part_index].descriptor->static_name.detail;
  tag_name += L"_" + std::to_wstring(topology.parts[param.part_index].type_index);
  tag_name += L"_" + std::wstring(param.descriptor->static_name.detail);
  std::string result = narrow_assume_ascii(tag_name);
  for (std::size_t c = 0; c < result.length(); c++)
  {
    if (std::isspace(result[c])) result[c] = '_';
    else result[c] = static_cast<char>(std::tolower(result[c]));
  }
  return result;
}

std::string
print_rgb_hex(rgb color, bool print_alpha, std::int32_t alpha_index)
{
  std::ostringstream oss;
  oss << std::hex << std::uppercase;
  oss << ((color.r >> 4) & 0xF);
  oss << (color.r & 0xF);
  oss << ((color.g >> 4) & 0xF);
  oss << (color.g & 0xF);
  oss << ((color.b >> 4) & 0xF);
  oss << (color.b & 0xF);
  if (!print_alpha) return oss.str();
  oss << ((color_alphas[alpha_index] >> 4) & 0xF);
  oss << (color_alphas[alpha_index] & 0xF);
  return oss.str();
}

} // namespace svn::vst::ui