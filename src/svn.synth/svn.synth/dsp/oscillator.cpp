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

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
float 
oscillator::generate_dsf(
  automation_view const& automation, std::int32_t sample, float phase, float frequency) const
{
  float const scale_factor = 0.975f;
  float const rolloff_range = 0.99f;
  float rolloff = automation.get(oscillator_param::dsf_rolloff, sample).real;
  float distance = frequency * automation.get(oscillator_param::dsf_distance, sample).real;
  std::int32_t partials = automation.get(oscillator_param::dsf_partials, sample).discrete;
  float max_partials = (_sample_rate * 0.5f - frequency) / distance;
  partials = std::min(partials, static_cast<std::int32_t>(max_partials));
   
  float n = partials;     
  float w = (1.0f - rolloff) * rolloff_range;
  float w_pow_np1 = std::pow(w, n + 1);
  float u = 2.0f * std::numbers::pi * phase;
  float v = 2.0f * std::numbers::pi * distance * phase / frequency;
  float a = w * std::sin(u + n * v) - std::sin(u + (n + 1) * v);
  float x = (w * std::sin(v - u) + std::sin(u)) + w_pow_np1 * a;
  float y = 1 + w * w - 2 * w * std::cos(v);
  float scale = (1.0f - w_pow_np1) / (1.0f - w);
  return sanity_bipolar(x * scale_factor / (y * scale));
}

float
oscillator::generate_wave(
  automation_view const& automation, std::int32_t sample, float phase, float frequency, float increment) const
{
  std::int32_t type = automation.get(oscillator_param::type, sample).discrete;
  switch (type)
  {
  case oscillator_type::dsf: return generate_dsf(automation, sample, phase, frequency);
  case oscillator_type::analog: return generate_analog(automation, sample, phase, increment);
  default: assert(false); return 0.0f;
  }
}

base::audio_sample32
oscillator::generate_unison(
  automation_view const& automation, std::int32_t s, float midi, float frequency, float panning)
{
  std::int32_t voices = automation.get(oscillator_param::uni_voices, s).discrete;
  if (voices == 1) 
  { 
    float sample = generate_wave(automation, s, _phases[0], frequency, frequency / _sample_rate);
    _phases[0] += frequency / _sample_rate;
    _phases[0] -= std::floor(_phases[0]);
    return { sample * (1.0f - panning), sample * panning };
  }

  base::audio_sample32 result = { 0.0f, 0.0f };
  float detune = automation.get(oscillator_param::uni_detune, s).real;
  float spread = automation.get(oscillator_param::uni_spread, s).real;
  float pan_range = panning < 0.5f? panning: 1.0f - panning;
  float pan_min = panning - spread * pan_range;
  float pan_max = panning + spread * pan_range;
  float midi_min = midi - detune * 0.5f;
  float midi_max = midi + detune * 0.5f;
  for (std::int32_t i = 0; i < voices; i++)
  {
    float this_pan = pan_min + (pan_max - pan_min) * i / static_cast<float>(voices - 1);
    float this_midi = midi_min + (midi_max - midi_min) * i / static_cast<float>(voices - 1);
    float this_frequency = note_to_frequency(this_midi);
    float sample = generate_wave(automation, s, _phases[i], this_frequency, this_frequency / _sample_rate);
    result += audio_sample32({ sample * (1.0f - this_pan), sample * this_pan });
    _phases[i] += this_frequency / _sample_rate;
    _phases[i] -= std::floor(_phases[i]);
  }
  return result / static_cast<float>(voices);
}

void
oscillator::process_block(voice_input const& input, 
  std::int32_t index, cv_state const& cv, audio_sample32* audio_out)
{
  automation_view automation(input.automation.rearrange_params(part_type::oscillator, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {  
    audio_out[s] = 0.0f;
    bool on = automation.get(oscillator_param::on, s).discrete != 0;
    if(!on) continue;
        
    float cent = automation.get(oscillator_param::cent, s).real;
    std::int32_t note = automation.get(oscillator_param::note, s).discrete;
    std::int32_t octave = automation.get(oscillator_param::oct, s).discrete;
    float midi = 12 * (octave + 1) + note + cent + _midi_note - 60;
    float frequency = note_to_frequency(midi);
    float amp = automation.get(oscillator_param::amp, s).real;
    float panning = automation.get(oscillator_param::pan, s).real;
    panning *= cv_in[cv_route_osc_output::pan][s];
    audio_sample32 sample = generate_unison(automation, s, midi, frequency, panning);
    audio_out[s].left = sanity_bipolar(sample.left * amp);
    audio_out[s].right = sanity_bipolar(sample.right * amp);
  }
}

} // namespace svn::synth