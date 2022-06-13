#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.vst.ui_generator/support/support.hpp>
#include <svn.vst.ui_generator/generator/support.hpp>
#include <svn.vst.ui_generator/generator/generator.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

using namespace svn::base;
using namespace rapidjson;

namespace svn::vst::ui_generator {

static Value
build_ui_bitmaps(Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  for (std::size_t c = 0; c < color_count; c++)
  {
    Value background_value(kObjectType);
    std::string background_name = std::string("background") + std::to_string(c + 1);
    add_member(background_value, "path", background_name + ".png", allocator);
    add_member(result, get_bitmap_name(background_name), background_value, allocator);
    std::string name = print_rgb_hex(color_values[c], false, color_alpha::opaque);
    Value value(kObjectType);
    Value path((name + ".png").c_str(), allocator);
    value.AddMember("path", path, allocator);
    add_member(result, get_bitmap_name(name), value, allocator);
  }
  return result;
}

static Value
build_ui_colors(Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  for (std::int32_t a = 0; a < color_alpha::count; a++)
  {
    add_member(result, get_color_name(white, a), get_color_value(white, a), allocator);
    add_member(result, get_color_name(black, a), get_color_value(black, a), allocator);
    for (std::size_t c = 0; c < color_count; c++)
      add_member(result, get_color_name(color_values[c], a), get_color_value(color_values[c], a), allocator);
  }
  return result;
}

static Value
build_ui_control_tags(
  runtime_topology const& topology, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  for (std::size_t p = 0; p < topology.params.size(); p++)
  {
    std::string tag = get_control_tag(topology.params[p].runtime_name);
    add_member(result, get_control_tag(topology.params[p].runtime_name), std::to_string(p), allocator);
  }
  return result;
}

static Value
build_ui_param_item_base(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, std::string const& control_class,
  std::int32_t left, std::int32_t width, std::int32_t top_margin,
  Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", control_class, allocator);
  add_attribute(result, "size", size_to_string(width, param_row_height), allocator);
  std::int32_t top = top_margin;//top_margin;// + param.row * (param_row_height + margin);
  std::int32_t l = left; //margin + /*param.column * param_total_width +*/ left;
  add_attribute(result, "origin", size_to_string(l, top), allocator);
  return result;
}

static Value
build_ui_param_control_base(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, std::string const& control_class,
  std::int32_t left, std::int32_t width, std::int32_t top_margin,
  Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base(topology, part, param, control_class, left, width, top_margin, allocator));
  std::string tag = get_control_tag(topology.params[param.runtime_param_index].runtime_name);
  add_attribute(result, "control-tag", tag, allocator);
  auto const& rt_param = topology.params[param.runtime_param_index];
  std::string tooltip = narrow_assume_ascii(rt_param.runtime_name);
  std::string unit = std::string(" (") + narrow_assume_ascii(rt_param.descriptor->unit) + ")";
  if (std::wcslen(rt_param.descriptor->unit) > 0) tooltip += unit;
  add_attribute(result, "tooltip", tooltip, allocator);
  return result;
}

static Value
build_ui_param_checkbox(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, rgb color, std::int32_t left,
  std::int32_t width, std::int32_t top_margin, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param);
  Value result(build_ui_param_control_base(topology, part, param, class_name, left, width, top_margin, allocator));
  add_attribute(result, "title", "", allocator);
  add_attribute(result, "frame-width", "1", allocator);
  add_attribute(result, "draw-crossbox", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "boxfill-color", get_color_name(color, color_alpha::quarter), allocator);
  add_attribute(result, "boxframe-color", get_color_name(color_values[part.color_index], color_alpha::opaque), allocator);
  add_attribute(result, "checkmark-color", get_color_name(color_values[part.color_index], color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_knob(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param);
  Value result(build_ui_param_control_base(topology, part, param, class_name, 0, param_col1_width, 0, allocator));
  add_attribute(result, "angle-start", "20", allocator);
  add_attribute(result, "angle-range", "320", allocator);
  add_attribute(result, "height-of-one-image", std::to_string(param_row_height), allocator);
  std::string bitmap = print_rgb_hex(color_values[part.color_index], false, 0x00);
  add_attribute(result, "bitmap", get_bitmap_name(bitmap), allocator);
  return result;
}

static Value
build_ui_param_menu(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param);
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  Value result(build_ui_param_control_base(topology, part, param, class_name, 0, param_col1_width + margin + param_col2_width, 0, allocator));
  add_attribute(result, "min-value", "0", allocator);
  add_attribute(result, "default-value", "0", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "menu-popup-style", "true", allocator);
  add_attribute(result, "menu-check-style", "false", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "max-value", std::to_string(descriptor.max.discrete), allocator);
  add_attribute(result, "font-color", get_color_name(color_values[part.color_index], color_alpha::opaque), allocator);
  add_attribute(result, "back-color", get_color_name(color_values[part.color_index], color_alpha::quarter), allocator);
  add_attribute(result, "shadow-color", get_color_name(color_values[part.color_index], color_alpha::half), allocator);
  return result;
}

static Value
build_ui_param_label(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, std::int32_t left, std::int32_t width,
  Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base(topology, part, param, "CTextLabel", left, width, 0, allocator));
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "font-color", get_color_name(color_values[part.color_index], color_alpha::opaque), allocator);
  std::string title = narrow_assume_ascii(topology.params[param.runtime_param_index].descriptor->static_name.short_);
  add_attribute(result, "title", title, allocator);
  return result;
}

static Value
build_ui_param_edit(
  runtime_topology const& topology, part_ui_description const& part,
  param_ui_description const& param, std::int32_t left, std::int32_t width,
  std::string const& alignment, Document::AllocatorType& allocator)
{
  std::string small_font = "~ NormalFontSmall";
  std::string very_small_font = "~ NormalFontVerySmall";
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  Value result(build_ui_param_control_base(topology, part, param, "CTextEdit", left, width, 0, allocator));
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "value-precision", std::to_string(descriptor.precision), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::eight), allocator);
  add_attribute(result, "font", descriptor.ui_edit_font_small ? very_small_font : small_font, allocator);
  add_attribute(result, "font-color", get_color_name(color_values[part.color_index], color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_background(std::int32_t row, std::int32_t column,
  std::int32_t group_index, std::int32_t color_index, Document::AllocatorType& allocator)
{
  std::string color;
  Value result(kObjectType);
  std::string style = "filled";
  std::int32_t width = param_total_width - padding_param_group;
  std::int32_t height = param_row_height + margin - padding_param_group;
  std::int32_t left = column * param_total_width + padding_param_group * 2;
  std::int32_t top = row * (param_row_height + margin) + 2 * padding_param_group;

  if (group_index == 0)
  {
    style = "stroked";
    top -= padding_param_group;
    left -= padding_param_group;
    width += padding_param_group * 2;
    height += padding_param_group * 2;
    color = get_color_name(black, color_alpha::quarter);
  }
  if (group_index == 1) color = get_color_name(white, color_alpha::eight);
  if (group_index == 2) color = get_color_name(color_values[color_index], color_alpha::eight);

  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "background-color", color, allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "background-color-draw-style", style, allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  return result;
}

static void
add_ui_input_param(
  runtime_topology const& topology, part_ui_description const& part,
  Value& container, std::size_t index, Document::AllocatorType& allocator)
{
  Value checkbox;
  std::int32_t left_col2 = param_col1_width + margin;
  std::int32_t left_col3 = left_col2 + param_col2_width + margin;
  std::int32_t col12_and_magin = param_col1_width + margin + param_col2_width;
  param_ui_description const& param = part.params[index];
  std::string control_class = get_param_control_class(topology, param);
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::real:
  case param_type::discrete:
  case param_type::discrete_list:
    add_child(container, control_class, build_ui_param_knob(topology, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col2, param_col2_width, allocator), allocator);
    add_child(container, "CTextEdit", build_ui_param_edit(topology, part, param, col12_and_magin, param_col3_width, "right", allocator), allocator);
    break;
  case param_type::toggle:
    checkbox = build_ui_param_checkbox(topology, part, param, color_values[part.color_index], margin, param_col1_width, 0, allocator);
    add_child(container, control_class, checkbox, allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col2, param_col2_width, allocator), allocator);
    break;
  case param_type::list:
    add_child(container, control_class, build_ui_param_menu(topology, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col3, param_col3_width, allocator), allocator);
    break;
  case param_type::discrete_text:
    add_child(container, control_class, build_ui_param_edit(topology, part, param, 0, col12_and_magin, "right", allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col3, param_col3_width, allocator), allocator);
    break;
  default:
    assert(false);
  }
}

static void
add_ui_output_param(
  runtime_topology const& topology, part_ui_description const& part,
  Value& container, std::size_t index, Document::AllocatorType& allocator)
{
  param_ui_description const& param = part.params[index];
  std::string control_class = get_param_control_class(topology, param);
  add_child(container, "CTextLabel", build_ui_param_label(
    topology, part, param, 0, param_output_col_width, allocator), allocator);
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::toggle:
    add_child(container, control_class, build_ui_param_checkbox(topology, part, param, color_values[part.color_index],
      param_output_col_width + margin, param_output_col_width, 0, allocator), allocator);
    break;
  case param_type::discrete_text:
    add_child(container, control_class, build_ui_param_edit(topology, part, param,
      param_output_col_width + margin, param_output_col_width - margin, "left", allocator), allocator);
    break;
  default:
    assert(false);
  }
}

static Value
build_ui_part_single_param_container(
  runtime_topology const& topology, part_ui_description const& description,
  std::size_t index, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  auto const& param = description.params[index];
  auto const& part = topology.parts[description.runtime_part_index];
  std::int32_t left = param.column * param_total_width + padding_param_group * 2 + padding_param_group;
  std::int32_t top = param.row * (param_row_height + margin) + 3 * padding_param_group;
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(param_total_width, param_row_height), allocator);
  add_attribute(result, "background-color", get_color_name(white, color_alpha::transparent), allocator); 
  if (part.descriptor->output)
    add_ui_output_param(topology, description, result, index, allocator);
  else
    add_ui_input_param(topology, description, result, index, allocator);
  return result;
}

static Value
build_ui_part_param_container(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height - param_row_height), allocator);
  add_attribute(result, "background-color", get_color_name(color_values[description.color_index], color_alpha::eight), allocator);
  for (std::size_t p = 0; p < description.params.size(); p++)
  {
    auto const& param = description.params[p];
    auto const& part = topology.parts[description.runtime_part_index];
    auto const& param_descriptor = *topology.params[param.runtime_param_index].descriptor;
    add_child(result, "CViewContainer", build_ui_param_background(
      param.row, param.column, param_descriptor.ui_param_group, description.color_index, allocator), allocator);
    if (param_descriptor.ui_param_group != 0)
      add_child(result, "CViewContainer", build_ui_param_background(
        param.row, param.column, 0, description.color_index, allocator), allocator);
    add_child(result, "CViewContainer", build_ui_part_single_param_container(topology, description, p, allocator), allocator);    
  }
  for (std::int32_t p = description.params.size(); p < description.rows * description.columns; p++)
    add_child(result, "CViewContainer", build_ui_param_background(
      p / description.rows, p % description.columns, 0, description.color_index, allocator), allocator);
  return result;
}

static Value
build_ui_part_param_container_border(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::transparent), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height - param_row_height), allocator);
  add_attribute(result, "frame-color", get_color_name(color_values[description.color_index], color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_header_label(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string title = " " + narrow_assume_ascii(topology.parts[description.runtime_part_index].runtime_name);
  add_attribute(result, "title", title, allocator);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "font-color", get_color_name(color_values[description.color_index], color_alpha::opaque), allocator);
  add_attribute(result, "origin", size_to_string(0, -2), allocator);
  add_attribute(result, "size", size_to_string(description.width, param_row_height), allocator);
  add_attribute(result, "background-color", get_color_name(color_values[description.color_index], color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_header_container(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(1, 1), allocator);
  add_attribute(result, "size", size_to_string(description.width - 2, param_row_height - 2), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);
  add_child(result, "CTextLabel", build_ui_part_header_label(topology, description, allocator), allocator);
  if (description.enabled_param.runtime_param_index != -1)
  {
    std::int32_t left = param_total_width - param_col1_width + margin;
    Value enabled_box = build_ui_param_checkbox(topology, description, description.enabled_param, black, left, param_col1_width, -1, allocator);
    add_child(result, "CCheckBox", enabled_box, allocator);
  }
  return result;
}

static Value
build_ui_part_header_container_background(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "background-color-draw-style", "filled", allocator);
  add_attribute(result, "size", size_to_string(description.width, param_row_height + 1), allocator);
  add_attribute(result, "background-color", get_color_name(color_values[description.color_index], color_alpha::quarter), allocator);
  return result;
}

static Value
build_ui_part_header_container_border(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::transparent), allocator);
  add_attribute(result, "size", size_to_string(description.width, param_row_height + 1), allocator);
  add_attribute(result, "frame-color", get_color_name(color_values[description.color_index], color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_inner_container(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);
  add_child(result, "CViewContainer", build_ui_part_header_container_background(topology, description, allocator), allocator);
  add_child(result, "CViewContainer", build_ui_part_header_container_border(topology, description, allocator), allocator);
  add_child(result, "CViewContainer", build_ui_part_header_container(topology, description, allocator), allocator);
  add_child(result, "CViewContainer", build_ui_part_param_container_border(topology, description, allocator), allocator);
  add_child(result, "CViewContainer", build_ui_part_param_container(topology, description, allocator), allocator);
  return result;
}

static Value
build_ui_part_outer_container(runtime_topology const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(description.left, description.top), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height), allocator);
  add_attribute(result, "bitmap", get_bitmap_name(std::string("background") + std::to_string(description.color_index + 1)), allocator);
  add_child(result, "CViewContainer", build_ui_part_inner_container(topology, description, allocator), allocator);
  return result;
}

static Value
build_ui_template(runtime_topology const& topology,
  controller_ui_description const& descriptor, Document::AllocatorType& allocator)
{
  Value view(kObjectType);
  std::string size = size_to_string(descriptor.width, descriptor.height);
  add_attribute(view, "size", size, allocator);
  add_attribute(view, "minSize", size, allocator);
  add_attribute(view, "maxSize", size, allocator);
  add_attribute(view, "class", "CViewContainer", allocator);
  for (std::size_t p = 0; p < descriptor.parts.size(); p++)
    add_child(view, "CViewContainer", build_ui_part_outer_container(topology, descriptor.parts[p], allocator), allocator);
  Value result(kObjectType);
  result.AddMember("view", view, allocator);
  return result;
}

Document
build_vstgui_json(
  runtime_topology const& topology,
  controller_ui_description const& description)
{
  Document result;
  result.SetObject();
  Document::AllocatorType& allocator = result.GetAllocator();
  Value root(kObjectType);
  root.AddMember("version", "1", allocator);
  root.AddMember("bitmaps", build_ui_bitmaps(allocator), allocator);
  root.AddMember("colors", build_ui_colors(allocator), allocator);
  root.AddMember("control-tags", build_ui_control_tags(topology, allocator), allocator);
  root.AddMember("templates", build_ui_template(topology, description, allocator), allocator);
  result.AddMember("vstgui-ui-description", root, allocator);
  return result;
}

} // namespace svn::vst::ui_generator