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
  automation_view automation(input.automation.rearrange_params(part_type::voice_lfo, index));
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    cv_out[s] = 0.0f;
    if(automation.get(voice_lfo_param::on, s).discrete == 0) continue;
    float frequency = automation.get(voice_lfo_param::freq, s).real;
    bool bipolar = automation.get(voice_lfo_param::bipolar, s).discrete != 0;
    float sample = sanity_bipolar(std::sin(2.0f * std::numbers::pi * _phase));
    cv_out[s] = bipolar ? sample : (sample + 1.0f) * 0.5f;
    _phase += frequency / _sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth