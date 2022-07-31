#include <svn.vst.ui/generator/generator.hpp>
#include <svn.vst.ui/description/ui_description.hpp>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#if WIN32
#define NOMINMAX 1
#include <Windows.h>
#undef GetObject
#else
#include <dlfcn.h>
#endif

#include <fstream>
#include <iostream>

using namespace rapidjson;
using namespace svn::base;
using namespace svn::vst::ui;

typedef bool (*svn_init_exit_dll_t)(void);
typedef topology_info const* (*svn_get_topology_t)(void);

static void*
load_library(char const* file)
{
#if WIN32
  return LoadLibraryA(file);
#else
  return dlopen(file, RTLD_NOW);
#endif
}

static void*
get_proc_address(void* module, char const* name)
{
#if WIN32
  return GetProcAddress(static_cast<HMODULE>(module), name);
#else
  return dlsym(module, name);
#endif
}

int
main(int argc, char** argv)
{
  void* library;
  void* init_dll;
  void* exit_dll;
  void* get_topology;
  topology_info const* topology;

  if (argc != 3)
    return std::cout << "Usage: seven.vst.ui <full\\path\\to\\plugin.vst3> (dll) <full\\path\\to\\file.uidesc> (json)\n", 1;
  if ((library = load_library(argv[1])) == nullptr)
    return std::cout << "Library " << argv[1] << " not found.\n", 1;
  if ((init_dll = get_proc_address(library, "InitDll")) == nullptr)
    return std::cout << argv[1] << " does not export InitDll.\n", 1;
  if ((exit_dll = get_proc_address(library, "ExitDll")) == nullptr)
    return std::cout << argv[1] << " does not export ExitDll.\n", 1;
  if ((get_topology = get_proc_address(library, "svn_vst_get_topology")) == nullptr)
    return std::cout << argv[1] << " does not export svn_vst_get_topology.\n", 1;
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
    controller_ui_description description = controller_ui_description::create(*topology);
    std::cout << "Parts: " << topology->parts.size() << ".\n";
    std::cout << "Parameters: " << topology->params.size() << ".\n";
    description.print(*topology, std::cout);
    Document json(build_vstgui_json(*topology, description));
    std::ofstream os(argv[2]);
    OStreamWrapper wrapper(os);
    PrettyWriter<OStreamWrapper> writer(wrapper);
    json.Accept(writer);
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
