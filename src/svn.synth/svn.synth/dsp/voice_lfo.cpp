#include <svn.base/dsp/support.hpp>
#include <svn.synth/topology/topology.hpp>
#include <svn.synth/dsp/voice_lfo.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

double
voice_lfo::process_block(voice_input const& input, std::int32_t index, base::cv_sample* cv_out)
{
  float frequency = 0.0f;
  double start_time = performance_counter();
  automation_view automation(input.automation.rearrange_params(part_type::voice_lfo, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    cv_out[s] = { 0.0f, false };
    if(automation.get(voice_lfo_param::on, s).discrete == 0) continue;
    std::int32_t kind = automation.get(envelope_param::kind, s).discrete;
    if(!cv_kind_is_synced(kind))
      frequency = automation.get(voice_lfo_param::freq_time, s).real;
    else
    {
      float timesig = voice_lfo_timesig_values[automation.get(voice_lfo_param::freq_sync, s).discrete];
      frequency = timesig_to_frequency(_sample_rate, input.bpm, timesig);
    }
    float sample = sanity_bipolar(std::sin(2.0f * std::numbers::pi * _phase));
    if (cv_kind_is_unipolar(kind)) cv_out[s] = { (sample + 1.0f) * 0.5f, false };
    else cv_out[s] = { sample, true };
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
  return performance_counter() - start_time;
}

} // namespace svn::synth