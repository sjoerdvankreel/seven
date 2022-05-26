#include <svn.base/dsp/dsp.hpp>
#include <svn.synth/topology.hpp>
#include <svn.synth/oscillator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

using namespace svn::base;

namespace svn::synth {

void
oscillator::process_block(
  input_buffer const& input, 
  audio_sample* audio, param_value* state)
{
  assert(state != nullptr);
  assert(audio != nullptr);

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    float level = automate_real(oscillator_params, input, state, oscillator_param::level, s);
    float panning = automate_real(oscillator_params, input, state, oscillator_param::panning, s);
    float sample = std::sin(2.0f * std::numbers::pi * _phase);

    audio[s].left = (1.0f - panning) * sample;
    audio[s].right = 1.0f * sample;   
    _phase += 440.0f / input.sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn::synth