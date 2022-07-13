#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/voice_lfo.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

void 
voice_lfo::process_block(voice_input const& input, std::int32_t index, float* cv_out)
{
  float frequency = 0.0f;
  automation_view automation(input.automation.rearrange_params(part_type::voice_lfo, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    cv_out[s] = 0.0f;
    if(automation.get(voice_lfo_param::on, s).discrete == 0) continue;
    std::int32_t sync_polarity = automation.get(envelope_param::sync_polarity, s).discrete;
    if(!cv_sync_polarity_is_synced(sync_polarity))
      frequency = automation.get(voice_lfo_param::freq_time, s).real;
    else
    {
      float timesig = voice_lfo_timesig_values[automation.get(voice_lfo_param::freq_sync, s).discrete];
      frequency = timesig_to_frequency(_sample_rate, input.bpm, timesig);
    }
    float sample = sanity_bipolar(std::sin(2.0f * std::numbers::pi * _phase));
    if (cv_sync_polarity_is_unipolar(sync_polarity)) cv_out[s] = (sample + 1.0f) * 0.5f;
    else cv_out[s] = sample;
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth