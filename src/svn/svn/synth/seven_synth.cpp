#include <svn/synth/seven_synth.hpp>

namespace svn {

template <typename sample_type>
seven_synth<sample_type>::
seven_synth(sample_type sample_rate, std::size_t max_buffer_size):
_units(),
_sample_rate(sample_rate),
_max_buffer_size(max_buffer_size),
_audio_scratch(max_buffer_size)
{
  for(std::size_t i = 0; i < unit_count; i++)
    _units[i].init(i);
}

template <typename sample_type> 
void seven_synth<sample_type>::
process_buffer(
  std::size_t size,
  audio_buffer<sample_type>& audio,
  automation_buffer<sample_type> const& automation)
{
  audio.clear(size);
  audio_buffer<sample_type> audio_scratch;
  audio_scratch.samples = _audio_scratch.data();

  for (std::size_t i = 0; i < unit_count; i++)
  {
    _units[i].process_buffer(size, _sample_rate, audio_scratch, automation);
    audio.add(size, audio_scratch);
  }
}

template class seven_synth<float>;
template class seven_synth<double>;

} // namespace svn