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

#define SVN_ENABLE_AVX2 1

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
void
oscillator::generate_blep_saw(
  voice_input const& input, svn::base::automation_view const& automation, 
  float const* const* modulated, std::int32_t midi, base::audio_sample32* audio_out)
{
  float const* amp = modulated[oscillator_param::amp];
  float const* pan = modulated[oscillator_param::pan];
  float const* cent = modulated[oscillator_param::cent];

  __m256 vector_0 = _mm256_set1_ps(0.0f);
  __m256 vector_1 = _mm256_set1_ps(1.0f);
  __m256 vector_2 = _mm256_set1_ps(2.0f);
  __m256 vector_sample_rate = _mm256_set1_ps(_sample_rate);

  for (std::int32_t simd = 0; simd < input.sample_count / 8; simd++)
  {
    float lane_freqs[8];
    float lane_phases[8];
    float const* lane_amp = amp + simd * 8;
    float const* lane_pan = pan + simd * 8;
    base::audio_sample32* lane_audio_out = audio_out + simd * 8;

    for (std::int32_t lane = 0; lane < 8; lane++)
    {
      std::int32_t s = simd * 8 + lane;
      lane_freqs[lane] = note_to_frequency_table(midi + cent[s]);
      lane_phases[lane] = _phases[0] + lane * lane_freqs[lane] / _sample_rate;
      lane_phases[lane] -= std::floor(lane_phases[lane]);
    }

#if SVN_ENABLE_AVX2
    __m256 frequency = _mm256_load_ps(lane_freqs);
    __m256 increment = _mm256_div_ps(frequency, vector_sample_rate);
    __m256 phase = _mm256_load_ps(lane_phases);
    phase = _mm256_add_ps(phase, _mm256_set1_ps(0.5f));
    phase = _mm256_sub_ps(phase, _mm256_floor_ps(phase));
    __m256 phase_lt_inc = _mm256_cmp_ps(phase, increment, _CMP_LT_OQ);
    __m256 blep_lt = _mm256_div_ps(phase, increment);
    blep_lt = _mm256_sub_ps(_mm256_mul_ps(_mm256_sub_ps(vector_2, blep_lt), blep_lt), vector_1);
    __m256 phase_gte_1_min_inc = _mm256_cmp_ps(phase, _mm256_sub_ps(vector_1, increment), _CMP_GE_OQ);
    __m256 blep_gte = _mm256_div_ps(_mm256_sub_ps(phase, vector_1), increment);
    blep_gte = _mm256_add_ps(_mm256_mul_ps(_mm256_add_ps(blep_gte, vector_2), blep_gte), vector_1);
    __m256 blep = _mm256_blendv_ps(vector_0, blep_lt, phase_lt_inc);
    blep = _mm256_blendv_ps(blep, blep_gte, phase_gte_1_min_inc);
    __m256 amp = _mm256_load_ps(lane_amp);
    __m256 pan = _mm256_load_ps(lane_pan);
    __m256 sample = _mm256_mul_ps(_mm256_sub_ps(_mm256_sub_ps(_mm256_mul_ps(vector_2, phase), vector_1), blep), amp);
    __m256 left = _mm256_mul_ps(sample, _mm256_sub_ps(vector_1, pan));
    __m256 right = _mm256_mul_ps(sample, pan);
    for(std::int32_t lane = 0; lane < 8; lane++)
    {
      lane_audio_out[lane].left = left.m256_f32[lane];
      lane_audio_out[lane].right = right.m256_f32[lane];
    }
#else
    for(std::int32_t lane = 0; lane < 8; lane++)
    {
      float frequency = lane_freqs[lane];
      float increment = frequency / _sample_rate;
      float phase = lane_phases[lane];
      float blep = 0.0f;
      phase += 0.5f;
      phase -= std::floor(phase);    
      if (phase < increment)
      {
        blep = phase / increment;
        blep = (2.0f - blep) * blep - 1.0f;
      }
      else if (phase >= 1.0f - increment)
      {
        blep = (phase - 1.0f) / increment;
        blep = (blep + 2.0f) * blep + 1.0f;
      }
      float sample = ((2.0f * phase - 1.0f) - blep) * lane_amp[lane];
      lane_audio_out[lane] = { sample * (1.0f - lane_pan[lane]), sample * lane_pan[lane] };
    }
#endif

    _phases[0] = lane_phases[7] + lane_freqs[7] / _sample_rate;
    _phases[0] -= std::floor(lane_phases[7]);
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
  generate_blep_saw(input, automation, modulated, midi, audio_out);
  return base::performance_counter() - start_time - (mod_time - start_mod_time);
}

} // namespace svn::synth