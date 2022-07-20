#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/envelope.hpp>

#include <cmath>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

static const float inv_log_half = 1.0f / std::log(0.5f);
 
float 
envelope::generate_slope(base::automation_view const& automation,
  std::int32_t slope_param, std::int32_t mid_param, std::int32_t s, float stage_pos)
{
  std::int32_t slope = automation.get(slope_param, s).discrete;
  switch (slope)
  {
  case envelope_slope::linear: return stage_pos;
  case envelope_slope::inverted: return sanity_unipolar(std::sqrt(stage_pos));
  case envelope_slope::quadratic: return sanity_unipolar(stage_pos * stage_pos);
  default:
    assert(slope == envelope_slope::logarithmic);
    break;
  }
  float mid = automation.get(mid_param, s).real;
  return sanity_unipolar(std::pow(stage_pos, std::log(mid) * inv_log_half));
}

std::pair<envelope_stage, float>
envelope::generate_stage(base::automation_view const& automation, std::int32_t s, 
  bool dahdsr, float delay, float attack, float hold, float decay, float sustain, float release)
{
  float pos = static_cast<float>(_position);

  float stage = 0.0f;
  if (pos < stage + delay) return std::make_pair(
    envelope_stage::delay, 0.0f);
  stage = std::ceil(stage + delay);
  if(pos < stage + attack) return std::make_pair(
    envelope_stage::attack, 
    generate_slope(automation, envelope_param::attack_slope, 
    envelope_param::attack_mid, s, (pos - stage) / attack));
  stage = std::ceil(stage + attack);
  if(pos < stage + hold) return std::make_pair(
    envelope_stage::hold, 1.0f);
  stage = std::ceil(stage + hold);
  if(pos < stage + decay) return std::make_pair(
    envelope_stage::decay,
    sanity_unipolar(1.0f - generate_slope(
    automation, envelope_param::decay_slope,
    envelope_param::decay_mid, s, (pos - stage) / decay) * (1.0f - sustain)));
  if(dahdsr && !_released) return std::make_pair(
    envelope_stage::sustain, sustain);
  stage = std::ceil(stage + decay);
  if(pos < stage + release) return std::make_pair(
    envelope_stage::release,
    sanity_unipolar(_release_level - generate_slope(
    automation, envelope_param::release_slope,
    envelope_param::release_mid, s, (pos - stage) / release) * _release_level));
  return std::make_pair(envelope_stage::end, 0.0f);
}

void 
envelope::setup_stages(automation_view const& automation, std::int32_t s,
  float bpm, float& delay, float& attack, float& hold, float& decay, float& release)
{
  if (automation.get(envelope_param::synced, s).discrete == 0)
  { 
    delay = automation.get(envelope_param::delay_time, s).real * _sample_rate;
    attack = automation.get(envelope_param::attack_time, s).real * _sample_rate;
    hold = automation.get(envelope_param::hold_time, s).real * _sample_rate;
    decay = automation.get(envelope_param::decay_time, s).real * _sample_rate;
    release = automation.get(envelope_param::release_time, s).real * _sample_rate;
    return;
  }
  delay = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.get(envelope_param::delay_sync, s).discrete]);
  attack = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.get(envelope_param::attack_sync, s).discrete]);
  hold = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.get(envelope_param::hold_sync, s).discrete]);
  decay = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.get(envelope_param::decay_sync, s).discrete]);
  release = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.get(envelope_param::release_sync, s).discrete]);
}

double
envelope::process_block(voice_input const& input, std::int32_t index, 
  base::cv_sample* cv_out, std::int32_t release_sample, bool& result)
{
  float delay, attack, hold, decay, release;
  double start_time = performance_counter();
  automation_view automation(input.automation.rearrange_params(part_type::envelope, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    if (_ended) { cv_out[s] = _end_sample; continue; }
    cv_out[s] = { 0.0f, false };
    if(automation.get(envelope_param::on, s).discrete == 0) return s;
    float sustain = automation.get(envelope_param::sustain_level, s).real;
    bool unipolar = automation.get(envelope_param::bipolar, s).discrete == 0;
    bool dahdsr = automation.get(envelope_param::type, s).discrete == envelope_type::dahdsr;
    setup_stages(automation, s, input.bpm, delay, attack, hold, decay, release);
    auto stage = generate_stage(automation, s, dahdsr, delay, attack, hold, decay, sustain, release);
    if(unipolar) cv_out[s] = { sanity_unipolar(stage.second), false };
    else cv_out[s] = { sanity_bipolar(stage.second * 2.0f - 1.0f), true };
    _end_sample = unipolar ? cv_sample({ 0.0f, false }) : cv_sample({ -1.0f, true });
    if(stage.first == envelope_stage::end) { _ended = true; cv_out[s] = _end_sample; continue; }
    if(stage.first != envelope_stage::release) _release_level = stage.second;
    if(s == release_sample) _released = true, _position = static_cast<int32_t>(std::ceil(delay + attack + hold + decay));
    else if(!dahdsr || stage.first != envelope_stage::sustain) _position++;
  }
  result = _ended;
  return performance_counter() - start_time;
} 
 
} // namespace svn::synth