#include <svn/dsp/shared_dsp.hpp>
#include <svn/dsp/unit_generator.hpp>

#include <cmath>
#include <cassert>
#include <numbers>
#include <cstdint>

namespace svn {

void
unit_generator::process_block(input_buffer const& input, audio_sample* audio, param_value* state)
{
  assert(state != nullptr);
  assert(audio != nullptr);

  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    float level = automate_real(input, state, unit_param::level, s);
    float panning = automate_real(input, state, unit_param::panning, s);

    float sample = std::sin(2.0f * std::numbers::pi * _phase);
    audio[s].left = (1.0f - panning) * sample;
    audio[s].right = 1.0f * sample;   
    _phase += 440.0f / input.sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn