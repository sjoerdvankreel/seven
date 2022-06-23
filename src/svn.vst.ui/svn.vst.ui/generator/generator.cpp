#include <svn.vst.ui/support/support.hpp>
#include <svn.vst.ui/generator/support.hpp>
#include <svn.vst.ui/generator/generator.hpp>
#include <svn.base/topology/part_descriptor.hpp>
#include <svn.base/topology/param_descriptor.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#include <set>

using namespace svn::base;
using namespace rapidjson;

namespace svn::vst::ui {

static Value
build_ui_bitmaps(Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  Value background_value(kObjectType);
  add_member(background_value, "path", "background.png", allocator);
  add_member(result, "background", background_value, allocator);
  return result;
}

static Value
build_ui_colors(topology_info const& topology, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::vector<part_ui_color> colors;
  for (std::int32_t a = 0; a < color_alpha::count; a++)
  {
    add_member(result, get_color_name(white, a), get_color_value(white, a), allocator);
    add_member(result, get_color_name(black, a), get_color_value(black, a), allocator);
    for (std::int32_t i = 0; i < topology.static_part_count; i++)
    {
      part_ui_color color = topology.static_parts[i].ui.color;
      add_member(result, get_color_name(color, a), get_color_value(color, a), allocator);
    }
  }
  return result;
}

static Value
build_ui_control_tags(
  topology_info const& topology, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  for (std::size_t p = 0; p < topology.params.size(); p++)
  {    
    auto const& param = topology.params[p];
    std::wstring tag_name = topology.parts[param.part_index].descriptor->static_name.detail;
    tag_name += L"_" + topology.parts[param.part_index].type_index;
    tag_name += L"_" + std::wstring(param.descriptor->static_name.detail);
    std::string tag = get_control_tag(topology, p);
    add_member(result, get_control_tag(topology, p), std::to_string(p), allocator);
  }
  return result;
}

static Value
build_ui_param_item_base(
  std::string const& control_class, std::int32_t left, 
  std::int32_t width, std::int32_t top_margin, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", control_class, allocator);
  add_attribute(result, "size", size_to_string(width, param_row_height), allocator);
  add_attribute(result, "origin", size_to_string(left, top_margin), allocator);
  return result;
}

static Value
build_ui_param_control_base(
  topology_info const& topology, param_ui_description const& param,
  std::string const& control_class, std::int32_t left, std::int32_t width, 
  std::int32_t top_margin, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base(control_class, left, width, top_margin, allocator));
  std::string tag = get_control_tag(topology, param.runtime_param_index);
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
  topology_info const& topology, part_ui_description const& part,
  param_ui_description const& param, part_ui_color const& color, std::int32_t left,
  std::int32_t width, std::int32_t top_margin, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param);
  Value result(build_ui_param_control_base(topology, param, class_name, left, width, top_margin, allocator));
  add_attribute(result, "title", "", allocator);
  add_attribute(result, "frame-width", "1", allocator);
  add_attribute(result, "draw-crossbox", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "boxfill-color", get_color_name(color, color_alpha::quarter), allocator);
  add_attribute(result, "boxframe-color", get_color_name(part.color, color_alpha::opaque), allocator);
  add_attribute(result, "checkmark-color", get_color_name(part.color, color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_knob(
  topology_info const& topology, part_ui_description const& part,
  param_ui_description const& param, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param);
  Value result(build_ui_param_control_base(topology, param, class_name, 0, param_col1_width, 0, allocator));
  add_attribute(result, "angle-start", "20", allocator);
  add_attribute(result, "angle-range", "320", allocator);
  add_attribute(result, "color", get_color_name(part.color, color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_menu(
  topology_info const& topology, part_ui_description const& part,
  param_ui_description const& param, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param);
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  Value result(build_ui_param_control_base(topology, param, class_name, 0, param_col1_width + margin + param_col2_width, 0, allocator));
  add_attribute(result, "min-value", "0", allocator);
  add_attribute(result, "default-value", "0", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "menu-popup-style", "true", allocator);
  add_attribute(result, "menu-check-style", "false", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "max-value", std::to_string(descriptor.discrete.max), allocator);
  add_attribute(result, "font-color", get_color_name(part.color, color_alpha::opaque), allocator);
  add_attribute(result, "back-color", get_color_name(part.color, color_alpha::quarter), allocator);
  add_attribute(result, "shadow-color", get_color_name(part.color, color_alpha::half), allocator);
  return result;
}

static Value
build_ui_param_label(
  topology_info const& topology, part_ui_description const& part,
  param_ui_description const& param, std::int32_t left, std::int32_t width,
  Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base("CTextLabel", left, width, 0, allocator));
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "font-color", get_color_name(part.color, color_alpha::opaque), allocator);
  std::string title = narrow_assume_ascii(topology.params[param.runtime_param_index].descriptor->static_name.short_);
  add_attribute(result, "title", title, allocator);
  return result;
}

static Value
build_ui_param_edit(
  topology_info const& topology, part_ui_description const& part,
  param_ui_description const& param, std::int32_t left, std::int32_t width,
  std::string const& alignment, Document::AllocatorType& allocator)
{
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  Value result(build_ui_param_control_base(topology, param, "CTextEdit", left, width, 0, allocator));
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font", "~ NormalFontVerySmall", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "value-precision", std::to_string(descriptor.real.precision), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::eight), allocator);
  add_attribute(result, "font-color", get_color_name(part.color, color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_background_or_border(
  part_ui_description const& part, std::int32_t row, 
  std::int32_t column, std::int32_t group_index, Document::AllocatorType& allocator)
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
  if (group_index == 2) color = get_color_name(part.color, color_alpha::eight);

  add_attribute(result, "background-color", color, allocator);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "background-color-draw-style", style, allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  return result;
}

static void
add_ui_input_param(
  topology_info const& topology, part_ui_description const& part,
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
  case param_type::knob:
  case param_type::knob_list:
    add_child(container, control_class, build_ui_param_knob(topology, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col2, param_col2_width, allocator), allocator);
    add_child(container, "CTextEdit", build_ui_param_edit(topology, part, param, col12_and_magin, param_col3_width, "right", allocator), allocator);
    break;
  case param_type::toggle:
    checkbox = build_ui_param_checkbox(topology, part, param, part.color, margin, param_col1_width, 0, allocator);
    add_child(container, control_class, checkbox, allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col2, param_col2_width, allocator), allocator);
    break;
  case param_type::list:
    add_child(container, control_class, build_ui_param_menu(topology, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col3, param_col3_width, allocator), allocator);
    break;
  case param_type::text:
    add_child(container, control_class, build_ui_param_edit(topology, part, param, 0, col12_and_magin, "right", allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, left_col3, param_col3_width, allocator), allocator);
    break;
  default:
    assert(false);
  }
}

static void
add_ui_output_param(
  topology_info const& topology, part_ui_description const& part,
  Value& container, std::size_t index, Document::AllocatorType& allocator)
{
  param_ui_description const& param = part.params[index];
  std::string control_class = get_param_control_class(topology, param);
  add_child(container, "CTextLabel", build_ui_param_label(
    topology, part, param, 0, param_output_col_width, allocator), allocator);
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::toggle:
    add_child(container, control_class, build_ui_param_checkbox(topology, part, param, part.color,
      param_output_col_width + margin, param_output_col_width, 0, allocator), allocator);
    break;
  case param_type::text:
    add_child(container, control_class, build_ui_param_edit(topology, part, param,
      param_output_col_width + margin, param_output_col_width - margin, "left", allocator), allocator);
    break;
  default:
    assert(false);
  }
}

static Value
build_ui_part_single_param_container(
  topology_info const& topology, part_ui_description const& description,
  std::size_t index, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  auto const& param = description.params[index];
  auto const& part = topology.parts[description.runtime_part_index];
  std::int32_t left = param.column * param_total_width + 3 * padding_param_group;
  std::int32_t top = param.row * (param_row_height + margin) + 3 * padding_param_group;
  add_attribute(result, "class", "view_container_fix", allocator);
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
build_ui_part_graph(topology_info const& topology,
  part_ui_description const& description, std::int32_t index, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  auto const& graph = description.graphs[index];
  std::int32_t left = graph.column * param_total_width + padding_param_group;
  std::int32_t top = graph.row * (param_row_height + margin) + padding_param_group;
  std::int32_t height = graph.row_span * (param_row_height + margin) + margin - 2 * padding_param_group;
  std::int32_t width = graph.column_span * param_total_width + (graph.column_span - 1) * margin - 2 * padding_param_group;
  add_attribute(result, "class", "seven_graph_plot", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  add_attribute(result, "color", get_color_name(description.color, color_alpha::opaque), allocator);
  auto const& part = topology.parts[description.runtime_part_index];
  add_attribute(result, "graph-type", std::to_string(graph.type), allocator);
  add_attribute(result, "part-index", std::to_string(part.type_index), allocator);
  add_attribute(result, "part-type", std::to_string(part.descriptor->type), allocator);
  return result;
}

static Value
build_ui_part_param_container(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height - param_row_height), allocator);
  add_attribute(result, "background-color", get_color_name(description.color, color_alpha::eight), allocator);
  
  // Build graph content.
  for(std::int32_t g = 0; g < description.graph_count; g++)
    add_child(result, "seven_graph_plot", build_ui_part_graph(topology, description, g, allocator), allocator);
  
  // Build cell decoration and filler cells.
  std::set<std::pair<std::int32_t, std::int32_t>> cells_decorated;
  for (std::size_t p = 0; p < description.params.size(); p++)
  {
    auto const& param = description.params[p];
    if(param.runtime_param_index == -1)
    {
      // Empty cell filler.
      add_child(result, "view_container_fix", build_ui_param_background_or_border(
        description, param.row, param.column, 0, allocator), allocator);
      continue;
    }

    // Only add background and borders the first time we encounter this cell.
    auto const& param_descriptor = *topology.params[param.runtime_param_index].descriptor;
    if(cells_decorated.find({ param.row, param.column}) == cells_decorated.end())
    {
      add_child(result, "view_container_fix", build_ui_param_background_or_border(
        description, param.row, param.column, param_descriptor.ui.param_group, allocator), allocator);
      if (param_descriptor.ui.param_group != 0)
        add_child(result, "view_container_fix", build_ui_param_background_or_border(
          description, param.row, param.column, 0, allocator), allocator);
    }
    cells_decorated.insert({ param.row, param.column });
  }

  // Build actual parameter content.
  for (std::size_t p = 0; p < description.params.size(); p++)
    if (description.params[p].runtime_param_index != -1)
      add_child(result, "view_container_fix", build_ui_part_single_param_container(topology, description, p, allocator), allocator);
  return result;
}

static Value
build_ui_part_param_container_border(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::transparent), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height - param_row_height), allocator);
  add_attribute(result, "frame-color", get_color_name(description.color, color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_header_label(topology_info const& topology,
  part_ui_description const& description, std::int32_t left, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string title = " " + narrow_assume_ascii(topology.parts[description.runtime_part_index].runtime_name);
  add_attribute(result, "title", title, allocator);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "origin", size_to_string(left, -1), allocator);
  add_attribute(result, "size", size_to_string(description.width, param_row_height), allocator);
  add_attribute(result, "font-color", get_color_name(description.color, color_alpha::opaque), allocator);
  add_attribute(result, "background-color", get_color_name(description.color, color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_header_container(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(1, 1), allocator);
  add_attribute(result, "size", size_to_string(description.width - 2, param_row_height - 2), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);
  if (description.enabled_param.runtime_param_index != -1)
  {
    Value enabled_box = build_ui_param_checkbox(topology, description, description.enabled_param, black, margin, header_checkbox_width, -1, allocator);
    add_child(result, "CCheckBox", enabled_box, allocator);
    add_child(result, "CTextLabel", build_ui_part_header_label(topology, description, header_checkbox_width + margin, allocator), allocator);
  } else
    add_child(result, "CTextLabel", build_ui_part_header_label(topology, description, 0, allocator), allocator);
  return result;
}

static Value
build_ui_part_header_container_background(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "background-color-draw-style", "filled", allocator);
  add_attribute(result, "size", size_to_string(description.width, param_row_height + 1), allocator);
  add_attribute(result, "background-color", get_color_name(description.color, color_alpha::quarter), allocator);
  return result;
}

static Value
build_ui_part_header_container_border(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::transparent), allocator);
  add_attribute(result, "size", size_to_string(description.width, param_row_height + 1), allocator);
  add_attribute(result, "frame-color", get_color_name(description.color, color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_inner_container(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);
  add_child(result, "view_container_fix", build_ui_part_header_container_background(topology, description, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_header_container_border(topology, description, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_header_container(topology, description, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_param_container_border(topology, description, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_param_container(topology, description, allocator), allocator);
  return result;
}

static Value
build_ui_part_outer_container(topology_info const& topology,
  part_ui_description const& description, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t offset_space = 25;
  std::int32_t offset_x = description.runtime_part_index / 3;
  std::int32_t offset_y = description.runtime_part_index % 3;
  add_attribute(result, "bitmap", "background", allocator);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(description.left, description.top), allocator);
  add_attribute(result, "size", size_to_string(description.width, description.height), allocator);
  add_attribute(result, "background-offset", size_to_string(offset_x * offset_space, offset_y * offset_space), allocator);
  add_child(result, "view_container_fix", build_ui_part_inner_container(topology, description, allocator), allocator);
  return result;
}

static Value
build_ui_template(topology_info const& topology,
  controller_ui_description const& descriptor, Document::AllocatorType& allocator)
{
  Value view(kObjectType);
  std::string size = size_to_string(descriptor.width, descriptor.height);
  add_attribute(view, "size", size, allocator);
  add_attribute(view, "minSize", size, allocator);
  add_attribute(view, "maxSize", size, allocator);
  add_attribute(view, "class", "view_container_fix", allocator);
  for (std::size_t p = 0; p < descriptor.parts.size(); p++)
    add_child(view, "view_container_fix", build_ui_part_outer_container(topology, descriptor.parts[p], allocator), allocator);
  Value result(kObjectType);
  result.AddMember("view", view, allocator);
  return result;
}

Document
build_vstgui_json(
  topology_info const& topology,
  controller_ui_description const& description)
{
  Document result;
  result.SetObject();
  Document::AllocatorType& allocator = result.GetAllocator();
  Value root(kObjectType);
  root.AddMember("version", "1", allocator);
  root.AddMember("bitmaps", build_ui_bitmaps(allocator), allocator);
  root.AddMember("colors", build_ui_colors(topology, allocator), allocator);
  root.AddMember("control-tags", build_ui_control_tags(topology, allocator), allocator);
  root.AddMember("templates", build_ui_template(topology, description, allocator), allocator);
  result.AddMember("vstgui-ui-description", root, allocator);
  return result;
}

} // namespace svn::vst::ui