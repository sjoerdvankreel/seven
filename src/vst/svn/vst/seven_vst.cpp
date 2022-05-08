#include <pluginterfaces/base/ftypes.h>
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

static std::int32_t svn_module_counter = 0;

SMTG_EXPORT_SYMBOL 
bool InitDll()
{
	if (++svn_module_counter == 1) return InitModule();
	return true;
}

SMTG_EXPORT_SYMBOL 
bool ExitDll()
{
	if(--svn_module_counter == 0) return DeinitModule();
	else if(svn_module_counter < 0) return false;
	else return true;
}

} // extern "C"
