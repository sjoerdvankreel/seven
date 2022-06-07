#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/static/param_descriptor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#define NOMINMAX 1
#include <Windows.h>
#undef GetObject

#include <set>
#include <cwchar>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace svn::base;
using namespace rapidjson;

// Used for dimension calculations.
// Parts are layed out top-to-bottom, then left-to-right, in topology.ui_order,
// and are wrapped based on the max ui height. Parameters are layed out in a grid, 
// in declaration order, based on the control column count in the part descriptor.

struct param_ui_descriptor
{
  std::int32_t row;
  std::int32_t column;
  std::int32_t runtime_param_index;
};

struct part_ui_descriptor
{
  std::int32_t top;
  std::int32_t left;
  std::int32_t width;
  std::int32_t height;
  std::int32_t rows;
  std::int32_t column;
  std::int32_t columns;
  std::int32_t color_index; // Cycles through the color wheel.
  std::int32_t runtime_part_index;
  std::vector<param_ui_descriptor> params;
};

struct ui_descriptor
{
  std::int32_t width;
  std::int32_t height;
  std::vector<part_ui_descriptor> parts;
  std::vector<std::int32_t> column_widths;
}; 

static std::int32_t const margin = 3;
static std::int32_t const edit_width = 28;
static std::int32_t const item_height = 20;
static std::int32_t const label_width = 26;
static std::int32_t const control_width = 20;

struct color_alpha_t { enum value { quarter, half, opaque, count }; };
typedef color_alpha_t::value color_alpha;
static std::uint8_t const
color_alphas[color_alpha::count] = { 0x40, 0x80, 0xFF };

struct rgb { std::uint8_t r, g, b; };
static rgb white = { 0xFF, 0xFF, 0xFF };
static rgb black = { 0x00, 0x00, 0x00 };
static std::size_t const color_count = 12;
static rgb const color_wheel[color_count] =
{
  { 0xFF, 0x00, 0x00 },
  { 0xFF, 0x80, 0x00 },
  { 0xFF, 0xFF, 0x00 },
  { 0x80, 0xFF, 0x00 },
  { 0x00, 0xFF, 0x00 },
  { 0x00, 0xFF, 0x80 },
  { 0x00, 0xFF, 0xFF },
  { 0x00, 0x80, 0xFF },
  { 0x00, 0x00, 0xFF },
  { 0x80, 0x00, 0xFF },
  { 0xFF, 0x00, 0xFF },
  { 0xFF, 0x00, 0x80 }
};

typedef bool (*svn_init_exit_dll_t)(void);
typedef runtime_topology const* (*svn_get_topology_t)(void);

static ui_descriptor build_ui_descriptor(
  runtime_topology const& topology);
static std::string print_rgb_hex(
  rgb color, bool print_alpha, std::int32_t alpha_index);
static void print_ui_descriptor(
  runtime_topology const& topology, ui_descriptor const& descriptor);
static Document build_ui_description(
  runtime_topology const& topology, ui_descriptor const& descriptor);

/* -------- driver -------- */

// Builds vst3 .uidesc file based on topology.
// We use the rapidjson distribution included with the vst3 sdk.
int 
main(int argc, char** argv)
{
  HMODULE library;
  FARPROC init_dll;
  FARPROC exit_dll;
  FARPROC get_topology;
  runtime_topology const* topology;

  if (argc != 3) 
    return std::cout << "Usage: ui_generator <full\\path\\to\\plugin.vst3> (dll) <full\\path\\to\\file.uidesc> (json)\n", 1;
  if ((library = LoadLibraryA(argv[1])) == nullptr) 
    return std::cout << "Library " << argv[1] << " not found.\n", 1;
  if((init_dll = GetProcAddress(library, "InitDll")) == nullptr)
    return std::cout << argv[1] << " does not export InitDll.\n", 1;
  if ((exit_dll = GetProcAddress(library, "ExitDll")) == nullptr)
    return std::cout << argv[1] << " does not export ExitDll.\n", 1;
  if ((get_topology = GetProcAddress(library, "svn_get_topology")) == nullptr)
    return std::cout << argv[1] << " does not export svn_get_topology.\n", 1;
  if(!reinterpret_cast<svn_init_exit_dll_t>(init_dll)())
    return std::cout << "InitDll returned false.\n", 1;

  if ((topology = reinterpret_cast<svn_get_topology_t>(get_topology)()) == nullptr)
  {
    std::cout << "svn_get_topology returned NULL.\n";
    reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
    return 1;
  }
  
  try
  {
    ui_descriptor descriptor = build_ui_descriptor(*topology);
    std::cout << "Parts: " << topology->parts.size() << ".\n";
    std::cout << "Parameters: " << topology->params.size() << ".\n";
    print_ui_descriptor(*topology, descriptor);
    Document ui_description(build_ui_description(*topology, descriptor));
    std::ofstream os(argv[2]);
    OStreamWrapper wrapper(os);
    PrettyWriter<OStreamWrapper> writer(wrapper);
    ui_description.Accept(writer);
    if (os.bad())
    {
      std::cout << "Failed to write " << argv[2] << ".\n";
      reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
      return 1;
    }
    os.flush();
    os.close();
  }
  catch(std::exception const& e)
  {
    std::cout << e.what() << "\n";
    std::cout << "Failed to write " << argv[2] << ".\n";
    reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
    return 1;
  }

  std::cout << "Ui description written to " << argv[2] << ".\n";
  reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
  return 0;
}

/* -------- topology to ui descriptor -------- */

static param_ui_descriptor
build_param_ui_descriptor(
  std::int32_t row, 
  std::int32_t column, 
  std::int32_t runtime_param_index)
{
  param_ui_descriptor result;
  result.row = row;
  result.column = column;
  result.runtime_param_index = runtime_param_index;
  return result;
}

static part_ui_descriptor
build_part_ui_descriptor(
  runtime_topology const& topology, 
  std::int32_t runtime_part_index, std::int32_t unordered_type_index)
{
  part_ui_descriptor result;
  auto const& part = topology.parts[runtime_part_index];
  std::int32_t param_count = part.descriptor->param_count;
 
  result.runtime_part_index = runtime_part_index;
  result.columns = part.descriptor->ui_control_columns;
  result.rows = param_count / result.columns;
  if(param_count % result.columns != 0) ++result.rows;
  result.color_index = (topology.ui_color_start_index + 
    unordered_type_index * topology.ui_color_cycle_step) % color_count;
  result.height = (result.rows + 1) * (item_height + margin);
  result.width = (result.columns) * (control_width + label_width + edit_width + 2 * margin) + margin;
  
  for(std::int32_t i = 0; i < part.descriptor->param_count; i++)
  {
    std::int32_t row = i / result.columns;
    std::int32_t column = i % result.columns;
    result.params.push_back(build_param_ui_descriptor(
      row, column, part.runtime_param_start + i));
  }

  return result;
}

static ui_descriptor
build_ui_descriptor(runtime_topology const& topology)
{
  // Build part list.
  ui_descriptor result;
  for(std::int32_t p = 0; p < topology.static_part_count; p++)
  {
    std::int32_t ui_index = topology.ui_order[p];
    auto const& static_part = topology.static_parts[ui_index];
    for (std::int32_t c = 0; c < static_part.part_count; c++)
    {
      std::int32_t runtime_part_index = topology.part_bounds[ui_index][c];
      part_ui_descriptor descriptor(build_part_ui_descriptor(topology, runtime_part_index, p));
      if(descriptor.height + 2 * margin > topology.max_ui_height)
        throw std::runtime_error("Part height exceeds max ui height.");
      result.parts.push_back(descriptor);
    }
  }

  // Fix up top/left/column.
  std::int32_t column = 0;
  std::int32_t top = margin;
  std::int32_t left = margin;
  std::int32_t column_width = 0;
  std::int32_t max_column_height = 0;
  for (std::size_t p = 0; p < result.parts.size(); p++)
  {
    auto& part = result.parts[p];
    if (top + part.height + margin > topology.max_ui_height)
    {
      max_column_height = std::max(top, max_column_height);
      column++;
      top = margin;
      left += column_width + margin;
      result.column_widths.push_back(column_width);
      column_width = 0;
    }

    part.top = top;
    part.left = left;
    part.column = column;
    column_width = std::max(column_width, part.width);
    top += part.height + margin;
  }
  result.column_widths.push_back(column_width);
  max_column_height = std::max(top, max_column_height);

  // Fix up column width.
  result.width = margin;
  for(std::size_t c = 0; c < result.column_widths.size(); c++)
    result.width += result.column_widths[c] + margin;
  for (std::size_t p = 0; p < result.parts.size(); p++)
    result.parts[p].width = result.column_widths[result.parts[p].column];

  // Fix up last block height.
  result.height = max_column_height;
  for (std::size_t p = 0; p < result.parts.size(); p++)
    if (p == result.parts.size() - 1 || result.parts[p].column != result.parts[p + 1].column)
      result.parts[p].height = result.height - result.parts[p].top - margin;

  return result;
}

static void
print_ui_descriptor(
  runtime_topology const& topology,
  ui_descriptor const& descriptor)
{
  std::cout << "UI descriptor:\n";
  std::cout << "\tWidth: " << descriptor.width << "\n";
  std::cout << "\tHeight: " << descriptor.height << "\n";
  std::cout << "\tColumn widths: ";
  for (std::size_t c = 0; c < descriptor.column_widths.size(); c++)
    std::cout << descriptor.column_widths[c] << " ";
  std::cout << "\n";
  for (std::size_t rp = 0; rp < descriptor.parts.size(); rp++)
  {
    std::int32_t rt_index = descriptor.parts[rp].runtime_part_index;
    std::cout << "\tPart " << (rp + 1) << ":\n";
    std::wcout << L"\t\tName: " << topology.parts[rt_index].runtime_name << L"\n";
    std::cout << "\t\tColumn: " << descriptor.parts[rp].column << "\n";
    std::cout << "\t\tParams: " << descriptor.parts[rp].params.size() << "\n";
    std::cout << "\t\tColumns: " << descriptor.parts[rp].columns << "\n";
    std::cout << "\t\tRows: " << descriptor.parts[rp].rows << "\n";
    std::cout << "\t\tWidth: " << descriptor.parts[rp].width << "\n";
    std::cout << "\t\tHeight: " << descriptor.parts[rp].height << "\n";
    std::cout << "\t\tLeft: " << descriptor.parts[rp].left << "\n";
    std::cout << "\t\tTop: " << descriptor.parts[rp].top << "\n";
    std::cout << "\t\tColor index: " << descriptor.parts[rp].color_index << "\n";
  }
  std::cout << "\n";
}

/* -------- ui descriptor to json support -------- */

static std::string 
get_bitmap_name(std::string const& rgb)
{ return "bitmap_" + rgb; }

static std::string
get_color_value(rgb color, std::int32_t alpha_index)
{ return "#" + print_rgb_hex(color, true, alpha_index); }

static std::string
get_color_name(rgb color, std::int32_t alpha_index)
{ return "color_" + print_rgb_hex(color, true, alpha_index); }

static std::string
get_param_control_class(
  runtime_topology const& topology,
  param_ui_descriptor const& param)
{
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::toggle: return "CCheckBox";
  case param_type::list: return "COptionMenu";
  case param_type::real: 
  case param_type::discrete: 
  case param_type::discrete_list:
  return "CAnimKnob";
  default: assert(false); return "";
  }
}

static std::string
size_to_string(std::int32_t w, std::int32_t h)
{
  std::string result;
  result += std::to_string(w);
  result += ", ";
  result += std::to_string(h);
  return result;
}

static Value&
add_member(Value& container, std::string const& key, 
  Value const& value, Document::AllocatorType& allocator)
{
  Value key_copy(key.c_str(), allocator);
  Value value_copy(value, allocator);
  return container.AddMember(key_copy, value_copy, allocator);
}

static Value&
add_member(Value& container, std::string const& key, 
  std::string const& value, Document::AllocatorType& allocator)
{
  Value key_copy(key.c_str(), allocator);
  Value value_copy(value.c_str(), allocator);
  return container.AddMember(key_copy, value_copy, allocator);
}

static void
add_child(
  Value& container, std::string const& key,
  Value const& value, Document::AllocatorType& allocator)
{
  if (!container.HasMember("children"))
  {
    Value children(kObjectType);
    container.AddMember("children", children, allocator);
  }
  add_member(container["children"], key, value, allocator);
}

static void
add_attribute(
  Value& container, std::string const& key, 
  std::string const& value, Document::AllocatorType& allocator)
{
  if (!container.HasMember("attributes"))
  {
    Value attrs(kObjectType);
    container.AddMember("attributes", attrs, allocator);
  }
  add_member(container["attributes"], key, value, allocator);
}

static std::string
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
  if(!print_alpha) return oss.str();
  oss << ((color_alphas[alpha_index] >> 4) & 0xF);
  oss << (color_alphas[alpha_index] & 0xF);
  return oss.str();
}

static std::string 
narrow_assume_ascii(std::wstring const& wide)
{
  std::string result;
  for (std::size_t c = 0; c < wide.length(); c++)
  {
    if (wide[c] > 127)
    {
      std::string error("Unexpected character: ");
      error += std::to_string(static_cast<int>(wide[c]));
      error += ".";
      throw std::runtime_error(error);
    }
    result += static_cast<char>(wide[c]);
  }
  return result;
}

static std::string
get_control_tag(std::wstring const& runtime_param_name)
{
  std::string result = narrow_assume_ascii(runtime_param_name);
  for (std::size_t c = 0; c < result.length(); c++)
  {
    if(std::isspace(result[c])) result[c] = '_';
    else result[c] = static_cast<char>(std::tolower(result[c]));
  }
  return result;
}

/* -------- ui descriptor to json -------- */

static Value
build_ui_bitmaps(Document::AllocatorType& allocator)
{
  Value result(kObjectType);  
  Value background_value(kObjectType);
  background_value.AddMember("path", "background.png", allocator);
  add_member(result, get_bitmap_name("background"), background_value, allocator);
  for (std::size_t c = 0; c < color_count; c++)
  {
    std::string name = print_rgb_hex(color_wheel[c], false, color_alpha::opaque);
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
      add_member(result, get_color_name(color_wheel[c], a), get_color_value(color_wheel[c], a), allocator);
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
build_ui_param_control_base(
  runtime_topology const& topology, part_ui_descriptor const& part,
  param_ui_descriptor const& param, std::int32_t offset_x, std::int32_t width,
  Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", get_param_control_class(topology, param), allocator);
  add_attribute(result, "size", size_to_string(width - offset_x, item_height), allocator);
  std::string tag = get_control_tag(topology.params[param.runtime_param_index].runtime_name);
  add_attribute(result, "control-tag", tag, allocator);
  std::int32_t top = margin + param.row * (item_height + margin);
  std::int32_t left = margin + param.column * (control_width + label_width + edit_width + 2 * margin) + offset_x;
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  auto const& rt_param = topology.params[param.runtime_param_index];
  std::string tooltip = narrow_assume_ascii(rt_param.runtime_name);
  std::string unit = std::string(" (") + narrow_assume_ascii(rt_param.descriptor->unit) + ")";
  if (std::wcslen(rt_param.descriptor->unit) > 0) tooltip += unit;
  add_attribute(result, "tooltip", tooltip, allocator);
  return result;
}

static Value
build_ui_param_knob(
  runtime_topology const& topology, part_ui_descriptor const& part, 
  param_ui_descriptor const& param, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_control_base(topology, part, param, 0, control_width, allocator));
  add_attribute(result, "angle-start", "20", allocator);
  add_attribute(result, "angle-range", "320", allocator);
  add_attribute(result, "height-of-one-image", std::to_string(item_height), allocator);
  std::string bitmap = print_rgb_hex(color_wheel[part.color_index], false, 0x00);
  add_attribute(result, "bitmap", get_bitmap_name(bitmap), allocator);
  return result;
}

static Value
build_ui_param_checkbox(
  runtime_topology const& topology, part_ui_descriptor const& part,
  param_ui_descriptor const& param, Document::AllocatorType& allocator)
{
  Value result(build_ui_param_control_base(topology, part, param, 3, control_width, allocator));
  add_attribute(result, "frame-width", "1", allocator);
  add_attribute(result, "draw-crossbox", "true", allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "boxfill-color", get_color_name(color_wheel[part.color_index], color_alpha::quarter), allocator);
  add_attribute(result, "boxframe-color", get_color_name(color_wheel[part.color_index], color_alpha::opaque), allocator);
  add_attribute(result, "checkmark-color", get_color_name(color_wheel[part.color_index], color_alpha::opaque), allocator);
  return result;
}

static Value
build_ui_param_menu(
  runtime_topology const& topology, part_ui_descriptor const& part,
  param_ui_descriptor const& param, Document::AllocatorType& allocator)
{
  auto const& descriptor = *topology.params[param.runtime_param_index].descriptor;
  Value result(build_ui_param_control_base(topology, part, param, 0, control_width + label_width + edit_width + margin, allocator));
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
  add_attribute(result, "font-color", get_color_name(color_wheel[part.color_index], color_alpha::opaque), allocator);
  add_attribute(result, "back-color", get_color_name(color_wheel[part.color_index], color_alpha::quarter), allocator);
  add_attribute(result, "shadow-color", get_color_name(color_wheel[part.color_index], color_alpha::half), allocator);
  return result;
}

static Value
build_ui_param_label(
  runtime_topology const& topology, part_ui_descriptor const& part,
  param_ui_descriptor const& param, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "size", size_to_string(label_width, item_height), allocator);
  add_attribute(result, "font-color", get_color_name(color_wheel[part.color_index], color_alpha::opaque), allocator);
  std::int32_t top = margin + param.row * (item_height + margin);
  std::int32_t left = margin + param.column * (control_width + label_width + edit_width + 2 * margin) + control_width + margin;
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  std::string title = narrow_assume_ascii(topology.params[param.runtime_param_index].descriptor->static_name.short_);
  add_attribute(result, "title", title, allocator);
  return result;
}

static Value
build_ui_param_edit(
  runtime_topology const& topology, part_ui_descriptor const& part,
  param_ui_descriptor const& param, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CTextEdit", allocator);
  add_attribute(result, "text-alignment", "right", allocator);
  add_attribute(result, "style-no-frame", "true", allocator);
  add_attribute(result, "style-round-rect", "true", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "text-inset", size_to_string(margin, 0), allocator);
  add_attribute(result, "round-rect-radius", std::to_string(margin), allocator);
  add_attribute(result, "size", size_to_string(edit_width, item_height), allocator);
  add_attribute(result, "font-color", get_color_name(color_wheel[part.color_index], color_alpha::opaque), allocator);
  add_attribute(result, "back-color", get_color_name(color_wheel[part.color_index], color_alpha::quarter), allocator);
  std::string tag = get_control_tag(topology.params[param.runtime_param_index].runtime_name);
  add_attribute(result, "control-tag", tag, allocator);
  std::int32_t top = margin + param.row * (item_height + margin);
  std::int32_t left = margin + param.column * (control_width + label_width + edit_width + 2 * margin) + control_width + label_width + margin;
  add_attribute(result, "origin", size_to_string(left, top), allocator);
  return result;
}

static void
add_ui_param(
  runtime_topology const& topology, part_ui_descriptor const& part, 
  Value& container, std::size_t index, Document::AllocatorType& allocator)
{
  param_ui_descriptor const& param = part.params[index];
  std::string control_class = get_param_control_class(topology, param);
  switch (topology.params[param.runtime_param_index].descriptor->type)
  {
  case param_type::real:
  case param_type::discrete:
  case param_type::discrete_list:
    add_child(container, control_class, build_ui_param_knob(topology, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, allocator), allocator);
    add_child(container, "CTextEdit", build_ui_param_edit(topology, part, param, allocator), allocator);
    break;
  case param_type::toggle:
    add_child(container, control_class, build_ui_param_checkbox(topology, part, param, allocator), allocator);
    add_child(container, "CTextLabel", build_ui_param_label(topology, part, param, allocator), allocator);
    break;
  case param_type::list:
    add_child(container, control_class, build_ui_param_menu(topology, part, param, allocator), allocator);
    break;
  default:
    assert(false);
  }
}

static Value
build_ui_part_param_container(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(0, item_height), allocator);
  add_attribute(result, "size", size_to_string(descriptor.width, descriptor.height - item_height), allocator);
  for(std::size_t p = 0; p < descriptor.params.size(); p++)
    add_ui_param(topology, descriptor, result, p, allocator);
  return result;
}

static Value
build_ui_part_header_label(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string title = narrow_assume_ascii(topology.parts[descriptor.runtime_part_index].runtime_name);
  add_attribute(result, "title", title, allocator);
  add_attribute(result, "class", "CTextLabel", allocator);
  add_attribute(result, "transparent", "true", allocator);
  add_attribute(result, "text-alignment", "left", allocator);
  add_attribute(result, "font", "~ NormalFontSmall", allocator);
  add_attribute(result, "font-color", get_color_name(white, color_alpha::opaque), allocator);
  add_attribute(result, "origin", size_to_string(margin, 0), allocator);
  add_attribute(result, "size", size_to_string(descriptor.width, item_height), allocator);
  add_attribute(result, "background-color", get_color_name(color_wheel[descriptor.color_index], color_alpha::half), allocator);
  return result;
}

static Value
build_ui_part_header_container(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "size", size_to_string(descriptor.width, item_height), allocator);
  add_attribute(result, "background-color", get_color_name(color_wheel[descriptor.color_index], color_alpha::half), allocator);
  add_child(result, "CTextLabel", build_ui_part_header_label(topology, descriptor, allocator), allocator);
  return result;
}

static Value
build_ui_part_inner_container(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "origin", size_to_string(0, 0), allocator);
  add_attribute(result, "size", size_to_string(descriptor.width, descriptor.height), allocator);
  add_attribute(result, "background-color", get_color_name(black, color_alpha::half), allocator);
  add_child(result, "CViewContainer", build_ui_part_header_container(topology, descriptor, allocator), allocator);
  add_child(result, "CViewContainer", build_ui_part_param_container(topology, descriptor, allocator), allocator);
  return result;
}

static Value
build_ui_part_outer_container(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_attribute(result, "class", "CViewContainer", allocator);
  add_attribute(result, "background-color-draw-style", "stroked", allocator);
  add_attribute(result, "origin", size_to_string(descriptor.left, descriptor.top), allocator);
  add_attribute(result, "size", size_to_string(descriptor.width, descriptor.height), allocator);
  add_attribute(result, "background-color", get_color_name(color_wheel[descriptor.color_index], color_alpha::opaque), allocator);
  add_child(result, "CViewContainer", build_ui_part_inner_container(topology, descriptor, allocator), allocator);
  return result;
}

static Value
build_ui_template(runtime_topology const& topology, 
  ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{  
  Value view(kObjectType);
  std::string size = size_to_string(descriptor.width, descriptor.height);
  add_attribute(view, "size", size, allocator);
  add_attribute(view, "minSize", size, allocator);
  add_attribute(view, "maxSize", size, allocator);
  add_attribute(view, "class", "CViewContainer", allocator);
  add_attribute(view, "bitmap", get_bitmap_name("background"), allocator);
  for (std::size_t p = 0; p < descriptor.parts.size(); p++)
    add_child(view, "CViewContainer", build_ui_part_outer_container(topology, descriptor.parts[p], allocator), allocator);
  Value result(kObjectType);
  result.AddMember("view", view, allocator);
  return result;
}

static Document
build_ui_description(
  runtime_topology const& topology, ui_descriptor const& descriptor)
{
  Document result;
  result.SetObject();
  Document::AllocatorType& allocator = result.GetAllocator();
  Value description(kObjectType);
  description.AddMember("version", "1", allocator);
  description.AddMember("bitmaps", build_ui_bitmaps(allocator), allocator);
  description.AddMember("colors", build_ui_colors(allocator), allocator);
  description.AddMember("control-tags", build_ui_control_tags(topology, allocator), allocator);
  description.AddMember("templates", build_ui_template(topology, descriptor, allocator), allocator);
  result.AddMember("vstgui-ui-description", description, allocator);
  return result;
}