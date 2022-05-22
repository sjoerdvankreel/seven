#include <svn/vst/seven_ids.hpp>
#include <svn/vst/seven_processor.hpp>
#include <svn/vst/seven_controller.hpp>
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
	if (++SvnModuleCounter == 1) return InitModule();
	return true;
}

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
	if(--SvnModuleCounter == 0) return DeinitModule();
	else if(SvnModuleCounter < 0) return false;
	else return true;
}

} // extern "C"

BEGIN_FACTORY_DEF(SVN_VST_COMPANY_NAME, SVN_VST_COMPANY_WEB, SVN_VST_COMPANY_MAIL)
  DEF_CLASS2(INLINE_UID_FROM_FUID(Svn::Vst::ProcessorId), PClassInfo::kManyInstances,
    kVstAudioEffectClass, SVN_VST_PLUGIN_NAME, Steinberg::Vst::kDistributable, SVN_VST_PLUGIN_CATEGORY,
    SVN_VST_PLUGIN_VERSION, kVstVersionString, Svn::Vst::SevenProcessor::createInstance)
  DEF_CLASS2(INLINE_UID_FROM_FUID(Svn::Vst::ControllerId), PClassInfo::kManyInstances,
    kVstComponentControllerClass, SVN_VST_CONTROLLER_NAME, 0, "", SVN_VST_PLUGIN_VERSION,
    kVstVersionString, Svn::Vst::SevenController::createInstance)
END_FACTORY