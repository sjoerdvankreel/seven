#ifndef SVN_SYNTH_UNIT_GENERATOR_HPP
#define SVN_SYNTH_UNIT_GENERATOR_HPP

#include <svn/param/synth_params.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/support/automation_buffer.hpp>

#include <cstddef>
#include <cstdint>

namespace svn {

template <typename sample_type>
class unit_generator 
{
  std::int32_t _index;
  sample_type _gain = synth_params::unit_gain_default;
  sample_type _panning = synth_params::unit_panning_default;
public:
  unit_generator();
public:
  void init(std::int32_t index);
  void process_buffer(
    std::size_t size,
    sample_type sample_rate,
    audio_buffer<sample_type>& audio,
    automation_buffer<sample_type> const& automation);
};

} // namespace svn
#endif // SVN_SYNTH_UNIT_GENERATOR_HPP