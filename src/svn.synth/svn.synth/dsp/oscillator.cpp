#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>
#include <immintrin.h>

using namespace svn::base;

namespace svn::synth {

oscillator::
oscillator(float sample_rate, std::int32_t midi_note) :
_sample_rate(sample_rate), _midi_note(midi_note), _phases()
{ _phases.fill(0.0f); }
    
// https://www.kvraudio.com/forum/viewtopic.php?t=375517
float 
oscillator::generate_poly_blep2(float phase, float increment) const
{
  float blep;
  if (phase < increment) return blep = phase / increment, (2.0f - blep) * blep - 1.0f;
  if (phase >= 1.0f - increment) return blep = (phase - 1.0f) / increment, (blep + 2.0f) * blep + 1.0f;
  return 0.0f;
}

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
float
oscillator::generate_poly_blamp2(float phase, float increment) const
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
oscillator::generate_blamp_triangle2(float phase, float increment) const
{
  phase = phase + 0.75f;
  phase -= std::floor(phase);
  float triangle = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
  triangle += generate_poly_blamp2(phase, increment);
  triangle += generate_poly_blamp2(1.0f - phase, increment);
  phase += 0.5f;
  phase -= std::floor(phase);
  triangle += generate_poly_blamp2(phase, increment);
  triangle += generate_poly_blamp2(1.0f - phase, increment);
  return triangle;
}

float
oscillator::generate_blep_saw2(float phase, float increment) const
{
  phase += 0.5f;
  phase -= std::floor(phase);
  float naive = (2.0f * phase - 1.0f);
  return naive - generate_poly_blep2(phase, increment);
}

float
oscillator::generate_blep_pulse2(
  automation_view const& automation, float const* const* modulated, 
  std::int32_t sample, float phase, float increment) const
{
  float pw = (min_pw + (1.0f - min_pw) * modulated[oscillator_param::analog_pw][sample]) * 0.5f;
  float saw1 = generate_blep_saw2(phase, increment);
  float saw2 = generate_blep_saw2(phase + pw, increment);
  return (saw1 - saw2) * 0.5f;
}
   
float
oscillator::generate_analog2(
  automation_view const& automation, float const* const* modulated, 
  std::int32_t sample, float phase, float increment) const
{
  std::int32_t type = automation.get_discrete(oscillator_param::analog_type, sample);
  switch (type)
  {
  case oscillator_analog_type::saw: return generate_blep_saw2(phase, increment);
  case oscillator_analog_type::sine: return std::sin(2.0f * std::numbers::pi * phase);
  case oscillator_analog_type::triangle: return generate_blamp_triangle2(phase, increment);
  case oscillator_analog_type::pulse: return generate_blep_pulse2(automation, modulated, sample, phase, increment);
  default: assert(false); return 0.0f;
  }
}   

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
float 
oscillator::generate_dsf2(
  automation_view const& automation, float const* const* modulated, 
  std::int32_t sample, float phase, float frequency) const
{
  float const scale_factor = 0.975f;
  float const rolloff_range = 0.99f;
  float rolloff = modulated[oscillator_param::dsf_rolloff][sample];
  float distance = frequency * modulated[oscillator_param::dsf_distance][sample];
  std::int32_t partials = automation.get_discrete(oscillator_param::dsf_partials, sample);
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
oscillator::generate_wave2(
  automation_view const& automation, float const* const* modulated, 
  std::int32_t sample, float phase, float frequency, float increment) const
{
  std::int32_t type = automation.get_discrete(oscillator_param::type, sample);
  switch (type)
  {
  case oscillator_type::dsf: return generate_dsf2(automation, modulated, sample, phase, frequency);
  case oscillator_type::analog: return generate_analog2(automation, modulated, sample, phase, increment);
  default: assert(false); return 0.0f;
  }
}

base::audio_sample32
oscillator::generate_unison2(
  automation_view const& automation, float const* const* modulated, 
  std::int32_t s, float midi, float frequency, float panning)
{
  std::int32_t voices = automation.get_discrete(oscillator_param::unison, s);
  if (voices == 1) 
  { 
    float sample = generate_wave2(automation, modulated, s, _phases[0], frequency, frequency / _sample_rate);
    _phases[0] += frequency / _sample_rate;
    _phases[0] -= std::floor(_phases[0]);
    return { sample * (1.0f - panning), sample * panning };
  }

  base::audio_sample32 result = { 0.0f, 0.0f };
  float detune = modulated[oscillator_param::unison_detune][s];
  float spread = modulated[oscillator_param::unison_spread][s];
  float pan_range = panning < 0.5f? panning: 1.0f - panning;
  float pan_min = panning - spread * pan_range;
  float pan_max = panning + spread * pan_range;
  float midi_min = midi - detune * 0.5f;
  float midi_max = midi + detune * 0.5f;
  for (std::int32_t i = 0; i < voices; i++)
  {
    float this_pan = pan_min + (pan_max - pan_min) * i / static_cast<float>(voices - 1);
    float this_midi = midi_min + (midi_max - midi_min) * i / static_cast<float>(voices - 1);
    float this_frequency = note_to_frequency_table(this_midi);
    float sample = generate_wave2(automation, modulated, s, _phases[i], this_frequency, this_frequency / _sample_rate);
    result += audio_sample32({ sample * (1.0f - this_pan), sample * this_pan });
    _phases[i] += this_frequency / _sample_rate;
    _phases[i] -= std::floor(_phases[i]);
  }
  return result / static_cast<float>(voices);
}

double
oscillator::process_block2(
  voice_input const& input, std::int32_t index, 
  cv_state& cv, audio_sample32* audio_out, double& mod_time)
{
  float const* const* modulated;
  automation_view automation(input.automation.rearrange_params(part_type::oscillator, index));
  mod_time += cv.modulate(input, automation, cv_route_osc_mapping, cv_route_output::osc, index, modulated);
  double start_time_sec = performance_counter();
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {  
    audio_out[s] = 0.0f;
    bool on = automation.get_discrete(oscillator_param::on, s) != 0;
    if(!on) continue;
        
    float cent = modulated[oscillator_param::cent][s];
    std::int32_t note = automation.get_discrete(oscillator_param::note, s);
    std::int32_t octave = automation.get_discrete(oscillator_param::octave, s);
    float midi = 12 * (octave + 1) + note + cent + _midi_note - 60;
    float frequency = note_to_frequency_table(midi);
    float amp = modulated[oscillator_param::amp][s];
    float pan = modulated[oscillator_param::pan][s];
    audio_sample32 sample = generate_unison2(automation, modulated, s, midi, frequency, pan);
    audio_out[s].left = sanity_bipolar(sample.left * amp);
    audio_out[s].right = sanity_bipolar(sample.right * amp);
  }
  return performance_counter() - start_time_sec;
}

#define SVN_SYNTH_ENABLE_AVX2 1

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
void
oscillator::generate_blep_saw(voice_input const& input, 
  svn::base::automation_view const& automation, float const* const* modulated,
  std::int32_t unison_voices, std::int32_t midi, base::audio_sample32* audio_out)
{
  float const* amp = modulated[oscillator_param::amp];
  float const* pan = modulated[oscillator_param::pan];
  float const* cent = modulated[oscillator_param::cent];
  float const* detune = modulated[oscillator_param::unison_detune];
  float const* spread = modulated[oscillator_param::unison_spread];

  __m256 vector_0 = _mm256_set1_ps(0.0f);
  __m256 vector_1 = _mm256_set1_ps(1.0f);
  __m256 vector_2 = _mm256_set1_ps(2.0f);
  __m256 vector_rate = _mm256_set1_ps(_sample_rate);
  __m256 voice_index = _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

  //assert(0);

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
#if SVN_SYNTH_ENABLE_AVX2
    float pan_range = pan[s] < 0.5f ? pan[s] : 1.0f - pan[s];
    __m256 pan_min = _mm256_set1_ps(pan[s] - spread[s] * pan_range);
    __m256 pan_max = _mm256_set1_ps(pan[s] + spread[s] * pan_range);
    __m256 midi_min = _mm256_set1_ps(midi - detune[s] * 0.5f);
    __m256 midi_max = _mm256_set1_ps(midi + detune[s] * 0.5f);
    __m256 left = vector_0;
    __m256 right = vector_0;
    __m256 voice_range = _mm256_set1_ps(unison_voices == 1 ? 1.0f : static_cast<float>(unison_voices - 1));

    //float this_pan = pan_min + (pan_max - pan_min) * v / voice_range;
    //float this_midi = midi_min + (midi_max - midi_min) * v / voice_range;
    __m256 this_pan = _mm256_add_ps(pan_min, _mm256_div_ps(_mm256_mul_ps(_mm256_sub_ps(pan_max, pan_min), voice_index), voice_range));
    __m256 this_midi = _mm256_add_ps(midi_min, _mm256_div_ps(_mm256_mul_ps(_mm256_sub_ps(midi_max, midi_min), voice_index), voice_range));
    __m256 this_frequency = _mm256_set_ps(
      note_to_frequency_table(this_midi.m256_f32[7]),
      note_to_frequency_table(this_midi.m256_f32[6]),
      note_to_frequency_table(this_midi.m256_f32[5]),
      note_to_frequency_table(this_midi.m256_f32[4]),
      note_to_frequency_table(this_midi.m256_f32[3]),
      note_to_frequency_table(this_midi.m256_f32[2]),
      note_to_frequency_table(this_midi.m256_f32[1]),
      note_to_frequency_table(this_midi.m256_f32[0])
    );
    __m256 this_increment = _mm256_div_ps(this_frequency, vector_rate);

    // Start at 0 instead of 0.5.
    __m256 this_blep = vector_0;
    __m256 this_phase = _mm256_set_ps(
      _phases[0] + 0.5f, _phases[1] + 0.5f, _phases[2] + 0.5f, _phases[3] + 0.5f, 
      _phases[4] + 0.5f, _phases[5] + 0.5f, _phases[6] + 0.5f, _phases[7] + 0.5f);
    this_phase = _mm256_sub_ps(this_phase, _mm256_floor_ps(this_phase));

    __m256 blep_low = _mm256_div_ps(this_phase, this_increment);
    blep_low = _mm256_sub_ps(_mm256_mul_ps(_mm256_sub_ps(vector_2, blep_low), blep_low), vector_1);
    __m256 phase_lt_inc = _mm256_cmp_ps(this_phase, this_increment, _CMP_LT_OQ);
    __m256 blep_hi = _mm256_div_ps(_mm256_sub_ps(this_phase, vector_1), this_increment);
    blep_hi = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(blep_hi, vector_2), blep_hi), vector_1);
    __m256 phase_ge_1_min_inc = _mm256_cmp_ps(this_phase, _mm256_sub_ps(vector_1, this_increment), _CMP_GE_OQ);
    this_blep = _mm256_blendv_ps(this_blep, blep_low, phase_lt_inc);
    this_blep = _mm256_blendv_ps(this_blep, blep_hi, phase_ge_1_min_inc);

    __m256 this_sample = _mm256_mul_ps(_mm256_sub_ps(_mm256_sub_ps(_mm256_mul_ps(vector_2, this_phase), vector_1), this_blep), _mm256_set1_ps(amp[s]));
    left = _mm256_mul_ps(this_sample, _mm256_sub_ps(vector_1, this_pan));
    right = _mm256_mul_ps(this_sample, this_pan);

    this_phase = _mm256_set_ps(
      _phases[0], _phases[1], _phases[2], _phases[3],
      _phases[4], _phases[5], _phases[6], _phases[7]);
    this_phase = _mm256_add_ps(this_phase, this_increment);
    this_phase = _mm256_sub_ps(this_phase, _mm256_floor_ps(this_phase));
    _phases[0] = this_phase.m256_f32[7];
    _phases[1] = this_phase.m256_f32[6];
    _phases[2] = this_phase.m256_f32[5];
    _phases[3] = this_phase.m256_f32[4];
    _phases[4] = this_phase.m256_f32[3];
    _phases[5] = this_phase.m256_f32[2];
    _phases[6] = this_phase.m256_f32[1];
    _phases[7] = this_phase.m256_f32[0];

    audio_out[s] = 0.0f;
    for (std::int32_t v = 0; v < unison_voices; v++)
    {
      audio_out[s].left += left.m256_f32[7 - v];
      audio_out[s].right += right.m256_f32[7 - v];
    }

#else
    float pan_range = pan[s] < 0.5f ? pan[s] : 1.0f - pan[s];
    float pan_min = pan[s] - spread[s] * pan_range;
    float pan_max = pan[s] + spread[s] * pan_range;
    float midi_min = midi - detune[s] * 0.5f;
    float midi_max = midi + detune[s] * 0.5f;
    float left[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float right[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float voice_range = unison_voices == 1 ? 1.0f : static_cast<float>(unison_voices - 1);

    for(std::int32_t v = 0; v < 8; v++)
    {
      float this_pan = pan_min + (pan_max - pan_min) * v / voice_range;
      float this_midi = midi_min + (midi_max - midi_min) * v / voice_range;
      float this_frequency = note_to_frequency_table(this_midi);
      float this_increment = this_frequency / _sample_rate;

      // Start at 0 instead of 0.5.
      float this_blep = 0.0f;
      float this_phase = _phases[v] + 0.5f;
      this_phase -= std::floor(this_phase);

      // https://www.kvraudio.com/forum/viewtopic.php?t=375517
      if (this_phase < this_increment)
      {
        this_blep = this_phase / this_increment;
        this_blep = (2.0f - this_blep) * this_blep - 1.0f;
      }
      else if (this_phase >= 1.0f - this_increment)
      {
        this_blep = (this_phase - 1.0f) / this_increment;
        this_blep = (this_blep + 2.0f) * this_blep + 1.0f;
      }

      float this_sample = ((2.0f * this_phase - 1.0f) - this_blep) * amp[s];
      left[v] = this_sample * (1.0f - this_pan);
      right[v] = this_sample * this_pan;
      _phases[v] += this_increment;
      _phases[v] -= std::floor(_phases[v]);
    }

    audio_out[s] = 0.0f;
    for (std::int32_t v = 0; v < unison_voices; v++)
    {
      audio_out[s].left += left[v];
      audio_out[s].right += right[v];
    }
#endif
    audio_out[s] = sanity_audio(audio_out[s] / static_cast<float>(unison_voices));
  }
}

double
oscillator::process_block(
  voice_input const& input, std::int32_t index,
  cv_state& cv, audio_sample32* audio_out, double& mod_time)
{
  //return process_block2(input, index, cv, audio_out, mod_time);
  
  // Note: discrete automation per block, not per sample!
  automation_view automation(input.automation.rearrange_params(part_type::oscillator, index));
  std::int32_t on = automation.get_discrete(oscillator_param::on, 0);
  if (on == 0)
  {
    std::memset(audio_out, 0, input.sample_count * sizeof(audio_sample32));
    return 0.0;
  }

  double start_time = performance_counter();
  std::int32_t type = automation.get_discrete(oscillator_param::type, 0);
  std::int32_t note = automation.get_discrete(oscillator_param::note, 0);
  std::int32_t voices = automation.get_discrete(oscillator_param::unison, 0);
  std::int32_t octave = automation.get_discrete(oscillator_param::octave, 0);
  std::int32_t partials = automation.get_discrete(oscillator_param::dsf_partials, 0);
  std::int32_t analog_type = automation.get_discrete(oscillator_param::analog_type, 0);

  float const* const* modulated;
  double start_mod_time = mod_time;
  mod_time += cv.modulate(input, automation, cv_route_osc_mapping, cv_route_output::osc, index, modulated);
  std::int32_t midi = 12 * (octave + 1) + note + _midi_note - 60;
  generate_blep_saw(input, automation, modulated, voices, midi, audio_out);
  return base::performance_counter() - start_time - (mod_time - start_mod_time);
}

} // namespace svn::synth