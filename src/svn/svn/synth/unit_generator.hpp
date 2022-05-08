#ifndef SVN_SYNTH_UNIT_GENERATOR_HPP
#define SVN_SYNTH_UNIT_GENERATOR_HPP

#include <svn/param/param_range.hpp>
#include <svn/param/synth_params.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/support/automation_data.hpp>

#include <cstddef>
#include <cstdint>

namespace svn {

template <typename sample_type>
class unit_generator 
{
  param_range _param_range;
  sample_type _phase = 0.0f;
  sample_type _gain = synth_params::unit_gain_default;
  sample_type _panning = synth_params::unit_panning_default;
public:
  void init(std::int32_t index);
  void process_buffer(
    sample_type sample_rate,
    std::size_t sample_count,
    audio_buffer<sample_type>& audio,
    automation_data<sample_type> const& automation);
};

} // namespace svn
#endif // SVN_SYNTH_UNIT_GENERATOR_HPP