#include <svn/support/topo_rt.hpp>
#include <svn/vst/support/ids.hpp>
#include <svn/vst/support/info.hpp>
#include <svn/vst/sdk/processor.hpp>
#include <svn/vst/sdk/controller.hpp>

#include <pluginterfaces/base/ftypes.h>
#include <public.sdk/source/main/pluginfactory.h>
#include <windows.h>
#include <cstdint>

extern bool InitModule();   
extern bool DeinitModule();
void* moduleHandle = nullptr;
  
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}

extern "C" {

static std::int32_t SvnModuleCounter = 0;

SMTG_EXPORT_SYMBOL 
bool InitDll()
{
	if (++SvnModuleCounter != 1) return true;
  if(!InitModule()) return false;
  svn::init_topology();
  return true;
}

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
  --SvnModuleCounter;
  if (SvnModuleCounter > 0) return true;
  if (SvnModuleCounter < 0) return false;
  if(!DeinitModule()) return false;
  svn::destroy_topology();
  return true;
}

} // extern "C"

BEGIN_FACTORY_DEF(SVN_VST_COMPANY_NAME, SVN_VST_COMPANY_WEB, SVN_VST_COMPANY_MAIL)
  DEF_CLASS2(INLINE_UID_FROM_FUID(Svn::Vst::ProcessorId), PClassInfo::kManyInstances,
    kVstAudioEffectClass, SVN_VST_PLUGIN_NAME, Steinberg::Vst::kDistributable, SVN_VST_PLUGIN_CATEGORY,
    SVN_VST_PLUGIN_VERSION, kVstVersionString, Svn::Vst::Processor::createInstance)
  DEF_CLASS2(INLINE_UID_FROM_FUID(Svn::Vst::ControllerId), PClassInfo::kManyInstances,
    kVstComponentControllerClass, SVN_VST_CONTROLLER_NAME, 0, "", SVN_VST_PLUGIN_VERSION,
    kVstVersionString, Svn::Vst::Controller::createInstance)
END_FACTORY