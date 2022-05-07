#include <svn/synth/unit_generator.hpp>
#include <cmath>
#include <cstdint>

namespace svn {

template <typename sample_type>
unit_generator<sample_type>::
unit_generator(std::size_t index):
_index(index) {}

template <typename sample_type>
void 
unit_generator<sample_type>::process_buffer(
  sample_type sample_rate,
  audio_buffer<sample_type>& buffer,
  automation_data<sample_type> const& automation)
{
  std::int32_t param_id_count = static_cast<std::int32_t>(param_id::unit1_panning);

  for (std::size_t i = 0; i < buffer.size; i++)
  {

  }
}

template class unit_generator<float>;
template class unit_generator<double>;

} // namespace svn