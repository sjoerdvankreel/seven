#include <svn.vst.base/sdk/graph.hpp>
#include <svn.vst.base/support/bootstrap.hpp>
#include <vstgui/vstgui_uidescription.h>
#include <windows.h>

extern bool InitModule();
extern bool DeinitModule();
void* moduleHandle = nullptr;

static std::int32_t _svn_module_counter = 0;
static svn::base::topology_info const* _topology = nullptr;
 
extern "C" {

svn::base::topology_info const* 
svn_vst_get_topology() { return _topology; }

SMTG_EXPORT_SYMBOL
bool InitDll()
{
  if (++_svn_module_counter != 1) return true;
  if (!InitModule()) return false;
  _topology = svn_vst_create_topology();
  VSTGUI::UIViewFactory::registerViewCreator(svn::vst::base::graph_factory());
  return true;
}

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
  --_svn_module_counter;
  if (_svn_module_counter > 0) return true;
  if (_svn_module_counter < 0) return false;
  if(!DeinitModule()) return false;
  delete _topology;
  _topology = nullptr;
  VSTGUI::UIViewFactory::unregisterViewCreator(svn::vst::base::graph_factory());
  return true;
}

BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}

} // extern "C"