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
// Parts are layed out top-to-bottom, then left-to-right, and are 
// wrapped based on the max ui height. Parameters are layed out in 
// a grid, based on the control column count in the part descriptor.

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
  if ((init_dll = GetProcAddress(library, "InitDll")) == nullptr)
    return std::cout << argv[1] << " does not export InitDll.\n", 1;
  if ((exit_dll = GetProcAddress(library, "ExitDll")) == nullptr)
    return std::cout << argv[1] << " does not export ExitDll.\n", 1;
  if ((get_topology = GetProcAddress(library, "svn_get_topology")) == nullptr)
    return std::cout << argv[1] << " does not export svn_get_topology.\n", 1;
  if (!reinterpret_cast<svn_init_exit_dll_t>(init_dll)())
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
  catch (std::exception const& e)
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

/* -------- ui descriptor to json support -------- */



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

