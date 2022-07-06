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
  case envelope_slope::lin: return stage_pos;
  case envelope_slope::sqrt: return sanity_unipolar(std::sqrt(stage_pos));
  case envelope_slope::quad: return sanity_unipolar(stage_pos * stage_pos);
  default:
    assert(slope == envelope_slope::log);
    break;
  }
  float mid = automation.get(mid_param, s).real;
  return sanity_unipolar(std::pow(stage_pos, std::log(mid) * inv_log_half));
}

std::pair<envelope_stage, float>
envelope::generate_stage(base::automation_view const& automation, std::int32_t s, 
  float delay, float attack, float hold, float decay, float sustain, float release)
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
  if (automation.get(envelope_param::sync, s).discrete == 0)
  {
    delay = automation.get(envelope_param::delay_time, s).real * _sample_rate;
    attack = automation.get(envelope_param::attack_time, s).real * _sample_rate;
    hold = automation.get(envelope_param::hold_time, s).real * _sample_rate;
    decay = automation.get(envelope_param::decay_time, s).real * _sample_rate;
    release = automation.get(envelope_param::release_time, s).real * _sample_rate;
    return;
  }
  delay = _sample_rate * 60.0f / bpm * env_synced_timesig_values[automation.get(envelope_param::delay_sync, s).discrete];
  attack = _sample_rate * 60.0f / bpm * env_synced_timesig_values[automation.get(envelope_param::attack_sync, s).discrete];
  hold = _sample_rate * 60.0f / bpm * env_synced_timesig_values[automation.get(envelope_param::hold_sync, s).discrete];
  decay = _sample_rate * 60.0f / bpm * env_synced_timesig_values[automation.get(envelope_param::decay_sync, s).discrete];
  release = _sample_rate * 60.0f / bpm * env_synced_timesig_values[automation.get(envelope_param::release_sync, s).discrete];
}

std::int32_t 
envelope::process_block(voice_input const& input, std::int32_t index, float* cv_out, std::int32_t release_sample)
{
  float delay, attack, hold, decay, release;
  automation_view automation(input.automation.rearrange_params(part_type::envelope, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    cv_out[s] = 0.0f;
    if(automation.get(envelope_param::on, s).discrete == 0) return s;
    float sustain = automation.get(envelope_param::sustain_level, s).real;
    setup_stages(automation, s, input.bpm, delay, attack, hold, decay, release);
    auto stage = generate_stage(automation, s, delay, attack, hold, decay, sustain, release);
    cv_out[s] = sanity_unipolar(stage.second);
    if(stage.first == envelope_stage::end) return s;
    if(stage.first != envelope_stage::release) _release_level = cv_out[s];
    if(s == release_sample) _position = static_cast<int32_t>(std::ceil(delay + attack + hold + decay));
    else _position++;
  }
  return input.sample_count;
}

} // namespace svn::synth