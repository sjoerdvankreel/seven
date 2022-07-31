#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/envelope.hpp>

#include <cmath>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

static const float inv_log_half = 1.0f / std::log(0.5f);
 
float 
envelope::generate_slope(std::int32_t slope, float stage_pos, float exp)
{
  switch (slope)
  {
  case envelope_slope::linear: return stage_pos;
  case envelope_slope::inverted: return sanity_unipolar(std::sqrt(stage_pos));
  case envelope_slope::quadratic: return sanity_unipolar(stage_pos * stage_pos);
  case envelope_slope::logarithmic: return sanity_unipolar(std::pow(stage_pos, exp));
  default: assert(false); return 0.0f;
  }
}

void 
envelope::setup_stages(
  automation_view const& automation, float const* const* transformed_cv,
  float bpm, float& delay, float& attack, float& hold, float& decay, float& release)
{
  if (automation.automation_discrete(envelope_param::synced, 0) == 0)
  { 
    delay = transformed_cv[envelope_param::delay_time][0] * _sample_rate;
    attack = transformed_cv[envelope_param::attack_time][0] * _sample_rate;
    hold = transformed_cv[envelope_param::hold_time][0] * _sample_rate;
    decay = transformed_cv[envelope_param::decay_time][0] * _sample_rate;
    release = transformed_cv[envelope_param::release_time][0] * _sample_rate;
    return;
  }
  delay = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.automation_discrete(envelope_param::delay_sync, 0)]);
  attack = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.automation_discrete(envelope_param::attack_sync, 0)]);
  hold = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.automation_discrete(envelope_param::hold_sync, 0)]);
  decay = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.automation_discrete(envelope_param::decay_sync, 0)]);
  release = timesig_to_samples(_sample_rate, bpm, env_timesig_values[automation.automation_discrete(envelope_param::release_sync, 0)]);
}

double
envelope::process_block(
  voice_input const& input, std::int32_t index, cv_state& cv, 
  base::cv_sample* cv_out, std::int32_t release_sample, bool& ended, double& cv_time)
{
  // Note: all automation per block, not per sample!
  automation_view automation(input.automation.rearrange_params(part_type::envelope, index));

  // Always run envelope 0, it's hardwired to the voice amp section.
  if (index > 0 && automation.automation_discrete(envelope_param::on, 0))
  {
    std::memset(cv_out, 0, input.sample_count * sizeof(cv_sample));
    return 0.0;
  }

  // Shortcut if we're done already.
  if (_ended)
  {
    std::fill(cv_out, cv_out + input.sample_count, _end_sample);
    return 0.0;
  }

  // We need only 1 sample per block.
  float const* const* transformed_cv;
  cv_time += cv.transform_unmodulated(automation, part_type::envelope, 1, transformed_cv);

  double start_time = performance_counter();
  float delay, attack, hold, decay, release;
  float sustain = transformed_cv[envelope_param::sustain_level][0];
  setup_stages(automation, transformed_cv, input.bpm, delay, attack, hold, decay, release);
  float decay_exp = std::log(transformed_cv[envelope_param::decay_mid][0]) * inv_log_half;
  float attack_exp = std::log(transformed_cv[envelope_param::attack_mid][0]) * inv_log_half;
  float release_exp = std::log(transformed_cv[envelope_param::release_mid][0]) * inv_log_half;

  std::int32_t type = automation.automation_discrete(envelope_param::type, 0);
  std::int32_t bipolar = automation.automation_discrete(envelope_param::bipolar, 0);
  std::int32_t decay_slope = automation.automation_discrete(envelope_param::decay_slope, 0);
  std::int32_t attack_slope = automation.automation_discrete(envelope_param::attack_slope, 0);
  std::int32_t release_slope = automation.automation_discrete(envelope_param::release_slope, 0);

  float env = 0.0f;
  float position = 0.0f;
  float stage_pos = 0.0f;
  envelope_stage stage_id = static_cast<envelope_stage>(-1);

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    if (_ended) { cv_out[s] = _end_sample; continue; }

    stage_pos = 0.0f;
    position = static_cast<float>(_position);
    stage_id = static_cast<envelope_stage>(-1);

    // Cycle through the envelope stages.
    if (position < stage_pos + delay)
    {
      stage_id = envelope_stage::delay;
      env = 0.0f;
      goto sample_generated;
    }

    stage_pos = std::ceil(stage_pos + delay);
    if (position < stage_pos + attack)
    {
      stage_id = envelope_stage::attack;
      env = generate_slope(attack_slope, (position - stage_pos) / attack, attack_exp);
      goto sample_generated;
    }

    stage_pos = std::ceil(stage_pos + attack);
    if (position < stage_pos + hold)
    {
      stage_id = envelope_stage::hold;
      env = 1.0f;
      goto sample_generated;
    }

    stage_pos = std::ceil(stage_pos + hold);
    if (position < stage_pos + decay)
    {
      stage_id = envelope_stage::decay,
      env = 1.0f - generate_slope(decay_slope, (position - stage_pos) / decay, decay_exp) * (1.0f - sustain);
      goto sample_generated;
    }

    if (type == envelope_type::dahdsr && !_released)
    {
      stage_id = envelope_stage::sustain;
      env = sustain;
      goto sample_generated;
    }

    stage_pos = std::ceil(stage_pos + decay);
    if (position < stage_pos + release)
    {
      _released = true;
      stage_id = envelope_stage::release;
      env = _release_level - generate_slope(release_slope, (position - stage_pos) / release, release_exp) * _release_level;
      goto sample_generated;
    }

    stage_id = envelope_stage::end;
    env = 0.0f;

sample_generated:
    
    // Fill output buffer.
    if(bipolar == 0) cv_out[s] = { sanity_unipolar(env), false };
    else cv_out[s] = { sanity_bipolar(env * 2.0f - 1.0f), true };
    _end_sample = bipolar == 0 ? cv_sample({ 0.0f, false }) : cv_sample({ -1.0f, true });

    // We're done, fill the rest of the buffer with the last sample.
    if(stage_id == envelope_stage::end) 
    { 
      _ended = true; 
      std::fill(cv_out + s, cv_out + input.sample_count, _end_sample);
      break;
    }
    
    // Keep track of last sample in case of early release (during the attack or decay stages).
    if(stage_id != envelope_stage::release)
      _release_level = env;

    // Starting release section, except dahdr2 which just unconditionally follows the envelope stages.
    if (s == release_sample && !_released)
    {
      if(type != envelope_type::dahdr2)
      {
        _released = true;
        _position = static_cast<int32_t>(std::ceil(delay + attack + hold + decay));
      }
      continue;
    }
    
    // Increment counter ("where are we in the current section"),
    // except for sustain which could go on forever.
    if(type != envelope_type::dahdsr || stage_id != envelope_stage::sustain)
      _position++;
  }
  
  ended = _ended;
  return performance_counter() - start_time;
} 
 
} // namespace svn::synth