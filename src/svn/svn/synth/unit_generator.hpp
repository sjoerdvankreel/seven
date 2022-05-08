#ifndef SVN_SYNTH_UNIT_GENERATOR_HPP
#define SVN_SYNTH_UNIT_GENERATOR_HPP

#include <svn/param/synth_params.hpp>
#include <svn/support/audio_buffer.hpp>
#include <svn/event/event_buffer.hpp>
#include <svn/synth/process_info.hpp>
#include <svn/synth/unit_automation_buffer.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
class unit_generator 
{
  sample_type _phase = 0.0f;
public:
  void process_buffer(
    process_info<sample_type> const& info,
    audio_buffer<sample_type>& audio,
    event_buffer<sample_type> const& events,
    unit_automation_buffer<sample_type>& unit_automation);
};

} // namespace svn
#endif // SVN_SYNTH_UNIT_GENERATOR_HPP