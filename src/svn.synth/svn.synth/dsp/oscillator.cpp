#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/oscillator.hpp>
#include <svn.base/support/platform.hpp>

#include <cmath>
#include <cassert>
#include <immintrin.h>

using namespace svn::base;

namespace svn::synth {

oscillator::
oscillator(float sample_rate, std::int32_t midi_note) :
_sample_rate(sample_rate), _midi_note(midi_note), _phases()
{ _phases.fill(0.0f); }

struct sine_generator
{
  float operator()(
    float frequency, float phase, float increment, float sample_rate, 
    float const* const* transformed_cv, std::int32_t sample) const
  { return std::sin(2.0f * pi32 * phase); }
};

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
struct blep_saw_generator
{
  float
  generate_blep(float phase, float increment) const
  {
    float blep;
    if (phase < increment) return blep = phase / increment, (2.0f - blep) * blep - 1.0f;
    if (phase >= 1.0f - increment) return blep = (phase - 1.0f) / increment, (blep + 2.0f) * blep + 1.0f;
    return 0.0f;
  }

  float operator()(
    float frequency, float phase, float increment, float sample_rate, 
    float const* const* transformed_cv, std::int32_t sample) const
  {
    phase += 0.5f;
    phase -= std::floor(phase);
    return ((2.0f * phase - 1.0f) - generate_blep(phase, increment));
  }
};

struct blep_pulse_generator
{
  static inline float const min_pw = 0.05f;

  float operator()(
    float frequency, float phase, float increment, float sample_rate, 
    float const* const* transformed_cv, std::int32_t sample) const
  {
    blep_saw_generator saw_generator;
    float pw = (min_pw + (1.0f - min_pw) * transformed_cv[oscillator_param::analog_pw][sample]) * 0.5f;
    float saw1 = saw_generator(frequency, phase, increment, sample_rate, transformed_cv, sample);
    float saw2 = saw_generator(frequency, phase + pw, increment, sample_rate, transformed_cv, sample);
    return (saw1 - saw2) * 0.5f;
  }
};

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
struct blamp_triangle_generator
{
  float
  generate_blamp(float phase, float increment) const
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

  float operator()(
    float frequency, float phase, float increment, float sample_rate, 
    float const* const* transformed_cv, std::int32_t sample) const
  {
    phase = phase + 0.75f;
    phase -= std::floor(phase);
    float triangle = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
    triangle += generate_blamp(phase, increment);
    triangle += generate_blamp(1.0f - phase, increment);
    phase += 0.5f;
    phase -= std::floor(phase);
    triangle += generate_blamp(phase, increment);
    triangle += generate_blamp(1.0f - phase, increment);
    return triangle;
  }
};

struct dsf_generator
{
  std::int32_t const _partials;
  dsf_generator(std::int32_t partials): _partials(partials) {}

  float operator()(
    float frequency, float phase, float increment, float sample_rate, 
    float const* const* transformed_cv, std::int32_t sample) const
  {
    std::int32_t partials = _partials;
    float const scale_factor = 0.975f;
    float const rolloff_range = 0.99f;
    float rolloff = transformed_cv[oscillator_param::dsf_rolloff][sample];
    float distance = frequency * transformed_cv[oscillator_param::dsf_distance][sample];
    float max_partials = (sample_rate * 0.5f - frequency) / distance;
    partials = std::min(partials, static_cast<std::int32_t>(max_partials));

    float n = static_cast<float>(partials);
    float w = (1.0f - rolloff) * rolloff_range;
    float w_pow_np1 = std::pow(w, n + 1);
    float u = 2.0f * pi32 * phase;
    float v = 2.0f * pi32 * distance * phase / frequency;
    float a = w * std::sin(u + n * v) - std::sin(u + (n + 1) * v);
    float x = (w * std::sin(v - u) + std::sin(u)) + w_pow_np1 * a;
    float y = 1 + w * w - 2 * w * std::cos(v);
    float scale = (1.0f - w_pow_np1) / (1.0f - w);
    return sanity_bipolar(x * scale_factor / (y * scale));
  }
};

template <class sample_generator_type> void
oscillator::generate_unison(
  voice_input const& input, std::int32_t unison_voices, std::int32_t midi, 
  float const* const* transformed_cv, sample_generator_type sample_generator, base::audio_sample32* audio_out)
{
  float const* amp = transformed_cv[oscillator_param::amp];
  float const* pan = transformed_cv[oscillator_param::pan];
  float const* cent = transformed_cv[oscillator_param::cent];
  float const* detune = transformed_cv[oscillator_param::unison_detune];
  float const* spread = transformed_cv[oscillator_param::unison_spread];

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio_out[s] = { 0.0f, 0.0f };
    float pan_range = pan[s] < 0.5f ? pan[s] : 1.0f - pan[s];
    float pan_min = pan[s] - spread[s] * pan_range;
    float pan_max = pan[s] + spread[s] * pan_range;
    float midi_min = midi + cent[s] - detune[s] * 0.5f;
    float midi_max = midi + cent[s] + detune[s] * 0.5f;
    float voice_range = unison_voices == 1 ? 1.0f : static_cast<float>(unison_voices - 1);

    for (std::int32_t v = 0; v < unison_voices; v++)
    {
      float this_midi = midi_min + (midi_max - midi_min) * v / voice_range;
      float this_frequency = note_to_frequency_table(this_midi);
      float this_increment = this_frequency / _sample_rate;
      float this_pan = pan_min + (pan_max - pan_min) * v / voice_range;
      float this_sample = sample_generator(this_frequency, _phases[v], this_increment, _sample_rate, transformed_cv, s) * amp[s];
      audio_out[s] += { this_sample * (1.0f - this_pan), this_sample * this_pan };
      _phases[v] += this_increment;
      _phases[v] -= std::floor(_phases[v]);
    }

    audio_out[s] = sanity_audio(audio_out[s] / static_cast<float>(unison_voices));
  }
}

double
oscillator::process_block(
  voice_input const& input, std::int32_t index,
  cv_state& cv, audio_sample32* audio_out, double& cv_time)
{  
  // Note: discrete automation per block, not per sample!
  automation_view automation(input.automation.rearrange_params(part_type::oscillator, index));
  std::int32_t on = automation.automation_discrete(oscillator_param::on, 0);
  if (on == 0)
  {
    std::memset(audio_out, 0, input.sample_count * sizeof(audio_sample32));
    return 0.0;
  }

  float const* const* transformed_cv = nullptr;
  cv_time += cv.transform_modulated(input, automation, cv_route_output::osc, index, cv_route_osc_mapping, transformed_cv);

  double start_time = performance_counter();
  std::int32_t type = automation.automation_discrete(oscillator_param::type, 0);
  std::int32_t note = automation.automation_discrete(oscillator_param::note, 0);
  std::int32_t voices = automation.automation_discrete(oscillator_param::unison, 0);
  std::int32_t octave = automation.automation_discrete(oscillator_param::octave, 0);
  std::int32_t partials = automation.automation_discrete(oscillator_param::dsf_partials, 0);
  std::int32_t analog_type = automation.automation_discrete(oscillator_param::analog_type, 0);
  std::int32_t midi = 12 * (octave + 1) + note + _midi_note - 60;

  switch (type)
  {
  case oscillator_type::analog:
    switch (analog_type)
    {
    case oscillator_analog_type::sine: generate_unison(input, voices, midi, transformed_cv, sine_generator(), audio_out); break;
    case oscillator_analog_type::saw:  generate_unison(input, voices, midi, transformed_cv, blep_saw_generator(), audio_out); break;
    case oscillator_analog_type::pulse: generate_unison(input, voices, midi, transformed_cv, blep_pulse_generator(), audio_out); break;
    case oscillator_analog_type::triangle: generate_unison(input, voices, midi, transformed_cv, blamp_triangle_generator(), audio_out); break;
    default: assert(false); break;
    }
    break;
  case oscillator_type::dsf: generate_unison(input, voices, midi, transformed_cv, dsf_generator(partials), audio_out); break;
  default: assert(false); break;
  }
  return base::performance_counter() - start_time;
}

} // namespace svn::synth