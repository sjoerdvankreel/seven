#include <svn/synth/seven_synth.hpp>

namespace svn {

template <typename sample_type>
seven_synth<sample_type>::
seven_synth(
  sample_type sample_rate,
  std::size_t max_buffer_size):
_units(),
_sample_rate(sample_rate),
_max_buffer_size(max_buffer_size),
_intermediate_buffer(max_buffer_size) {}

template <typename sample_type> 
void 
seven_synth<sample_type>::process_buffer(
  audio_buffer<sample_type>& buffer,
  automation_data<sample_type> const& automation)
{
  buffer.clear();
  audio_buffer<sample_type> intermediate_buffer;
  intermediate_buffer.size = buffer.size;
  intermediate_buffer.samples = _intermediate_buffer.data();

  for (std::size_t i = 0; i < unit_count; i++)
  {
    _units[i].process_buffer(_sample_rate, intermediate_buffer, automation);
    buffer.add(intermediate_buffer);
  }
}

template class seven_synth<float>;
template class seven_synth<double>;

} // namespace svn