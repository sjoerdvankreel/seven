#include <svn.base/dsp/support.hpp>
#include <svn.synth/dsp/lfo.hpp>
#include <svn.synth/topology/topology.hpp>

#include <cmath>
#include <cassert>

using namespace svn::base;

namespace svn::synth {

double
lfo::process_block(
  voice_input const& input, std::int32_t index, 
  cv_state& cv, base::cv_sample* cv_out, double& cv_time)
{
  // Note: discrete automation per block, not per sample!
  automation_view automation(input.automation.rearrange_params(part_type::lfo, index));
  if (automation.automation_discrete(lfo_param::on, 0) == 0)
  {
    std::memset(cv_out, 0, input.sample_count * sizeof(cv_sample));
    return 0.0;
  }

  float frequency = 0.0f;
  float const* const* transformed_cv;
  cv_time += cv.transform_unmodulated(automation, part_type::lfo, input.sample_count, transformed_cv);

  double start_time = performance_counter();
  float const* period_time = transformed_cv[lfo_param::period_time];
  std::int32_t synced = automation.automation_discrete(lfo_param::synced, 0);
  std::int32_t bipolar = automation.automation_discrete(lfo_param::bipolar, 0);
  std::int32_t period_sync = automation.automation_discrete(lfo_param::period_sync, 0);
  float timesig = lfo_timesig_values[period_sync];
  float sync_frequency = timesig_to_frequency(input.bpm, timesig);
  
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    if(synced == 0) frequency = 1.0f / period_time[s];
    else frequency = sync_frequency;
    float sample = sanity_bipolar(std::sin(2.0f * pi32 * _phase));
    if (bipolar == 0) cv_out[s] = { (sample + 1.0f) * 0.5f, false };
    else cv_out[s] = { sample, true }; 
    _phase += frequency / _sample_rate; 
    _phase -= std::floor(_phase);
  }
  return performance_counter() - start_time;
}

} // namespace svn::synth