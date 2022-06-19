#include <svn.vst.base/sdk/graph.hpp>
#include <svn.vst.base/support/bootstrap.hpp>
#include <vstgui/vstgui_uidescription.h>
#include <windows.h>

extern bool InitModule();
extern bool DeinitModule();
void* moduleHandle = nullptr;

static std::int32_t _svn_module_counter = 0;
static VSTGUI::IViewCreator const* _graph_creator;
static svn::base::topology_info const* _topology = nullptr;
 
extern "C" {

svn::base::topology_info const* 
svn_vst_get_topology() { return _topology; }

BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}

SMTG_EXPORT_SYMBOL
bool InitDll()
{
  if (++_svn_module_counter != 1) return true;
  if (!InitModule()) return false;
  _topology = svn_vst_create_topology();
  _graph_creator = new svn::vst::base::graph_creator();
  VSTGUI::UIViewFactory::registerViewCreator(*_graph_creator);
  return true;
}

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
  --_svn_module_counter;
  if (_svn_module_counter > 0) return true;
  if (_svn_module_counter < 0) return false;
  if(!DeinitModule()) return false;
  VSTGUI::UIViewFactory::unregisterViewCreator(*_graph_creator);
  delete _graph_creator;
  _graph_creator = nullptr;
  delete _topology;
  _topology = nullptr;
  return true;
}

} // extern "C"