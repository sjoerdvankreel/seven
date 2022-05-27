#ifndef SVN_VST_BASE_SDK_PROCESSOR_HPP
#define SVN_VST_BASE_SDK_PROCESSOR_HPP

#include <pluginterfaces/vst/ivstevents.h>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <public.sdk/source/vst/utility/sampleaccurate.h>
#include <svn.base/dsp/input_buffer.hpp>
#include <svn.base/runtime/runtime_topology.hpp>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/support/param_value.hpp>

#include <memory>
#include <vector>

namespace svn::vst::base {

class processor : 
public Steinberg::Vst::AudioEffect
{
private:
  using TBool = Steinberg::TBool;
  using int32 = Steinberg::int32;
  using tresult = Steinberg::tresult;
  using Event = Steinberg::Vst::Event;
  using IBStream = Steinberg::IBStream;
  using ProcessData = Steinberg::Vst::ProcessData;
  using ProcessSetup = Steinberg::Vst::ProcessSetup;
  using Parameter = Steinberg::Vst::SampleAccurate::Parameter;
  using SpeakerArrangement = Steinberg::Vst::SpeakerArrangement;

public:
  typedef std::unique_ptr<svn::base::audio_processor> (*
    audio_processor_factory)(
      svn::base::runtime_topology const* topology, 
      ProcessSetup const& setup);

private:
  audio_processor_factory _factory;
  std::vector<svn::base::param_value> _state;
  std::vector<Parameter> _accurateParameters;
  svn::base::runtime_topology const* const _topology;
  std::unique_ptr<svn::base::audio_processor> _processor;

public:
  processor(
    Steinberg::FUID controller_id,
    svn::base::runtime_topology const* topology,
    audio_processor_factory factory);

	tresult PLUGIN_API setState(IBStream* state) override;
	tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolic_size) override;
  tresult PLUGIN_API setBusArrangements(
    SpeakerArrangement* inputs, int32 input_count,
    SpeakerArrangement* outputs, int32 output_count) override;

private:
  tresult process_parameters(ProcessData const& data);
  void process_notes(svn::base::input_buffer& input, ProcessData const& data);
  void process_automation(svn::base::input_buffer& input, ProcessData const& data);
  void process_note(svn::base::input_buffer& input, std::int32_t index, Event const& event);
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_PROCESSOR_HPP