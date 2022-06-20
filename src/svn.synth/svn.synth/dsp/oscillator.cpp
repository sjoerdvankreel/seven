#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

oscillator::
oscillator(float sample_rate, std::int32_t midi_note) :
_sample_rate(sample_rate), _midi_note(midi_note), _phases()
{ _phases.fill(0.0f); }
    
// https://www.kvraudio.com/forum/viewtopic.php?t=375517
float 
oscillator::generate_poly_blep(float phase, float increment) const
{
  float blep;
  if (phase < increment) return blep = phase / increment, (2.0f - blep) * blep - 1.0f;
  if (phase >= 1.0f - increment) return blep = (phase - 1.0f) / increment, (blep + 2.0f) * blep + 1.0f;
  return 0.0f;
}

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
float
oscillator::generate_poly_blamp(float phase, float increment) const
{
  float y = 0.0f;
  if (0.0f <= phase && phase < 2.0f * increment) 
  {
    float x = phase / increment;
    float u = 2.0f - x;
    u *= u * u * u * u;
    y -= u;
    if (phase < increment) 
    {
      float v = 1.0f - x;
      v *= v * v * v * v;
      y += 4 * v;
    }
  }
  return y * increment / 15;
}

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
float
oscillator::generate_blamp_triangle(float phase, float increment) const
{
  phase = phase + 0.75f;
  phase -= std::floor(phase);
  float triangle = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
  triangle += generate_poly_blamp(phase, increment);
  triangle += generate_poly_blamp(1.0f - phase, increment);
  phase += 0.5f;
  phase -= std::floor(phase);
  triangle += generate_poly_blamp(phase, increment);
  triangle += generate_poly_blamp(1.0f - phase, increment);
  return triangle;
}

float
oscillator::generate_blep_saw(float phase, float increment) const
{
  phase += 0.5f;
  phase -= std::floor(phase);
  float naive = (2.0f * phase - 1.0f);
  return naive - generate_poly_blep(phase, increment);
}

float
oscillator::generate_blep_pulse(
  automation_view const& automation, std::int32_t sample, float phase, float increment) const
{
  float pw = (min_pw + (1.0f - min_pw) * automation.get(oscillator_param::anlg_pw, sample).real) * 0.5f;
  float saw1 = generate_blep_saw(phase, increment);
  float saw2 = generate_blep_saw(phase + pw, increment);
  return (saw1 - saw2) * 0.5f;
}
  
float
oscillator::generate_analog(
  automation_view const& automation, std::int32_t sample, float phase, float increment) const
{
  std::int32_t type = automation.get(oscillator_param::anlg_type, sample).discrete;
  switch (type)
  {
  case oscillator_anlg_type::saw: return generate_blep_saw(phase, increment);
  case oscillator_anlg_type::tri: return generate_blamp_triangle(phase, increment);
  case oscillator_anlg_type::sin: return std::sin(2.0f * std::numbers::pi * phase);
  case oscillator_anlg_type::pulse: return generate_blep_pulse(automation, sample, phase, increment);
  default: assert(false); return 0.0f;
  }
}

float 
oscillator::generate_analog_spread(
  automation_view const& automation, std::int32_t sample, float midi, float frequency)
{
  std::int32_t spread = automation.get(oscillator_param::anlg_spread, sample).discrete;
  if(spread == 1) return generate_analog(automation, sample, _phase, frequency / _sample_rate);

  float result = 0.0f;
  float detune = automation.get(oscillator_param::anlg_detune, sample).real;
  float midi_min = midi - detune * 0.5f;
  float midi_max = midi + detune * 0.5f;
  for (std::int32_t i = 0; i < spread; i++)
  {
    float this_midi = midi_min + (midi_max - midi_min) * i / static_cast<float>(spread - 1);
    float this_frequency = note_to_frequency(this_midi);
    result += generate_analog(automation, sample, _phases[i], this_frequency / _sample_rate);
    _phases[i] += this_frequency / _sample_rate;
    _phases[i] -= std::floor(_phases[i]);
  }
  return result / static_cast<float>(spread);
}

float 
oscillator::generate_dsf(float frequency) const
{
  float falloff = 0.5f;
  std::int32_t partials = 3;
  float distance = frequency;
  //((w * sin(v - u) + sin(u)) + std::pow(w, n + 1) * (w * sin(u + n * v) - sin(u + (n + 1) * v))) / (1 + w * w - 2 * w * cos(v));

  float w = falloff;
  float n = partials;     
  float position = _phase * frequency / _sample_rate;
  float v = 2.0f * std::numbers::pi * distance * position;
  float u = 2.0f * std::numbers::pi * frequency * position;
  float a = w * std::sin(u + n * v) - std::sin(u + (n + 1) * v);
  float x = (w * std::sin(v - u) + std::sin(u)) + std::pow(w, n + 1) * a;
  float y = 1 + w * w - 2 * w * cos(v);
  return x / y;
}

float
oscillator::generate_wave(
  automation_view const& automation, std::int32_t sample, float midi, float frequency)
{
  std::int32_t type = automation.get(oscillator_param::type, sample).discrete;
  switch (type)
  {
  case oscillator_type::dsf: return generate_dsf(frequency);
  case oscillator_type::analog: return generate_analog_spread(automation, sample, midi, frequency);
  default: assert(false); return 0.0f;
  }
}

void
oscillator::process_block(voice_input const& input, audio_sample* audio)
{
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {  
    audio[s] = 0.0f;
    bool on = input.automation.get(oscillator_param::on, s).discrete != 0;
    if(!on) continue;
        
    float cent = input.automation.get(oscillator_param::cent, s).real;
    std::int32_t note = input.automation.get(oscillator_param::note, s).discrete;
    std::int32_t octave = input.automation.get(oscillator_param::oct, s).discrete;
    float midi = 12 * (octave + 1) + note + cent + _midi_note - 60;
    float frequency = note_to_frequency(midi);
    float sample = generate_wave(input.automation, s, midi, frequency);
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);

    float amp = input.automation.get(oscillator_param::amp, s).real;
    float panning = input.automation.get(oscillator_param::pan, s).real;
    audio[s].left = (1.0f - panning) * sample * amp;
    audio[s].right = panning * sample * amp;
  }
}

} // namespace svn::synth