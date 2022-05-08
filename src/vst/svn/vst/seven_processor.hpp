#ifndef SVN_VST_SEVEN_PROCESSOR_HPP
#define SVN_VST_SEVEN_PROCESSOR_HPP

#include <public.sdk/source/vst/vstaudioeffect.h>

namespace Svn::Vst {

class SevenProcessor : 
public Steinberg::Vst::AudioEffect
{
  using TBool = Steinberg::TBool;
  using int32 = Steinberg::int32;
  using tresult = Steinberg::tresult;
  using IBStream = Steinberg::IBStream;
  using ProcessData = Steinberg::Vst::ProcessData;
  using ProcessSetup = Steinberg::Vst::ProcessSetup;
  using SpeakerArrangement = Steinberg::Vst::SpeakerArrangement;
public:
  SevenProcessor();
  static FUnknown* createInstance(void* context);
public:
	tresult PLUGIN_API setActive(TBool active) override;
	tresult PLUGIN_API setState(IBStream* state) override;
	tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;
  tresult PLUGIN_API setBusArrangements(
    SpeakerArrangement* inputs, int32 inputCount,
    SpeakerArrangement* outputs, int32 outputCount) override;
};

} // namespace Svn::Vst
#endif // SVN_VST_SEVEN_PROCESSOR_HPP