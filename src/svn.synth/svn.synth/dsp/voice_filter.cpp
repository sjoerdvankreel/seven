#include <svn.synth/dsp/voice_filter.hpp>
#include <svn.base/dsp/support.hpp>

#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

voice_filter::
voice_filter(float sample_rate, std::int32_t midi_note) :
_state_var(), _comb(), _sample_rate(sample_rate)
{
  float this_frequency = note_to_frequency(midi_note);
  float base_frequency = note_to_frequency(midi_note_c4);
  _state_var.kbd_track_base = this_frequency / base_frequency;
}

// https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
audio_sample32 
voice_filter::process_state_variable(
  automation_view const& automation, audio_sample32 source, std::int32_t sample)
{
  float res = automation.get(voice_filter_param::stvar_res, sample).real;
  float kbd = automation.get(voice_filter_param::stvar_kbd, sample).real;
  float freq = automation.get(voice_filter_param::stvar_freq, sample).real;
  std::int32_t type = automation.get(voice_filter_param::stvar_type, sample).discrete;

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
  case voice_filter_stvar_type::lpf: result = v2; break;
  case voice_filter_stvar_type::bpf: result = v1; break;
  case voice_filter_stvar_type::bsf: result = v0 - k * v1; break;
  case voice_filter_stvar_type::hpf: result = v0 - k * v1 - v2; break;
  case voice_filter_stvar_type::apf: result = v0 - 2.0 * k * v1; break;
  default: assert(false); break;
  }  
  return sanity_audio(result.to32());
}

// https://www.dsprelated.com/freebooks/filters/Analysis_Digital_Comb_Filter.html
audio_sample32
voice_filter::process_comb(
  automation_view const& automation, audio_sample32 source, std::int32_t sample)
{
  float dly_min_sec = automation.get(voice_filter_param::comb_dly_min, sample).real;
  float dly_plus_sec = automation.get(voice_filter_param::comb_dly_plus, sample).real;
  float gain_min = automation.get(voice_filter_param::comb_gain_min, sample).real;
  float gain_plus = automation.get(voice_filter_param::comb_gain_plus, sample).real;
  std::int32_t dly_min_samples = static_cast<std::int32_t>(dly_min_sec * _sample_rate);
  std::int32_t dly_plus_samples = static_cast<std::int32_t>(dly_plus_sec * _sample_rate);
  audio_sample32 min = _comb.output.get(dly_min_samples) * gain_min;
  audio_sample32 plus = _comb.input.get(dly_plus_samples) * gain_plus;
  _comb.input.push(source);
  _comb.output.push(source + plus + min);
  return sanity_audio(_comb.output.get(0));
}

void 
voice_filter::process_block(voice_input const& input, std::int32_t index,
  base::audio_sample32 const* audio_in, base::audio_sample32* audio_out)
{
  automation_view automation(input.automation.rearrange_params(part_type::voice_filter, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    audio_out[s] = audio_in[s];
    bool on = automation.get(voice_filter_param::on, s).discrete != 0;
    if (!on) continue;
    std::int32_t type = automation.get(voice_filter_param::type, s).discrete;
    switch (type)
    {
    case voice_filter_type::comb: 
      audio_out[s] = process_comb(automation, audio_in[s], s);
      break;
    case voice_filter_type::state_var: 
      audio_out[s] = process_state_variable(automation, audio_in[s], s);
      break;
    default: 
      assert(false); 
      break;
    }
  }
}

} // namespace svn::synth