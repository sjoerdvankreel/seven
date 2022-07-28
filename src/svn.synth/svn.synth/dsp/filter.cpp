#include <svn.synth/dsp/filter.hpp>
#include <svn.base/dsp/support.hpp>

#include <cassert>
#include <numbers>

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
audio_sample32 
filter::process_state_variable(automation_view const& automation, 
  audio_sample32 source, std::int32_t sample, float const* const* transformed_cv)
{
  float res = automation.get_transformed_dsp(filter_param::state_var_res, sample, transformed_cv);
  float kbd = automation.get_transformed_dsp(filter_param::state_var_kbd, sample, transformed_cv);
  float freq = automation.get_transformed_dsp(filter_param::state_var_freq, sample, transformed_cv);
  std::int32_t type = automation.automation_discrete(filter_param::state_var_type, sample);

  if (kbd > 0.0f) freq = (1.0f - kbd) * freq + kbd * freq * _state_var.kbd_track_base;
  if (kbd < 0.0f) freq = (1.0f + kbd) * freq - kbd * freq / _state_var.kbd_track_base;
  freq = std::clamp(freq, filter_min_freq, filter_max_freq);

  double g = std::tan(std::numbers::pi * freq / _sample_rate);
  double k = 2.0 - 2.0 * res;
  double a1 = 1.0 / (1.0 + g * (g + k));
  double a2 = g * a1;
  double a3 = g * a2;

  auto v0 = source.to64();
  auto v3 = v0 - _state_var.ic2eq;
  auto v1 = a1 * _state_var.ic1eq + a2 * v3;
  auto v2 = _state_var.ic2eq + a2 * _state_var.ic1eq + a3 * v3;
  _state_var.ic1eq = 2.0 * v1 - _state_var.ic1eq;
  _state_var.ic2eq = 2.0 * v2 - _state_var.ic2eq;

  audio_sample64 result = {};
  switch (type)
  {
  case filter_state_var_type::lpf: result = v2; break;
  case filter_state_var_type::bpf: result = v1; break;
  case filter_state_var_type::bsf: result = v0 - k * v1; break;
  case filter_state_var_type::hpf: result = v0 - k * v1 - v2; break;
  case filter_state_var_type::apf: result = v0 - 2.0 * k * v1; break;
  default: assert(false); break;
  }  
  return sanity_audio(result.to32());
}

// https://www.dsprelated.com/freebooks/filters/Analysis_Digital_Comb_Filter.html
audio_sample32
filter::process_comb(automation_view const& automation, 
  audio_sample32 source, std::int32_t sample, float const* const* transformed_cv)
{
  float dly_min_sec = automation.get_transformed_dsp(filter_param::comb_dly_min, sample, transformed_cv);
  float dly_plus_sec = automation.get_transformed_dsp(filter_param::comb_dly_plus, sample, transformed_cv);
  float gain_min = automation.get_transformed_dsp(filter_param::comb_gain_min, sample, transformed_cv);
  float gain_plus = automation.get_transformed_dsp(filter_param::comb_gain_plus, sample, transformed_cv);
  std::int32_t dly_min_samples = static_cast<std::int32_t>(dly_min_sec * _sample_rate);
  std::int32_t dly_plus_samples = static_cast<std::int32_t>(dly_plus_sec * _sample_rate);
  audio_sample32 min = _comb.output.get(dly_min_samples) * gain_min;
  audio_sample32 plus = _comb.input.get(dly_plus_samples) * gain_plus;
  _comb.input.push(source);
  _comb.output.push(source + plus + min);
  return sanity_audio(_comb.output.get(0));
}

double 
filter::process_block(voice_input const& input, std::int32_t index, cv_state& cv,
  base::audio_sample32 const* audio_in, base::audio_sample32* audio_out, double& cv_time)
{
  float const* const* transformed_cv;
  automation_view automation(input.automation.rearrange_params(part_type::filter, index));
  cv_time += cv.transform_modulated(input, automation, cv_route_output::filter, index, cv_route_filter_mapping, transformed_cv);
  double start_time = performance_counter();
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio_out[s] = audio_in[s];
    bool on = automation.automation_discrete(filter_param::on, s) != 0;
    if (!on) continue;
    std::int32_t type = automation.automation_discrete(filter_param::type, s);
    switch (type)
    {
    case filter_type::comb: 
      audio_out[s] = process_comb(automation, audio_in[s], s, transformed_cv);
      break;
    case filter_type::state_var: 
      audio_out[s] = process_state_variable(automation, audio_in[s], s, transformed_cv);
      break;
    default: 
      assert(false); 
      break;
    }
  }
  return performance_counter() - start_time;
}

} // namespace svn::synth