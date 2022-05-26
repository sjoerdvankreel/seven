#ifndef SVN_VST_SDK_CONTROLLER_HPP
#define SVN_VST_SDK_CONTROLLER_HPP

#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

namespace Svn::Vst {

class Controller: 
public Steinberg::Vst::EditControllerEx1
{
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using FUnknown = Steinberg::FUnknown;
  using IPlugView = Steinberg::IPlugView;
public:
	static FUnknown* createInstance(void* context);
public:
	tresult PLUGIN_API initialize(FUnknown* context) override;
	tresult PLUGIN_API setComponentState(IBStream* state) override;
};

inline Steinberg::FUnknown*
Controller::createInstance(void* context)
{ return static_cast<Steinberg::Vst::IEditController*>(new Controller); }

} // namespace Svn::Vst
#endif // SVN_VST_SDK_CONTROLLER_HPP
