#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/envelope.hpp>

#include <cmath>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

float 
envelope::generate_stage(base::automation_view const& automation,
std::int32_t s, float delay, float attack, float hold, float decay, float release)
{
  float pos = static_cast<float>(_position);

  float stage = 0.0f;
  if (pos < stage + delay) return 0.0f;
  stage += delay;
  if(pos < stage + attack) return (pos - stage) / (attack - stage);
  stage += attack;
  if(pos < stage + hold) return 1.0f;
  stage += hold;
  if(pos < stage + decay) return (pos - stage) / (decay - stage);
  stage += decay;
  if(pos < stage + release) return (pos - stage) / (release - stage);
  return 0.0f;
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

void 
envelope::process_block(voice_input const& input, std::int32_t index, float* cv_out)
{
  float delay;
  float attack;
  float hold;
  float decay;
  float release;

  automation_view automation(input.automation.rearrange_params(part_type::envelope, index));
  for (std::int32_t s = 0; s < input.sample_count; s++, _position++)
  {
    cv_out[s] = 1.0f;
    bool on = automation.get(envelope_param::on, s).discrete != 0;
    if (!on) continue;

    float sustain = automation.get(envelope_param::sustain_level, s).real;
    bool dahdsr = automation.get(envelope_param::type, s).discrete == envelope_type::dahdsr;
    setup_stages(automation, s, input.bpm, delay, attack, hold, decay, release);
    cv_out[s] = generate_stage(automation, s, delay, attack, hold, decay, release);
  }
}

} // namespace svn::synth