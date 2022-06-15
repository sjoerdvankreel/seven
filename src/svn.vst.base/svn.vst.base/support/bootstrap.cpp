#include <pluginterfaces/base/ftypes.h>
#include <public.sdk/source/main/pluginfactory.h>
#include <svn.vst.base/sdk/processor.hpp>
#include <svn.vst.base/sdk/controller.hpp>
#include <svn.vst.base/support/bootstrap.hpp>

#include <cstdint>
#include <windows.h>

using namespace svn::base;
using namespace svn::vst::base;

extern bool InitModule();
extern bool DeinitModule();
void* moduleHandle = nullptr;

static std::int32_t svn_module_counter = 0;
static topology_info _topology;
  
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}

extern "C" {

// Ui generator support.
SMTG_EXPORT_SYMBOL
svn::base::topology_info const*
svn_get_topology()
{ return &_topology; }

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
  --svn_module_counter;
  if (svn_module_counter > 0) return true;
  if (svn_module_counter < 0) return false;
  if(!DeinitModule()) return false;
  _topology = topology_info();
  return true;
}

SMTG_EXPORT_SYMBOL
bool InitDll()
{
  if (++svn_module_counter != 1) return true;
  if (!InitModule()) return false;
  _topology = init_create_topology();
  return true;
}

} // extern "C"

namespace svn::vst::base {

Steinberg::FUnknown*
controller_factory(void* context)
{
  auto result = new controller(&_topology);
  return static_cast<Steinberg::Vst::IEditController*>(result);
}

Steinberg::FUnknown*
processor_factory(void* context)
{ 
  auto result = new processor(&_topology, init_get_controller_id());
  return static_cast<Steinberg::Vst::IAudioProcessor*>(result); 
}

} // namespace svn::vst::base