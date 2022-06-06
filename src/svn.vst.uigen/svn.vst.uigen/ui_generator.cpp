#include <svn.base/runtime/runtime_topology.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <cstdint>

using namespace svn::base;
using namespace rapidjson;

struct rgba { std::uint8_t r, g, b, a; };
typedef bool (*svn_init_exit_dll_t)(void);
typedef runtime_topology const*(*svn_get_topology_t)(void);
static Document build_ui_description(runtime_topology const& toplogy);

static rgba const color_wheel[12] =
{
  { 0xFF, 0x00, 0x00, 0xFF },
  { 0xFF, 0x80, 0x00, 0xFF },
  { 0xFF, 0xFF, 0x00, 0xFF },
  { 0x80, 0xFF, 0x00, 0xFF },
  { 0x00, 0xFF, 0x00, 0xFF },
  { 0x00, 0xFF, 0x80, 0xFF },
  { 0x00, 0xFF, 0xFF, 0xFF },
  { 0x00, 0x80, 0xFF, 0xFF },
  { 0x00, 0x00, 0xFF, 0xFF },
  { 0x80, 0x00, 0xFF, 0xFF },
  { 0xFF, 0x00, 0xFF, 0xFF },
  { 0xFF, 0x00, 0x80, 0xFF }
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
  catch(...)
  {
    std::cout << "Failed to write " << argv[2] << ".\n";
    reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
    return 1;
  }

  std::cout << "Ui description written to " << argv[2] << ".\n";
  reinterpret_cast<svn_init_exit_dll_t>(exit_dll)();
  return 0;
}

static Document
build_ui_description(runtime_topology const& toplogy)
{
  Document result;
  result.SetObject();
  Document::AllocatorType& allocator = result.GetAllocator();
  Value description(kObjectType);
  description.AddMember("version", 1, allocator);
  result.AddMember("vstgui-ui-description", description, allocator);
  return result;
}