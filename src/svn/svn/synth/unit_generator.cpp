#include <svn/synth/unit_generator.hpp>
#include <cmath>
#include <cstdint>

namespace svn {

template <typename sample_type>
void unit_generator<sample_type>::
init(std::int32_t index) 
{ _param_range = param_range(index, param_id::unit1_gain, param_id::unit1_panning); }

template <typename sample_type>
void unit_generator<sample_type>::
process_buffer(
  sample_type sample_rate,
  std::size_t sample_count,
  audio_buffer<sample_type>& audio, 
  automation_data<sample_type> const& automation)
{
  sample_type frequency = _param_range.offset == 0? 440.0f: 880.0f;
  for (std::size_t s = 0; s < sample_count; s++)
  {
  }
}

template class unit_generator<float>;
template class unit_generator<double>;

} // namespace svn