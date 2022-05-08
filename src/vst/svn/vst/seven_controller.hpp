#ifndef SVN_VST_SEVEN_CONTROLLER_HPP
#define SVN_VST_SEVEN_CONTROLLER_HPP

#include <svn/vst/seven_vst.hpp>
#include <pluginterfaces/vst/vsttypes.h>
#include <vstgui/plugin-bindings/vst3editor.h>
#include <public.sdk/source/vst/vsteditcontroller.h>

namespace Svn::Vst {

class SevenController: 
public Steinberg::Vst::EditController, 
public VSTGUI::VST3EditorDelegate
{
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using IPlugView = Steinberg::IPlugView;
public:
	static FUnknown* createInstance(void* context);
public:
	tresult PLUGIN_API initialize(FUnknown* context) override;
	IPlugView* PLUGIN_API createView(char const* name) override;
	tresult PLUGIN_API setComponentState(IBStream* state) override;
};

} // namespace Svn::Vst
#endif // SVN_VST_SEVEN_CONTROLLER_HPP
