#include <svn.base/runtime/runtime_topology.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#include <Windows.h>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace svn::base;
using namespace rapidjson;

struct rgb { std::uint8_t r, g, b; };
typedef bool (*svn_init_exit_dll_t)(void);
typedef runtime_topology const*(*svn_get_topology_t)(void);
static Document build_ui_description(runtime_topology const& toplogy);

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

  std::cout << "Parts: " << topology->parts.size() << ".\n";
  std::cout << "Input parameters: " << topology->params.size() << ".\n";
  std::cout << "Output parameters: " << topology->output_param_count << ".\n";
  
  Document ui_description(build_ui_description(*topology));
  try
  {
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
    std::cout << e.what() << "\n.";
    std::cout << "Failed to write " << argv[2] << ".\n";
    reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
    return 1;
  }

  std::cout << "Ui description written to " << argv[2] << ".\n";
  reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
  return 0;
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

static Value
build_ui_bitmaps(Document::AllocatorType& allocator)
{
  Value result(kObjectType);
  for (std::size_t c = 0; c < color_count; c++)
  {
    std::string name = print_rgb_hex(color_wheel[c], false, 0x0);
    Value key(name.c_str(), allocator);
    Value value(kObjectType);
    Value path((name + ".png").c_str(), allocator);
    value.AddMember("path", path, allocator);
    result.AddMember(key, value, allocator);
  }
  return result;
}

static Value
build_ui_colors(Document::AllocatorType& allocator)
{
  Value result(kObjectType);

  std::string white_opaque = print_rgb_hex(white, true, 0xFF);
  Value white_opaque_key(white_opaque.c_str(), allocator);
  Value white_opaque_value(("#" + white_opaque).c_str(), allocator);
  result.AddMember(white_opaque_key, white_opaque_value, allocator);

  std::string white_50 = print_rgb_hex(white, true, 0x80);
  Value white_50_key(white_50.c_str(), allocator);
  Value white_50_value(("#" + white_50).c_str(), allocator);
  result.AddMember(white_50_key, white_50_value, allocator);

  std::string black_opaque = print_rgb_hex(black, true, 0xFF);
  Value black_opaque_key(black_opaque.c_str(), allocator);
  Value black_opaque_value(("#" + black_opaque).c_str(), allocator);
  result.AddMember(black_opaque_key, black_opaque_value, allocator);

  std::string black_50 = print_rgb_hex(black, true, 0x80);
  Value black_50_key(black_50.c_str(), allocator);
  Value black_50_value(("#" + black_50).c_str(), allocator);
  result.AddMember(black_50_key, black_50_value, allocator);

  for(std::size_t c = 0; c < color_count; c++)
  {
    std::string name_opaque = print_rgb_hex(color_wheel[c], true, 0xFF);
    Value key_opaque(name_opaque.c_str(), allocator);
    Value value_opaque(("#" + name_opaque).c_str(), allocator);
    result.AddMember(key_opaque, value_opaque, allocator);

    std::string name_50 = print_rgb_hex(color_wheel[c], true, 0x80);
    Value key_50(name_50.c_str(), allocator);
    Value value_50(("#" + name_50).c_str(), allocator);
    result.AddMember(key_50, value_50, allocator);
  }
  return result;
}

static Value
build_ui_control_tags(
  Document::AllocatorType& allocator, runtime_topology const& topology)
{
  Value result(kObjectType);
  for (std::size_t p = 0; p < topology.params.size(); p++)
  {
    std::string name = narrow_assume_ascii(topology.params[p].runtime_name);
    Value key(name.c_str(), allocator);
    Value value(std::to_string(p).c_str(), allocator);
    result.AddMember(key, value, allocator);
  }
  return result;
}

static Document
build_ui_description(runtime_topology const& topology)
{
  Document result;
  result.SetObject();
  Document::AllocatorType& allocator = result.GetAllocator();
  Value description(kObjectType);
  description.AddMember("version", 1, allocator);
  description.AddMember("bitmaps", build_ui_bitmaps(allocator), allocator);
  description.AddMember("colors", build_ui_colors(allocator), allocator);
  description.AddMember("control-tags", build_ui_control_tags(allocator, topology), allocator);
  result.AddMember("vstgui-ui-description", description, allocator);
  return result;
}