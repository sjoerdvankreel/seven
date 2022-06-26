#include <svn.synth/dsp/voice_filter.hpp>
#include <svn.base/dsp/support.hpp>

#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

// https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
audio_sample32 
voice_filter::process_state_variable(
  automation_view const& automation, audio_sample32 const* source, std::int32_t sample)
{
  float res = automation.get(voice_filter_param::stvar_res, sample).real;
  float kbd = automation.get(voice_filter_param::stvar_kbd, sample).real;
  float freq = automation.get(voice_filter_param::stvar_freq, sample).real;
  std::int32_t type = automation.get(voice_filter_param::stvar_type, sample).discrete;

  double g = std::tan(std::numbers::pi * freq / _sample_rate);
  double k = 2.0 - 2.0 * res;
  double a1 = 1.0 / (1.0 + g * (g + k));
  double a2 = g * a1;
  double a3 = g * a2;

  auto v0 = source[sample].to64();
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
  default: assert(false); break;
  }
  return sanity_audio(result.to32());
}

void 
voice_filter::process_block(voice_input const& input, 
  audio_sample32 const* source, audio_sample32* output)
{
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    output[s] = 0.0f;
    bool on = input.automation.get(voice_filter_param::on, s).discrete != 0;
    if (!on) continue;

    std::int32_t type = input.automation.get(voice_filter_param::type, s).discrete;
    switch (type)
    {
    case voice_filter_type::comb: 
      break;
    case voice_filter_type::state_var: 
      output[s] = process_state_variable(input.automation, source, s); 
      break;
    default: 
      assert(false); 
      break;
    }
  }
}

} // namespace svn::synth