#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/lfo.hpp>
#include <svn.synth/topology/topology.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

double
lfo::process_block(voice_input const& input, std::int32_t index, base::cv_sample* cv_out)
{
  float frequency = 0.0f;
  double start_time = performance_counter();
  automation_view automation(input.automation.rearrange_params(part_type::lfo, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    cv_out[s] = { 0.0f, false };
    if(automation.get(lfo_param::on, s).discrete == 0) continue;
    if(automation.get(lfo_param::synced, s).discrete == 0)
      frequency = automation.get(lfo_param::frequency_time, s).real;
    else
    {  
      float timesig = lfo_timesig_values[automation.get(lfo_param::frequency_sync, s).discrete];
      frequency = timesig_to_frequency(_sample_rate, input.bpm, timesig);
    }
    float sample = sanity_bipolar(std::sin(2.0f * std::numbers::pi * _phase));
    if (automation.get(lfo_param::bipolar, s).discrete == 0) cv_out[s] = { (sample + 1.0f) * 0.5f, false };
    else cv_out[s] = { sample, true }; 
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
  return performance_counter() - start_time;
}

} // namespace svn::synth