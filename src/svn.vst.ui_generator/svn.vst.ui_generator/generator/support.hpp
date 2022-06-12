#ifndef SVN_VST_UI_GENERATOR_GENERATOR_SUPPORT_HPP
#define SVN_VST_UI_GENERATOR_GENERATOR_SUPPORT_HPP

#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.vst.ui_generator/description/ui_description.hpp>
#include <svn.vst.ui_generator/description/param_ui_description.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <string>

namespace svn::vst::ui_generator {

std::string get_bitmap_name(std::string const& rgb);
std::string size_to_string(std::int32_t w, std::int32_t h);
std::string get_color_name(rgb color, std::int32_t alpha_index);
std::string get_color_value(rgb color, std::int32_t alpha_index);
std::string get_control_tag(std::wstring const& runtime_param_name);
std::string print_rgb_hex(rgb color, bool print_alpha, std::int32_t alpha_index);

std::string get_param_control_class(
  svn::base::runtime_topology const& topology, 
  param_ui_description const& param);

rapidjson::Value& add_member(
  rapidjson::Value& container, std::string const& key,
  std::string const& value, rapidjson::Document::AllocatorType& allocator);
rapidjson::Value& add_member(
  rapidjson::Value& container, std::string const& key,
  rapidjson::Value const& value, rapidjson::Document::AllocatorType& allocator);

void add_attribute(
  rapidjson::Value& container, std::string const& key,
  std::string const& value, rapidjson::Document::AllocatorType& allocator);
void add_child(
  rapidjson::Value& container, std::string const& key,
  rapidjson::Value const& value, rapidjson::Document::AllocatorType& allocator);

} // namespace svn::vst::ui_generator
#endif // SVN_VST_UI_GENERATOR_GENERATOR_SUPPORT_HPP