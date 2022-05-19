#include <svn/dsp/unit.hpp>
#include <cstdint>

namespace svn {

void
unit::process(input_buffer const& input, output_buffer& output)
{
  for (std::int32_t s = 0; s < input.sample_count; s++)
  {
    if(input.automation[unit_param::level] != nullptr)
      output.param_values[unit_param::level].real = static_cast<float const*>(input.automation[unit_param::level])[s];
  }
}

} // namespace svn