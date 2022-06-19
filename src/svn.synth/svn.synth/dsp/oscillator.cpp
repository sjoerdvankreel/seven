#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {
    
static float
oscillator_blep_saw(float sample_rate, float frequency, float phase)
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

static float
oscillator_blep(std::int32_t blep_type, float sample_rate, float frequency, float phase)
{
  float saw1;
  float saw2;
  float phase2;

  switch (blep_type)
  {
  case oscillator_blep_type::triangle:
    return 0.0f;
  case oscillator_blep_type::saw:
    return oscillator_blep_saw(sample_rate, frequency, phase);
  case oscillator_blep_type::pulse:
    phase2 = phase + 0.5f;
    phase2 -= std::floor(phase2);
    saw1 = oscillator_blep_saw(sample_rate, frequency, phase);
    saw2 = oscillator_blep_saw(sample_rate, frequency, phase2);
    return (saw1 - saw2) * 0.5f;
  default:
    assert(false);
    return 0.0f;
  }
}
  
void
oscillator::process_block(voice_input const& input, audio_sample* audio)
{
  std::int32_t blep_type;
  assert(audio != nullptr);

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio[s] = 0.0f;
    bool on = input.automation.get(oscillator_param::on, s).discrete != 0;
    if(!on) continue;
        
    float amp = input.automation.get(oscillator_param::amp, s).real;
    float cent = input.automation.get(oscillator_param::cent, s).real;
    float panning = input.automation.get(oscillator_param::pan, s).real;
    std::int32_t type = input.automation.get(oscillator_param::type, s).discrete;
    std::int32_t note = input.automation.get(oscillator_param::note, s).discrete;
    std::int32_t octave = input.automation.get(oscillator_param::oct, s).discrete;
     
    float sample;
    float frequency = note_to_frequency(12 * (octave + 1) + note + cent + _midi_note - 60);
    switch (type)
    {
    case oscillator_type::blmp: 
      sample = 0.0f; 
      break;
    case oscillator_type::sine: 
      sample = std::sin(2.0f * std::numbers::pi * _phase); 
      break;
    case oscillator_type::blep: 
      blep_type = input.automation.get(oscillator_param::blep_type, s).discrete;
      sample = oscillator_blep(blep_type, _sample_rate, frequency, _phase); 
      break;
    default: 
      assert(false); 
      break;
    } 
     
    audio[s].left = (1.0f - panning) * sample * amp;
    audio[s].right = panning * sample * amp;
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth