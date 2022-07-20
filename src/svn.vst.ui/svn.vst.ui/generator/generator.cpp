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
  std::vector<ui_color> colors;
  for (std::int32_t a = 0; a < color_alpha::count; a++)
  {
    add_member(result, get_color_name(white, a), get_color_value(white, a), allocator);
    add_member(result, get_color_name(black, a), get_color_value(black, a), allocator);
    for (std::int32_t i = 0; i < topology.static_part_count; i++)
    {
      if(topology.static_parts[i].selector) continue;
      ui_color color = ui_color_gradient(topology, i);
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
  topology_info const& topology, std::int32_t runtime_param_index,
  std::string const& control_class, std::int32_t left, std::int32_t width, 
  std::int32_t top_margin, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base(control_class, left, width, top_margin, allocator));
  std::string tag = get_control_tag(topology, runtime_param_index);
  add_attribute(result, "control-tag", tag, allocator);
  auto const& rt_param = topology.params[runtime_param_index];
  std::string tooltip = narrow_assume_ascii(rt_param.runtime_name);
  std::string unit = std::string(" (") + narrow_assume_ascii(rt_param.descriptor->unit) + ")";
  if (std::wcslen(rt_param.descriptor->unit) > 0) tooltip += unit;
  add_attribute(result, "tooltip", tooltip, allocator);
  return result;
}

static Value
build_ui_param_checkbox(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, param_ui_description const& param, ui_color const& color, 
  std::int32_t left, std::int32_t width, std::int32_t top_margin, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param.runtime_param_index);
  Value result(build_ui_param_control_base(topology, param.runtime_param_index, class_name, left, width, top_margin, allocator));
  add_attribute(result, "title", "", allocator);
  add_attribute(result, "frame-width", "1", allocator);
  add_attribute(result, "draw-crossbox", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "boxfill-color", get_color_name(color, color_alpha::quarter), allocator);
  add_attribute(result, "boxframe-color", get_color_name(type.color, color_alpha::opaque), allocator);
  add_attribute(result, "checkmark-color", get_color_name(type.color, color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_knob(
  topology_info const& topology, part_type_ui_description const& type, 
  part_ui_description const& part, param_ui_description const& param, 
  Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, param.runtime_param_index);
  Value result(build_ui_param_control_base(topology, param.runtime_param_index, class_name, 0, param_col1_width, 0, allocator));
  add_attribute(result, "angle-start", "20", allocator);
  add_attribute(result, "angle-range", "320", allocator);
  add_attribute(result, "color", get_color_name(type.color, color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_menu(
  topology_info const& topology, part_type_ui_description const& type, 
  std::int32_t runtime_param_index, std::int32_t left, std::int32_t top_margin, Document::AllocatorType& allocator)
{
  std::string class_name = get_param_control_class(topology, runtime_param_index);
  auto const& descriptor = *topology.params[runtime_param_index].descriptor;
  std::string font = descriptor.ui.label_font_small ? "~ NormalFontSmall" : "~ NormalFont";
  Value result(build_ui_param_control_base(topology, runtime_param_index, class_name, left, param_col1_width + margin + param_col2_width, top_margin, allocator));
  add_attribute(result, "font", font, allocator);
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
  add_attribute(result, "font-color", get_color_name(type.color, color_alpha::opaque), allocator);
  add_attribute(result, "back-color", get_color_name(type.color, color_alpha::quarter), allocator);
  add_attribute(result, "shadow-color", get_color_name(type.color, color_alpha::half), allocator);
  return result;
}

static Value
build_ui_param_label(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, param_ui_description const& param, 
  std::int32_t left, std::int32_t width, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_item_base("CTextLabel", left, width, -1, allocator));
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  std::string font = descriptor.ui.label_font_small ? "~ NormalFontSmall" : "~ NormalFont";
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", font, allocator);
  add_attribute(result, "font-color", get_color_name(type.color, color_alpha::opaque), allocator);
  std::string title = narrow_assume_ascii(topology.params[param.runtime_param_index].descriptor->static_name.short_);
  add_attribute(result, "title", title, allocator);
  return result;
}

static Value
build_ui_param_edit(
  topology_info const& topology, part_type_ui_description const& type, 
  part_ui_description const& part, param_ui_description const& param, 
  std::int32_t left, std::int32_t width, std::string const& alignment, 
  Document::AllocatorType& allocator)
{
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  Value result(build_ui_param_control_base(topology, param.runtime_param_index, "CTextEdit", left, width, 0, allocator));
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "value-precision", std::to_string(descriptor.real.precision), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::eight), allocator);
  add_attribute(result, "font-color", get_color_name(type.color, color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_background_or_border(
  part_type_ui_description const& type, part_ui_description const& part, 
  std::int32_t row, std::int32_t column, std::int32_t group_index, 
  Document::AllocatorType& allocator)
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
  if (group_index == 2) color = get_color_name(type.color, color_alpha::eight);

  add_attribute(result, "background-color", color, allocator);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "background-color-draw-style", style, allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  return result;
}

static void
add_ui_input_param(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Value& container, 
  std::size_t index, Document::AllocatorType& allocator)
{
  Value checkbox;
  std::int32_t left_col2 = param_col1_width + margin;
  std::int32_t left_col3 = left_col2 + param_col2_width + margin;
  std::int32_t col12_and_magin = param_col1_width + margin + param_col2_width;
  param_ui_description const& param = part.params[index];
  std::string control_class = get_param_control_class(topology, param.runtime_param_index);
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::list:
    add_child(container, control_class, build_ui_param_menu(topology, type, param.runtime_param_index, 0, 0, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, type, part, param, left_col3, param_col3_width, allocator), allocator);
    break;
  case param_type::text:
    add_child(container, control_class, build_ui_param_edit(topology, type, part, param, 0, col12_and_magin, "right", allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, type, part, param, left_col3, param_col3_width, allocator), allocator);
    break;
  case param_type::real:
  case param_type::knob:
  case param_type::knob_list:
    add_child(container, control_class, build_ui_param_knob(topology, type, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, type, part, param, left_col2, param_col2_width, allocator), allocator);
    add_child(container, "CTextEdit", build_ui_param_edit(topology, type, part, param, col12_and_magin, param_col3_width, "right", allocator), allocator);
    break;
  case param_type::toggle:
    checkbox = build_ui_param_checkbox(topology, type, part, param, type.color, margin, param_col1_width, 0, allocator);
    add_child(container, control_class, checkbox, allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, type, part, param, left_col2, param_col2_width + margin + param_col3_width, allocator), allocator);
    break;  
  default:
    assert(false);
  }
}

static void
add_ui_output_param(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Value& container, 
  std::size_t index, Document::AllocatorType& allocator)
{
  param_ui_description const& param = part.params[index];
  std::string control_class = get_param_control_class(topology, param.runtime_param_index);
  add_child(container, "CTextLabel", build_ui_param_label(
    topology, type, part, param, 0, param_output_col_width, allocator), allocator);
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::toggle:
    add_child(container, control_class, build_ui_param_checkbox(topology, type, part, param, 
      type.color, param_output_col_width + margin, param_output_col_width, 0, allocator), allocator);
    break;
  case param_type::text:
    add_child(container, control_class, build_ui_param_edit(topology, type, part, param,
      param_output_col_width + margin, param_output_col_width - margin, "left", allocator), allocator);
    break;
  default:
    assert(false);
  }
}

static Value
build_ui_part_single_param_container(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& description, std::size_t index, Document::AllocatorType& allocator)
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
    add_ui_output_param(topology, type, description, result, index, allocator);
  else
    add_ui_input_param(topology, type, description, result, index, allocator);
  return result;
}

static Value
build_ui_part_graph(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, std::int32_t index, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  auto const& graph = part.graphs[index];
  auto const& part_desc = topology.parts[part.runtime_part_index];
   
  std::int32_t width = graph.column_span * param_total_width - padding_param_group;
  std::int32_t left = graph.column * param_total_width + 2 * padding_param_group;
  std::int32_t top = graph.row * (param_row_height + margin) + 2 * padding_param_group;
  std::int32_t height = graph.row_span * param_row_height + (graph.row_span - 1) * margin + 2 * padding_param_group;
  add_attribute(result, "class", "seven_graph_plot", allocator);
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  add_attribute(result, "size", size_to_string(width, height), allocator);
  add_attribute(result, "graph-type", std::to_string(graph.type), allocator);
  add_attribute(result, "row-span", std::to_string(graph.row_span), allocator);
  add_attribute(result, "column-span", std::to_string(graph.column_span), allocator);
  add_attribute(result, "tooltip", narrow_assume_ascii(graph.description), allocator);
  add_attribute(result, "part-index", std::to_string(part_desc.type_index), allocator);
  add_attribute(result, "part-type", std::to_string(part_desc.descriptor->type), allocator);
  add_attribute(result, "color", get_color_name(type.color, color_alpha::opaque), allocator);
  return result;
} 

static Value
build_ui_part_param_container(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "background-color", get_color_name(type.color, color_alpha::eight), allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height + padding_param_group * 4), allocator);
  add_attribute(result, "size", size_to_string(part.width, part.height - param_row_height - padding_param_group * 4), allocator);
  
  // Build graph content.
  for(std::int32_t g = 0; g < part.graph_count; g++)
    add_child(result, "seven_graph_plot", build_ui_part_graph(
      topology, type, part, g, allocator), allocator);
  
  // Build cell decoration and filler cells.
  std::set<std::pair<std::int32_t, std::int32_t>> cells_decorated;
  for (std::size_t p = 0; p < part.params.size(); p++)
  {
    auto const& param = part.params[p];
    if(param.runtime_param_index == -1)
    {
      // Empty cell filler.
      add_child(result, "view_container_fix", build_ui_param_background_or_border(
        type, part, param.row, param.column, 0, allocator), allocator);
      continue;
    }

    // Only add background and borders the first time we encounter this cell.
    auto const& param_descriptor = *topology.params[param.runtime_param_index].descriptor;
    if(cells_decorated.find({ param.row, param.column}) == cells_decorated.end())
    {
      add_child(result, "view_container_fix", build_ui_param_background_or_border(
        type, part, param.row, param.column, param_descriptor.ui.param_group, allocator), allocator);
      if (param_descriptor.ui.param_group != 0)
        add_child(result, "view_container_fix", build_ui_param_background_or_border(
          type, part, param.row, param.column, 0, allocator), allocator);
    }
    cells_decorated.insert({ param.row, param.column });
  }

  // Build actual parameter content.
  for (std::size_t p = 0; p < part.params.size(); p++)
    if (part.params[p].runtime_param_index != -1)
      add_child(result, "view_container_fix", build_ui_part_single_param_container(
        topology, type, part, p, allocator), allocator);
  return result;
}

static Value
build_ui_part_param_container_border(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "frame-color", get_color_name(type.color, color_alpha::half), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::transparent), allocator);
  add_attribute(result, "origin", size_to_string(0, param_row_height + padding_param_group * 4), allocator);
  add_attribute(result, "size", size_to_string(part.width, part.height - param_row_height - padding_param_group * 4), allocator);
  return result;
}

static Value
build_ui_part_header_label(
  topology_info const& topology, part_type_ui_description const& type, 
  std::string const& alignment, std::string const& title, 
  std::int32_t left, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "title", title, allocator);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", alignment, allocator);
  add_attribute(result, "font", "~ NormalFont", allocator);
  add_attribute(result, "origin", size_to_string(left, -2), allocator);
  add_attribute(result, "font-color", get_color_name(type.color, color_alpha::opaque), allocator);
  add_attribute(result, "background-color", get_color_name(type.color, color_alpha::half), allocator);
  add_attribute(result, "size", size_to_string(param_total_width, param_row_height + padding_param_group * 4), allocator);
  return result;
}

static Value
build_ui_part_header_container(
  topology_info const& topology, part_type_ui_description const& type, 
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(1, 1), allocator);
  add_attribute(result, "size", size_to_string(part.width - 2, param_row_height - 2 + padding_param_group * 4), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);

  std::int32_t selector_offset = type.selector_param.runtime_param_index == -1? 0: param_col1_width + param_col2_width + margin;
  std::string title = " " + narrow_assume_ascii(topology.parts[part.runtime_part_index].runtime_name);
  if (part.enabled_param.runtime_param_index != -1)
  {
    Value enabled_box = build_ui_param_checkbox(
      topology, type, part, part.enabled_param, black, 2 * margin + selector_offset, header_checkbox_width, 2 * padding_param_group - 1, allocator);
    add_child(result, "CCheckBox", enabled_box, allocator);
    std::string enabled = type.selector_param.runtime_param_index == -1? title: "Enabled";
    add_child(result, "CTextLabel", build_ui_part_header_label(
      topology, type, "left", enabled, header_checkbox_width + 3 * margin + selector_offset, allocator), allocator);
  } else
    add_child(result, "CTextLabel", build_ui_part_header_label(topology, type, "left", title, selector_offset, allocator), allocator);

  std::int32_t info_left = part.width - param_total_width - 2 * margin;
  std::string info = " " + narrow_assume_ascii(topology.parts[part.runtime_part_index].descriptor->ui.info);
  add_child(result, "CTextLabel", build_ui_part_header_label(topology, type, "right", info, info_left, allocator), allocator);
  return result;
}

static Value
build_ui_part_header_container_background(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "background-color-draw-style", "filled", allocator);
  add_attribute(result, "size", size_to_string(part.width, param_row_height + 1 + padding_param_group * 4), allocator);
  add_attribute(result, "background-color", get_color_name(type.color, color_alpha::quarter), allocator);
  return result;
}

static Value
build_ui_part_header_container_border(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "back-color", get_color_name(black, color_alpha::transparent), allocator);
  add_attribute(result, "size", size_to_string(part.width, param_row_height + 1 + padding_param_group * 4), allocator);
  add_attribute(result, "frame-color", get_color_name(type.color, color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_inner_container(
  topology_info const& topology, part_type_ui_description const& type,
  part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "size", size_to_string(part.width, part.height), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);
  add_child(result, "view_container_fix", build_ui_part_header_container_background(topology, type, part, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_header_container_border(topology, type, part, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_header_container(topology, type, part, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_param_container_border(topology, type, part, allocator), allocator);
  add_child(result, "view_container_fix", build_ui_part_param_container(topology, type, part, allocator), allocator);
  return result;
}

static Value
build_ui_part_outer_container(
  topology_info const& topology, part_type_ui_description const& type,
  std::int32_t type_index, part_ui_description const& part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t offset_space = 25;
  std::int32_t offset_x = type_index / 3;
  std::int32_t offset_y = type_index % 3;
  add_attribute(result, "bitmap", "background", allocator);
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "size", size_to_string(part.width, part.height), allocator);
  add_attribute(result, "name", std::to_string(part.runtime_part_index), allocator);
  add_attribute(result, "background-offset", size_to_string(offset_x * offset_space, offset_y * offset_space), allocator);
  add_child(result, "view_container_fix", build_ui_part_inner_container(topology, type, part, allocator), allocator);
  return result;
}

static Value
build_ui_part_switch_container(
  topology_info const& topology, part_type_ui_description const& type, 
  std::int32_t type_index, Value& templates_part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string template_names = "";
  std::vector<std::string> template_names_list;
  std::string tag = get_control_tag(topology, type.selector_param.runtime_param_index);
  for(std::int32_t i = 0; i < type.parts.size(); i++)
  {
    std::string template_name = std::to_string(type.parts[i].runtime_part_index);
    template_names_list.push_back(template_name);
    template_names += template_name;
    if(i < type.parts.size() - 1) template_names += ",";
  }
  add_attribute(result, "animation-time", "0", allocator);
  add_attribute(result, "template-switch-control", tag, allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "template-names", template_names, allocator);
  add_attribute(result, "class", "UIViewSwitchContainer", allocator);
  add_attribute(result, "size", size_to_string(type.width, type.height), allocator);
  for (std::size_t i = 0; i < type.parts.size(); i++)
    add_member(templates_part, template_names_list[i], 
      build_ui_part_outer_container(topology, type, type_index, type.parts[i], allocator), allocator);
  return result;
}

static Value
build_ui_part_type_container(
  topology_info const& topology, part_type_ui_description const& type, 
  std::int32_t type_index, Value& templates_part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::int32_t selector_index = type.selector_param.runtime_param_index;
  add_attribute(result, "class", "view_container_fix", allocator);
  add_attribute(result, "origin", size_to_string(type.left, type.top), allocator);
  add_attribute(result, "size", size_to_string(type.width, type.height), allocator);
  if(selector_index == -1)
    for (std::size_t i = 0; i < type.parts.size(); i++)
      add_child(result, "view_container_fix", build_ui_part_outer_container(topology, type, type_index, type.parts[i], allocator), allocator);
  else
  {
    add_child(result, "UIViewSwitchContainer", build_ui_part_switch_container(topology, type, type_index, templates_part, allocator), allocator);
    add_child(result, "COptionMenu", build_ui_param_menu(topology, type, selector_index, margin, 2 * padding_param_group, allocator), allocator);
  }
  return result;
}

static Value
build_ui_template(
  topology_info const& topology, controller_ui_description const& descriptor, 
  Value& templates_part, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string size = size_to_string(descriptor.width, descriptor.height);
  add_attribute(result, "size", size, allocator);
  add_attribute(result, "minSize", size, allocator);
  add_attribute(result, "maxSize", size, allocator);
  add_attribute(result, "class", "view_container_fix", allocator);
  for (std::size_t type = 0; type < descriptor.part_types.size(); type++)
    add_child(result, "view_container_fix", build_ui_part_type_container(
      topology, descriptor.part_types[type], static_cast<std::int32_t>(type), templates_part, allocator), allocator);
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
  Value templates(kObjectType);
  add_member(templates, "view", build_ui_template(topology, description, templates, allocator), allocator);
  root.AddMember("version", "1", allocator);
  root.AddMember("bitmaps", build_ui_bitmaps(allocator), allocator);
  root.AddMember("colors", build_ui_colors(topology, allocator), allocator);
  root.AddMember("control-tags", build_ui_control_tags(topology, allocator), allocator);
  root.AddMember("templates", templates, allocator);
  result.AddMember("vstgui-ui-description", root, allocator);
  return result;
}

} // namespace svn::vst::ui