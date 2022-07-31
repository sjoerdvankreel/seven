#include <svn.vst.base/ui/graph_plot.hpp>
#include <svn.vst.base/ui/rotary_knob.hpp>
#include <svn.vst.base/ui/view_container_fix.hpp>
#include <svn.vst.base/support/bootstrap.hpp>
#include <vstgui/vstgui_uidescription.h>

#if WIN32
#include <Windows.h>
void* moduleHandle = nullptr;
#endif

extern bool InitModule();
extern bool DeinitModule();

static std::int32_t _svn_module_counter = 0;
static svn::base::topology_info const* _topology = nullptr;
static VSTGUI::IViewCreator const* _graph_plot_creator = nullptr;
static VSTGUI::IViewCreator const* _rotary_knob_creator = nullptr;
static VSTGUI::IViewCreator const* _view_container_fix_creator = nullptr;
 
extern "C" { 

svn::base::topology_info const* 
svn_vst_get_topology() { return _topology; }

#if WIN32
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}
#endif

SMTG_EXPORT_SYMBOL
bool InitDll()
{
  if (++_svn_module_counter != 1) return true;
  if (!InitModule()) return false;
  _topology = svn_vst_create_topology();
  _graph_plot_creator = new svn::vst::base::graph_plot_creator();
  VSTGUI::UIViewFactory::registerViewCreator(*_graph_plot_creator);
  _rotary_knob_creator = new svn::vst::base::rotary_knob_creator();
  VSTGUI::UIViewFactory::registerViewCreator(*_rotary_knob_creator); 
  _view_container_fix_creator = new svn::vst::base::view_container_fix_creator();
  VSTGUI::UIViewFactory::registerViewCreator(*_view_container_fix_creator);
  return true;
}

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
  --_svn_module_counter;
  if (_svn_module_counter > 0) return true;
  if (_svn_module_counter < 0) return false;
  if(!DeinitModule()) return false; 
  VSTGUI::UIViewFactory::unregisterViewCreator(*_view_container_fix_creator);
  delete _view_container_fix_creator;
  _view_container_fix_creator = nullptr;
  VSTGUI::UIViewFactory::unregisterViewCreator(*_rotary_knob_creator);
  delete _rotary_knob_creator;
  _rotary_knob_creator = nullptr;
  VSTGUI::UIViewFactory::unregisterViewCreator(*_graph_plot_creator);
  delete _graph_plot_creator;
  _graph_plot_creator = nullptr;
  delete _topology;
  _topology = nullptr;
  return true;
}

} // extern "C"