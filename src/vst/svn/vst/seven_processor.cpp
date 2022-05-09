#include <svn/vst/seven_vst.hpp>
#include <svn/vst/seven_processor.hpp>
#include <svn/synth/seven_synth.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/utility/processdataslicer.h>

#include <cstdint>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

FUnknown* 
SevenProcessor::createInstance(void* context) 
{ return static_cast<IAudioProcessor*>(new SevenProcessor); }

SevenProcessor::
SevenProcessor():
_state(),
_synth32(),
_synth64(),
_parameters()
{
	setControllerClass(ControllerId);
  for(std::uint32_t i = 0; i < svn::param_id::count; i++)
    _parameters[i].setParamID(i);
}

tresult PLUGIN_API
SevenProcessor::getState(IBStream* state)
{
  IBStreamer streamer(state, kLittleEndian);
  for (std::int32_t i = 0; i < svn::param_id::count; i++)
    streamer.writeFloat(_state.normalized[i]);
  return kResultOk;
}

tresult PLUGIN_API 
SevenProcessor::initialize(FUnknown* context)
{
	tresult result = AudioEffect::initialize(context);
	if (result != kResultTrue) return kResultFalse;
  addEventInput(STR16("Event In"), 1);
	addAudioOutput (STR16 ("Stereo Out"), SpeakerArr::kStereo);
	return kResultTrue;
}

tresult PLUGIN_API 
SevenProcessor::setState(IBStream* state)
{
  float value;
  if (state == nullptr) return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);
  for(std::int32_t i = 0; i < svn::param_id::count; i++)
    if(!streamer.readFloat(value)) 
      return kResultFalse;
    else
    {
      _state.normalized[i] = value;
      _state.values[i] = svn::synth_params::all[i].info.norm_to_real(value);
    }
  return kResultOk;
}

tresult PLUGIN_API 
SevenProcessor::setBusArrangements(
  SpeakerArrangement* inputs, int32 inputCount,
  SpeakerArrangement* outputs, int32 outputCount)
{
	if(inputCount != 0 || outputCount != 1 || outputs[0] != 2) return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, inputCount, outputs, outputCount);
}

tresult PLUGIN_API
SevenProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
  if (symbolicSampleSize == kSample32 || symbolicSampleSize == kSample64) return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API 
SevenProcessor::setupProcessing(ProcessSetup& setup)
{
  _synth32.reset();
  _synth64.reset();
  if (setup.symbolicSampleSize == kSample32)
    _synth32.reset(new svn::seven_synth<float>(&_state, setup.sampleRate, setup.maxSamplesPerBlock));
  if (setup.symbolicSampleSize == kSample64)
    _synth64.reset(new svn::seven_synth<double>(&_state, setup.sampleRate, setup.maxSamplesPerBlock));
	return AudioEffect::setupProcessing(setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::process (Vst::ProcessData& data)
{
  bool beganchanges = false; 

	//--- Read inputs parameter changes-----------
	if (data.inputParameterChanges)
	{
    int32 changeCount = data.inputParameterChanges->getParameterCount();
    for (auto i = 0; i < changeCount; ++i)
    {
      if (auto queue = data.inputParameterChanges->getParameterData(i))
      {
        auto paramID = queue->getParameterId();
        if (paramID == HelloWorldParams::kParamVolId)
        {
          parm1SA.beginChanges(queue);
          beganchanges = true;
        }
      }
    }
	}

	//--- Process Audio---------------------
	//--- ----------------------------------
	if (data.numOutputs == 0)
	{
    if (beganchanges)
    {
      parm1SA.endChanges();
    }
		// nothing to do
		return kResultOk;
	}

  svn::unit_generate();

  //test

  static bool on = false;
  static int note = -1;

  //---2) Read input events-------------
  Steinberg::Vst::IEventList* eventList = data.inputEvents;
  if (eventList)
  {
    int32 numEvent = eventList->getEventCount();
    for (int32 i = 0; i < numEvent; i++)
    {
      Steinberg::Vst::Event event;
      if (eventList->getEvent(i, event) == kResultOk)
      {
        switch (event.type)
        {
          //----------------------
        case Steinberg::Vst::Event::kNoteOnEvent:
        {
          on = true;
          note = event.noteOn.pitch;
          //mLastNoteOnPitch = event.noteOn.pitch;
          //mLastNoteOnId = event.noteOn.noteId;
          /*String str;
          str.printf (STR("noteON %d"), event.noteOff.noteId);
          sendTextMessage (str);*/
        }
        break;

        //----------------------
        case Steinberg::Vst::Event::kNoteOffEvent:
          on = false;
          note = -1;
        {
          /*	String str;
            str.printf (STR("noteOff %d"), event.noteOff.noteId);
            sendTextMessage (str);
          */}
        break;
        }
      }
    }
  }

  static float phase = 0;
	if (on &&  data.numSamples > 0)
	{
    Steinberg::Vst::ProcessDataSlicer slicer(8);

    auto doProcessing = [this](Steinberg::Vst::ProcessData& data) {
      // get the gain value for this block
      auto gain = parm1SA.advance(data.numSamples);

      for (int i = 0; i < data.numSamples; i++)
      {
        data.outputs[0].channelBuffers32[0][i] = std::sinf(2.0f * 3.14 * phase) * gain;
        data.outputs[0].channelBuffers32[1][i] = std::sinf(2.0f * 3.14 * phase) * gain;
        phase += (note * 100.0f) / processSetup.sampleRate;
        if (phase >= 1.0f)phase -= 1.0f;
      }
    };

    slicer.process<Steinberg::Vst::SymbolicSampleSizes::kSample32>(data, doProcessing);
		// Process Algorithm
		// Ex: algo.process (data.inputs[0].channelBuffers32, data.outputs[0].channelBuffers32,
		// data.numSamples);
	}

  if (beganchanges)
  {
    parm1SA.endChanges();
  }

	return kResultOk;
}

} // namespace Svn::Vst
