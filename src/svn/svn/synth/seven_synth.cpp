#include <svn/synth/seven_synth.hpp>

namespace svn {

template <typename sample_type>
seven_synth<sample_type>::
seven_synth(sample_type sample_rate, std::size_t max_sample_count):
_units(),
_sample_rate(sample_rate),
_max_sample_count(max_sample_count),
_audio_scratch(max_sample_count)
{
  for(std::size_t i = 0; i < unit_count; i++)
    _units[i].init(i);
}

template <typename sample_type> 
void seven_synth<sample_type>::
process_buffer(
  std::size_t sample_count,
  audio_buffer<sample_type>& audio,
  automation_data<sample_type> const& automation)
{
  audio.clear(sample_count);
  audio_buffer<sample_type> audio_scratch;
  audio_scratch.samples = _audio_scratch.data();

  for (std::size_t i = 0; i < unit_count; i++)
  {
    _units[i].process_buffer(_sample_rate, sample_count, audio_scratch, automation);
    audio.add(sample_count, audio_scratch);
  }
}

template class seven_synth<float>;
template class seven_synth<double>;

} // namespace svn