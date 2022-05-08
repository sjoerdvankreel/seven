#include <svn/vst/seven_vst.hpp>
#include <svn/vst/seven_processor.hpp>
#include <svn/synth/seven_synth.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/utility/processdataslicer.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Svn::Vst {

FUnknown* 
SevenProcessor::createInstance(void* context) 
{ return static_cast<IAudioProcessor*>(new SevenProcessor); }

SevenProcessor::
SevenProcessor()
{
	setControllerClass(ControllerId);
  //parm1SA.setParamID(HelloWorldParams::kParamVolId);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::initialize (FUnknown* context)
{
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	if (result != kResultTrue)
		return kResultFalse;

	//---create Audio In/Out buses------
	// we want a stereo Input and a Stereo Output
  addEventInput(STR16("Event In"), 1);
	addAudioOutput (STR16 ("Stereo Out"), Vst::SpeakerArr::kStereo);

	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setBusArrangements (Vst::SpeakerArrangement* inputs,
                                                            int32 numIns,
                                                            Vst::SpeakerArrangement* outputs,
                                                            int32 numOuts)
{
	// we only support one in and output bus and these buses must have the same number of channels
	if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0])
	{
		return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setupProcessing (Vst::ProcessSetup& setup)
{
	// here you get, with setup, information about:
	// sampleRate, processMode, maximum number of samples per audio block
	return AudioEffect::setupProcessing (setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setActive (TBool state)
{
	if (state) // Initialize
	{
		// Allocate Memory Here
		// Ex: algo.create ();
	}
	else // Release
	{
		// Free Memory if still allocated
		// Ex: if(algo.isCreated ()) { algo.destroy (); }
	}
	return AudioEffect::setActive (state);
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


//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
  if (symbolicSampleSize == Steinberg::Vst:: kSample32)
    return kResultTrue;

  // we support double processing
  if (symbolicSampleSize == Steinberg::Vst::kSample64)
    return kResultFalse;

  return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	// called when we load a preset or project, the model has to be reloaded

	IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
		return kResultFalse;

	mParam1 = savedParam1;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
	// here we need to save the model (preset or project)

	float toSaveParam1 = mParam1;

	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (toSaveParam1);

	return kResultOk;
}

} // namespace Svn::Vst
