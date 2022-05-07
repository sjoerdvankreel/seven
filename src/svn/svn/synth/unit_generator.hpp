#ifndef SVN_SYNTH_UNIT_GENERATOR_HPP
#define SVN_SYNTH_UNIT_GENERATOR_HPP

#include <svn/param/synth_params.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/automation/automation_data.hpp>
#include <cstddef>

namespace svn {

template <typename sample_type>
class unit_generator 
{
  std::size_t const _index;
  sample_type _gain = synth_params::unit_gain_default;
  sample_type _panning = synth_params::unit_panning_default;
public:
  unit_generator(
    std::size_t index);
public:
  void
  process_buffer(
    sample_type sample_rate,
    audio_buffer<sample_type>& buffer,
    automation_data<sample_type> const& automation);
};

} // namespace svn
#endif // SVN_SYNTH_UNIT_GENERATOR_HPP