#ifndef SVN_VST_BASE_SDK_PROCESSOR_HPP
#define SVN_VST_BASE_SDK_PROCESSOR_HPP

#include <pluginterfaces/vst/ivstevents.h>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <public.sdk/source/vst/utility/sampleaccurate.h>

#include <svn.base/dsp/io_types.hpp>
#include <svn.base/dsp/audio_processor.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <memory>
#include <vector>

namespace svn::vst::base {

// Vst3 processor wrapping svn::base::audio_processor.
class processor : 
public Steinberg::Vst::AudioEffect
{
private:
  using FUID = Steinberg::FUID;
  using TBool = Steinberg::TBool;
  using int32 = Steinberg::int32;
  using tresult = Steinberg::tresult;
  using Event = Steinberg::Vst::Event;
  using IBStream = Steinberg::IBStream;
  using ProcessData = Steinberg::Vst::ProcessData;
  using ProcessSetup = Steinberg::Vst::ProcessSetup;
  using Parameter = Steinberg::Vst::SampleAccurate::Parameter;
  using SpeakerArrangement = Steinberg::Vst::SpeakerArrangement;

private:
  // State of all parameters. 
  // This fully defines the audio_processor and thus the plugin state.
  std::vector<svn::base::param_value> _state;
  // For sample accurate.
  std::vector<Parameter> _accurate_parameters;
  // Don't update output too often.
  std::int64_t _previous_block_end_samples = 0;
  std::int64_t _output_param_update_samples = 0;
  // Need topology for parameter dsp bounds etc.
  svn::base::topology_info const* const _topology;
  // Where it happens. We just translate automation and audio values from/to vst3 format.
  std::unique_ptr<svn::base::audio_processor> _processor;

public:
  processor(svn::base::topology_info const* topology, FUID controller_id);

	tresult PLUGIN_API setState(IBStream* state) override;
	tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolic_size) override;
  tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, 
    int32 input_count, SpeakerArrangement* outputs, int32 output_count) override;

private:
  // Translating from/to vst3.
  void process_input_parameters(ProcessData const& data);
  void process_notes(svn::base::block_input& input, ProcessData const& data);
  void process_automation(svn::base::block_input& input, ProcessData const& data);
  void process_output_parameters(svn::base::block_output const& output, ProcessData& data);
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SDK_PROCESSOR_HPP