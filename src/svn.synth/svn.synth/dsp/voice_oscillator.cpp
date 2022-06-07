#include <svn.base/dsp/dsp.hpp>
#include <svn.synth/static/topology.hpp>
#include <svn.synth/dsp/voice_input.hpp>
#include <svn.synth/dsp/voice_oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

static inline float 
osc_sine(float phase)
{
  assert(0.0f <= phase && phase < 1.0f);
  return std::sin(2.0f * std::numbers::pi * phase);
}

static inline float
osc_blep(float sample_rate, float frequency, float phase)
{
  float saw = 2.0f * phase - 1.0f;
  float increment = frequency / sample_rate;
  assert(0.0f <= phase && phase < 1.0f);

  if (phase < increment)
  {
    float blep = phase / increment;
    return saw - ((2.0f - blep) * blep - 1.0f);
  }
  if (phase >= 1.0f - increment)
  {
    float blep = (phase - 1.0f) / increment;
    return saw - ((blep + 2.0f) * blep + 1.0f);
  }
  return saw;
}

voice_oscillator::
voice_oscillator(
  float sample_rate, std::int32_t midi_note):
_sample_rate(sample_rate),
_midi_note(midi_note)
{
  assert(sample_rate > 0.0f);
  assert(0 <= midi_note && midi_note < 128);
}

void
voice_oscillator::process_block(
  voice_input const& input, audio_sample* audio)
{
  assert(audio != nullptr);
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio[s] = 0.0f;
    bool on = input.automation.get(voice_osc_param::on, s).discrete != 0;
    if(!on) continue;
       
    float panning = input.automation.get(voice_osc_param::pan, s).real;
    float cent = input.automation.get(voice_osc_param::detune, s).real;
    std::int32_t type = input.automation.get(voice_osc_param::type, s).discrete;
    std::int32_t note = input.automation.get(voice_osc_param::note, s).discrete;
    std::int32_t octave = input.automation.get(voice_osc_param::oct, s).discrete;

    float sample;
    float frequency = note_to_frequency(12 * (octave + 1) + note + cent + _midi_note - 60);
    switch (type)
    {
    case voice_osc_type::sine: sample = osc_sine(_phase); break;
    case voice_osc_type::blep: sample = osc_blep(_sample_rate, frequency, _phase); break;
    default: assert(false); break;
    } 

    audio[s].left = (1.0f - panning) * sample;
    audio[s].right = panning * sample;
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth