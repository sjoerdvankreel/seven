#include <svn/dsp/dsp.hpp>
#include <svn/dsp/unit.hpp>

#include <cmath>
#include <numbers>
#include <cstdint>

namespace svn {

void
unit::process(input_buffer const& input, audio_sample* audio, param_value* state)
{
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