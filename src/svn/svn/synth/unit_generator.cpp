#include <svn/synth/unit_generator.hpp>
#include <cmath>
#include <cstdint>

namespace svn {

template <typename sample_type>
unit_generator<sample_type>::
unit_generator():
_index(-1) {}

template <typename sample_type>
void unit_generator<sample_type>::
init(std::int32_t index) 
{ _index = index; }

template <typename sample_type>
void unit_generator<sample_type>::
process_buffer(
  std::size_t size, 
  sample_type sample_rate, 
  audio_buffer<sample_type>& audio, 
  automation_buffer<sample_type> const& automation)
{
}

template class unit_generator<float>;
template class unit_generator<double>;

} // namespace svn