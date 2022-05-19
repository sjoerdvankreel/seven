#include <svn/dsp/unit.hpp>
#include <cmath>
#include <numbers>
#include <cstdint>

namespace svn {

void
unit::process(input_buffer const& input, output_buffer& output)
{
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    if(input.automated(unit_param::level))
      output.param_real(unit_param::level, input.param_real(unit_param::level, s));
    if (input.automated(unit_param::panning))
      output.param_real(unit_param::panning, input.param_real(unit_param::panning, s));

    float level = output.param_real(unit_param::level);
    float panning = output.param_real(unit_param::panning);

    float sample = std::sin(2.0f * std::numbers::pi * _phase);
    output.audio[s].left = (1.0f - panning) * sample;
    output.audio[s].right = 1.0f * sample;
   
    _phase += 440.0f / input.sample_rate;
    _phase -= std::floor(_phase);
  }
}

} // namespace svn