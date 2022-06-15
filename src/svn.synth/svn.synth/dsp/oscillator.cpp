#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {
    
static inline float
oscillator_blep(float sample_rate, float frequency, float phase)
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
  
void
oscillator::process_block(voice_input const& input, audio_sample* audio)
{
  assert(audio != nullptr);
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio[s] = 0.0f;
    bool on = input.automation.get(oscillator_param::on, s).discrete != 0;
    if(!on) continue;
        
    float cent = input.automation.get(oscillator_param::cent, s).real;
    float panning = input.automation.get(oscillator_param::pan, s).real;
    std::int32_t type = input.automation.get(oscillator_param::type, s).discrete;
    std::int32_t note = input.automation.get(oscillator_param::note, s).discrete;
    std::int32_t octave = input.automation.get(oscillator_param::oct, s).discrete;

    float sample;
    float frequency = note_to_frequency(12 * (octave + 1) + note + cent + _midi_note - 60);
    switch (type)
    {
    case oscillator_type::blmp: sample = 0.0f; break;
    case oscillator_type::sine: sample = std::sin(2.0f * std::numbers::pi * _phase); break;
    case oscillator_type::blep: sample = oscillator_blep(_sample_rate, frequency, _phase); break;
    default: assert(false); break;
    } 
     
    audio[s].left = (1.0f - panning) * sample;
    audio[s].right = panning * sample;
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth