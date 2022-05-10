#ifndef SVN_SYNTH_UNIT_AUTOMATION_BUFFER_HPP
#define SVN_SYNTH_UNIT_AUTOMATION_BUFFER_HPP

#include <svn/synth/param_state.hpp>
#include <svn/event/automation_buffer.hpp>
#include <cstdint>

namespace svn {

template <typename sample_type>
struct unit_automation_buffer
{
  param_id* params;
  std::int32_t offset;
  std::int32_t param_count;
  sample_type const** samples;

  void process(
    std::int32_t sample_index,
    param_state_t& param_state);

  sample_type value(
    param_id id,
    param_state_t const& param_state) const;

  void setup(
    param_id first, 
    param_id last, 
    std::int32_t unit_index, 
    std::int32_t automation_count,
    automation_buffer<sample_type> const* automation);
};

template <typename sample_type>
inline sample_type unit_automation_buffer<sample_type>::
value(
  param_id id,
  param_state_t const& param_state) const
{ return static_cast<sample_type>(param_state.values[id + offset]); }

} // namespace svn
#endif // SVN_SYNTH_UNIT_AUTOMATION_BUFFER_HPP