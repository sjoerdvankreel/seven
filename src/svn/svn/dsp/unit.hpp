#ifndef SVN_DSP_UNIT_HPP
#define SVN_DSP_UNIT_HPP

#include <svn/support/block.hpp>
#include <svn/support/audio.hpp>

namespace svn {

class unit
{
  float _phase = 0.0f;
public:
  void process(
    block_t const& block,
    audio_buffer& audio,
    process_info<sample_type> const& info,
    audio_buffer<sample_type>& audio,
    event_buffer<sample_type> const& events,
    unit_automation_buffer<sample_type>& unit_automation);
};

} // namespace svn
#endif // SVN_DSP_UNIT_HPP