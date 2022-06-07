#include <svn.base/static/part_descriptor.hpp>
#include <svn.base/runtime/runtime_topology.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#define NOMINMAX 1
#include <Windows.h>

#include <cstdint>
#include <set>
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

static std::int32_t const margin = 5;
static std::int32_t const item_height = 20;
static std::int32_t const label_width = 40;
static std::int32_t const control_width = 20;

static char const* color_name_whiteFF = "whiteFF";
static char const* color_name_white80 = "white80";
static char const* color_name_blackFF = "blackFF";
static char const* color_name_black80 = "black80";
static char const* color_value_whiteFF = "#FFFFFFFF";
static char const* color_value_white80 = "#FFFFFF80";
static char const* color_value_blackFF = "#000000FF";
static char const* color_value_black80 = "#00000080";

struct rgb { std::uint8_t r, g, b; };
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
  rgb color, bool print_alpha, std::uint8_t alpha);

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

/* -------- topology to descriptor -------- */

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
  std::int32_t runtime_part_index,
  std::int32_t unordered_type_index,
  std::int32_t column, std::int32_t left, std::int32_t top)
{
  part_ui_descriptor result;
  auto const& part = topology.parts[runtime_part_index];
  std::int32_t param_count = part.descriptor->param_count;

  result.top = top;
  result.left = left;
  result.column = column;
  result.runtime_part_index = runtime_part_index;
  result.columns = part.descriptor->ui_control_columns;
  result.rows = param_count / result.columns;
  if(param_count % result.columns != 0) ++result.rows;
  result.color_index = unordered_type_index % color_count;
  result.height = (result.rows + 1) * item_height;
  result.width = (result.columns) * (control_width + label_width);
  
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
  ui_descriptor result;
  std::int32_t top = 0;
  std::int32_t left = 0;
  std::int32_t current_column = 0;
  std::int32_t max_column_height = 0;
  std::int32_t current_column_width = 0;

  for(std::int32_t p = 0; p < topology.static_part_count; p++)
  {
    std::int32_t ui_index = topology.ui_order[p];
    auto const& static_part = topology.static_parts[ui_index];
    for (std::int32_t c = 0; c < static_part.part_count; c++)
    {
      std::int32_t runtime_part_index = topology.part_bounds[ui_index][c];
      part_ui_descriptor descriptor(build_part_ui_descriptor(
        topology, runtime_part_index, p, current_column, left, top));
      if(descriptor.height > topology.max_ui_height)
        throw std::runtime_error("Part height exceeds max ui height.");
      current_column_width = std::max(current_column_width, descriptor.width);
      top += descriptor.height;
      if (top >= topology.max_ui_height)
      {
        if (top > topology.max_ui_height)
        {
          descriptor.column++;
          descriptor.top = 0;
          descriptor.left = descriptor.left + current_column_width;
        }
        top = 0;
        current_column++;
        left += current_column_width;
        current_column_width = 0;
      }
      result.parts.push_back(descriptor);
      max_column_height = std::max(max_column_height, descriptor.top + descriptor.height);
    }
  }

  result.width = 0;
  result.height = max_column_height;
  std::int32_t column_count = 0;
  for(std::size_t p = 0; p < result.parts.size(); p++)
    column_count = std::max(column_count, result.parts[p].column + 1);
  result.column_widths.resize(column_count);
  for(std::int32_t c = 0; c < column_count; c++)
    for(std::size_t p = 0; p < result.parts.size(); p++)
      if(result.parts[p].column == c)
        result.column_widths[c] = std::max(result.column_widths[c], result.parts[p].width);
  for(std::int32_t c = 0; c < column_count; c++)
    result.width += result.column_widths[c];
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

/* -------- descriptor to json -------- */

static std::string 
get_color_name(std::string const& rgb)
{ return "color_" + rgb; }

static std::string 
get_bitmap_name(std::string const& rgb)
{ return "bitmap_" + rgb; }

static std::string
get_color_name(std::int32_t wheel_index, std::uint8_t alpha)
{ return get_color_name(print_rgb_hex(color_wheel[wheel_index], true, alpha)); }

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

static std::string
print_rgb_hex(rgb color, bool print_alpha, std::uint8_t alpha)
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
  oss << ((alpha >> 4) & 0xF);
  oss << (alpha & 0xF);
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

static Value
build_ui_bitmaps(Document::AllocatorType& allocator)
{
  Value result(kObjectType);  
  Value background_value(kObjectType);
  background_value.AddMember("path", "background.png", allocator);
  add_member(result, get_bitmap_name("background"), background_value, allocator);
  for (std::size_t c = 0; c < color_count; c++)
  {
    std::string name = print_rgb_hex(color_wheel[c], false, 0x0);
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
  add_member(result, get_color_name(color_name_whiteFF), color_value_whiteFF, allocator);
  add_member(result, get_color_name(color_name_white80), color_value_white80, allocator);
  add_member(result, get_color_name(color_name_blackFF), color_value_blackFF, allocator);
  add_member(result, get_color_name(color_name_black80), color_value_black80, allocator);
  for(std::size_t c = 0; c < color_count; c++)
  {
    std::string name_opaque = print_rgb_hex(color_wheel[c], true, 0xFF);
    add_member(result, get_color_name(name_opaque), "#" + name_opaque, allocator);
    std::string name_50 = print_rgb_hex(color_wheel[c], true, 0x80);
    add_member(result, get_color_name(name_50), ("#" + name_50), allocator);
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

/*
<view class="CViewContainer" origin="5, 5" size="105, 50" background-color="orange" background-color-draw-style="stroked">
      <view class="CViewContainer" origin="0, 0" size="105, 50" background-color="black_50">
        <view class="CViewContainer" origin="0, 0" size="105, 20" background-color="orange_50">
          <view class="CTextLabel" origin="0, 0" size="105, 20" text-alignment="left" font="~ NormalFontSmall" font-color="white" title=" Amp" transparent="true" />
        </view>
        <view class="CViewContainer" origin="5, 25" size="100, 25" transparent="true">
          <view class="CAnimKnob" origin="0, 0" size="20, 20" control-tag="voice_amp_amp" bitmap="orange" angle-start="20" angle-range="320" height-of-one-image="20"/>
          <view class="CTextLabel" origin="25, 0" size="20, 20" text-alignment="left" font="~ NormalFontSmall" font-color="orange" title="Amp" transparent="true" />
          <view class="CAnimKnob" origin="50, 0" size="20, 20" control-tag="voice_amp_dcy" bitmap="orange" angle-start="20" angle-range="320" height-of-one-image="20"/>
          <view class="CTextLabel" origin="75, 0" size="20, 20" text-alignment="left" font="~ NormalFontSmall" font-color="orange" title="Dcy" transparent="true" />
        </view>
      </view>
    </view>
*/

/*
static Value 
build_ui_part_view(
  Document::AllocatorType& allocator, runtime_part const& part)
{
  Value header_label_attrs(kObjectType);
  header_label_attrs.AddMember("origin", "0, 0", allocator);
  header_label_attrs.AddMember("size", "105, 20", allocator);
  header_label_attrs.AddMember("font-color", "white", allocator);
  header_label_attrs.AddMember("transparent", "true", allocator);
  header_label_attrs.AddMember("text-alignment", "left", allocator);
  header_label_attrs.AddMember("font", "~ NormalFontSmall", allocator);

  Value header_attrs(kObjectType);
  header_attrs.AddMember("origin", "0, 0", allocator);
  header_attrs.AddMember("size", "105, 20", allocator);
  header_attrs.AddMember("background-color", "color_FF000080", allocator);
  


  Value inner_attrs(kObjectType);

  Value inner(kObjectType);

  Value outer_attrs(kObjectType);
  outer_attrs.AddMember("origin", "5, 5", allocator);
  outer_attrs.AddMember("size", "105, 50", allocator);
  outer_attrs.AddMember("background-color", "color_80FF0080", allocator);
  outer_attrs.AddMember("background-color-draw-style", "stroked", allocator);
  Value outer(kObjectType);
  outer.AddMember("attributes", outer_attrs, allocator);
  return outer_attrs;
}

static Value
build_ui_part_views(
  Document::AllocatorType& allocator, runtime_topology const& topology)
{
  Value result(kObjectType);
  for(std::size_t i = 0; i < 1; i++)
    result.AddMember("CViewContainer", build_ui_part_view(allocator, topology.parts[i]), allocator);
  return result;
}

*/

static Value
build_ui_part_inner_container_attrs(
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_member(result, "origin", size_to_string(descriptor.left, descriptor.top), allocator);
  add_member(result, "size", size_to_string(descriptor.width, descriptor.height), allocator);
  add_member(result, "background-color", get_color_name(color_name_black80), allocator);
  return result;
}

static Value
build_ui_part_inner_container(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  result.AddMember("attributes", build_ui_part_inner_container_attrs(descriptor, allocator), allocator);
  Value children(kObjectType);
  //children.AddMember("CViewContainer", build_ui_part_header(topology, descriptor, allocator), allocator);
  result.AddMember("children", children, allocator);
  return result;
}

static Value
build_ui_part_outer_container_attrs(
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  add_member(result, "background-color-draw-style", "stroked", allocator);
  add_member(result, "origin", size_to_string(descriptor.left, descriptor.top), allocator);
  add_member(result, "size", size_to_string(descriptor.width, descriptor.height), allocator);
  add_member(result, "background-color", get_color_name(descriptor.color_index, 0xFF), allocator);
  return result;
}

static Value
build_ui_part_outer_container(runtime_topology const& topology,
  part_ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  result.AddMember("attributes", build_ui_part_outer_container_attrs(descriptor, allocator), allocator);
  Value children(kObjectType);
  children.AddMember("CViewContainer", build_ui_part_inner_container(topology, descriptor, allocator), allocator);
  result.AddMember("children", children, allocator);
  return result;
}

static Value
build_ui_template_attrs(
  ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  std::string size = size_to_string(descriptor.width, descriptor.height);
  add_member(result, "size", size, allocator);
  add_member(result, "minSize", size, allocator);
  add_member(result, "maxSize", size, allocator);
  result.AddMember("class", "CViewContainer", allocator);
  add_member(result, "background-color", color_value_blackFF, allocator);
  return result;
}

static Value
build_ui_template(
  runtime_topology const& topology, 
  ui_descriptor const& descriptor, Document::AllocatorType& allocator)
{  
  Value view(kObjectType);
  view.AddMember("attributes", build_ui_template_attrs(descriptor, allocator), allocator);
  Value children(kObjectType);
  for(std::size_t p = 0; p < descriptor.parts.size(); p++)
    children.AddMember("CViewContainer", build_ui_part_outer_container(topology, descriptor.parts[p], allocator), allocator);
  view.AddMember("children", children, allocator);
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