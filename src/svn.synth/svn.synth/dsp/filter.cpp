#include <svn.synth/dsp/filter.hpp>
#include <svn.base/dsp/support.hpp>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

filter::
filter(float sample_rate, std::int32_t midi_note) :
_state_var(), _comb(), _sample_rate(sample_rate)
{
  float this_frequency = note_to_frequency(midi_note);
  float base_frequency = note_to_frequency(midi_note_c4);
  _state_var.kbd_track_base = this_frequency / base_frequency;
}

// https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
void 
filter::process_state_variable(voice_input const& input, 
  std::int32_t type, base::audio_sample32 const* audio_in,
  base::audio_sample32* audio_out, float const* const* transformed_cv)
{
  float const* res = transformed_cv[filter_param::state_var_res];
  float const* kbd = transformed_cv[filter_param::state_var_kbd];
  float const* freq = transformed_cv[filter_param::state_var_freq];

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    float frequency = freq[s];
    if (kbd[s] > 0.0f) frequency = (1.0f - kbd[s]) * frequency + kbd[s] * frequency * _state_var.kbd_track_base;
    if (kbd[s] < 0.0f) frequency = (1.0f + kbd[s]) * frequency - kbd[s] * frequency / _state_var.kbd_track_base;
    frequency = std::clamp(frequency, filter_min_freq, filter_max_freq);

    double g = std::tan(pi64 * frequency / _sample_rate);
    double k = 2.0 - 2.0 * res[s];
    double a1 = 1.0 / (1.0 + g * (g + k));
    double a2 = g * a1;
    double a3 = g * a2;

    auto v0 = audio_in[s].to64();
    auto v3 = v0 - _state_var.ic2eq;
    auto v1 = a1 * _state_var.ic1eq + a2 * v3;
    auto v2 = _state_var.ic2eq + a2 * _state_var.ic1eq + a3 * v3;
    _state_var.ic1eq = 2.0 * v1 - _state_var.ic1eq;
    _state_var.ic2eq = 2.0 * v2 - _state_var.ic2eq;

    audio_sample64 sample = {};
    switch (type)
    {
    case filter_state_var_type::lpf: sample = v2; break;
    case filter_state_var_type::bpf: sample = v1; break;
    case filter_state_var_type::bsf: sample = v0 - k * v1; break;
    case filter_state_var_type::hpf: sample = v0 - k * v1 - v2; break;
    case filter_state_var_type::apf: sample = v0 - 2.0 * k * v1; break;
    default: assert(false); break;
    }  
    audio_out[s] = sanity_audio(sample.to32());
  }
}

// https://www.dsprelated.com/freebooks/filters/Analysis_Digital_Comb_Filter.html
void
filter::process_comb(
  voice_input const& input, base::audio_sample32 const* audio_in, 
  base::audio_sample32* audio_out, float const* const* transformed_cv)
{
  float const* gain_min = transformed_cv[filter_param::comb_gain_min];
  float const* gain_plus = transformed_cv[filter_param::comb_gain_plus];
  float const* dly_min_sec = transformed_cv[filter_param::comb_dly_min];
  float const* dly_plus_sec = transformed_cv[filter_param::comb_dly_plus];

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    std::int32_t dly_min_samples = static_cast<std::int32_t>(dly_min_sec[s] * _sample_rate);
    std::int32_t dly_plus_samples = static_cast<std::int32_t>(dly_plus_sec[s] * _sample_rate);
    audio_sample32 min = _comb.output.get(dly_min_samples) * gain_min[s];
    audio_sample32 plus = _comb.input.get(dly_plus_samples) * gain_plus[s];
    _comb.input.push(audio_in[s]);
    _comb.output.push(audio_in[s] + plus + min);
    audio_out[s] = base::sanity_audio(_comb.output.get(0));
  }
}

double 
filter::process_block(voice_input const& input, std::int32_t index, cv_state& cv,
  base::audio_sample32 const* audio_in, base::audio_sample32* audio_out, double& cv_time)
{
  // Note: discrete automation per block, not per sample! 
  automation_view automation(input.automation.rearrange_params(part_type::filter, index));
  if (automation.automation_discrete(filter_param::on, 0) == 0)
  {
    std::copy(audio_in, audio_in + input.sample_count, audio_out);
    return 0.0;
  }

  float const* const* transformed_cv;
  cv_time += cv.transform_modulated(input, automation, cv_route_output::filter, index, cv_route_filter_mapping, transformed_cv);  
  double start_time = performance_counter();
  std::int32_t type = automation.automation_discrete(filter_param::type, 0);
  std::int32_t state_var_type = automation.automation_discrete(filter_param::state_var_type, 0);
  switch (type)
  {
  case filter_type::comb: process_comb(input, audio_in, audio_out, transformed_cv); break;
  case filter_type::state_var: process_state_variable(input, state_var_type, audio_in, audio_out, transformed_cv); break;
  default: assert(false); break;
  }
  return performance_counter() - start_time;
}

} // namespace svn::synth